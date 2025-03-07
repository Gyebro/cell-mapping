#ifndef CSCM_EXECUTOR_H
#define CSCM_EXECUTOR_H

#include <QFrame>
#include <QImage>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include <QVector>

#include "cmlib.h"
#include "scmb_qt.h"
#include "ikeda.h"

using namespace cm;

using namespace cm;

class JobExecutor {
public:
    JobExecutor() {
        colors = std::make_shared<QVector<QRgb>>();
        for (size_t i=0; i< gridW*gridH; i++) {
            QRgb randomColor = qRgb(random()%256, random()%256, random()%256);
            colors->append(randomColor);
        }
        mBlockIdMap = std::make_shared<std::map<std::pair<size_t, size_t>, size_t>>();
        blockCenters.push_back({mCenter[0], mCenter[1]});
        mBlockIdMap->operator[](std::make_pair(0,0)) = 0; // Initial zone is at (0,0)
        mpSCM = std::make_shared<BSCMQt<SCMCell<uint32_t>, uint32_t, vec2>>(blockCenters[0], mWidth, mCells, &mMap);

        run();
    }
    std::shared_ptr<QVector<QRgb>> getColors() {
        return colors;
    }
    std::shared_ptr<std::vector<QImage>> getResults() {
        return mpSCM->getResults();
    }
    std::shared_ptr<std::map<std::pair<size_t, size_t>, size_t>> getBlockMap() {
        return mBlockIdMap;
    }
    void update(size_t i, size_t j, bool rerun=true) {
        //const size_t index = i*gridH + j;
        //colors->data()[index] = qRgb(random()%256, random()%256, random()%256);
        blockCenters.push_back({mCenter[0]+i*mWidth[0], mCenter[1]-j*mWidth[1]});
        size_t id = mpSCM->addBlock(blockCenters.back(), mWidth, mCells);
        mBlockIdMap->operator[](std::make_pair(i,j)) = id;
        if (rerun) run();
    }
    void run() {
        mpSCM->solve(1);
        mpSCM->generateImage(&mColoringMethod);
    }
    int gW() const {
        return gridW;
    }
    int gH() const {
        return gridH;
    }
    int cW() const {
        return cTileW;
    }
    int cH() const {
        return cTileH;
    }
private:
    int gridW{8}, gridH{6};
    const uint32_t cTileW{120};
    const uint32_t cTileH{80};
    std::shared_ptr<QVector<QRgb>> colors;
    std::shared_ptr<BSCMQt<SCMCell<uint32_t>, uint32_t, vec2>> mpSCM;
    std::vector<vec2> blockCenters;
    IkedaMap mMap{0.96};
    SCMHeatMapColoring<SCMCell<uint32_t>, uint32_t> mColoringMethod;
    std::vector<uint32_t> mCells = {cTileW, cTileH}; // State space tile cell counts
    vec2 mWidth{6.0, 6.0}; // State space tile width
    vec2 mCenter{-3*6.0, 3*6.0}; // State space center
    std::shared_ptr<std::map<std::pair<size_t, size_t>, size_t>> mBlockIdMap;
};

#endif //CSCM_EXECUTOR_H
