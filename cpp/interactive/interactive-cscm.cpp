#include <iostream>
#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>

#include "cscm-frame.h"

class CSCMExplorerWindow : public QMainWindow {
public:
    // TODO: Job handle to run background tasks

    explicit CSCMExplorerWindow() : QMainWindow() {
        // TODO: Start process thread
        mpJobexecutor = std::make_shared<JobExecutor>();
        mFrame.attachExecutor(mpJobexecutor);
        setCentralWidget(&mFrame);
        setupMenuBar();
        statusBar()->showMessage(tr("Ready"));
    }

    ~CSCMExplorerWindow() {
        // TODO: Stop process thread
    }

private:
    CSCMFrame mFrame;
    std::shared_ptr<JobExecutor> mpJobexecutor;
    void setupMenuBar() {
        QMenu *menu = menuBar()->addMenu(tr("&File"));
        menu->addAction(tr("&Reset"), this, &CSCMExplorerWindow::reset);
        menu->addAction(tr("&Save"), this, &CSCMExplorerWindow::saveLayout);
        menu->addAction(tr("&Load"), this, &CSCMExplorerWindow::loadLayout);
        menu->addSeparator();
        menu->addAction(tr("&Quit"), this, &QWidget::close);

        QMenu *systemMenu = menuBar()->addMenu(tr("&System"));
        systemMenu->addAction(tr("Micro-chaos map (negative stiffness)"), this, &CSCMExplorerWindow::saveLayout);
        systemMenu->addAction(tr("Ikeda-map"), this, &CSCMExplorerWindow::loadLayout);
    }
    void reset() {
        std::cout << "Resetting CSCM executor...";
        mFrame.reset();
    }
    void saveLayout() {
        std::cout << "Menu: Save\n";
    }
    void loadLayout() {
        std::cout << "Menu: Load\n";
    }
};

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    CSCMExplorerWindow mainWin;
    mainWin.resize(800, 600);
    mainWin.show();
    return app.exec();
}
