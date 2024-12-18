#include <QApplication>

#include "mainwindow.h"
#include "communication.h"
#include "key_filter.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    app.setWindowIcon(QIcon(":/pictures/ui/icon.png"));  // Path to your resource

    KeyFilter keyFilter;
    app.installEventFilter(&keyFilter); // Install the event filter

    w.show();
    return app.exec();
}
