//
// Created by Gyebro on 2019-02-18.
//

#ifndef CELL_MAPPING_CPP_SCM_H
#define CELL_MAPPING_CPP_SCM_H

#include "css.h"
#include "system.h"
#include <stdio.h>
#include <jpeg-9a/jpeglib.h>

#include <iostream>

namespace cm {

    template <class CellType, class IDType, class StateVectorType>
    class SCM {
    private:
        SCMUniformCellStateSpace<CellType, IDType, StateVectorType> css;
        DynamicalSystemBase<StateVectorType> *systemPointer;
        IDType periodicGroups;
        std::vector<IDType> periodicities;
        void hsv2rgb(double h, double s, double v, double& r, double& g, double& b) const {
            h *= 6.0; // Span to 360 deg.
            double c = v * s;
            double x = c * (1.0 - fabs(fmod(h, 2.0)-1.0));
            r = g = b = 0.0;
            if( h < 1.0f ) {
                r = c;
                g = x;
            } else if ( h < 2.0f ) {
                g = c;
                r = x;
            } else if ( h < 3.0f ) {
                g = c;
                b = x;
            } else if ( h < 4.0f ) {
                b = c;
                g = x;
            } else if ( h < 5.0f ) {
                b = c;
                r = x;
            } else {
                r = c;
                b = x;
            }
            r += v-c;
            g += v-c;
            b += v-c;
        }
        std::vector<char> createRGBColor(IDType group, IDType step) const {
            double h, s, v;
            h = double(group) / double(periodicGroups + 1);
            s = 0.8; // Constant saturation
            if (step > 0) { /* Transient */
                v = 0.85 - fmin(0.5 * (double(step) / 500.0), 0.5);
            } else {
                /* Periodic cell */
                v=1.0;
                s=0.0; // White
            }
            if (group == 0) {
                // Override for sink cell
                v=1.0;
                s=0.0;
            }
            double r, g, b;
            hsv2rgb(h, s, v, r, g, b);
            std::vector<char> rgb;
            rgb.push_back(r*255);
            rgb.push_back(g*255);
            rgb.push_back(b*255);
            return rgb;
        }
    public:
        SCM(StateVectorType center, StateVectorType width, const std::vector<IDType>& cellCounts,
            DynamicalSystemBase<StateVectorType> *systemPointer) :
            css(center, width, cellCounts), systemPointer(systemPointer) {
            periodicGroups = 0;
            periodicities.resize(0);
        }
        void solve(IDType max_steps = 1) {
            // Calculate images
            std::cout << "Initializing Cell state space with " << css.getCellSum() << " cells\n";
#pragma omp parallel for
            for (IDType i=1; i<css.getCellSum(); i++) {
                IDType steps = 0; IDType image = i;
                StateVectorType imageState = css.getCenter(i);
                while (image == i && steps < max_steps) {
                    imageState = systemPointer->step(imageState);
                    image = css.getID(imageState);
                    steps++;
                }
                css.setImage(i, image);
            }
            // Determine cell evolutions for cells
            IDType z,p,s;
            bool processing = false;
            std::vector<IDType> sequence;
            std::vector<IDType> currentPG;
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
                                //std::cout << "New PG found\n";
                                //currentPG.resize(0);
                                // Set properties for periodic cells,
                                for (size_t j=0; j<p; j++) {
                                    css.setGroup(sequence[s-1-j], periodicGroups-1);
                                    css.setStep(sequence[s-1-j], 0);
                                    css.getCell(sequence[s-1-j]).setState(CellState::Processed);
                                    //currentPG.push_back(sequence[s-1-j]);
                                }
                                // Add current PG to the container
                                //periodicGroupStorage.push_back(currentPG);
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
        void printSummary();
        void generateImage(std::string filepath) {
            std::cout << "Generating JPG: " << filepath << std::endl;
            // TODO: Coordinate values for other dimensions!
            // TODO: Interface for accessing a plane from CellStateSpace
            FILE* outfile = fopen(filepath.c_str(), "wb");
            if (outfile == NULL) {
                std::cout << "Failed to open output file: " << filepath << std::endl;
            }
            struct jpeg_compress_struct cinfo;
            struct jpeg_error_mgr       jerr;
            size_t xpixels = css.getCellCounts()[0];
            size_t ypixels = css.getCellCounts()[1];
            size_t components = 3; // RGB
            // TODO: Use only line buffers (generate and save on-the-fly)
            char *buffer;
            buffer = new char[xpixels*ypixels*components];
            size_t buff_p;
            std::vector<IDType> cellCoord(2);
            IDType id;
            for (size_t i=0; i<ypixels; i++) {
                for (size_t j=0; j<xpixels; j++) {
                    // TODO: Use input dimensions here!!!
                    cellCoord[0]=j;
                    cellCoord[1]=ypixels-1-i;
                    id = css.getIDFromCellCoord(cellCoord);
                    size_t g = css.getGroup(id);
                    size_t s = css.getStep(id);
                    std::vector<char> rgb = createRGBColor(g,s); // TODO: Add coloringmethod
                    buff_p = (i*xpixels+j)*components;
                    buffer[buff_p]	= rgb[0];
                    buffer[buff_p+1]= rgb[1];
                    buffer[buff_p+2]= rgb[2];
                }
            }

            cinfo.err = jpeg_std_error(&jerr);
            jpeg_create_compress(&cinfo);
            jpeg_stdio_dest(&cinfo, outfile);
            cinfo.image_width      = (JDIMENSION) xpixels;
            cinfo.image_height     = (JDIMENSION) ypixels;
            cinfo.input_components = (int) components;
            cinfo.in_color_space   = JCS_RGB;

            jpeg_set_defaults(&cinfo);
            jpeg_set_quality (&cinfo, 100, true); // Set the quality [0..100]
            jpeg_start_compress(&cinfo, true);
            JSAMPROW row_pointer;
            // TODO: Parallelize this too!
            while (cinfo.next_scanline < cinfo.image_height) {
                row_pointer = (JSAMPROW) &buffer[cinfo.next_scanline*components*xpixels];
                jpeg_write_scanlines(&cinfo, &row_pointer, 1);
            }
            jpeg_finish_compress(&cinfo);
            delete[] buffer;
        }
    };

}




#endif //CELL_MAPPING_CPP_SCM_H
