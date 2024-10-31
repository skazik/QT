#include "led_indicator.h"
#include <QPainter>
#include <QBrush>
#include <QColor>

QLedIndicator::QLedIndicator(QWidget *parent)
    : QWidget(parent), _isOn(false)
{
    setMinimumSize(10, 10);
}

QSize QLedIndicator::sizeHint() const
{
    return QSize(10, 10);  // Default size of the LED
}

void QLedIndicator::turnOn()
{
    _isOn = true;
    update();  // Request a repaint
}

void QLedIndicator::turnOff()
{
    _isOn = false;
    update();  // Request a repaint
}

void QLedIndicator::toggle()
{
    _isOn = !_isOn;
    update();  // Request a repaint
}

bool QLedIndicator::isOn() const
{
    return _isOn;
}

void QLedIndicator::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    // Draw a circle to represent the LED
    if (_isOn)
        painter.setBrush(_recordSchema ? QBrush(Qt::red) : QBrush(Qt::green));  // LED is on (green)
    else
        painter.setBrush(_recordSchema ? QBrush(my_grey) : QBrush(Qt::red));    // LED is off (red)

    painter.setPen(Qt::black);  // Optional: Black outline
    painter.drawEllipse(0, 0, width() - 1, height() - 1);
}
