#ifndef CELL_MAPPING_CPP_ICM_H
#define CELL_MAPPING_CPP_ICM_H

#include "css.h"
#include "system.h"
#include "coloring.h"
#include <stdio.h>
#include <jpeg-9a/jpeglib.h>
#include <fstream>
#include <algorithm>

#include <iostream>
#include "omp.h"

namespace cm {

    enum class TerminationState {
        Sink,
        Periodic,
        TransientJoining,
        MaxSteps,
        Unknown
    };

    constexpr size_t SINK_GROUP = 0;
    constexpr size_t UNDETERMINED_GROUP = 1000000000;

    template <class CellType, class IDType, class StateVectorType>
    class ICM {
    private:
        ICMUniformCellStateSpace<CellType, IDType, StateVectorType> css;
        DynamicalSystemBase<StateVectorType> *systemPointer;
        IDType periodicGroups;
        std::map<IDType, std::vector<StateVectorType>> periodicGroupMaps;
        std::vector<IDType> periodicities;
        std::vector<std::shared_ptr<IcmGraphNode<IDType, StateVectorType>>> IcmGraph;
        std::vector<StateVectorType> graphHoles;
    public:
        ICM(StateVectorType center, StateVectorType width, const std::vector<IDType>& cellCounts,
            DynamicalSystemBase<StateVectorType> *systemPointer) :
            css(center, width, cellCounts), systemPointer(systemPointer) {
            periodicGroups = 0;
            periodicities.resize(0);
            periodicGroupMaps.clear();
        }
        // TODO: Maybe there is no need to differentiate the two lambdas
        // TODO: Question for paper: what if lambda is increased above 1?
        void build_graph(size_t max_steps = 500, double lambda_closure = 0.1, double lambda_join = 0.2) {
            // Calculate images
            std::cout << "Initializing Cell state space with " << css.getCellSum() << " cells, OMP parallel workers: " << omp_get_max_threads() << "\n";
            std::cout << "Integration step...\n";
            StateVectorType halfCellWidth = 0.5 * css.getCellWidth();
            // Sink cell (ID=0)
            css.setImage(0, 0);
            css.getCell(0).setState(CellState::Processed);
            // Calculate images
#pragma omp parallel for
            for (IDType i=1; i<css.getCellSum(); i++) {
                IDType steps = 0; IDType image = i;
                StateVectorType imageState = css.getCenter(i) - halfCellWidth; // Top-left corner
                css.getCell(i).setState(CellState::Untouched);
                imageState = systemPointer->step(imageState);
                image = css.getID(imageState);
                css.setImage(i, image);
                css.setImageState(i, imageState);
            }
            // Fetch neighbours
            for (IDType i=1; i<css.getCellSum(); i++) {
                auto corners = css.getPositiveNeighbours(i);
                css.getCell(i).setOtherCorners(corners);
            }
            std::cout << "Generating graph " << std::endl;
            //periodicGroupMaps.clear();
            IDType sequence_id = 0;
            IDType joined_sequence = 0; // TODO: Remove
            //IDType neighbour_seq = 0;
            double characteristicCellSize = norm(css.getCellWidth());
            double join_distance = lambda_join * characteristicCellSize;
            double periodicity_distance = lambda_closure * characteristicCellSize;
            double group_merge_distance = 1.0 * characteristicCellSize; // TODO: Restore group merge
            IDType total_new_nodes = 0;
            IcmGraph.resize(0);
            IDType stats = css.getCellSum()/100;
            std::shared_ptr<IcmGraphNode<IDType, StateVectorType>> Sink = std::make_shared<IcmGraphNode<IDType, StateVectorType>>(); total_new_nodes++;
            Sink->image = Sink;
            Sink->parent = Sink;
            Sink->sequence = sequence_id;
            IcmGraph.push_back(Sink);
            size_t has_parent_chk = 1;
            // Stats
            size_t stat_periodic = 0;
            size_t stat_transient_join = 0;
            size_t stat_maxsteps = 0;
            size_t stat_sink = 0;
            for (IDType i = 1; i < css.getCellSum(); i++) {
                // Start new sequence
                sequence_id++;
                if (i % stats == 0) {
                    std::cout << "Progress: " << i << "/" << css.getCellSum() << std::endl;
                    std::cout << "Current graph size: " << IcmGraph.size() << std::endl;
                    std::cout << "Current expansion: " << static_cast<double>(IcmGraph.size())/static_cast<double>(i) << std::endl;
                }
                // Start a sequence starting from cell 'i'
                std::vector<std::shared_ptr<IcmGraphNode<IDType, StateVectorType>>> seq;
                StateVectorType state = css.getCenter(i);
                std::shared_ptr<IcmGraphNode<IDType, StateVectorType>> node = std::make_shared<IcmGraphNode<IDType, StateVectorType>>(); total_new_nodes++;
                node->state = state;
                node->sequence = sequence_id;
                node->parent = nullptr;
                css.addNode(i, node);
                StateVectorType newState;
                seq.push_back(node);
                bool terminated = false;
                TerminationState terminationState = TerminationState::Unknown; // TODO: Remove
                double distance = 0;
                bool enable_sequence_matching = true;
                // Interpolate path until one of the termination conditions are met
                std::shared_ptr<IcmGraphNode<IDType, StateVectorType>> parent = node;
                for (IDType steps = 0; steps < max_steps; steps++) {
                    if (css.interpolate(state, newState)) {
                        // Create new node for the new State
                        std::shared_ptr<IcmGraphNode<IDType, StateVectorType>> newNode = std::make_shared<IcmGraphNode<IDType, StateVectorType>>(); total_new_nodes++;
                        newNode->state = newState;
                        newNode->sequence = sequence_id;
                        newNode->parent = parent; has_parent_chk++;
                        parent->image = newNode;
                        // Tag cells with sequence
                        css.addNode(css.getID(newState), newNode);
                        // Check termination vs current sequence
                        for (const auto& s : seq) {
                            if (norm(newState - s->state) < periodicity_distance) {
                                // Break cycle, mark as periodic
                                terminated = true;
                                terminationState = TerminationState::Periodic;
                                stat_periodic += seq.size()+1; // Account for the current node
                                newNode->image = s;
                                // Note: s already has a parent
                            }
                        }
                        // Store into current sequence
                        seq.push_back(newNode);
                        if (!terminated) {
                            // Check termination vs previously marked paths
                            std::shared_ptr<IcmGraphNode<IDType, StateVectorType>> join_node = nullptr;
                            distance = css.getClosestMatch(newState, sequence_id, join_node);
                            //if (join_node) {
                            //    std::cout << "Closest seq: " << join_node->sequence << " at " << distance << std::endl;
                            //}
                            if (enable_sequence_matching && (distance <= join_distance)) {
                                terminated = true;
                                terminationState = TerminationState::TransientJoining;
                                stat_transient_join += seq.size();
                                newNode->image = join_node;
                                // Note: join node already has parent
                                joined_sequence = join_node->sequence;
                                // TODO: Re-tag sequence (probably not needed)
                            }
                        }
                        // Update state
                        state = newState;
                        parent = newNode;
                        if (terminated) {
                            // Store sequence into graph
                            for (auto n : seq) {
                                IcmGraph.push_back(n);
                            }
                            break;
                        }
                    } else { // Left region / entered sink cell
                        terminated = true;
                        terminationState = TerminationState::Sink;
                        seq.back()->image = Sink;
                        // Store sequence into graph
                        for (auto n : seq) {
                            IcmGraph.push_back(n);
                        }
                        stat_sink += seq.size();
                        break;
                    }
                }
                if (!terminated) {
                    std::cout << "Maxsteps reached" << std::endl;
                    terminationState = TerminationState::MaxSteps;
                    seq.back()->image = Sink; // TODO: Find a better way. Currently tag with Sink.
                    // Store sequence into graph
                    for (auto n : seq) {
                        IcmGraph.push_back(n);
                    }
                    stat_maxsteps += seq.size();
                }
            }
            std::cout << "Expansion rate: " << static_cast<double>(IcmGraph.size())/static_cast<double>(css.getCellSum()) << std::endl;
            size_t has_parent = 0;
            size_t has_image = 0;
            size_t has_sequence = 0;
            size_t roots = 0;
            for (auto n : IcmGraph) {
                if (n->parent) {
                    has_parent++;
                } else {
                    roots++;
                }
                if (n->image) has_image++;
                if (n->sequence != 0) has_sequence++;
            }
            // Note: 2026.10.08: All stats are passing
            std::cout << "Total new nodes created: " << total_new_nodes << std::endl;
            std::cout << "Size of graph: " << IcmGraph.size() << std::endl;
            std::cout << "Has image: " << has_image << " " << (static_cast<double>(has_image)*100.0/IcmGraph.size()) << "%" << std::endl;
            std::cout << "Has sequence: " << has_sequence+1 << " " << (static_cast<double>(has_sequence+1)*100.0/IcmGraph.size()) << "%" << std::endl; // Note: +1 for Sink with 0 seqID
            std::cout << "Has parent: " << has_parent << " (chk: " << has_parent_chk << ")" << " (roots: " << roots << ")" << std::endl;
            std::cout << " Periodic: " << stat_periodic << std::endl;
            std::cout << " Transient join: " << stat_transient_join << std::endl;
            std::cout << " Transient (maxsteps): " << stat_maxsteps << std::endl;
            std::cout << " Transient (sink): " << stat_sink << std::endl;
            auto stat_sum = stat_periodic+stat_transient_join+stat_maxsteps+stat_sink+1; // Note: +1 for Sink node (no graph traversal classification)
            std::cout << "Total stat sum: " << stat_sum << " ?= " << IcmGraph.size() << std::endl;
            if (stat_sum != IcmGraph.size()) {
                std::cout << " Diff (graph - stats): " << IcmGraph.size() - stat_sum << std::endl;
            }
        }
        void solve() {
            // Calculate images
            std::cout << "Running SCM on graph with " << IcmGraph.size() << " nodes\n";
            // TODO: Proper initialization on first pass in ctor.
            //periodicGroups = 0;
            //periodicities.resize(0);
            // Determine cell evolutions for cells
            std::shared_ptr<IcmGraphNode<IDType, StateVectorType>> z;
            IDType p, s;
            bool processing;
            std::vector<std::shared_ptr<IcmGraphNode<IDType, StateVectorType>>> sequence;
            // TODO: Initialize state of sink? (Not needed if first element in graph)
            for (auto n : IcmGraph) {
                z = n;
                if (z->cState == CellState::Untouched) {
                    z->cState = CellState::UnderProcessing;
                    processing = true;
                    sequence.resize(0);
                    sequence.push_back(z);
                    // Start processing sequence for i
                    while (processing) {
                        z = z->image;
                        if (!z->image) {
                            std::cout << "Critical error, no image!" << std::endl;
                        }
                        switch (z->cState) {
                            case CellState::Untouched:
                                // Mark cell as under processing, store in the sequence then continue
                                z->cState = CellState::UnderProcessing;
                                sequence.push_back(z);
                                break;
                            case CellState::UnderProcessing:
                                // New periodic group and possibily some transients
                                processing = false;
                                // First find the periodic group by scanning sequence backwards
                                s = sequence.size();
                                p = 0;
                                for (size_t j=0; j<s; j++) {
                                    if (sequence[s-1-j]==z) { p = j+1; }
                                }
                                // Create new PG
                                periodicities.push_back(p);
                                std::cout << "New PG (" << periodicGroups << ") with P = " << p << std::endl;
                                periodicGroups++;						// Increase group counter
                                // Set properties for periodic cells,
                                for (size_t j=0; j<p; j++) {
                                    sequence[s-1-j]->group = periodicGroups-1;
                                    sequence[s-1-j]->step = 0;
                                    sequence[s-1-j]->cState = CellState::Processed;
                                }
                                // Set properties for transient cells
                                for (size_t j=p; j<s; j++) {
                                    sequence[s-1-j]->group = periodicGroups-1;
                                    sequence[s-1-j]->step = j-p+1;
                                    sequence[s-1-j]->cState = CellState::Processed;
                                }
                                break;
                            case CellState::Processed:
                                // A set of transient cells leading to an already processed cell
                                processing = false;
                                s = sequence.size();
                                p = z->group;
                                IDType step = z->step;
                                for(size_t j=0; j<s; j++) {
                                    sequence[s-1-j]->group = p;
                                    sequence[s-1-j]->step = step+1+j;
                                    sequence[s-1-j]->cState = CellState::Processed;
                                }
                                break;
                        }
                    }
                }
                else if (z->cState == CellState::Processed) {
                    // Skip the cell (already processed)
                }
            } // end for
            std::cout << "Number of PGs: " << periodicGroups << std::endl;
            // TODO: Remove the following debug check
            size_t state_proc = 0;
            size_t state_underproc = 0;
            size_t state_untouched = 0;
            for (const auto n : IcmGraph) {
                switch (n->cState) {
                    case CellState::Untouched:
                        state_untouched++;
                        break;
                    case CellState::UnderProcessing:
                        state_underproc++;
                        break;
                    case CellState::Processed:
                        state_proc++;
                        break;
                }
            }
            std::cout << "Processed: " << state_proc << std::endl;
            std::cout << "UnderProcessing: " << state_underproc << std::endl;
            std::cout << "Untouched: " << state_untouched << std::endl;
        }
        void exportGroups(std::string filename, IDType dim) {
            std::ofstream out(filename);
            std::string sep = ", ";
            std::cout << "Summary:\n";
            std::cout << "Number of PGs: " << periodicGroups << std::endl;
            std::cout << "PG \tPeriodicity\n";
            for (size_t pg=0; pg<periodicGroups; pg++) {
                std::cout << pg << "\t" << periodicities[pg] << (pg==0?"(Sink)":"") << std::endl;
            }
            for (size_t i=0; i<css.getCellSum(); i++) {
                if ((css.getCell(i).getGroup() > 0) && (css.getCell(i).getStep() == 0)) {
                    StateVectorType cnt = css.getCenter(i);
                    for (size_t j=0; j<dim; j++) out << cnt[j] << sep;
                    out << css.getCell(i).getGroup() << std::endl;
                }
            }
            out.close();
        }
        void exportSolution(std::string filename, IDType dim) {
            std::ofstream out(filename);
            // Format: state vector (length = dim), group number, step number
            std::string sep = ", ";
            for (size_t i=0; i<css.getCellSum(); i++) {
                StateVectorType cnt = css.getCenter(i);
                for (size_t j=0; j<dim; j++) out << cnt[j] << sep;
                out << css.getCell(i).getGroup() << sep << css.getCell(i).getStep() << std::endl;
            }
            out.close();
        }
        std::vector<StateVectorType> getTransients(IDType min_step, IDType max_step, boolean excl_sink = true) {
            std::vector<StateVectorType> TCLS;
            for (size_t i=0; i<css.getCellSum(); i++) {
                if (excl_sink) {
                    if ((css.getCell(i).getGroup() > 0) && (css.getCell(i).getStep() >= min_step) && (css.getCell(i).getStep() <= max_step)) {
                        TCLS.emplace_back(css.getCenter(i));
                    }
                } else {
                    if ((css.getCell(i).getStep() >= min_step) && (css.getCell(i).getStep() <= max_step)) {
                        TCLS.emplace_back(css.getCenter(i));
                    }
                }
            }
            return TCLS;
        }
        std::vector<StateVectorType> getTransients(IDType step_num, boolean excl_sink = true) {
            std::vector<StateVectorType> TCLS;
            for (size_t i=0; i<css.getCellSum(); i++) {
                if (excl_sink) {
                    if ((css.getCell(i).getGroup() > 0) && (css.getCell(i).getStep() == step_num)) {
                        TCLS.emplace_back(css.getCenter(i));
                    }
                } else {
                    if ((css.getCell(i).getStep() == step_num)) {
                        TCLS.emplace_back(css.getCenter(i));
                    }
                }
            }
            return TCLS;
        }
        std::vector<StateVectorType> getPG(IDType id) {
            std::vector<StateVectorType> PG;
            if (id > 0) {
                if (id >= periodicGroups) {
                    std::cout << "The highest valid PG index is " << periodicGroups-1 << std::endl;
                } else {
                    for (size_t i=0; i<css.getCellSum(); i++) {
                        if ((css.getCell(i).getGroup() == id) && (css.getCell(i).getStep() == 0)) {
                            PG.emplace_back(css.getCenter(i));
                        }
                    }
                }
            } else {
                std::cout << "PG 0 is the sink cell!" << std::endl;
            }
            return PG;
        }
        std::vector<StateVectorType> getAllPGs() {
            std::vector<StateVectorType> PG;
            for (size_t i=0; i<css.getCellSum(); i++) {
                if ((css.getCell(i).getGroup() > 0) && (css.getCell(i).getStep() == 0)) {
                    PG.emplace_back(css.getCenter(i));
                }
            }
            return PG;
        }
        void generateImage(std::string filepath, SCMColoringMethod<CellType, IDType>* coloringMethod=nullptr,
                IDType x0=0, IDType y0=0, IDType xw=0, IDType yw=0, IDType mult=1) {
            // Instantiate coloring method if not provided
            SCMDefaultColoring<CellType, IDType> defaultColoring;
            if (coloringMethod == nullptr) {
                coloringMethod = &defaultColoring;
            }
            std::cout << "Generating JPG: " << filepath << std::endl;
            // TODO: Coordinate values for other dimensions!
            // TODO: Interface for accessing a plane from CellStateSpace
            FILE* outfile = fopen(filepath.c_str(), "wb");
            if (outfile == NULL) {
                std::cout << "Failed to open output file: " << filepath << std::endl;
            }
            struct jpeg_compress_struct cinfo;
            struct jpeg_error_mgr       jerr;
            if (xw == 0) xw = css.getCellCounts()[0];
            if (yw == 0) yw = css.getCellCounts()[1];
            size_t components = 3; // RGB
            // TODO: Use only line buffers (generate and save on-the-fly)
            std::vector<char> buffer(xw*yw*components);
            // Initial image buffer contains xw pixels per line (xw*components bytes) and
            // yw lines
            size_t buff_p;
            std::vector<IDType> cellCoord(2);
            IDType id;
            for (size_t i=0; i<yw; i++) {
                for (size_t j=0; j<xw; j++) {
                    cellCoord[0]=x0+j;
                    cellCoord[1]=y0+yw-1-i;
                    id = css.getIDFromCellCoord(cellCoord);
                    CellType cell = css.getCell(id);
                    std::vector<char> rgb = coloringMethod->createColor(cell, periodicGroups);
                    buff_p = (i*xw+j)*components;
                    buffer[buff_p]	= rgb[0];
                    buffer[buff_p+1]= rgb[1];
                    buffer[buff_p+2]= rgb[2];
                }
            }

            size_t dest_p;
            char r, g, b;
            std::vector<char> mult_buffer(xw*yw*components*mult);
            /**
             * multiplied buffer contains xw*mult pixels per line (xw*mult*components bytes)
             * but still yw lines
             */
            for (size_t i=0; i<yw; i++) {
                for (size_t j=0; j<xw; j++) {
                    // source pointer
                    buff_p = (i*xw+j)*components;
                    r = buffer[buff_p];
                    g = buffer[buff_p+1];
                    b = buffer[buff_p+2];
                    for (size_t m=0; m<mult; m++) {
                        dest_p = (i*xw*mult+mult*j+m)*components;
                        mult_buffer[dest_p] = r;
                        mult_buffer[dest_p+1] = g;
                        mult_buffer[dest_p+2] = b;
                    }
                }
            }

            cinfo.err = jpeg_std_error(&jerr);
            jpeg_create_compress(&cinfo);
            jpeg_stdio_dest(&cinfo, outfile);
            cinfo.image_width      = (JDIMENSION) xw*mult;
            cinfo.image_height     = (JDIMENSION) yw*mult;
            cinfo.input_components = (int) components;
            cinfo.in_color_space   = JCS_RGB;

            jpeg_set_defaults(&cinfo);
            jpeg_set_quality (&cinfo, 100, true); // Set the quality [0..100]
            jpeg_start_compress(&cinfo, true);
            JSAMPROW row_pointer;
            size_t write_h = 0;
            while (write_h < yw) {
                row_pointer = (JSAMPROW) &(mult_buffer.data()[write_h * cinfo.image_width * cinfo.input_components]);
                for (size_t m=0; m<mult; m++) { // Write lines m times
                    jpeg_write_scanlines(&cinfo, &row_pointer, 1);
                }
                write_h++;
            }
            jpeg_finish_compress(&cinfo);
        }
        void generateImage2(std::string filepath, SCMColoringMethod<CellType, IDType>* coloringMethod=nullptr) {
            // Pre-render ICMgraph into an array
            size_t expansion = 2;
            IDType xw = css.getCellCounts()[0]*expansion;
            IDType yw = css.getCellCounts()[1]*expansion;
            std::vector<IDType> newGrid{xw, yw};
            UniformCellStateSpace<uint8_t,IDType,StateVectorType> auxCss(css.getCenter(0), css.getWidth(), newGrid);
            size_t components = 3; // RGB
            std::vector<char> buffer(xw*yw*components);
            // Instantiate coloring method if not provided
            SCMDefaultColoring<CellType, IDType> defaultColoring;
            if (coloringMethod == nullptr) {
                coloringMethod = &defaultColoring;
            }
            std::cout << "Generating JPG: " << filepath << std::endl;
            FILE* outfile = fopen(filepath.c_str(), "wb");
            if (outfile == NULL) {
                std::cout << "Failed to open output file: " << filepath << std::endl;
            }
            struct jpeg_compress_struct cinfo;
            struct jpeg_error_mgr       jerr;
            // Initialize the image
            size_t buff_p;
            for (size_t i=0; i<yw; i++) {
                for (size_t j=0; j<xw; j++) {
                    buff_p = (i*xw+j)*components;
                    buffer[buff_p]	= 255u;
                    buffer[buff_p+1]= 255u;
                    buffer[buff_p+2]= 255u;
                }
            }
            std::vector<IDType> indices;
            // Loop through the graph and colour pixels
            size_t out_of_bounds = 0, invalid_indices = 0;
            for (const auto n : IcmGraph) {
                // Look up destination based on state
                auto state = n->state;
                bool ok = auxCss.getCellCoord(state, indices);
                if (ok) {
                    size_t i = indices[1];
                    size_t j = indices[0];
                    if (i < yw && j < xw) {
                        i = (yw-1)-i;
                        CellType cell;
                        cell.setGroup(n->group);
                        cell.setStep(n->step);
                        std::vector<char> rgb = coloringMethod->createColor(cell, periodicGroups);
                        buff_p = (i*xw+j)*components;
                        buffer[buff_p]	= rgb[0];
                        buffer[buff_p+1]= rgb[1];
                        buffer[buff_p+2]= rgb[2];
                        // Also mark cell
                        auto cellID = auxCss.getID(state);
                        auxCss.getCell(cellID) = 1U;
                    } else {
                        invalid_indices++;
                    }
                } else {
                    out_of_bounds++;
                }
            }
            std::cout << "Out of bounds nodes: " << out_of_bounds << std::endl;
            std::cout << "Invalid image indices: " << invalid_indices << std::endl;
            // Gather "gaps" in the rasterized image, and do another round of ICM interpolations starting from them
            size_t gapsInImage = 0;
            graphHoles.resize(0);
            for (IDType c=1; c<auxCss.getCellSum(); c++) {
                if (auxCss.getCell(c) == 0U) {
                    gapsInImage++;
                    graphHoles.push_back(auxCss.getCenter(c));
                }
            }
            std::cout << "Gaps in generated image: " << gapsInImage << std::endl;
            // Write JPEG
            cinfo.err = jpeg_std_error(&jerr);
            jpeg_create_compress(&cinfo);
            jpeg_stdio_dest(&cinfo, outfile);
            cinfo.image_width      = (JDIMENSION) xw;
            cinfo.image_height     = (JDIMENSION) yw;
            cinfo.input_components = (int) components;
            cinfo.in_color_space   = JCS_RGB;
            jpeg_set_defaults(&cinfo);
            jpeg_set_quality (&cinfo, 100, true); // Set the quality [0..100]
            jpeg_start_compress(&cinfo, true);
            JSAMPROW row_pointer;
            size_t write_h = 0;
            while (write_h < yw) {
                row_pointer = (JSAMPROW) &(buffer.data()[write_h * cinfo.image_width * cinfo.input_components]);
                jpeg_write_scanlines(&cinfo, &row_pointer, 1);
                write_h++;
            }
            jpeg_finish_compress(&cinfo);
        }
        void fill_graph(size_t max_steps = 500, double lambda_closure = 0.1, double lambda_join = 0.2) {
            // TODO: Rework the class, so that it uses graphHoles as centers too in build_graph
            // TODO: This function is mostly a copy-paste of the build_graph function currently

            std::cout << "Filling gaps in ICM graph with " << graphHoles.size() << " new trajectories" << std::endl;
            IDType sequence_id = 0;
            double characteristicCellSize = norm(css.getCellWidth());
            double join_distance = lambda_join * characteristicCellSize;
            double periodicity_distance = lambda_closure * characteristicCellSize;
            IDType stats = graphHoles.size()/100;
            for (IDType h = 1; h < graphHoles.size(); h++) {
                // Start new sequence
                sequence_id++;
                if (h % stats == 0) {
                    std::cout << "Progress: " << h << "/" << graphHoles.size() << std::endl;
                    std::cout << "Current graph size: " << IcmGraph.size() << std::endl;
                }
                // Start a sequence starting from hole i
                std::vector<std::shared_ptr<IcmGraphNode<IDType, StateVectorType>>> seq;
                StateVectorType state = graphHoles[h];
                std::shared_ptr<IcmGraphNode<IDType, StateVectorType>> node = std::make_shared<IcmGraphNode<IDType, StateVectorType>>();
                node->state = state;
                node->sequence = sequence_id;
                node->parent = nullptr;
                css.addNode(h, node);
                StateVectorType newState;
                seq.push_back(node);
                bool terminated = false;
                TerminationState terminationState = TerminationState::Unknown; // TODO: Remove
                double distance = 0;
                bool enable_sequence_matching = true;
                // Interpolate path until one of the termination conditions are met
                std::shared_ptr<IcmGraphNode<IDType, StateVectorType>> parent = node;
                std::shared_ptr<IcmGraphNode<IDType, StateVectorType>> Sink = IcmGraph[0]; // TODO: This assumption should be enforced
                for (IDType steps = 0; steps < max_steps; steps++) {
                    if (css.interpolate(state, newState)) {
                        // Create new node for the new State
                        std::shared_ptr<IcmGraphNode<IDType, StateVectorType>> newNode = std::make_shared<IcmGraphNode<IDType, StateVectorType>>();
                        newNode->state = newState;
                        newNode->sequence = sequence_id;
                        newNode->parent = parent;
                        parent->image = newNode;
                        // Tag cells with sequence
                        css.addNode(css.getID(newState), newNode);
                        // Check termination vs current sequence
                        for (const auto& s : seq) {
                            if (norm(newState - s->state) < periodicity_distance) {
                                // Break cycle, mark as periodic
                                terminated = true;
                                terminationState = TerminationState::Periodic;
                                newNode->image = s;
                                // Note: s already has a parent
                            }
                        }
                        // Store into current sequence
                        seq.push_back(newNode);
                        if (!terminated) {
                            // Check termination vs previously marked paths
                            std::shared_ptr<IcmGraphNode<IDType, StateVectorType>> join_node = nullptr;
                            distance = css.getClosestMatch(newState, sequence_id, join_node);
                            if (enable_sequence_matching && (distance <= join_distance)) {
                                terminated = true;
                                terminationState = TerminationState::TransientJoining;
                                newNode->image = join_node;
                            }
                        }
                        // Update state
                        state = newState;
                        parent = newNode;
                        if (terminated) {
                            // Store sequence into graph
                            for (auto n : seq) {
                                IcmGraph.push_back(n);
                            }
                            break;
                        }
                    } else { // Left region / entered sink cell
                        terminated = true;
                        terminationState = TerminationState::Sink;
                        seq.back()->image = Sink;
                        // Store sequence into graph
                        for (auto n : seq) {
                            IcmGraph.push_back(n);
                        }
                        break;
                    }
                }
                if (!terminated) {
                    std::cout << "Maxsteps reached" << std::endl;
                    terminationState = TerminationState::MaxSteps;
                    seq.back()->image = Sink; // TODO: Find a better way. Currently tag with Sink.
                    // Store sequence into graph
                    for (auto n : seq) {
                        IcmGraph.push_back(n);
                    }
                }
            }
            std::cout << "Final expansion rate: " << static_cast<double>(IcmGraph.size())/static_cast<double>(css.getCellSum()) << std::endl;
        }
        DynamicalSystemBase<StateVectorType> *getSystemPointer() const {
            return systemPointer;
        }
        const ICMUniformCellStateSpace<CellType, IDType, StateVectorType> &getCss() const {
            return css;
        }
        ICMUniformCellStateSpace<CellType, IDType, StateVectorType> &getCss() {
            return css;
        }
        IDType getPeriodicGroups() const {
            return periodicGroups;
        }
    };

    template <class StateVectorType>
    using ICM32 = ICM<SCMCell<uint32_t>, uint32_t, StateVectorType>;

    template <class StateVectorType>
    using ICM64 = ICM<SCMCell<uint64_t>, uint64_t, StateVectorType>;

}


#endif //CELL_MAPPING_CPP_ICM_H