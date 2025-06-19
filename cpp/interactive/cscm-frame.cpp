#include "cscm-frame.h"

#include <cstdlib>
#include <ctime>
#include <iostream>

#include <QPointF>

#include "executor.h"

#define DEFAULT_BG_R 0xaa
#define DEFAULT_BG_G 0xaa
#define DEFAULT_BG_B 0xaa

CSCMFrame::CSCMFrame(QWidget* parent, Qt::WindowFlags f)
  : QFrame(parent, f)
  , frame_count_(0)
  , id_counter_(0) {

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
  // Get cluster dimensions
  cW = mpExecutor->cW();
  cH = mpExecutor->cH();
}

CSCMFrame::~CSCMFrame() {
  delete update_timer_;
}

void CSCMFrame::reset(JobExecutor::SystemTypes type) {
  mpExecutor->reset(type);
}

void CSCMFrame::setParameters(const std::vector<double>& parameters) {
  mpExecutor->setParameters(parameters);
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
    float targetW = zoom*static_cast<float>(mpExecutor->cW());
    float targetH = zoom*static_cast<float>(mpExecutor->cH());
    float i_ = static_cast<float>(event->x() - originX) / targetW;
    float j_ = static_cast<float>(event->y() - originY) / targetH;
    int32_t i = static_cast<int32_t>(floor(i_));
    int32_t j = static_cast<int32_t>(floor(j_));
    std::cout << "Requesting update on tile (i,j) = (" << i << "," << j << ")" << std::endl;
    mpExecutor->update(i, j);
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

void CSCMFrame::wheelEvent(QWheelEvent *event) {
  QFrame::wheelEvent(event);
  if(event->angleDelta().y() > 0) {
    zoom += 0.02f;
    if (zoom > 5.0f) zoom = 5.0f;
  }
  else if(event->angleDelta().y() < 0) {
    zoom -= 0.02f;
    if (zoom < 0.1f) zoom = 0.1f;
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
  QPen penW(Qt::white, 1);
  QPen penX(Qt::red, 2);
  QPen penY(Qt::green, 2);

  int axisSizePixels = 100;
  float targetW = zoom*static_cast<float>(mpExecutor->cW());
  float targetH = zoom*static_cast<float>(mpExecutor->cH());
  // Paint cluster textures (as images)
  for(auto it = blockMap->begin(); it != blockMap->end(); ++it) {
    auto loc = it->first;
    int i = loc.first;
    int j = loc.second;
    if (results->size() > blockMap->at(loc)) {
      painter.drawImage(QRectF(originX+moveX+i*targetW, originY+moveY+j*targetH, targetW, targetH), results->at(blockMap->at(loc)));
    } else {
      std::cout << "Result not available yet...\n";
    }
  }
      /*else {
        //size_t index = i*gH + j;
        QPainterPath path;
        path.addRect(originX+moveX+i*targetW, originY+moveY+j*targetH, targetW, targetH);
        painter.setPen(penW);
        painter.fillRect(originX+moveX+i*targetW, originY+moveY+j*targetH, targetW, targetH, Qt::black);
        painter.drawPath(path);
      }*/
  // Paint display CS indicator
  painter.setPen(penX);
  painter.drawLine(originX+moveX, originY+moveY, originX+moveX + zoom*axisSizePixels, originY+moveY);
  painter.setPen(penY);
  painter.drawLine(originX+moveX, originY+moveY, originX+moveX, originY+moveY + zoom*axisSizePixels);

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

