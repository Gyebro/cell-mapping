//
// Created by Gyebro on 2019-02-13.
//

#ifndef CELL_STATE_SPACE_H
#define CELL_STATE_SPACE_H

#include "cell.h"
#include <vector>
#include <cmath>

namespace cm {

    /**
     * \class CellStateSpace
     * Base class for classes describing state space discretization
     */
    template <class CellType, class IDType, class StateVectorType>
    class CellStateSpaceBase {
    public:
        virtual const CellType& getCellAtState(const StateVectorType& state) const = 0;
        virtual CellType& getCellAtState(const StateVectorType& state) = 0;
        virtual const CellType& getCell(const IDType id) const = 0;
        virtual CellType& getCell(const IDType id) = 0;
        virtual const StateVectorType& getCenter() const = 0;
        virtual const StateVectorType& getWidth() const = 0;
    };

    template <class CellType, class IDType, class StateVectorType>
    class UniformCellStateSpace : public CellStateSpaceBase<CellType, IDType, StateVectorType> {
    protected:
        std::vector<CellType> cells;        /// Directly accessible container for cell objects
        StateVectorType center;             /// The center of the state space
        StateVectorType width;              /// The width of the state space
        std::vector<IDType> cellCounts;   /// Number of cells along each dimension
        std::vector<IDType> cellBase;     /// Base for addressing the 1D array for multi dimensional cases
        IDType cellSum;                   /// The total number of cells within the state space
        IDType dimension;                 /// The dimension of the state space
        StateVectorType cellWidth;          /// The width of all cells
    public:
        UniformCellStateSpace(StateVectorType center, StateVectorType width, const std::vector<IDType>& cellCounts) :
            center(center), width(width), cellCounts(cellCounts) {
            dimension = cellCounts.size();
            cellSum = 1;
            for (const IDType& u : cellCounts) {
                cellSum *= u;
            }
            cellBase.resize(dimension);
            cellBase[0]=1;
            for (IDType i=1; i<dimension; i++) {
                cellBase[i] = cellCounts[i-1] * cellBase[i-1];
            }
            cells.resize(cellSum+1); // +1 for the Sink cell
            for (IDType i=0; i<dimension; i++) {
                cellWidth[i] = width[i] / cellCounts[i];
            }
        }
        const StateVectorType& getWidth() const {
            return width;
        }
        const StateVectorType& getCenter() const {
            return center;
        }
        IDType getIDFromCellCoord(std::vector<IDType> cellCoord) {
            IDType id = 1;
            for (IDType i=0; i<dimension; i++) {
                id += cellCoord[i] * cellBase[i];
            }
            return id;
        }
        IDType getID(const StateVectorType& state) const {
            bool inside = true; // Out of state space bounds check
            for (IDType i=0; i<dimension; i++) {
                inside &= (center[i]-0.5*width[i] <= state[i]) && (state[i] <= center[i]+0.5*width[i]);
            }
            IDType id;
            if (!inside) {
                id = 0; // Sink cell
            } else {
                IDType cellCoord;
                id = 1; // Skip the Sink cell (id = 0)
                for (IDType i=0; i<dimension; i++) {
                    cellCoord = (IDType)floor((state[i] - (center[i]-0.5*width[i]))/cellWidth[i]);
                    id += cellCoord * cellBase[i];
                }
            }
            return id;
        }
        CellType& getCellAtState(const StateVectorType& state) {
            return cells[getID(state)];
        }
        const CellType& getCellAtState(const StateVectorType& state) const {
            return cells[getID(state)];
        }
        CellType& getCell(const IDType ID) {
            return cells[ID];
        }
        const CellType& getCell(const IDType ID) const {
            return cells[ID];
        }
        const StateVectorType getCenter(const IDType ID) const {
            // Calculate the center point of the given cell
            if (ID == 0) {
                return center;
            } else {
                IDType id = ID-1;
                std::vector<IDType> cellCoord(dimension);
                // Calculate cell-coordinate
                for (IDType j=dimension-1; j>0; j--) {
                    cellCoord[j] = id / cellBase[j];
                    id -= cellCoord[j]*cellBase[j];
                }
                cellCoord[0]=id;
                StateVectorType centerState = center - 0.5*width + 0.5*cellWidth;
                for (IDType j=0; j<dimension; j++) {
                    centerState[j] += cellCoord[j] * cellWidth[j];
                }
                return centerState;
            }
        }
        IDType getCellSum() const {
            return cellSum;
        }
        const std::vector<IDType> &getCellCounts() const {
            return cellCounts;
        }
    };

    /**
     * \class SCMUniformCellStateSpace
     * @tparam CellType
     * @tparam StateVectorType
     * SCM needs three attributes for every cell: image, group, step
     */
    template <class CellType, class IDType, class StateVectorType>
    class SCMUniformCellStateSpace : public UniformCellStateSpace<CellType, IDType, StateVectorType> {
    public:
        SCMUniformCellStateSpace(StateVectorType center, StateVectorType width, const std::vector<IDType>& cellCounts) :
            UniformCellStateSpace<CellType, IDType, StateVectorType>(center, width, cellCounts) {
        }
        const IDType getImage(const IDType ID) const {
            return this->cells[ID].getImage();
        }
        const IDType getGroup(const IDType ID) const {
            return this->cells[ID].getGroup();
        }
        const IDType getStep(const IDType ID) const {
            return this->cells[ID].getStep();
        }
        void setImage(const IDType ID, const IDType image) {
            this->cells[ID].setImage(image);
        }
        void setGroup(const IDType ID, const IDType group) {
            this->cells[ID].setGroup(group);
        }
        void setStep(const IDType ID, const IDType step) {
            this->cells[ID].setStep(step);
        }
    };

}

#endif //CELL_STATE_SPACE_H
