#ifndef CELL_MAPPING_CPP_CELL_H
#define CELL_MAPPING_CPP_CELL_H

#include <cstdint>

namespace cm {

    /**
     * \brief Used during cell mapping algorithms.
     */
    enum class CellState : uint8_t {
        Untouched,
        UnderProcessing,
        Processed
    };

    /**
     * \brief CellBase class for state space cells.
     *
     * Every cell type should have a state at least.
     */
    class CellBase {
    private:
        CellState state;    /// The state of the cell
    public:
        CellBase() {
            state = CellState::Untouched;
        }
        /**
         * Returns the state of the cell.
         */
        CellState getState() const {
            return state;
        }
        /**
         * Sets the state of the cell
         */
        void setState(CellState state) {
            CellBase::state = state;
        }
    };

    /**
     * \brief Template class for Cells used with Simple Cell Mapping
     *
     * If ID_type is uint32_t, max number of cells is: ~4294 million
     *                         sizeof(SCMCell) is: 4*4 = 16 bytes (due to padding)
     *                         max usable memory: ~68.7 Gigabytes
     * If you have more memory, use uint64_t as IDType.
     */
     template <class IDType>
     class SCMCell : public CellBase {
     private:
         IDType image;  /// ID of the image of this cell
         IDType group;  /// Group number of this cell
         IDType step;   /// Step number of this cell (0 for periodic cells, positive for transient cells)
     public:
         SCMCell() : image(0), group(0), step(0) {}
         /**
          *  Returns the image of the cell.
          */
         IDType getImage() const {
             return image;
         }
         /**
          *  Sets the image of the cell.
          */
         void setImage(IDType image) {
             SCMCell::image = image;
         }
         /**
          *  Returns the group number of the cell.
          */
         IDType getGroup() const {
             return group;
         }
         /**
          *  Sets the group number of the cell.
          */
         void setGroup(IDType group) {
             SCMCell::group = group;
         }
         /**
          *  Returns the step number of the cell.
          */
         IDType getStep() const {
             return step;
         }
         /**
          *  Sets the step number of the cell.
          */
         void setStep(IDType step) {
             SCMCell::step = step;
         }
     };

}

#endif //CELL_MAPPING_CPP_CELL_H