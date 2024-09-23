#include "mainwindow.h"
#include "communication.h"
#include "key_filter.h"

#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;

    KeyFilter keyFilter;
    app.installEventFilter(&keyFilter); // Install the event filter

    w.show();
    return app.exec();
}
