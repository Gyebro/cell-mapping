#include <iostream>
#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QHBoxLayout>
#include <QTableWidget>

#include "cscm-frame.h"

class CSCMExplorerWindow : public QMainWindow {
public:
    // TODO: Job handle to run background tasks

    class ParameterHelper {
    public:
        ParameterHelper() {
            labels = {Name_xCells, Name_yCells, Name_xWidth, Name_yHeight, Name_xZero, Name_yZero,
                Name_par1, Name_par2, Name_par3, Name_par4};
            values = {200.0, 200.0, 9.0, 9.0, -18.0, 18.0,
                0.007, 0.02, 0.07, 0.0};
        };
        void fill(QTableWidget* t) {
            for (size_t i=0; i<labels.size(); i++) {
                t->setItem(i, 0, new QTableWidgetItem(labels[i].data()));
                t->setItem(i, 1, new QTableWidgetItem(std::to_string(values[i]).data()));
            }
        }
        void readFrom(QTableWidget* t) {
            for (size_t i=0; i<labels.size(); i++) {
                double value = 0.0;
                try {
                    value = std::stod(t->item(i, 1)->text().toStdString());
                } catch (const std::exception& e) {
                    std::cout << "Cannot interpret parameter: " << t->item(i, 1)->text().toStdString() << ", exception: " << e.what() << std::endl;
                }
                values[i] = value;
            }
        }
        const std::vector<double>& getParameters() {
            return values;
        }
    private:
        std::vector<std::string> labels;
        std::vector<double> values;
        std::string Name_xCells = "Cells (X)";
        std::string Name_yCells = "Cells (Y)";
        std::string Name_xWidth = "Width (along X)";
        std::string Name_yHeight = "Height (along Y)";
        std::string Name_xZero = "Origin (X0)";
        std::string Name_yZero = "Origin (Y0)";
        std::string Name_par1 = "Parameter #1";
        std::string Name_par2 = "Parameter #2";
        std::string Name_par3 = "Parameter #3";
        std::string Name_par4 = "Parameter #4";
    };

    explicit CSCMExplorerWindow() : QMainWindow() {
        // TODO: Start process thread
        type = JobExecutor::SystemMicroChaosStatic;
        mpJobexecutor = std::make_shared<JobExecutor>();
        mFrame.attachExecutor(mpJobexecutor);
        mpTableWidget = new QTableWidget(10, 2);
        mpTableWidget->setFixedWidth(320);
        mpTableWidget->setColumnWidth(0, 190);
        mpTableWidget->setColumnWidth(1, 100);
        mParameterHelper.fill(mpTableWidget);

        // Set layout
        QHBoxLayout *layout = new QHBoxLayout;

        layout->addWidget(mpTableWidget);
        layout->addWidget(&mFrame);

        // Set layout in QWidget
        QWidget *window = new QWidget();
        window->setLayout(layout);

        // Set QWidget as the central layout of the main window
        setCentralWidget(window);
        //setCentralWidget(&mFrame);
        setupMenuBar();
        statusBar()->showMessage(tr("Ready"));
    }

    ~CSCMExplorerWindow() {
        // TODO: Stop process thread
    }

private:
    CSCMFrame mFrame;
    ParameterHelper mParameterHelper;
    std::shared_ptr<JobExecutor> mpJobexecutor;
    JobExecutor::SystemTypes type;
    QTableWidget* mpTableWidget;
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
        mParameterHelper.readFrom(mpTableWidget);
        mFrame.setParameters(mParameterHelper.getParameters());
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
