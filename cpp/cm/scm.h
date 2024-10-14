#ifndef CELL_MAPPING_CPP_SCM_H
#define CELL_MAPPING_CPP_SCM_H

#include "css.h"
#include "system.h"
#include "coloring.h"
#include <stdio.h>
#include <jpeg-9a/jpeglib.h>
#include <fstream>

#include <iostream>

namespace cm {

    template <class CellType, class IDType, class StateVectorType>
    class SCM {
    private:
        SCMUniformCellStateSpace<CellType, IDType, StateVectorType> css;
        DynamicalSystemBase<StateVectorType> *systemPointer;
        IDType periodicGroups;
        std::vector<std::vector<IDType>> periodicGroupIDs;
        std::vector<IDType> periodicities;
    public:
        SCM(StateVectorType center, StateVectorType width, const std::vector<IDType>& cellCounts,
            DynamicalSystemBase<StateVectorType> *systemPointer) :
            css(center, width, cellCounts), systemPointer(systemPointer) {
            periodicGroups = 0;
            periodicities.resize(0);
            periodicGroupIDs.resize(0);
        }
        void solve(IDType max_steps = 1) {
            // Calculate images
            std::cout << "Initializing Cell state space with " << css.getCellSum() << " cells\n";
#pragma omp parallel for
            for (IDType i=1; i<css.getCellSum(); i++) {
                IDType steps = 0; IDType image = i;
                StateVectorType imageState = css.getCenter(i);
                css.getCell(i).setState(CellState::Untouched);
                while (image == i && steps < max_steps) {
                    imageState = systemPointer->step(imageState);
                    image = css.getID(imageState);
                    if (image > css.getCellSum()) {
                        // TODO: Related to #1
                        std::cout << "Error, invalid image ID\n";
                    }
                    steps++;
                }
                css.setImage(i, image);
            }
            std::cout << "Solving SCM on the state space...\n";
            periodicGroups = 0;
            periodicities.resize(0);
            periodicGroupIDs.resize(0);
            // Determine cell evolutions for cells
            IDType z,p,s;
            bool processing;
            std::vector<IDType> sequence;
            std::vector<IDType> newPG;
            // Store the first PG (sink cell)
            newPG.push_back(0);
            periodicGroupIDs.push_back(newPG);
            for (IDType i = 0; i < css.getCellSum(); i++) {
                z = i;
                if (css.getCell(z).getState() == CellState::Untouched) {
                    css.getCell(z).setState(CellState::UnderProcessing);
                    processing = true;
                    sequence.resize(0);
                    sequence.push_back(z);
                    // Start processing sequence for i
                    while (processing) {
                        z = css.getImage(z);
                        switch (css.getCell(z).getState()) {
                            case CellState::Untouched:
                                // Mark cell as under processing, store in the sequence then continue
                                css.getCell(z).setState(CellState::UnderProcessing);
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
                                periodicGroups++;						// Increase group counter
                                newPG.resize(0);
                                // Set properties for periodic cells,
                                for (size_t j=0; j<p; j++) {
                                    css.setGroup(sequence[s-1-j], periodicGroups-1);
                                    css.setStep(sequence[s-1-j], 0);
                                    css.getCell(sequence[s-1-j]).setState(CellState::Processed);
                                    newPG.push_back(sequence[s-1-j]);
                                }
                                // Add current PG to the container
                                periodicGroupIDs.push_back(newPG);
                                // Set properties for transient cells
                                for (size_t j=p; j<s; j++) {
                                    css.setGroup(sequence[s-1-j], periodicGroups-1);
                                    css.setStep(sequence[s-1-j], j-p+1);
                                    css.getCell(sequence[s-1-j]).setState(CellState::Processed);
                                }
                                break;
                            case CellState::Processed:
                                // A set of transient cells leading to an already processed cell
                                processing = false;
                                s = sequence.size();
                                p = css.getGroup(z);
                                IDType step = css.getStep(z);
                                for(size_t j=0; j<s; j++) {
                                    css.setGroup(sequence[s-1-j], p);
                                    css.setStep(sequence[s-1-j], step+1+j);
                                    css.getCell(sequence[s-1-j]).setState(CellState::Processed);
                                }
                                break;
                        }
                    }
                }
                else if (css.getCell(z).getState() == CellState::Processed) {
                    // Skip the cell (already processed)
                }
            } // end for
            std::cout << "Number of PGs: " << periodicGroups << std::endl;
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
        void generateImageProjection(std::string filepath, SCMColoringMethod<CellType, IDType>* coloringMethod=nullptr,
                           IDType idX=0, IDType idY=1, IDType idZ=2,
                           IDType x0=0, IDType y0=0, IDType xw=0, IDType yw=0) {
            // Instantiate coloring method if not provided
            SCMDefaultColoring<CellType, IDType> defaultColoring;
            if (coloringMethod == nullptr) {
                coloringMethod = &defaultColoring;
            }
            std::cout << "Generating JPG: " << filepath << std::endl;
            // Accessing a plane from CellStateSpace
            FILE* outfile = fopen(filepath.c_str(), "wb");
            if (outfile == NULL) {
                std::cout << "Failed to open output file: " << filepath << std::endl;
            }
            struct jpeg_compress_struct cinfo;
            struct jpeg_error_mgr       jerr;
            if (xw == 0) xw = css.getCellCounts()[idX];
            if (yw == 0) yw = css.getCellCounts()[idY];
            IDType zw = css.getCellCounts()[idZ];
            size_t components = 3; // RGB
            // TODO: Use only line buffers (generate and save on-the-fly)
            std::vector<char> buffer(xw*yw*components);
            size_t buff_p;
            std::vector<IDType> cellCoord(3);
            IDType id;
            for (size_t i=0; i<yw; i++) {
                for (size_t j=0; j<xw; j++) {
                    std::vector<char> rgbFinal(3);
                    rgbFinal[0] = 0;
                    rgbFinal[1] = 0;
                    rgbFinal[2] = 0;
                    for (size_t k=0; k<zw; k++) {
                        cellCoord[idX]=x0+j;
                        cellCoord[idY]=y0+yw-1-i;
                        cellCoord[idZ]=k;
                        id = css.getIDFromCellCoord(cellCoord);
                        CellType cell = css.getCell(id);
                        std::vector<char> rgb = coloringMethod->createColor(cell, periodicGroups);
                        // TODO: Export blending function!
                        rgbFinal[0] = std::max(rgbFinal[0], rgb[0]);
                        rgbFinal[1] = std::max(rgbFinal[1], rgb[1]);
                        rgbFinal[2] = std::max(rgbFinal[2], rgb[2]);
                    }
                    buff_p = (i*xw+j)*components;
                    buffer[buff_p]	= rgbFinal[0];
                    buffer[buff_p+1]= rgbFinal[1];
                    buffer[buff_p+2]= rgbFinal[2];
                }
            }
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
            while (cinfo.next_scanline < cinfo.image_height) {
                row_pointer = (JSAMPROW) &(buffer.data()[cinfo.next_scanline*components*xw]);
                jpeg_write_scanlines(&cinfo, &row_pointer, 1);
            }
            jpeg_finish_compress(&cinfo);
        }
        void generateImageSlice(std::string filepath, SCMColoringMethod<CellType, IDType>* coloringMethod=nullptr,
                                     IDType idZcell=0,
                                     IDType idX=0, IDType idY=1, IDType idZ=2,
                                     IDType x0=0, IDType y0=0, IDType xw=0, IDType yw=0) {
            // Instantiate coloring method if not provided
            SCMDefaultColoring<CellType, IDType> defaultColoring;
            if (coloringMethod == nullptr) {
                coloringMethod = &defaultColoring;
            }
            std::cout << "Generating JPG: " << filepath << std::endl;
            // Accessing a plane from CellStateSpace
            FILE* outfile = fopen(filepath.c_str(), "wb");
            if (outfile == NULL) {
                std::cout << "Failed to open output file: " << filepath << std::endl;
            }
            struct jpeg_compress_struct cinfo;
            struct jpeg_error_mgr       jerr;
            if (xw == 0) xw = css.getCellCounts()[idX];
            if (yw == 0) yw = css.getCellCounts()[idY];
            IDType zw = css.getCellCounts()[idZ];
            size_t components = 3; // RGB
            // TODO: Use only line buffers (generate and save on-the-fly)
            std::vector<char> buffer(xw*yw*components);
            size_t buff_p;
            std::vector<IDType> cellCoord(3);
            IDType id;
            for (size_t i=0; i<yw; i++) {
                for (size_t j=0; j<xw; j++) {
                    cellCoord[idX]=x0+j;
                    cellCoord[idY]=y0+yw-1-i;
                    cellCoord[idZ]=idZcell;
                    // TODO: getIDFromCellCoord is unsafe outside!
                    id = css.getIDFromCellCoord(cellCoord);
                    CellType cell = css.getCell(id);
                    std::vector<char> rgb = coloringMethod->createColor(cell, periodicGroups);
                    buff_p = (i*xw+j)*components;
                    buffer[buff_p]	= rgb[0];
                    buffer[buff_p+1]= rgb[1];
                    buffer[buff_p+2]= rgb[2];
                }
            }
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
            while (cinfo.next_scanline < cinfo.image_height) {
                row_pointer = (JSAMPROW) &(buffer.data()[cinfo.next_scanline*components*xw]);
                jpeg_write_scanlines(&cinfo, &row_pointer, 1);
            }
            jpeg_finish_compress(&cinfo);
        }
        DynamicalSystemBase<StateVectorType> *getSystemPointer() const {
            return systemPointer;
        }
        const SCMUniformCellStateSpace<CellType, IDType, StateVectorType> &getCss() const {
            return css;
        }
        SCMUniformCellStateSpace<CellType, IDType, StateVectorType> &getCss() {
            return css;
        }
        IDType getPeriodicGroups() const {
            return periodicGroups;
        }
        void setPeriodicGroups(IDType periodicGroups) {
            SCM::periodicGroups = periodicGroups;
        }
    };

    template <class StateVectorType>
    using SCM32 = SCM<SCMCell<uint32_t>, uint32_t, StateVectorType>;

    template <class StateVectorType>
    using SCM64 = SCM<SCMCell<uint64_t>, uint64_t, StateVectorType>;

}


#endif //CELL_MAPPING_CPP_SCM_H