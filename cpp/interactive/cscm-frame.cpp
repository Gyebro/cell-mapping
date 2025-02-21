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
  setFixedSize(1000, 640);
  setWindowTitle("CSCM explorer");
  originX = 100; originY = 100;
  moveX = 0; moveY = 0;
  dragging = false;
  gridW = 20; gridH = 10;

  srand(time(NULL));

  for (size_t i=0; i< gridW*gridH; i++) {
    QRgb randomColor = qRgb(random()%256, random()%256, random()%256);
    colors.append(randomColor);
  }

  update_timer_ = new QTimer(this);
  update_timer_->setInterval(16);
  update_timer_->start();

  connect(update_timer_, SIGNAL(timeout()), this, SLOT(onUpdate()));
  clear();
}

CSCMFrame::~CSCMFrame() {
  delete update_timer_;
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
    int i = (event->x() - originX) / cTileW;
    int j = (event->y() - originY) / cTileH;
    if (i >= 0 && i < gridW && j >= 0 && j < gridH) {
      std::cout << "Clicked on tile (i,j) = (" << i << "," << j << ")" << std::endl;
      size_t index = i*gridH + j;
      colors[index] = qRgb(random()%256, random()%256, random()%256);
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
  for (size_t i=0; i<gridW; i++) {
    for (size_t j=0; j<gridH; j++) {
      size_t index = i*gridH + j;
      painter.fillRect(originX+moveX+i*cTileW, originY+moveY+j*cTileH, cTileW, cTileH, colors[index]);
    }
  }

}

void CSCMFrame::updateInternals() {
  /*
  // Check if any background task has finished
  if (modified) {
    update();
  }
  */
  update();
}

