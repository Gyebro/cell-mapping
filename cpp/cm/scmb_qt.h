#ifndef CELL_MAPPING_CPP_BLOCKED_SCM_QT_H
#define CELL_MAPPING_CPP_BLOCKED_SCM_QT_H

#include "css.h"
#include "system.h"
#include "coloring.h"
#include <stdio.h>
#include <QImage>

#include <iostream>

namespace cm {

    template <class CellType, class IDType, class StateVectorType>
    class BSCMQt {
    private:
        const IDType ID_SINK_CELL = 0;
        BlockedUniformCellStateSpace<CellType, IDType, StateVectorType> css;
        DynamicalSystemBase<StateVectorType> *systemPointer;
        IDType periodicGroups;
        std::vector<std::vector<IDType>> periodicGroupIDs;
        std::vector<IDType> periodicities;
        std::shared_ptr<std::vector<QImage>> blockResults;
    public:
        BSCMQt(StateVectorType center, StateVectorType width, const std::vector<IDType>& cellCounts,
            DynamicalSystemBase<StateVectorType> *systemPointer) : systemPointer(systemPointer) {
            SCMUniformCellStateSpace<CellType, IDType, StateVectorType> block(center, width, cellCounts);
            css.addBlock(block);
            blockResults = std::make_shared<std::vector<QImage>>();
            blockResults->push_back(QImage(cellCounts[0], cellCounts[1], QImage::Format_RGB32));
            periodicGroups = 0;
            periodicities.resize(0);
            periodicGroupIDs.resize(0);
        }
        size_t addBlock(StateVectorType center, StateVectorType width, const std::vector<IDType>& cellCounts) {
            SCMUniformCellStateSpace<CellType, IDType, StateVectorType> block(center, width, cellCounts);
            css.addBlock(block);
            blockResults->push_back(QImage(cellCounts[0], cellCounts[1], QImage::Format_RGB32));
            return blockResults->size()-1;
        }
        std::shared_ptr<std::vector<QImage>> getResults() {
            return blockResults;
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
        void generateImage(SCMColoringMethod<CellType, IDType>* coloringMethod=nullptr) {
            // Instantiate coloring method if not provided
            SCMDefaultColoring<CellType, IDType> defaultColoring;
            if (coloringMethod == nullptr) {
                coloringMethod = &defaultColoring;
            }
            // Generate multiple images corresponding to each block
            for (IDType b = 0; b<css.getBlockCount(); b++) {
                //std::cout << "Generating QImage: " << b << std::endl;
                size_t xw = css.getBlock(b).getCellCounts()[0];
                size_t yw = css.getBlock(b).getCellCounts()[1];
                size_t x0 = 0; size_t y0 = 0;
                QImage result(xw, yw, QImage::Format_RGB32);
                std::vector<IDType> cellCoord(2);
                IDType id;
                for (size_t i=0; i<yw; i++) {
                    for (size_t j=0; j<xw; j++) {
                        cellCoord[0]=x0+j;
                        cellCoord[1]=y0+yw-1-i;
                        id = css.getBlock(b).getIDFromCellCoord(cellCoord);
                        CellType cell = css.getBlock(b).getCell(id);
                        std::vector<char> rgb = coloringMethod->createColor(cell, periodicGroups);
                        QRgb color = qRgb(rgb[0], rgb[1], rgb[2]);
                        result.setPixel(j, i, color);
                    }
                }
                blockResults->operator[](b) = result;
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
            BSCMQt::periodicGroups = periodicGroups;
        }
    };

}


#endif //CELL_MAPPING_CPP_BLOCKED_SCM_QT_H