#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "communication.h"
#include "web_camera.h"

#include <QLayout>
#include <QLayoutItem>
#include <QCameraImageCapture>
#include <QTimer>

MainWindow * MainWindow::pMainWindow = nullptr;
MainWindow * MainWindow:: getMainWinPtr() {return pMainWindow;}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), cameraThread(new QThread(this)) {
    ui->setupUi(this);
    pMainWindow = this;
    send_message("Hello");

    //set elements
    ui->textEdit->setLineWrapMode(QTextEdit::NoWrap); // Disable line wrapping
    statusBar()->setStyleSheet("QStatusBar {"
                               "background-color: #3465A4;"
                               "height: 20px; "
                               "padding: 2px; "
                               "font-size: 14px; }");

    this->resize(width(), ui->quitButton->y()+ui->quitButton->height()+statusBar()->height()-6);

    // Access the QLedIndicator
    // ui->ledIndicator->turnOff();   // Turn off the LED - red

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
    statusBar()->showMessage(tmp, 10000);
    Serial->sendData(txt);
    Serial->readData();
}

void MainWindow::on_serial_connect(bool connected) {
    if (connected)
        ui->ledIndicator->turnOn();
    else
        ui->ledIndicator->turnOff();
}

void MainWindow::on_serial_input(QString line)
{
    static const char* kBaseStationPefix = "Base station - rssi:";
    ui->textEdit->append(line);

    if (line.contains(kBaseStationPefix)) {
        const char* tmp = strstr(line.toStdString().c_str(), kBaseStationPefix);
        if (tmp) {
            QString fmt;
            int rssi = 0, throughput = 0, ms = 0;
            if (3 == sscanf(tmp, "Base station - rssi: %d dB, throughput: %d b/s, %d m/s", &rssi, &throughput, &ms))
            {
                fmt.sprintf("rssi: %d dB", rssi);
                ui->label_rssi->setText(fmt);
                fmt.sprintf("throughput: %d b/s, %d m/s", throughput, ms);
                ui->label_throu->setText(fmt);
            }
        }
    }
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
    case Qt::Key_Q:     on_quitButton_clicked(); break;
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
void MainWindow::on_quitButton_clicked()        {QApplication::quit();}

void MainWindow::on_camera_image_update(QImage image)
{
#ifdef NOT_SCALED_CONTENT // image for flip
    // Get the QLabel's size
    QSize labelSize = ui->imageLabel->size();

    // Create a QPixmap from the QImage
    QPixmap pixmap = QPixmap::fromImage(image);

    // Scale the pixmap to fit within the label's size, while keeping the aspect ratio
    QPixmap scaledPixmap = pixmap.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Set the scaled pixmap to the label
    ui->imageLabel->setPixmap(scaledPixmap);
#else
    ui->imageLabel->setPixmap(QPixmap::fromImage(image));
#endif
}

void MainWindow::on_resetButton_clicked()
{
    webCamera->setCameraZoom(true); // reset
}

void MainWindow::on_zoomButton_clicked()
{
    webCamera->setCameraZoom();
}
