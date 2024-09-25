#include <QDebug>
#include <QTimer>
#include <QTextStream>
#include <QApplication>

#include "communication.h"
#include "mainwindow.h"

// Initialize the static instance pointer
SerialCommunication* SerialCommunication::m_instance = nullptr;

// Singleton access method
SerialCommunication* SerialCommunication::instance() {
    if (m_instance == nullptr) {
        m_instance = new SerialCommunication();
    }
    return m_instance;
}

// Private constructor
SerialCommunication::SerialCommunication(QObject *parent)
    : QObject(parent), serialPort(new QSerialPort(this)) {
}

// Initialization method
void SerialCommunication::init(const QString &portName, qint32 baudRate) {
    serialPort->setPortName(portName);
    serialPort->setBaudRate(baudRate);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (serialPort->open(QIODevice::ReadWrite)) {
        qDebug() << "Serial port opened successfully";
        MainWindow::getMainWinPtr()->on_serial_connect(true);
    } else {
        qDebug() << "Failed to open serial port:" << serialPort->errorString();
        MainWindow::getMainWinPtr()->on_serial_connect(false);
    }

    // Connect the readyRead signal to the readData slot
    connect(serialPort, &QSerialPort::readyRead, this, &SerialCommunication::readData);
}

// Method to send data
void SerialCommunication::sendData(const QString &data) {
    if (serialPort->isOpen()) {
        QByteArray byteArray = data.toUtf8() + '\n';
        serialPort->write(byteArray);
        // qDebug() << "Sent to device:" << data;
    }
}

// Method to read data
void SerialCommunication::readData() {
    while (serialPort->canReadLine()) {
        QByteArray byteArray = serialPort->readLine().trimmed();
        if (!QString(byteArray).isEmpty()) {
            MainWindow::getMainWinPtr()->on_serial_input(QString(byteArray));
            //qDebug() << "Received:" << QString(byteArray);

        }
    }
}

SerialCommunication* get_instance() {
    static SerialCommunication* serialComm = nullptr;

    if (serialComm)
        return serialComm;

    QString portName = "/dev/ttyACM0";  // Change this to match your serial port
    qint32 baudRate = 115200;

    // Get the singleton instance and initialize it
    serialComm = SerialCommunication::instance();
    serialComm->init(portName, baudRate);

    return serialComm;
//    QTextStream input(stdin);
//    QTextStream output(stdout);

//    // Timer to periodically ask for user input
//    QTimer* timer = new QTimer();
//    QObject::connect(timer, &QTimer::timeout, [&]() {
//        output << "Enter data to send to the device: " << flush;
//        QString userInput = input.readLine();
//        serialComm->sendData(userInput);
//    });

//    timer->start(1000); // Prompt for input every second
}
