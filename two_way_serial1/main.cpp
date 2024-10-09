#include "mainwindow.h"
#include "communication.h"
#include "key_filter.h"
#include "navigator.hpp"

#include <QApplication>
int main(int argc, char *argv[])
{
    navigator_init();
    QApplication app(argc, argv);
    MainWindow w;

    app.setWindowIcon(QIcon(":/pictures/icon.png"));  // Path to your resource

    KeyFilter keyFilter;
    app.installEventFilter(&keyFilter); // Install the event filter

    w.show();
    return app.exec();
}
