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
        type = JobExecutor::SystemMicroChaosStatic;
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
    JobExecutor::SystemTypes type;
    void setupMenuBar() {
        QMenu *menu = menuBar()->addMenu(tr("&File"));
        menu->addAction(tr("&Reset"), this, &CSCMExplorerWindow::reset);
        menu->addAction(tr("&Save"), this, &CSCMExplorerWindow::saveLayout);
        menu->addAction(tr("&Load"), this, &CSCMExplorerWindow::loadLayout);
        menu->addSeparator();
        menu->addAction(tr("&Quit"), this, &QWidget::close);

        QMenu *systemMenu = menuBar()->addMenu(tr("&System"));
        systemMenu->addAction(tr("Micro-chaos map (negative stiffness)"), this, &CSCMExplorerWindow::useSystem1);
        systemMenu->addAction(tr("Ikeda-map"), this, &CSCMExplorerWindow::useSystem2);
        systemMenu->addAction(tr("Lozi-map"), this, &CSCMExplorerWindow::useSystem3);
    }
    void reset() {
        std::cout << "Resetting CSCM executor...";
        mFrame.reset(type);
    }
    void saveLayout() {
        std::cout << "Menu: Save [not implemented]\n";
    }
    void loadLayout() {
        std::cout << "Menu: Load [not implemented]\n";
    }
    void useSystem1() {
        type = JobExecutor::SystemMicroChaosStatic;
        this->reset();
    }
    void useSystem2() {
        type = JobExecutor::SystemIkedaMap;
        this->reset();
    }
    void useSystem3() {
        type = JobExecutor::SystemLoziMap;
        this->reset();
    }
};

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    CSCMExplorerWindow mainWin;
    mainWin.resize(1280, 800);
    mainWin.show();
    return app.exec();
}
