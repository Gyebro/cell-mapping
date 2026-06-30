#ifndef CELL_STATE_SPACE_H
#define CELL_STATE_SPACE_H

#include "cell.h"
#include <vector>
#include <cmath>
#include <oneapi/tbb/info.h>

namespace cm {

    /**
     * Base class for classes describing state space discretization.
     *
     * This is an abstract class describing a simple interface for cell state spaces.
     * Every derived CellStateSpace class should implement two query functions:
     * getCell: to return a state space cell based on its unique ID
     * getCellAtState: to return a state space cell which corresponds to a given point in the state space
     *
     * @tparam CellType type of the used cell.
     * @tparam IDType type of the ID of the cells (typically 32 or 64-bit unsigned integers).
     * @tparam StateVectorType type of the state vector corresponding to the state space.
     */
    template <class CellType, class IDType, class StateVectorType>
    class CellStateSpaceBase {
    public:
        /**
         * Returns the cell corresponding to a point in the state space (const)
         */
        virtual const CellType& getCellAtState(const StateVectorType& state) const = 0;
        /**
         * Returns the cell corresponding to a point in the state space
         */
        virtual CellType& getCellAtState(const StateVectorType& state) = 0;
        /**
         * Returns the cell with the given ID (const)
         */
        virtual const CellType& getCell(const IDType id) const = 0;
        /**
         * Returns the cell with the given ID
         */
        virtual CellType& getCell(const IDType id) = 0;
        //virtual const StateVectorType& getCenter() const = 0;
        //virtual const StateVectorType& getWidth() const = 0;
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
        IDType getIDFromCellCoord(std::vector<IDType> cellCoord) const {
            IDType id = 1;
            for (IDType i=0; i<dimension; i++) {
                id += cellCoord[i] * cellBase[i];
            }
            return id;
        }
        IDType getID(const StateVectorType& state) const {
            bool inside = true; // Out of state space bounds check
            for (IDType i=0; i<dimension; i++) {
                inside &= (center[i]-0.5*width[i] <= state[i]) && (state[i] < center[i]+0.5*width[i]);
            }
            IDType id;
            if (!inside) {
                id = 0; // Sink cell
            } else {
                IDType cellCoord;
                id = 1; // Skip the Sink cell (id = 0)
                for (IDType i=0; i<dimension; i++) {
                    cellCoord = (IDType)floor((state[i] - (center[i]-0.5*width[i]))/cellWidth[i]);
                    // TODO: Check validity of cellCoord -> bug, when state is FLT_EPSILON close to cell boundary
                    // TODO: Related to #1
                    if (cellCoord >= cellCounts[i]) {
                        // Outside!
                        id = 0;
                        break;
                    } else {
                        id += cellCoord * cellBase[i];
                    }
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
        const StateVectorType& getCellWidth() const {
            return cellWidth;
        }
    };

    // TODO: Avoid copying blocks
    // TODO: Introduce helper functions for accessing cells without local block/localID variables
    // TODO: Code-review on uniform state space and it's cellSum data member.
    /**
     * The BlockedUniformCellStateSpace wraps multiple UniformCellStateSpaces together
     * the Cell ID will be 0..N1 for the first block
     *  N1+0...N2 for the second block,
     *  N1+N2+0...N3 for the third, etc, where Ni is the cell count for the i-th block
     * @tparam CellType
     * @tparam IDType
     * @tparam StateVectorType
     */
    template <class CellType, class IDType, class StateVectorType>
    class BlockedUniformCellStateSpace : public CellStateSpaceBase<CellType, IDType, StateVectorType> {
    protected:
        const IDType ID_SINK_CELL = 0; // TODO: Multiple definitions -> move to a global constant?
        std::vector<UniformCellStateSpace<CellType, IDType, StateVectorType>> ucss;
        std::vector<IDType> blockSizes;
    public:
        BlockedUniformCellStateSpace() {

        }
        const UniformCellStateSpace<CellType, IDType, StateVectorType>& getBlock(IDType bid) const {
            return ucss[bid];
        }
        UniformCellStateSpace<CellType, IDType, StateVectorType>& getBlock(IDType bid) {
            return ucss[bid];
        }
        const IDType getBlockCount() const {
            return blockSizes.size();
        }
        IDType addBlock(UniformCellStateSpace<CellType, IDType, StateVectorType> css) {
            // TODO: This will be a copy for debug purposes first! Optimize this!
            ucss.push_back(css);
            blockSizes.push_back(css.getCellSum()+1);
            // TODO: Possible overflow! FIXME css.getcellSum currently returns N, but there's N+1 cells, last valid index is N.
            // TODO: Logic -> check if CSS block overlaps with others!
            return ucss.size()-1;
        }
        /*IDType getIDFromCellCoord(std::vector<IDType> cellCoord) {
            IDType id = 0;
            return id;
        }*/
        IDType getID(const StateVectorType& state, IDType& block) const {
            IDType id;
            block = 0;
            for (const UniformCellStateSpace<CellType, IDType, StateVectorType> &css : ucss) {
                id = css.getID(state);
                if (id != 0) {
                    break; // If CSSs are not overlapping, only one CSS should return nonzero ID.
                } else {
                    block++; // Retry in next block
                }
            }
            if (id==ID_SINK_CELL) block = 0; // Tie sink cell to block 0
            return id; // Another output is 'block' argument
        }
        IDType getID(const StateVectorType& state) const {
            IDType block, id;
            id = getID(state, block);
            IDType globalID = id;
            for (IDType b = 0; b < block; b++) {
                globalID += blockSizes[b];
            }
            return globalID;
        }
        CellType& getCellAtState(const StateVectorType& state) {
            IDType block, localID;
            localID = getID(state, block);
            return ucss[block].getCell(localID);
        }
        const CellType& getCellAtState(const StateVectorType& state) const {
            IDType block, localID;
            localID = getID(state, block);
            return ucss[block].getCell(localID);
        }
        void getBlockAndId(const IDType ID, IDType& block, IDType& localID) const {
            localID = ID;
            for (block = 0; block < blockSizes.size(); block++) {
                if (localID < blockSizes[block]) break;
                else localID -= blockSizes[block];
            }
        }
        CellType& getCell(const IDType ID) {
            IDType block, localID;
            getBlockAndId(ID, block, localID);
            return ucss[block].getCell(localID);
        }
        const CellType& getCell(const IDType ID) const {
            IDType block, localID;
            getBlockAndId(ID, block, localID);
            return ucss[block].getCell(localID);
        }
        const StateVectorType getCenter(const IDType ID) const {
            // Return the center point of the given cell
            IDType block, localID;
            getBlockAndId(ID, block, localID);
            return ucss[block].getCenter(localID);
        }
        IDType getCellSum() const {
            IDType cellSum = 0;
            for (const IDType& c : blockSizes) {
                cellSum += c;
            }
            return cellSum;
        }
        const std::vector<IDType> &getCellCounts() const {
            return blockSizes;
        }
        // These helper functions are the "pairs" of those in SCMUniformCellStateSpace
        const IDType getImage(const IDType ID) const {
            IDType block, localID;
            getBlockAndId(ID, block, localID);
            return this->ucss[block].getCell(localID).getImage();
        }
        const IDType getGroup(const IDType ID) const {
            IDType block, localID;
            getBlockAndId(ID, block, localID);
            return this->ucss[block].getCell(localID).getGroup();
        }
        const IDType getStep(const IDType ID) const {
            IDType block, localID;
            getBlockAndId(ID, block, localID);
            return this->ucss[block].getCell(localID).getStep();
        }
        void setImage(const IDType ID, const IDType image) {
            IDType block, localID;
            getBlockAndId(ID, block, localID);
            this->ucss[block].getCell(localID).setImage(image);
        }
        void setGroup(const IDType ID, const IDType group) {
            IDType block, localID;
            getBlockAndId(ID, block, localID);
            this->ucss[block].getCell(localID).setGroup(group);
        }
        void setStep(const IDType ID, const IDType step) {
            IDType block, localID;
            getBlockAndId(ID, block, localID);
            this->ucss[block].getCell(localID).setStep(step);
        }
    };


    //TODO: Note to self: blocking makes it inconvenient to add these helper functions. It would be better to avoid this inheritance step
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

    template <class CellType, class IDType, class StateVectorType>
    class ICMUniformCellStateSpace : public SCMUniformCellStateSpace<CellType, IDType, StateVectorType> {
    private:
        StateVectorType halfCellWidth;
    public:
        ICMUniformCellStateSpace(StateVectorType center, StateVectorType width, const std::vector<IDType>& cellCounts) :
            SCMUniformCellStateSpace<CellType, IDType, StateVectorType>(center, width, cellCounts) {
            halfCellWidth = 0.5 * this->getCellWidth();
        }
        const StateVectorType getImageState(const IDType ID) const {
            return this->cells[ID].getImageState();
        }
        void setImageState(const IDType ID, const StateVectorType image) {
            this->cells[ID].setImageState(image);
        }
        const std::vector<std::shared_ptr<IcmGraphNode<IDType, StateVectorType>>>& getNodes(const IDType ID) const {
            return this->cells[ID].getSequenceIds();
        }
        void addNode(const IDType ID, const std::shared_ptr<IcmGraphNode<IDType, StateVectorType>>& node) {
            this->cells[ID].addNode(node);
        }
        std::vector<IDType> getPositiveNeighbours(const IDType ID) {
            std::vector<StateVectorType> posCorners;
            std::vector<IDType> cornerIDs;
            cornerIDs.reserve(this->dimension*(this->dimension+1)/2);
            auto center = this->getCenter(ID);
            StateVectorType zeros;
            for (IDType d = 0; d < this->dimension; d++) {
                StateVectorType corner;
                corner[d] = this->getCellWidth()[d];
                posCorners.push_back(center+corner);
                for (IDType d2 = d+1; d2 < this->dimension; d2++) {
                    corner[d2] = this->getCellWidth()[d2];
                    posCorners.push_back(center+corner);
                }
            }
            for (auto corner : posCorners) {
                cornerIDs.push_back(this->getID(corner));
            }
            return cornerIDs;
        }
        double getClosestMatch(const StateVectorType& state, const IDType& ignoreSequenceId, std::shared_ptr<IcmGraphNode<IDType, StateVectorType>>& out) {
            // Simplest approach: loop through the recorded sequences amongst the neighborhood
            auto ID = this->getID(state);
            auto nodes = this->getNodes(ID);
            double minNorm = 100.0*norm(this->getCellWidth());
            for (auto node : nodes) {
                if (node->sequence == ignoreSequenceId) continue;
                double dist = norm(state-node->state);
                if (dist < minNorm) {
                    minNorm = dist;
                    out = node;
                }
            }
            return minNorm;
        }
        bool interpolate(const StateVectorType& state, StateVectorType& out) {
            auto ID = this->getID(state);
            if (ID == 0) {
                return false;
            }
            // Subtract the top-left corner coord of the cell from the state vector
            StateVectorType ksi = state - (this->getCenter(ID) - halfCellWidth);
            // Divide with widths to get relative ksi values (w.r.t top-left corner)
            ksi = ksi / this->getCellWidth();
            // Build corner weights
            std::vector<StateVectorType> weights;
            weights.reserve(this->dimension*(this->dimension+1)/2+1);
            StateVectorType corner({1.0-ksi[0], 1.0-ksi[1]});
            weights.push_back(corner);
            for (IDType d = 0; d < this->dimension; d++) {
                corner = StateVectorType({1.0-ksi[0], 1.0-ksi[1]});
                corner[d] = ksi[d];
                weights.push_back(corner);
                for (IDType d2 = d+1; d2 < this->dimension; d2++) {
                    corner[d2] = ksi[d2];
                    weights.push_back(corner);
                }
            }
            // Get system-images at corners
            std::vector<StateVectorType> cornerImages;
            cornerImages.reserve(weights.size());
            cornerImages.push_back(this->getImageState(ID));
            for (auto &c : this->getCell(ID).getOtherCorners()) {
                if (c == 0) return false; // We're at the edge of the state space
                cornerImages.push_back(this->getImageState(c));
            }
            // Interpolate
            StateVectorType result;
            for (IDType i = 0; i < weights.size(); i++) {
                result = result + prod(weights[i])*cornerImages[i];
            }
            out = result;
            return true;
        }
    };

}

#endif //CELL_STATE_SPACE_H