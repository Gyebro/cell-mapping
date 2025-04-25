#ifndef CSCM_FRAME_H
#define CSCM_FRAME_H

#include <QFrame>
#include <QImage>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QVector>

#include "executor.h"

const int KEY_ACTION_1 = Qt::Key_C;

class CSCMFrame : public QFrame {
    Q_OBJECT
  public:
    explicit CSCMFrame(QWidget* parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
    void attachExecutor(std::shared_ptr<JobExecutor> pExecutor);
    ~CSCMFrame();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void updateInternals();

    private slots:
      void onUpdate();

private:
    void clear();

    QTimer* update_timer_;
    QPainter path_painter_;
    int originX; int originY;
    int startX, startY;
    int moveX, moveY;
    bool dragging;
    int gW, gH, cW, cH;

    uint64_t frame_count_;
    uint32_t id_counter_;

    QVector<QImage> cscm_images_;
    std::shared_ptr<JobExecutor> mpExecutor;
};

#endif //CSCM_FRAME_H
