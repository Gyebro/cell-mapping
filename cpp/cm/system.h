#ifndef CELL_MAPPING_CPP_SYSTEM_H
#define CELL_MAPPING_CPP_SYSTEM_H

namespace cm {

    template <class StateVectorType>
    class DynamicalSystemBase {
    private:
    public:
        virtual StateVectorType step(const StateVectorType& state) const = 0;
    };
}

#endif //CELL_MAPPING_CPP_SYSTEM_H