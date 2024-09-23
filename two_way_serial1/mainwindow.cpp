#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "communication.h"
#include "web_camera.h"

#include <QLayout>
#include <QLayoutItem>

MainWindow * MainWindow::pMainWindow = nullptr;
MainWindow * MainWindow:: getMainWinPtr() {return pMainWindow;}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), cameraThread(new QThread(this)) {
    ui->setupUi(this);
    pMainWindow = this;
    send_message("Hello");

    // Create the WebCamera object
    webCamera = new WebCamera();

    // Move the WebCamera object to the camera thread
    webCamera->moveToThread(cameraThread);

    // example how to connect signals to slots (buttons to function) manually
    // connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartCamera);
    // connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::onStopCamera);

    connect(webCamera, &WebCamera::cameraStarted, this, &MainWindow::onCameraStarted);
    connect(webCamera, &WebCamera::cameraStopped, this, &MainWindow::onCameraStopped);

    // Ensure the WebCamera object is deleted when the thread finishes
    connect(cameraThread, &QThread::finished, webCamera, &QObject::deleteLater);
}

MainWindow::~MainWindow() {
    if (cameraThread->isRunning()) {
        QMetaObject::invokeMethod(webCamera, "stopCamera", Qt::QueuedConnection);
        cameraThread->quit();   // Signal the thread to stop
        cameraThread->wait();    // Wait for it to finish
    }
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);  // Call base class implementation
    onStartCamera();
}

void MainWindow::onStartCamera() {
    // Ensure the camera thread is running
    if (!cameraThread->isRunning()) {
        cameraThread->start();  // Start the camera thread
    }

    // Get the camera viewfinder
    QCameraViewfinder *viewfinder = webCamera->getViewfinder();
    qDebug() << "Viewfinder initialized:" << (viewfinder != nullptr);

    // Clear any existing widgets in the layout of webcamView
    QLayout *layout = ui->webcamView->layout();
    if (!layout) {
        // If layout is null, create a new layout
        layout = new QVBoxLayout(ui->webcamView);
        ui->webcamView->setLayout(layout);
    } else {
        // Clear the existing layout
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();  // Delete the old widget
            delete item;            // Delete the layout item
        }
    }

    // Add the viewfinder to the layout
    layout->addWidget(viewfinder);

    // Start the camera in the thread
    QMetaObject::invokeMethod(webCamera, "startCamera", Qt::QueuedConnection);
}

void MainWindow::onStopCamera() {
    QMetaObject::invokeMethod(webCamera, "stopCamera", Qt::QueuedConnection);
}

void MainWindow::onCameraStarted() {
    // Handle any UI updates needed when the camera starts
    qDebug() << "Camera started!";
}

void MainWindow::onCameraStopped() {
    // Handle any UI updates needed when the camera stops
    qDebug() << "Camera stopped!";
}

void MainWindow::send_message(QString txt)
{
    static SerialCommunication* Serial = get_instance();
    if (!Serial)
    {
        statusBar()->showMessage("SerialCommunication failed", 5000);
        return;
    }
    QString tmp = "Send to ESP32: ";
    tmp += txt;
    statusBar()->showMessage(tmp, 3000);
    Serial->sendData(txt);
    Serial->readData();
}

void MainWindow::on_serial_input(QString line)
{
    ui->textEdit->append(line);
}

void MainWindow::on_keyboard_input(int key)
{
    QString str_to_send{"FLX"};
    switch (key) {
    case Qt::Key_Up:    str_to_send += 'U'; break;
    case Qt::Key_Down:  str_to_send += 'D'; break;
    case Qt::Key_Left:  str_to_send += 'L'; break;
    case Qt::Key_Right: str_to_send += 'R'; break;
    case Qt::Key_Return:
    case Qt::Key_Enter: str_to_send += 'K'; break;
    case Qt::Key_End:   str_to_send += 'r'; break;
    default: return;
    }
    send_message(str_to_send);
}

void MainWindow::on_pushButton_up_clicked()     {on_keyboard_input(Qt::Key_Up);}
void MainWindow::on_pushButton_down_clicked()   {on_keyboard_input(Qt::Key_Down);}
void MainWindow::on_pushButton_right_clicked()  {on_keyboard_input(Qt::Key_Right);}
void MainWindow::on_pushButton_left_clicked()   {on_keyboard_input(Qt::Key_Left);}
void MainWindow::on_pushButton_ok_clicked()     {on_keyboard_input(Qt::Key_Enter);}
void MainWindow::on_pushButton_rear_clicked()   {on_keyboard_input(Qt::Key_End);}

