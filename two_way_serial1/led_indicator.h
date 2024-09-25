#ifndef QLEDINDICATOR_H
#define QLEDINDICATOR_H

#include <QWidget>

class QLedIndicator : public QWidget
{
    Q_OBJECT

public:
    explicit QLedIndicator(QWidget *parent = nullptr);

    QSize sizeHint() const override;
    void turnOn();
    void turnOff();
    void toggle();
    bool isOn() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool _isOn;
};

#endif // QLEDINDICATOR_H
