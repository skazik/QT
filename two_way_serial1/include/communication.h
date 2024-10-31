#ifndef SERIALCOMMUNICATION_H
#define SERIALCOMMUNICATION_H

#include <QObject>
#include <QSerialPort>
#include <QString>

static QString _portName{"/dev/ttyACM0"};

class SerialCommunication : public QObject {
    Q_OBJECT

public:
    // Static method to access the singleton instance
    static SerialCommunication* instance();
    static QString get_port_name() {return _portName;}
    static void set_port_name(QString _name) {_portName = _name;}

    // Method to initialize the serial communication (call once during init)
    void init(const QString &portName, qint32 baudRate);

public slots:
    bool sendData(const QString &data);
    bool sendData(QByteArray byteArray);

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

SerialCommunication* get_instance(QString portName);


#endif // SERIALCOMMUNICATION_H
