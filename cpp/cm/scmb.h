#ifndef CELL_MAPPING_CPP_BLOCKED_SCM_H
#define CELL_MAPPING_CPP_BLOCKED_SCM_H

#include "css.h"
#include "system.h"
#include "coloring.h"
#include <stdio.h>
#include <jpeg-9a/jpeglib.h>

#include <iostream>

namespace cm {

    template <class CellType, class IDType, class StateVectorType>
    class BSCM {
    private:
        const IDType ID_SINK_CELL = 0;
        BlockedUniformCellStateSpace<CellType, IDType, StateVectorType> css;
        DynamicalSystemBase<StateVectorType> *systemPointer;
        IDType periodicGroups;
        std::vector<std::vector<IDType>> periodicGroupIDs;
        std::vector<IDType> periodicities;
    public:
        BSCM(StateVectorType center, StateVectorType width, const std::vector<IDType>& cellCounts,
            DynamicalSystemBase<StateVectorType> *systemPointer) : systemPointer(systemPointer) {
            SCMUniformCellStateSpace<CellType, IDType, StateVectorType> block(center, width, cellCounts);
            css.addBlock(block);
            periodicGroups = 0;
            periodicities.resize(0);
            periodicGroupIDs.resize(0);
        }
        void addBlock(StateVectorType center, StateVectorType width, const std::vector<IDType>& cellCounts) {
            SCMUniformCellStateSpace<CellType, IDType, StateVectorType> block(center, width, cellCounts);
            css.addBlock(block);
        }
        void solve(IDType max_steps = 1) {
            // Calculate images
            std::cout << "Initializing Cell state space with " << css.getCellSum() << " cells\n";
//TODO: Re-add pragma omp parallel for
            for (IDType i=1; i<css.getCellSum(); i++) {
                IDType steps = 0; IDType image = i;
                StateVectorType imageState = css.getCenter(i);
                css.getCell(i).setState(CellState::Untouched);
                while (image == i && steps < max_steps) {
                    imageState = systemPointer->step(imageState);
                    image = css.getID(imageState);
                    steps++;
                }
                css.setImage(i, image);
            }
            periodicGroups = 0;
            periodicities.resize(0);
            periodicGroupIDs.resize(0);
            // Determine cell evolutions for cells
            IDType z,p,s;
            bool processing;
            std::vector<IDType> sequence;
            std::vector<IDType> newPG;
            // Store the first PG (sink cell)
            newPG.push_back(ID_SINK_CELL);
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
        void printSummary() {
            std::cout << "Summary:\n";
            std::cout << "Number of PGs: " << periodicGroups << std::endl;
            std::cout << "PG \tPeriodicity\n";
            for (size_t pg=0; pg<periodicGroups; pg++) {
                std::cout << pg << "\t" << periodicities[pg] << (pg==0?"(Sink)":"") << std::endl;
            }
        }
        void generateImage(std::string filepath, SCMColoringMethod<CellType, IDType>* coloringMethod=nullptr, bool emphasize = false,
                IDType x0=0, IDType y0=0, IDType xw=0, IDType yw=0) {
            // Instantiate coloring method if not provided
            SCMDefaultColoring<CellType, IDType> defaultColoring;
            if (coloringMethod == nullptr) {
                coloringMethod = &defaultColoring;
            }
            // Generate multiple images corresponding to each block
            for (IDType b = 0; b<css.getBlockCount(); b++) {
                std::string blockfilepath = filepath+"block_"+std::to_string(b)+".jpg";
                std::cout << "Generating JPG: " << blockfilepath << std::endl;
                FILE* outfile = fopen(blockfilepath.c_str(), "wb");
                if (outfile == NULL) {
                    std::cout << "Failed to open output file: " << blockfilepath << std::endl;
                }
                struct jpeg_compress_struct cinfo;
                struct jpeg_error_mgr       jerr;
                if (xw == 0) xw = css.getBlock(b).getCellCounts()[0];
                if (yw == 0) yw = css.getBlock(b).getCellCounts()[1];
                size_t components = 3; // RGB
                std::vector<char> buffer(xw*yw*components);
                size_t buff_p;
                std::vector<IDType> cellCoord(2);
                IDType id;
                for (size_t i=0; i<yw; i++) {
                    for (size_t j=0; j<xw; j++) {
                        cellCoord[0]=x0+j;
                        cellCoord[1]=y0+yw-1-i;
                        id = css.getBlock(b).getIDFromCellCoord(cellCoord);
                        CellType cell = css.getBlock(b).getCell(id);
                        std::vector<char> rgb = coloringMethod->createColor(cell, periodicGroups);
                        buff_p = (i*xw+j)*components;
                        buffer[buff_p]	= rgb[0];
                        buffer[buff_p+1]= rgb[1];
                        buffer[buff_p+2]= rgb[2];
                        if (emphasize && cell.getGroup()>0) {
                            buff_p = ((i+1)*xw+j)*components;
                            if (buff_p+2 < xw*yw*components) {
                                buffer[buff_p] = rgb[0]; buffer[buff_p+1] = rgb[1]; buffer[buff_p+2] = rgb[2];
                            }
                            buff_p = ((i+1)*xw+j+1)*components;
                            if (buff_p+2 < xw*yw*components) {
                                buffer[buff_p] = rgb[0]; buffer[buff_p+1] = rgb[1]; buffer[buff_p+2] = rgb[2];
                            }
                            buff_p = ((i+1)*xw+j-1)*components;
                            if (buff_p+2 < xw*yw*components) {
                                buffer[buff_p] = rgb[0]; buffer[buff_p+1] = rgb[1]; buffer[buff_p+2] = rgb[2];
                            }
                            buff_p = ((i)*xw+j+1)*components;
                            if (buff_p+2 < xw*yw*components) {
                                buffer[buff_p] = rgb[0]; buffer[buff_p+1] = rgb[1]; buffer[buff_p+2] = rgb[2];
                            }
                            buff_p = ((i)*xw+j-1)*components;
                            if (buff_p+2 < xw*yw*components) {
                                buffer[buff_p] = rgb[0]; buffer[buff_p+1] = rgb[1]; buffer[buff_p+2] = rgb[2];
                            }
                            buff_p = ((i-1)*xw+j)*components;
                            if (buff_p+2 < xw*yw*components) {
                                buffer[buff_p] = rgb[0]; buffer[buff_p+1] = rgb[1]; buffer[buff_p+2] = rgb[2];
                            }
                            buff_p = ((i-1)*xw+j+1)*components;
                            if (buff_p+2 < xw*yw*components) {
                                buffer[buff_p] = rgb[0]; buffer[buff_p+1] = rgb[1]; buffer[buff_p+2] = rgb[2];
                            }
                            buff_p = ((i-1)*xw+j-1)*components;
                            if (buff_p+2 < xw*yw*components) {
                                buffer[buff_p] = rgb[0]; buffer[buff_p+1] = rgb[1]; buffer[buff_p+2] = rgb[2];
                            }
                        }
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
            BSCM::periodicGroups = periodicGroups;
        }
    };

    template <class StateVectorType>
    using SCMB32 = BSCM<SCMCell<uint32_t>, uint32_t, StateVectorType>;

    template <class StateVectorType>
    using SCMB64 = BSCM<SCMCell<uint64_t>, uint64_t, StateVectorType>;

}


#endif //CELL_MAPPING_CPP_BLOCKED_SCM_H