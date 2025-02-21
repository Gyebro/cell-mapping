#include <iostream>
#include <QApplication>

#include "cmlib.h"
#include "cscm-frame.h"

using namespace cm;
using namespace std;

class InteractiveExplorer : public QApplication {
public:
    // TODO: Job handle to run background tasks

    explicit InteractiveExplorer(int& argc, char** argv) : QApplication(argc, argv) {
        // TODO: Start process thread
    }

    ~InteractiveExplorer() {
        // TODO: Stop process thread
    }

    int exec() {
        CSCMFrame frame;
        frame.show();
        return QApplication::exec();
    }
};

int main(int argc, char** argv) {
    InteractiveExplorer app(argc, argv);
    return app.exec();
}
