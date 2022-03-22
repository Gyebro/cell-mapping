#ifndef CELL_MAPPING_CPP_CSCM_H
#define CELL_MAPPING_CPP_CSCM_H

#include "scm.h"

namespace cm {

    template <class IDType>
    class CellTree {
    private:
        IDType cmid;      /// Cell mapping ID of the SCM, which this path belongs to
        std::vector<IDType> cells;
        CellState state;
        IDType imageCell; /// This image always belongs to the another-cmid SCM solution
        IDType imageCmid; // TODO: Rename CMID to ClasterID
    public:
        CellTree() {
            state = CellState::Untouched;
            cmid = 0;
            imageCell = 0;
        }
        CellTree(IDType cmid,
                 const std::vector<IDType> &cells,
                 const CellState &state,
                 IDType imageCell) : cmid(cmid), cells(cells), state(state), imageCell(imageCell) {
            /* Nothing to do yet */
        }
        CellTree(IDType cmid,
                 const std::vector<IDType> &cells,
                 const CellState &state) : cmid(cmid), cells(cells), state(state) {
            /* Nothing to do yet */
        }
        void setClusterID(IDType cmid) {
            CellTree::cmid = cmid;
        }
        void setCells(const std::vector<IDType> &cells) {
            CellTree::cells = cells;
        }
        void setState(const CellState &state) {
            CellTree::state = state;
        }
        IDType getClusterID() const {
            return cmid;
        }
        const std::vector<IDType> &getCells() const {
            return cells;
        }
        const CellState &getState() const {
            return state;
        }
        IDType getImageCell() const {
            return imageCell;
        }
        void setImageCell(IDType imageCell) {
            CellTree::imageCell = imageCell;
        }
        IDType getImageCmid() const {
            return imageCmid;
        }
        void setImageCmid(IDType imageCmid) {
            CellTree::imageCmid = imageCmid;
        }
    };

    template <class CellType, class IDType, class StateVectorType>
    class ClusteredSCM {
    private:
        SCM<CellType, IDType, StateVectorType>* scm1;
        SCM<CellType, IDType, StateVectorType>* scm2;
        std::vector<CellTree<IDType>> cellTrees;
        void join_stage1(DynamicalSystemBase<StateVectorType>* systemp,
                         SCM<CellType, IDType, StateVectorType>* scm1p,
                         SCM<CellType, IDType, StateVectorType>* scm2p,
                         std::vector<IDType>& sinkDoA1) {
            /**
             * STAGE 1:
             *  - Enumerate all cellTrees in the sink DoAs,
             *  - Tag those cellTrees processed, which lead to
             *    - the SINK, or
             *    - to the other SCM's non-0 group number PG/TG
             *  - Enumerate cellTrees which need to be investigated in Stage 2
             */
            IDType cmid1 = scm1p->getCss().getCell(0).getClusterID();
            IDType cmid2 = scm2p->getCss().getCell(0).getClusterID();
            IDType z; IDType imz; IDType cmidz;
            std::vector<IDType> seq;
            CellTree<IDType> cellTree;
            for(size_t i=0; i<sinkDoA1.size(); i++) {
                seq.resize(0);
                z = sinkDoA1[i];
                // Skip cell if already processed
                CellState stz = scm1p->getCss().getCell(z).getState();
                if (stz == CellState::Processed || stz == CellState::UnderProcessing) {
                    // skip cell
                } else {
                    // State is UNTOUCHED
                    seq.push_back(z);
                    // Generate sequence from this cell until it leaves its own SCM's state space
                    bool left = false;
                    while(!left) {
                        imz = scm1p->getCss().getCell(z).getImage();
                        if (imz != 0) {
                            // Image is not the sink cell, we are still in the original CSS
                            cmidz = scm1p->getCss().getCell(imz).getClusterID();
                            if (cmidz == cmid1) {
                                // Check for cell state before accumulating cellTrees
                                CellState stimz = scm1p->getCss().getCell(imz).getState();
                                if(stimz == CellState::UnderProcessing) {
                                    left = true;
                                    // Get the index of the celltree which contains the touched cell
                                    size_t ct = scm1p->getCss().getCell(imz).getCellTreeID();
                                    // Tag cells
                                    for (size_t k = 0; k < seq.size(); k++) {
                                        if (scm1p->getCss().getCell(seq[k]).getState() == CellState::UnderProcessing) {
                                            //cout << "ERROR while appending!\n";
                                        }
                                        scm1p->getCss().getCell(seq[k]).setState(CellState::UnderProcessing);
                                        scm1p->getCss().getCell(seq[k]).setCellTreeID(ct);
                                    }
                                    // Append current seq to that cellpath
                                    std::vector<IDType> cpcells = cellTrees[ct].getCells();
                                    for (size_t w=0; w<cpcells.size(); w++) {
                                        seq.push_back(cpcells[w]);
                                    }
                                    // Now the original seq contains the current seq plus the cells of the touched seq
                                    // Overwrite cells of that seq
                                    cellTrees[ct].setCells(seq);
                                } else if (stimz == CellState::Processed) {
                                    // Set properties of the current cycle
                                    left = true;
                                    size_t otherG = scm1p->getCss().getCell(imz).getGroup();
                                    size_t otherCmid = scm1p->getCss().getCell(imz).getClusterID();
                                    for (size_t k = 0; k < seq.size(); k++) {
                                        scm1p->getCss().getCell(seq[k]).setClusterID(otherCmid);
                                        scm1p->getCss().getCell(seq[k]).setGroup(otherG);
                                        scm1p->getCss().getCell(seq[k]).setState(CellState::Processed);
                                    }
                                } else {
                                    // Add cell to seq
                                    seq.push_back(imz);
                                    z = imz;
                                }
                            } else {
                                // We are still in the original CSS, but touching a transiting seq
                                left = true;
                                // Set properties of the current seq as the target
                                size_t otherG = scm1p->getCss().getCell(imz).getGroup();
                                size_t otherCmid = scm1p->getCss().getCell(imz).getClusterID();
                                for (size_t k = 0; k < seq.size(); k++) {
                                    scm1p->getCss().getCell(seq[k]).setClusterID(otherCmid);
                                    scm1p->getCss().getCell(seq[k]).setGroup(otherG);
                                    scm1p->getCss().getCell(seq[k]).setState(CellState::Processed);
                                }
                                // Terminating current processing cycle
                            } // end if imz != 0
                        } else {
                            // We have left the original CSS
                            left = true;
                            // Check whether this state is in the other SCM's state space
                            StateVectorType center = systemp->step(scm1p->getCss().getCenter(z));
                            imz = scm2p->getCss().getID(center);
                            if (imz != 0) {
                                // The target is a regular cell of the other CSS, get its group number
                                size_t otherG = scm2p->getCss().getCell(imz).getGroup();
                                if (otherG != 0) {
                                    // Tag cells and terminate current processing cycle
                                    for (size_t k = 0; k < seq.size(); k++) {
                                        scm1p->getCss().getCell(seq[k]).setClusterID(cmid2);
                                        scm1p->getCss().getCell(seq[k]).setGroup(otherG);
                                        scm1p->getCss().getCell(seq[k]).setState(CellState::Processed);
                                    }
                                } else {
                                    // Tag cells and Save this tree for later analysis, terminate proc. cycle
                                    // Also set cellTreeIndex of cells in this sequence
                                    size_t ctIndex = cellTrees.size();
                                    for (size_t k = 0; k < seq.size(); k++) {
                                        if (scm1p->getCss().getCell(seq[k]).getState() == CellState::UnderProcessing) {
                                            //cout << "ERROR while saving cycle!\n";
                                        }
                                        scm1p->getCss().getCell(seq[k]).setState(CellState::UnderProcessing);
                                        scm1p->getCss().getCell(seq[k]).setCellTreeID(ctIndex);
                                    }
                                    // Set cell tree properties
                                    cellTree.setCells(seq);
                                    cellTree.setClusterID(cmid1);
                                    cellTree.setState(CellState::UnderProcessing);
                                    cellTree.setImageCell(imz);
                                    cellTree.setImageCmid(cmid2);
                                    cellTrees.push_back(cellTree);
                                }
                            } else {
                                // This seq leads to the (real) sink cell, tag cells, terminate proc. cycle
                                for (size_t k = 0; k < seq.size(); k++) {
                                    // Since the group number was 0, it is not necessary to overwrite
                                    scm1p->getCss().getCell(seq[k]).setState(CellState::Processed);
                                }
                            }
                        } /* end if */
                    } /* end while */
                } /* end if already processed or under_processing */
            } /* end for */
        }
        void join_stage2(SCM<CellType, IDType, StateVectorType>* scm1p,
                         SCM<CellType, IDType, StateVectorType>* scm2p) {
            IDType cmid1 = scm1p->getCss().getCell(0).getClusterID();
            IDType cmid2 = scm2p->getCss().getCell(0).getClusterID();
            /*
             * STAGE 2: Do an SCM on the cellTrees, their state should be currently UNTOUCHED
             */
            for (size_t i=0; i<cellTrees.size(); i++) {
                CellState cs = cellTrees[i].getState();
                // Select i-th cellTree, check its state
                if (cs == CellState::Processed) {
                    // skip
                } else if (cs == CellState::Untouched) {
                    // Start processing cycle
                    //cout << "Processing cycle at cellTree: " << i << endl;
                    std::vector<size_t> cycle;
                    cycle.push_back(i);
                    cellTrees[i].setState(CellState::UnderProcessing);
                    size_t cpImage = i;
                    CellState cpImageState;
                    bool processing = true;
                    while (processing) {
                        // Before continuing the processing cycle, check if this cellpath leads to an already processed cell
                        size_t cpImageCell = cellTrees[cpImage].getImageCell();
                        size_t cpImageCmid = cellTrees[cpImage].getImageCmid();
                        CellState imageCellState;
                        size_t imageCellGroup;
                        // TODO: Rework this part
                        if (cpImageCmid == cmid1) {
                            imageCellState = scm1->getCss().getCell(cpImageCell).getState();
                            imageCellGroup = scm1->getCss().getCell(cpImageCell).getGroup();
                        } else {
                            imageCellState = scm2->getCss().getCell(cpImageCell).getState();
                            imageCellGroup = scm2->getCss().getCell(cpImageCell).getGroup();
                        }
                        if (imageCellState == CellState::Processed) {
                            // Tag current cell paths in cycle as the last processed image
                            // Tag internal cells (in paths) as well
                            for (size_t k=0; k<cycle.size(); k++) {
                                cellTrees[cycle[k]].setState(CellState::Processed);
                                std::vector<IDType> cellsk = cellTrees[cycle[k]].getCells();
                                if(cellTrees[cycle[k]].getClusterID() == cmid1) {
                                    for (size_t ck=0; ck<cellsk.size(); ck++) {
                                        scm1->getCss().getCell(cellsk[ck]).setGroup(imageCellGroup);
                                        scm1->getCss().getCell(cellsk[ck]).setState(CellState::Processed);
                                        scm1->getCss().getCell(cellsk[ck]).setClusterID(cmid1);
                                    }
                                } else {
                                    for (size_t ck=0; ck<cellsk.size(); ck++) {
                                        scm2->getCss().getCell(cellsk[ck]).setGroup(imageCellGroup);
                                        scm2->getCss().getCell(cellsk[ck]).setState(CellState::Processed);
                                        scm2->getCss().getCell(cellsk[ck]).setClusterID(cmid1);
                                    }
                                }
                            }
                            processing = false;
                            //cout << "Known destination (cell) found, terminating cycle at cellTree: " << i << endl;
                        } else {
                            // get the cellpath image index
                            cpImage = cellPathImage(cellTrees, cpImage);
                            // get cellpath images state
                            cpImageState = cellTrees[cpImage].getState();
                            if (cpImageState == CellState::Untouched) {
                                // append current cell seq to the cycle, and also tag it as UNDER_PROCESSING
                                cellTrees[cpImage].setState(CellState::UnderProcessing);
                                cycle.push_back(cpImage);
                                //cout << "Cycle size: " << cycle.size() << endl;
                            } else if (cpImageState == CellState::Processed) {
                                // TODO: Find the group number of image
                                size_t imageGroup = 1;
                                // Tag current cell paths in cycle as the last processed image
                                // Tag internal cells (in paths) as well
                                for (size_t k=0; k<cycle.size(); k++) {
                                    cellTrees[cycle[k]].setState(CellState::Processed);
                                    std::vector<IDType> cellsk = cellTrees[cycle[k]].getCells();
                                    if(cellTrees[cycle[k]].getClusterID() == cmid1) {
                                        for (size_t ck=0; ck<cellsk.size(); ck++) {
                                            scm1->getCss().getCell(cellsk[ck]).setGroup(imageGroup);
                                            scm1->getCss().getCell(cellsk[ck]).setState(CellState::Processed);
                                        }
                                    } else {
                                        for (size_t ck=0; ck<cellsk.size(); ck++) {
                                            scm2->getCss().getCell(cellsk[ck]).setGroup(imageGroup);
                                            scm2->getCss().getCell(cellsk[ck]).setState(CellState::Processed);
                                        }
                                    }
                                }
                                processing = false;
                                //cout << "Known destination (cellpath) found, terminating cycle at cellTree: " << i << endl;
                            } else if (cpImageState == CellState::UnderProcessing) {
                                // new periodic group found, tag all cells in the cycle
                                // TODO: Create new group number for clusterID1
                                size_t newGroup = scm1->getPeriodicGroups() + 1;
                                scm1->setPeriodicGroups(newGroup);
                                // Tag internal cells (in paths) as well, also set cmid to cmid1
                                for (size_t k=0; k<cycle.size(); k++) {
                                    cellTrees[cycle[k]].setState(CellState::Processed);
                                    std::vector<IDType> cellsk = cellTrees[cycle[k]].getCells();
                                    if(cellTrees[cycle[k]].getClusterID() == cmid1) {
                                        for (size_t ck=0; ck<cellsk.size(); ck++) {
                                            scm1->getCss().getCell(cellsk[ck]).setGroup(newGroup);
                                            scm1->getCss().getCell(cellsk[ck]).setState(CellState::Processed);
                                            scm1->getCss().getCell(cellsk[ck]).setClusterID(cmid1);
                                        }
                                    } else {
                                        for (size_t ck=0; ck<cellsk.size(); ck++) {
                                            scm2->getCss().getCell(cellsk[ck]).setGroup(newGroup);
                                            scm2->getCss().getCell(cellsk[ck]).setState(CellState::Processed);
                                            scm2->getCss().getCell(cellsk[ck]).setClusterID(cmid1); // To have the same color
                                        }
                                    }
                                }
                                processing = false;
                                //std::cout << "New " << cycle.size() <<" PG found, terminating cycle at cellTree: " << i << std::endl;
                            }
                        }
                    }
                } else {
                    std::cout << "ERROR: CellTree with UNDER_PROCESSING state found!\n";
                    // This should not happen
                }
            }
        }
    public:
        ClusteredSCM(SCM<CellType, IDType, StateVectorType>* scm1p,
                     SCM<CellType, IDType, StateVectorType>* scm2p) {
            ClusteredSCM::scm1 = scm1p;
            ClusteredSCM::scm2 = scm2p;
        }
        /**
         * Finds the index of the image of a CellTree within the container cellPaths
         */
        size_t cellPathImage(std::vector<CellTree<IDType>>& cellPaths, size_t cellPathId) {
            IDType cellPathImageCell = cellPaths[cellPathId].getImageCell();
            IDType cellPathImageCmid = cellPaths[cellPathId].getImageCmid();
            // Loop on cellPaths and try to find the image
            IDType targetCellPath = cellPaths.size();
            std::vector<IDType> cells;
            for (IDType cp = 0; cp < cellPaths.size(); cp++) {
                if (cellPaths[cp].getClusterID() == cellPathImageCmid) {
                    cells = cellPaths[cp].getCells();
                    for (IDType cpc = 0; cpc < cells.size(); cpc++) {
                        if (cells[cpc] == cellPathImageCell) {
                            targetCellPath = cp;
                            break;
                        }
                    }
                }
                if (targetCellPath != cellPaths.size()) { break; }
            }
            if(targetCellPath == cellPaths.size()) {
                //cout << "ERROR: Cell path image not found!\n";
            }
            return targetCellPath;
        }
        /**
         * Joins the two SCM solutions
         */
        void join(bool verbose = false) {
            /* TODO: Check overlap between the two SCM state spaces, raise error
             * TODO: Check whether the two systems are the same!
             */
            // Get the DoA of sink cell for both SCMs
            std::vector<IDType> sinkDoA1;
            std::vector<IDType> sinkDoA2;
            if (verbose) std::cout << "\nInitialization: " << std::endl;
            size_t scm1count = scm1->getCss().getCellSum();
            size_t scm2count = scm2->getCss().getCellSum();
            IDType cmid1 = scm1->getCss().getCell(0).getClusterID();
            IDType cmid2 = scm2->getCss().getCell(0).getClusterID();
            ClusteredSCMDefaultColoring<CellType, IDType> coloringMethod;
            if (cmid2 == cmid1) {
                cmid2++;
        #pragma omp parallel for
                for(size_t i=0; i<scm2count; i++) {
                    scm2->getCss().getCell(i).setClusterID(cmid2);
                }
            }
            // TODO: Check if two SCMs use the same dynamical system!
            DynamicalSystemBase<StateVectorType>* systemp = scm1->getSystemPointer();
            for(size_t i=1; i<scm1count; i++) {
                if (scm1->getCss().getCell(i).getGroup() == 0) {
                    sinkDoA1.push_back(i);
                    scm1->getCss().getCell(i).setState(CellState::Untouched);
                }
            }
            for(size_t i=1; i<scm2count; i++) {
                if (scm2->getCss().getCell(i).getGroup() == 0) {
                    sinkDoA2.push_back(i);
                    scm2->getCss().getCell(i).setState(CellState::Untouched);
                }
            }

            size_t sinkCount = sinkDoA1.size() + sinkDoA2.size();
            if (verbose) {
                std::cout << " SCM1's sink cell DoA contains: " << sinkDoA1.size() << " cells." << std::endl;
                std::cout << " SCM2's sink cell DoA contains: " << sinkDoA2.size() << " cells." << std::endl;
                std::cout << " Total number of cells in the joining procedure: " << sinkCount << std::endl;
                //scm1->generateImage("scm1_st0.jpg", &coloringMethod);
                //scm2->generateImage("scm2_st0.jpg", &coloringMethod);
                std::cout << "\nStage 1:" << std::endl;
            }

            cellTrees.resize(0);
            join_stage1(systemp, scm1, scm2, sinkDoA1);
            join_stage1(systemp, scm2, scm1, sinkDoA2);
            IDType pathsum = 0;
            IDType cellStates[3];
            if(verbose) {
                cellStates[0] = 0;
                cellStates[1] = 0;
                cellStates[2] = 0;
                for (IDType i = 0; i < sinkDoA1.size(); i++) {
                    cellStates[(int) scm1->getCss().getCell(sinkDoA1[i]).getState()]++;
                }
                for (IDType i = 0; i < sinkDoA2.size(); i++) {
                    cellStates[(int) scm2->getCss().getCell(sinkDoA2[i]).getState()]++;
                }
                //std::cout << "Cell States:\n";
                //std::cout << " Untouched:\t\t"      << cellStates[(int) CellState::Untouched] << std::endl;
                //std::cout << " UnderProcessing:\t"  << cellStates[(int) CellState::UnderProcessing] << std::endl;
                //std::cout << " Processed:\t\t"      << cellStates[(int) CellState::Processed] << std::endl;
                //std::cout << (cellStates[0] + cellStates[1] + cellStates[2]) << "/" << sinkCount << "\n";
                std::cout << " Processed " << cellStates[(int) CellState::Processed] << " cells (";
                std::cout << double(cellStates[(int) CellState::Processed])/sinkCount*100.0 << "%)" << std::endl;
            }
            // List seq statistics, also reset their state to UNTOUCHED
            pathsum = 0;
            for (IDType p=0; p < cellTrees.size(); p++) {
                //std::cout << "Path " << p << ": clusterID = " << cellTrees[p].getClusterID() << ", cells: " << cellTrees[p].getCells().size() << std::endl;
                pathsum += cellTrees[p].getCells().size();
                cellTrees[p].setState(CellState::Untouched);
            }
            if (verbose) {
                std::cout << " Cell trees constructed: " << cellTrees.size() << std::endl;
                std::cout << " Number of cells in cell trees: " << pathsum << std::endl;
                //scm1->generateImage("scm1_st1.jpg", &coloringMethod);
                //scm2->generateImage("scm2_st1.jpg", &coloringMethod);
                std::cout << "\nStage 2:" << std::endl;
            }

            join_stage2(scm1, scm2);

            if (verbose) {
                cellStates[0] = 0;
                cellStates[1] = 0;
                cellStates[2] = 0;
                for (size_t i = 0; i < sinkDoA1.size(); i++) {
                    cellStates[(int) scm1->getCss().getCell(sinkDoA1[i]).getState()]++;
                }
                for (size_t i = 0; i < sinkDoA2.size(); i++) {
                    cellStates[(int) scm2->getCss().getCell(sinkDoA2[i]).getState()]++;
                }
                size_t processedPaths = 0;
                for (size_t p=0; p < cellTrees.size(); p++) {
                    if(cellTrees[p].getState() == CellState::Processed) {processedPaths++;}
                }
                //std::cout << "Cell States:\n";
                //std::cout << " Untouched:\t\t"      << cellStates[(int) CellState::Untouched] << std::endl;
                //std::cout << " UnderProcessing:\t" << cellStates[(int) CellState::UnderProcessing] << std::endl;
                //std::cout << " Processed:\t\t"      << cellStates[(int) CellState::Processed] << std::endl;
                //std::cout << (cellStates[0] + cellStates[1] + cellStates[2]) << "/" << sinkCount << "\n";
                std::cout << " Processed " << cellStates[(int) CellState::Processed] << " cells (";
                std::cout << double(cellStates[(int) CellState::Processed])/sinkCount*100.0 << "%)" << std::endl;
                std::cout << " Processed cell trees: " << processedPaths << std::endl;
            }

            // Shift SCM2's group numbers
            IDType groupshift = scm1->getPeriodicGroups();
            IDType totalgroups = scm1->getPeriodicGroups() + scm2->getPeriodicGroups();
            scm1->setPeriodicGroups(totalgroups);
            scm2->setPeriodicGroups(totalgroups);
        #pragma omp parallel for
            // TODO: getCellSum should be N+1
            for (IDType z=1; z<scm1->getCss().getCellSum(); z++) {
                if (scm1->getCss().getCell(z).getClusterID() == cmid2 && scm1->getCss().getCell(z).getGroup() != 0) {
                    scm1->getCss().getCell(z).setGroup(scm1->getCss().getCell(z).getGroup()+groupshift);
                }
            }
        #pragma omp parallel for
            for (IDType z=1; z<scm2->getCss().getCellSum(); z++) {
                if (scm2->getCss().getCell(z).getClusterID() == cmid2 && scm2->getCss().getCell(z).getGroup() != 0) {
                    scm2->getCss().getCell(z).setGroup(scm2->getCss().getCell(z).getGroup()+groupshift);
                }
            }
            if (verbose) {
                scm1->generateImage("scm1_joined.jpg", &coloringMethod);
                scm2->generateImage("scm2_joined.jpg", &coloringMethod);
            }
        }
        /**
         * Merges the two SCMs into a single SCM object
         */
        void merge(bool verbose = false) {

        }
    };

}

#endif //CELL_MAPPING_CPP_CSCM_H
