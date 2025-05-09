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
#include "microchaos.h"

using namespace cm;

using namespace cm;

class JobExecutor {
public:
    JobExecutor() {
        mBlockIdMap = std::make_shared<std::map<std::pair<size_t, size_t>, size_t>>();
        blockCenters.push_back({mCenter[0], mCenter[1]});
        mBlockIdMap->operator[](std::make_pair(0,0)) = 0; // Initial zone is at (0,0)
        mpSCM = std::make_shared<BSCMQt<SCMCell<uint32_t>, uint32_t, vec2>>(blockCenters[0], mWidth, mCells, &mMap);
        mpSCM->solve(scm_max_steps);
        mpSCM->generateImage("interactive_cscm", &mColoringMethod);
    }
    std::shared_ptr<std::vector<QImage>> getResults() {
        return mpSCM->getResults();
    }
    std::shared_ptr<std::map<std::pair<size_t, size_t>, size_t>> getBlockMap() {
        return mBlockIdMap;
    }
    void reset() {
        blockCenters.clear();
        mBlockIdMap = std::make_shared<std::map<std::pair<size_t, size_t>, size_t>>();
        blockCenters.push_back({mCenter[0], mCenter[1]});
        mBlockIdMap->operator[](std::make_pair(0,0)) = 0; // Initial zone is at (0,0)
        mpSCM = std::make_shared<BSCMQt<SCMCell<uint32_t>, uint32_t, vec2>>(blockCenters[0], mWidth, mCells, &mMap);
        mpSCM->solve(scm_max_steps);
        mpSCM->generateImage("interactive_cscm", &mColoringMethod);
    }
    void update(size_t i, size_t j, bool run=true) {
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
    [[nodiscard]] int gW() const {
        return gridW;
    }
    [[nodiscard]] int gH() const {
        return gridH;
    }
    [[nodiscard]] int cW() const {
        return cTileW;
    }
    [[nodiscard]] int cH() const {
        return cTileH;
    }
private:
    int gridW{24}, gridH{24};
    const uint32_t cTileW{200}; // TODO: Tile aspect ratio
    const uint32_t cTileH{160};
    std::shared_ptr<BSCMQt<SCMCell<uint32_t>, uint32_t, vec2>> mpSCM;
    std::vector<vec2> blockCenters;
    int scm_max_steps{20};
    //IkedaMap mMap{0.96};
    //vec2 mWidth{6.0, 6.0}; // State space tile width
    ///vec2 mCenter{-3*6.0, 3*6.0}; // State space center
    MicroChaosMapStatic mMap{0.007, 0.02, 0.07, 0.0};
    vec2 mWidth{24.0, 2.0}; // State space tile width
    vec2 mCenter{-320, 5*2.0}; // State space center
    SCMHeatMapColoring<SCMCell<uint32_t>, uint32_t> mColoringMethod;
    std::vector<uint32_t> mCells = {cTileW, cTileH}; // State space tile cell counts

    std::shared_ptr<std::map<std::pair<size_t, size_t>, size_t>> mBlockIdMap;
};

#endif //CSCM_EXECUTOR_H
