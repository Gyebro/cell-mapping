#ifndef CELL_MAPPING_CPP_CELL_H
#define CELL_MAPPING_CPP_CELL_H

#include <cstdint>

namespace cm {

    /**
     * \class CellState class
     */
    enum class CellState : uint8_t {
        Untouched,
        UnderProcessing,
        Processed
    };

    /**
     * \class Base class for state space cells
     */
    class CellBase {
    private:
        CellState state;
    public:
        CellBase() {
            state = CellState::Untouched;
        }
        CellState getState() const {
            return state;
        }
        void setState(CellState state) {
            CellBase::state = state;
        }
    };

    /**
     * \class Template class for Cells used with Simple Cell Mapping
     * If ID_type is uint32_t, max number of cells is: ~4294 million
     *                         sizeof(SCMCell) is: 4*4 = 16 bytes (due to padding)
     *                         max usable memory: ~68.7 Gigabytes
     * If you have more memory, use uint64_t.
     */
     template <class IDType>
     class SCMCell : public CellBase {
     private:
         IDType image;
         IDType group;
         IDType step;
     public:
         SCMCell() : image(0), group(0), step(0) {}
         IDType getImage() const {
             return image;
         }
         void setImage(IDType image) {
             SCMCell::image = image;
         }
         IDType getGroup() const {
             return group;
         }
         void setGroup(IDType group) {
             SCMCell::group = group;
         }
         IDType getStep() const {
             return step;
         }
         void setStep(IDType step) {
             SCMCell::step = step;
         }
     };

}

#endif //CELL_MAPPING_CPP_CELL_H