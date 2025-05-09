#include "cscm-frame.h"

#include <cstdlib>
#include <ctime>
#include <iostream>

#include <QPointF>

#define DEFAULT_BG_R 0xaa
#define DEFAULT_BG_G 0xaa
#define DEFAULT_BG_B 0xaa

CSCMFrame::CSCMFrame(QWidget* parent, Qt::WindowFlags f)
  : QFrame(parent, f)
  , frame_count_(0)
  , id_counter_(0) {
  setFixedSize(1280, 1024);
  setWindowTitle("CSCM explorer");
  originX = 0; originY = 0;
  moveX = 0; moveY = 0;
  dragging = false;

  srand(time(NULL));

  update_timer_ = new QTimer(this);
  update_timer_->setInterval(16);
  update_timer_->start();

  connect(update_timer_, SIGNAL(timeout()), this, SLOT(onUpdate()));
  clear();
}

void CSCMFrame::attachExecutor(std::shared_ptr<JobExecutor> pExecutor) {
  mpExecutor = pExecutor;
  gW = mpExecutor->gW();
  gH = mpExecutor->gH();
  cW = mpExecutor->cW();
  cH = mpExecutor->cH();
}

CSCMFrame::~CSCMFrame() {
  delete update_timer_;
}

void CSCMFrame::reset() {
  mpExecutor->reset(JobExecutor::SystemMicroChaosStatic);
}

void CSCMFrame::clear() {
  update();
}

void CSCMFrame::onUpdate() {
  updateInternals();
}

void CSCMFrame::mousePressEvent(QMouseEvent* event) {
  QWidget::mousePressEvent(event);
  if (event->button() == Qt::LeftButton) {
    dragging = true;
    startX = event->x();
    startY = event->y();
  } else if (event->button() == Qt::RightButton) {
    // Map click coordinate to grid tile index
    int i = (event->x() - originX) / cW;
    int j = (event->y() - originY) / cH;
    if (i >= 0 && i < gW && j >= 0 && j < gH) {
      std::cout << "Requesting update on tile (i,j) = (" << i << "," << j << ")" << std::endl;
      mpExecutor->update(i, j);
    } else {
      std::cout << "Out of bounds" << std::endl;
    }
  }
}

void CSCMFrame::mouseMoveEvent(QMouseEvent* event) {
  QWidget::mouseMoveEvent(event);
  if (dragging) {
    moveX = event->x()-startX;
    moveY = event->y()-startY;
  }
}

void CSCMFrame::mouseReleaseEvent(QMouseEvent* event) {
  QWidget::mouseReleaseEvent(event);
  if (event->button() == Qt::LeftButton) {
    dragging = false;
    originX = originX+moveX;
    originY = originY+moveY;
    moveX = moveY = 0;
  }
}

void CSCMFrame::keyReleaseEvent(QKeyEvent* event) {
  QWidget::keyReleaseEvent(event);
  switch (event->key()) {
    case KEY_ACTION_1:
      std::cout << "Key " << event->key() << " pressed" << std::endl;
      break;
  }
}

void CSCMFrame::paintEvent(QPaintEvent*) {
  QPainter painter(this);
  QRgb background_color = qRgb(DEFAULT_BG_R, DEFAULT_BG_G, DEFAULT_BG_B);
  painter.fillRect(0, 0, width(), height(), background_color);
  std::shared_ptr<std::vector<QImage>> results = mpExecutor->getResults();
  auto blockMap = mpExecutor->getBlockMap();
  QPen pen(Qt::white, 1);
  for (size_t i=0; i<gW; i++) {
    for (size_t j=0; j<gH; j++) {
      auto loc = std::make_pair(i,j);
      if (blockMap->contains(loc)) {
        if (results->size() > blockMap->at(loc)) {
          painter.drawImage(originX+moveX+i*cW, originY+moveY+j*cH, results->at(blockMap->at(loc)));
        } else {
          std::cout << "Result not available yet...\n";
        }
      } else {
        //size_t index = i*gH + j;
        QPainterPath path;
        path.addRect(originX+moveX+i*cW, originY+moveY+j*cH, cW, cH);
        painter.setPen(pen);
        painter.fillRect(originX+moveX+i*cW, originY+moveY+j*cH, cW, cH, Qt::black);
        painter.drawPath(path);
      }
    }
  }

}

void CSCMFrame::updateInternals() {
  /*
  // TODO: Check if any background task has finished
  if (modified) {
    update();
  }
  */
  update();
}

