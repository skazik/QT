#ifndef SERIALCOMMUNICATION_H
#define SERIALCOMMUNICATION_H

#include <QObject>
#include <QSerialPort>
#include <QString>

class SerialCommunication : public QObject {
    Q_OBJECT

public:
    // Static method to access the singleton instance
    static SerialCommunication* instance();

    // Method to initialize the serial communication (call once during init)
    void init(const QString &portName, qint32 baudRate);

public slots:
    void sendData(const QString &data);

public slots:
    void readData();

private:
    // Private constructor to prevent external instantiation
    SerialCommunication(QObject *parent = nullptr);

    // Pointer to the QSerialPort object
    QSerialPort *serialPort;

    // Static instance pointer for the singleton pattern
    static SerialCommunication* m_instance;
};

SerialCommunication* get_instance();

#endif // SERIALCOMMUNICATION_H
