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
    void setRecordSchema() {_recordSchema = true;}

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool _isOn{false};
    bool _recordSchema{false};
    QColor my_grey{211, 215, 207};
};

#endif // QLEDINDICATOR_H
