#ifndef KEYFILTER_H
#define KEYFILTER_H

#include <QDebug>
#include <QWidget>
#include <QKeyEvent>

#include "mainwindow.h"

class KeyFilter : public QObject {
    Q_OBJECT

protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = (QKeyEvent *)event;
//            qDebug() << "Key pressed in event filter:" << QKeySequence(keyEvent->key()).toString();
            return MainWindow::getMainWinPtr()->on_keyboard_input(keyEvent->key());
        }
        return QObject::eventFilter(obj, event);
    }
};

#endif // KEYFILTER_H
