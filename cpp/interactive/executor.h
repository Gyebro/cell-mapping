#ifndef CSCM_EXECUTOR_H
#define CSCM_EXECUTOR_H

#include <QFrame>
#include <QImage>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include <QVector>

#include "cmlib.h"
#include "ikeda.h"
#include "lozi.h"
#include "microchaos.h"
#include "sgms_stribeck.h"

using namespace cm;

using namespace cm;

class JobExecutor {
public:
    enum SystemTypes {
        SystemMicroChaosStatic,
        SystemIkedaMap,
        SystemLoziMap,
        SystemSGMS
    };
    JobExecutor() {
        mpMap = nullptr;
        mSystemParameters = {0.007, 0.02, 0.07, 0.0};
        reset(SystemMicroChaosStatic);
    }
    std::shared_ptr<std::vector<QImage>> getResults() {
        return mpSCM->getResults();
    }
    std::shared_ptr<std::map<std::pair<int32_t, int32_t>, size_t>> getBlockMap() {
        return mBlockIdMap;
    }
    void setParameters(const std::vector<double>& params) {
        cTileW = static_cast<uint32_t>(params[0]);
        cTileH = static_cast<uint32_t>(params[1]);
        mWidth = vec2{static_cast<double>(params[2]), static_cast<double>(params[3])};
        mCenter = vec2{static_cast<double>(params[4]), static_cast<double>(params[5])};
        mSystemParameters.resize(4);
        std::copy(params.begin()+6, params.end(), mSystemParameters.begin());
    }
    void reset(SystemTypes type) {
        blockCenters.clear();
        mBlockIdMap = std::make_shared<std::map<std::pair<int32_t, int32_t>, size_t>>();
        blockCenters.push_back({mCenter[0], mCenter[1]});
        mBlockIdMap->operator[](std::make_pair(0,0)) = 0; // Initial zone is at (0,0)
        if (mpMap != nullptr) {
            delete mpMap;
        }
        // TODO: Forward mSystemParameters to system constructors
        switch (type) {
            case SystemMicroChaosStatic:
                mpMap = new MicroChaosMapStatic(mSystemParameters[0], mSystemParameters[1], mSystemParameters[2], mSystemParameters[3]);
                break;
            case SystemIkedaMap:
                mpMap = new IkedaMap(0.96);
                break;
            case SystemLoziMap:
                mpMap = new LoziMap();
                break;
            case SystemSGMS:
                mpMap = new SGMSStribeckModel();
                break;
        }
        mpSCM = std::make_shared<BSCMQt<SCMCell<uint32_t>, uint32_t, vec2>>(blockCenters[0], mWidth, mCells, mpMap);
        mpSCM->solve(scm_max_steps);
        mpSCM->generateImage("interactive_cscm", &mColoringMethod);
    }
    void update(int32_t i, int32_t j, bool run=true) {
        vec2 newCenter = {mCenter[0]+i*mWidth[0], mCenter[1]-j*mWidth[1]};
        if (std::find(blockCenters.begin(), blockCenters.end(), newCenter) == blockCenters.end()) {
            blockCenters.push_back(newCenter);
            size_t id = mpSCM->addImageBlock(blockCenters.back(), mWidth, mCells);
            mBlockIdMap->operator[](std::make_pair(i,j)) = id;
            if (run) rerun();
        } else {
            std::cout << "Block already present in solution!" << std::endl;
        }
    }
    void rerun() {
        mpSCM->update(scm_max_steps);
        mpSCM->generateImage("interactive_cscm", &mColoringMethod);
    }
    [[nodiscard]] int cW() const {
        return cTileW;
    }
    [[nodiscard]] int cH() const {
        return cTileH;
    }
    [[nodiscard]] double tW() const {
        return mWidth[0];
    }
    [[nodiscard]] double tH() const {
        return mWidth[1];
    }
    [[nodiscard]] double getCenterX() const {
        return mCenter[0];
    }
    [[nodiscard]] double getCenterY() const {
        return mCenter[1];
    }
private:
    uint32_t cTileW{200};
    uint32_t cTileH{200};
    std::shared_ptr<BSCMQt<SCMCell<uint32_t>, uint32_t, vec2>> mpSCM;
    std::vector<vec2> blockCenters;
    std::shared_ptr<std::map<std::pair<int32_t, int32_t>, size_t>> mBlockIdMap;
    int scm_max_steps{1};
    cm::DynamicalSystemBase<vec2>* mpMap;
    vec2 mWidth{9.0, 9.0}; // State space tile width
    vec2 mCenter{-18, 18}; // State space center
    std::vector<double> mSystemParameters;
    SCMHeatMapColoring<SCMCell<uint32_t>, uint32_t> mColoringMethod;
    std::vector<uint32_t> mCells = {cTileW, cTileH}; // State space tile cell counts

};

#endif //CSCM_EXECUTOR_H
