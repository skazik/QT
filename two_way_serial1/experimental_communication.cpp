#include "communication.h"
#include <QTextStream>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QDebug>

// Constructor
SerialCommunication::SerialCommunication(const QString &portName, qint32 baudRate, QObject *parent)
    : QObject(parent), serialPort(new QSerialPort(this)) {

    serialPort->setPortName(portName);
    serialPort->setBaudRate(baudRate);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    // Open the serial port
    if (serialPort->open(QIODevice::ReadWrite)) {
        qDebug() << "Serial port opened successfully";
    } else {
        qDebug() << "Failed to open serial port:" << serialPort->errorString();
    }

    // Connect the readyRead signal to the readData slot
    connect(serialPort, &QSerialPort::readyRead, this, &SerialCommunication::readData);
}

// Destructor
SerialCommunication::~SerialCommunication() {
    if (serialPort->isOpen()) {
        serialPort->close();
        qDebug() << "Serial port closed";
    }
}

// Method to send data
void SerialCommunication::sendData(const QString &data) {
    if (serialPort->isOpen()) {
        QByteArray byteArray = data.toUtf8() + '\n';
        serialPort->write(byteArray);
        qDebug() << "Sent to device:" << data;
    }
}

// Method to read data
void SerialCommunication::readData() {
    while (serialPort->canReadLine()) {
        QByteArray byteArray = serialPort->readLine().trimmed();
        qDebug() << "Received:" << QString(byteArray);
    }
}

void comms_init() {
    QString portName = "/dev/ttyACM0";  // Change this to match your serial port
    qint32 baudRate = 115200;

    SerialCommunication serialComm(portName, baudRate);

    QTextStream input(stdin);
    QTextStream output(stdout);

    // Timer to periodically ask for user input
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        output << "Enter data to send to the device: " << flush;
        QString userInput = input.readLine();
        serialComm.sendData(userInput);
    });

    timer.start(1000); // Prompt for input every second
}

#if 0
#include <QCoreApplication>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTextStream>
#include <QTimer>
#include <QDebug>

class SerialCommunication : public QObject {
    Q_OBJECT

public:
    SerialCommunication(const QString &portName, qint32 baudRate, QObject *parent = nullptr)
        : QObject(parent), serialPort(new QSerialPort(this)) {

        // Set serial port parameters
        serialPort->setPortName(portName);
        serialPort->setBaudRate(baudRate);
        serialPort->setDataBits(QSerialPort::Data8);
        serialPort->setParity(QSerialPort::NoParity);
        serialPort->setStopBits(QSerialPort::OneStop);
        serialPort->setFlowControl(QSerialPort::NoFlowControl);

        // Open the serial port
        if (serialPort->open(QIODevice::ReadWrite)) {
            qDebug() << "Serial port opened successfully";
        } else {
            qDebug() << "Failed to open serial port:" << serialPort->errorString();
        }

        // Connect signals for reading data
        connect(serialPort, &QSerialPort::readyRead, this, &SerialCommunication::readData);
    }

    ~SerialCommunication() {
        // Close the serial port when the application exits
        if (serialPort->isOpen()) {
            serialPort->close();
            qDebug() << "Serial port closed";
        }
    }

public slots:
    void sendData(const QString &data) {
        if (serialPort->isOpen()) {
            QByteArray byteArray = data.toUtf8() + '\n'; // Add newline to match the Python version
            serialPort->write(byteArray);
            qDebug() << "Sent to device:" << data;
        }
    }

    void readData() {
        while (serialPort->canReadLine()) {
            QByteArray byteArray = serialPort->readLine().trimmed();
            qDebug() << "Received:" << QString(byteArray);
        }
    }

private:
    QSerialPort *serialPort;
};

void comms_init() {
    QString portName = "/dev/ttyACM0";  // Change this to match your serial port
    qint32 baudRate = 115200;

    SerialCommunication serialComm(portName, baudRate);

    QTextStream input(stdin);
    QTextStream output(stdout);

    // Timer to periodically ask for user input
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        output << "Enter data to send to the device: " << flush;
        QString userInput = input.readLine();
        serialComm.sendData(userInput);
    });

    timer.start(1000); // Prompt for input every second
}

#include "communication.moc"
#endif
