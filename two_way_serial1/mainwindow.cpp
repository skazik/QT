#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "communication.h"
#include "web_camera.h"
#include "serializer.h"

#include <QLayout>
#include <QLayoutItem>
#include <QCameraImageCapture>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>

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
    webCamera->setCameraZoom();

    // Access the QLedIndicator
//    ui->ledIndicator->turnOff();   // Turn off the LED - red
    ui->recordIndicator->setRecordSchema();
    ui->recordIndicator->turnOff();

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
    if (ui->recordIndicator->isOn()) {
        QString str = save_command(txt, playRecordStream, lastRecTimeSeconds);
        if (str.isEmpty())
            ui->recEdit->append("--error formating--");
        else
            ui->recEdit->append(str);
    }

    static SerialCommunication* Serial = get_instance();
    if (!Serial)
    {
        statusBar()->showMessage("SerialCommunication failed", 5000);
        return;
    }
    QString tmp = "Send to ESP32: ";
    tmp += txt;
    statusBar()->showMessage(tmp, 10000);

    auto msg = serialize_message(txt);
    if (Serial->sendData(QByteArray((const char*) msg.byte_array,
                                    sizeof(SerializedWireMessage)))) {
        Serial->readData();
    } else {
        qDebug() << "sending ESP32 failed";
        ui->ledIndicator->turnOff();
    }
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
    static const char* kHandleControllerPrefix = "handle controller loop:";

    qDebug() << line;
    const char* tmp = nullptr;

    if (line.contains(kBaseStationPefix)) {
        if (nullptr != (tmp = strstr(line.toStdString().c_str(), kBaseStationPefix))) {
            QString fmt;
            int rssi = 0;
            float throughput = 0, ms = 0;

            tmp += strlen(kBaseStationPefix);
            if (3 == sscanf(tmp, " %d dB, throughput: %f b/s, %f m/s", &rssi, &throughput, &ms)) {
                fmt.sprintf("rssi: %d dB", rssi);
                ui->label_rssi->setText(fmt);
                fmt.sprintf("throughput: %.2f b/s, %.2f m/s", throughput, ms);
                ui->label_throu->setText(fmt);
                return;
            }
        }
        qDebug() << "problem with parse: check formatting:\n" << line;
    }
    else if (line.contains(kHandleControllerPrefix)) {
        if (nullptr != (tmp = strstr(line.toStdString().c_str(), kHandleControllerPrefix))) {
            QString fmt;
            int loop = 0;

            tmp += strlen(kHandleControllerPrefix);
            if (1 == sscanf(tmp, "  %dus", &loop)) {
                fmt.sprintf("%dus", loop);
                ui->hndl_loop_val->setText(fmt);
                return;
            }
        }
        qDebug() << "problem with parse: check formatting:\n" << line;
    }

    ui->textEdit->append(line);
}

void MainWindow::on_keyboard_input(int key)
{
    QString str_to_send{"*"};
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

void MainWindow::on_startRecord_clicked()
{
    if (ui->recordIndicator->isOn()) {
        ui->startRecord->setText("Record");
        ui->recordIndicator->turnOff();
        ui->playButton->setEnabled(true);

        save_delay(playRecordStream, lastRecTimeSeconds);
        inOutFileTmp.close();

    }
    else {
        cleanPlayEdit();
        ui->recEdit->clear();
        ui->startRecord->setText("Stop");
        ui->playButton->setEnabled(false);
        ui->recordIndicator->turnOn();
        ui->playButton->setEnabled(false);

        if (!inOutFileTmp.open(QIODevice::WriteOnly| QIODevice::Text)) {
            qWarning("Could not open file for writing");
            return;
        }
        playRecordStream.setDevice(&inOutFileTmp);
        lastRecTimeSeconds = 0;
    }
}

void MainWindow::cleanPlayEdit() {
    // Get a QTextCursor and select all text
    QTextCursor cursor = ui->recEdit->textCursor();
    cursor.select(QTextCursor::Document);

    // Create a QTextCharFormat with no background (default formatting)
    QTextCharFormat fmt;
    fmt.clearBackground();  // Clear any background formatting

    // Apply this format to the entire document to reset highlights
    cursor.setCharFormat(fmt);

    // Reposition cursor at the start of the document
    cursor.movePosition(QTextCursor::Start);
    ui->recEdit->setTextCursor(cursor);

    // Show the QTextEdit
    ui->recEdit->show();

    playbackCount = 0;
}

void MainWindow::on_playButton_clicked()
{
    if (ui->playButton->text().contains("Stop")) {
        onPlaybackStopped("-- playback stopped --");
        return;
    }

    ui->recEdit->clear();

    if (!inOutFileTmp.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Could not open file for reading");
        ui->recEdit->append("Could not open file for reading");
        return;
    }

    playRecordStream.setDevice(&inOutFileTmp);
    while (!playRecordStream.atEnd()) {
        QString line = playRecordStream.readLine();
        ui->recEdit->append(line);
        // qDebug() << line;  // Output the line
    }
    inOutFileTmp.close();

    onPlaybackStarted();
    cleanPlayEdit();

    QTimer::singleShot(100, this, &MainWindow::onTimerTimeout);
}

void MainWindow::onPlaybackStopped(QString str) {
    playbackInProgress = false;
    ui->playButton->setText("Play");
    ui->startRecord->setEnabled(true);
    if (!str.isEmpty())
        ui->recEdit->append(str);
}

void MainWindow::onPlaybackStarted(QString str) {
    playbackInProgress = true;
    ui->playButton->setText("Stop");
    ui->startRecord->setEnabled(false);
    if (!str.isEmpty())
        ui->recEdit->append(str);
}

void MainWindow::onTimerTimeout(){
    // qDebug() << "Timer timeout! Performing routine...";

    if (!playbackInProgress)
        return;

    // Get a QTextCursor
     QTextCursor cursor = ui->recEdit->textCursor();

     // Move the cursor to the second line (e.g., line 2)
     cursor.movePosition(QTextCursor::Start);      // Move to the start of the document
     if (cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, playbackCount++)) {  // Move to the second line

         // Select the entire line
         cursor.select(QTextCursor::LineUnderCursor);

         // Create a format for highlighting (e.g., yellow background)
         QTextCharFormat fmt;
         fmt.setBackground(QColor(Qt::yellow));

         // Apply the format to the selected line
         cursor.setCharFormat(fmt);

         // Show the QTextEdit
         ui->recEdit->show();

        QTimer::singleShot(1000, this, &MainWindow::onTimerTimeout);
     }
     else {
         onPlaybackStopped("-------- done -------");
     }
}

void MainWindow::on_saveButton_clicked()
{
    // Open the save file dialog
    QString fileName = QFileDialog::getSaveFileName(
        nullptr,                   // Parent widget
        "Save File",              // Dialog title
        "",                       // Default directory
        "Text Files (*.txt);;All Files (*)" // File filters
    );

    // Check if the user selected a file
    if (!fileName.isEmpty()) {
        // Copy the file to the destination
        if (inOutFileTmp.copy(fileName)) {
            QMessageBox::information(nullptr, "Success", "File copied successfully!");
        } else {
            QMessageBox::warning(nullptr, "Error", "Failed to copy file: " + inOutFileTmp.errorString());
        }
    } else {
        QMessageBox::warning(nullptr, "Error", "No destination file selected.");
    }
}
