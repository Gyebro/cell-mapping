#include <iostream>
#include <QApplication>

#include "cscm-frame.h"

class InteractiveExplorer : public QApplication {
public:
    // TODO: Job handle to run background tasks

    explicit InteractiveExplorer(int& argc, char** argv) : QApplication(argc, argv) {
        // TODO: Start process thread
        mpJobexecutor = std::make_shared<JobExecutor>();
    }

    ~InteractiveExplorer() {
        // TODO: Stop process thread
    }

    int exec() {
        CSCMFrame frame;
        frame.attachExecutor(mpJobexecutor);
        frame.show();
        return QApplication::exec();
    }
private:
    std::shared_ptr<JobExecutor> mpJobexecutor;
};

int main(int argc, char** argv) {
    InteractiveExplorer app(argc, argv);
    return app.exec();
}
