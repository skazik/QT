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

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <string>
#include <stack>

MainWindow * MainWindow::pMainWindow = nullptr;
MainWindow * MainWindow:: getMainWinPtr() {return pMainWindow;}
const char * kConfigFileName{"2wconfig.bin"};

void MainWindow::on_quitButton_clicked()        {QApplication::quit();}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), cameraThread(new QThread(this)) {
    ui->setupUi(this);
    pMainWindow = this;

    //set elements
    ui->textEdit->setLineWrapMode(QTextEdit::NoWrap); // Disable line wrapping
    statusBar()->setStyleSheet("QStatusBar {"
                               "background-color: #424242;"
                               "color: #9999FF;"
                               "height: 20px; "
                               "padding: 2px; "
                               "font-size: 14px; }");

    this->resize(width(), ui->quitButton->y()+ui->quitButton->height()+statusBar()->height()-6);

    // Create the WebCamera object
    webCamera = new WebCamera();

    // Move the WebCamera object to the camera thread
    webCamera->moveToThread(cameraThread);

    connect(webCamera, &WebCamera::cameraStarted, this, &MainWindow::onCameraStarted);
    connect(webCamera, &WebCamera::cameraStopped, this, &MainWindow::onCameraStopped);

    // Ensure the WebCamera object is deleted when the thread finishes
    connect(cameraThread, &QThread::finished, webCamera, &QObject::deleteLater);

    // restore some config
    this->RestoreConfig();

    load_rec_edit_from_tmp();
    ui->recordIndicator->setRecordSchema();
    ui->recordIndicator->turnOff();

    ui->portName->setText(SerialCommunication::get_port_name());
    send_message("Hello");

    this->parseAndLoadCsvPageTree();
}

MainWindow::~MainWindow() {
    // save some config
    this->SaveConfig();

    if (cameraThread->isRunning()) {
        QMetaObject::invokeMethod(webCamera, "stopCamera", Qt::QueuedConnection);
        cameraThread->quit();   // Signal the thread to stop
        cameraThread->wait();    // Wait for it to finish
    }
    delete ui;
}

void MainWindow::on_camera_image_update(QImage image) {
    ui->imageLabel->setPixmap(QPixmap::fromImage(image));
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
    if (txt.isEmpty())
        return;

    if (ui->recordIndicator->isOn()) {
        QString str = serializer::save_command(txt, playRecordStream, lastRecTimeSeconds);
        if (str.isEmpty())
            ui->recEdit->append("--error formating--");
        else
            ui->recEdit->append(str);
    }

    static SerialCommunication* Serial = get_instance(ui->portName->text());
    if (!Serial)
    {
        statusBar()->showMessage("SerialCommunication failed", 5000);
        return;
    }
    QString tmp = "Send to ESP32: ";
    tmp += txt;
    statusBar()->showMessage(tmp, 10000);
//    qDebug() << tmp;

    auto msg = serializer::serialize_message(txt);
    if (Serial->sendData(QByteArray((const char*) msg.byte_array,
                                    sizeof(serializer::SerializedWireMessage)))) {
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
    static const char* kSerialCommandEcho = "[*" ;// "[Bgn";
    static const char* kScrActNotification = "feature_idx:";

    qDebug() << line;
    const char* tmp = nullptr;

    if (line.contains(kScrActNotification)) {
        if (nullptr != (tmp = strstr(line.toStdString().c_str(), kScrActNotification))) {
            tmp += strlen(kScrActNotification);
            uint8_t page_active = static_cast<uint8_t>(*tmp) - '0';
            if (page_active < page_names.size())
            {
                ui->ContRep->setText(page_names[page_active]);
            }
        }
    }
    else if (line.contains(kSerialCommandEcho)) {
        if (nullptr != (tmp = strstr(line.toStdString().c_str(), kSerialCommandEcho))) {
            QString qtmp(tmp);
            ui->textEdit->append(qtmp);
            qDebug() << qtmp;
            return;
        }
        qDebug() << "problem with parse: check formatting:\n" << line;
    }
    else if (line.contains(kBaseStationPefix)) {
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
    ui->textEdit->moveCursor(QTextCursor::End);
    ui->textEdit->ensureCursorVisible();
}

bool MainWindow::on_keyboard_input(int key)
{
    if (ui->portName->hasFocus())
        return false;

    if (key == Qt::Key_Q) {
        on_quitButton_clicked();
        return true;
    }
    if (navigator_sync) switch (key) {
    case Qt::Key_Up:
        break;
    case Qt::Key_Down:
        break;
    case Qt::Key_Right:
        ui->navi_page->setText(navigator.onRight().c_str());
        break;
    case Qt::Key_Left:
        ui->navi_page->setText(navigator.onLeft().c_str());
        break;
    case Qt::Key_Enter:
        ui->navi_page->setText(navigator.onEnter().c_str());
        qDebug() << "level " << navigator.get_current_level();
        if (3 == navigator.get_current_level()) {
            ui->ContExp->setText(navigator.get_current_parent().c_str());
        }
        break;
    case Qt::Key_End:
        ui->navi_page->setText(navigator.onBack().c_str());
        break;
    }

    send_message(serializer::translate_key_to_cmd(key));
    return true;
}

void MainWindow::on_startRecord_clicked()
{
    if (ui->recordIndicator->isOn()) {
        ui->startRecord->setText("Record");
        ui->recordIndicator->turnOff();
        ui->playButton->setEnabled(true);

        serializer::save_delay(playRecordStream, lastRecTimeSeconds);
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
     if (cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, playbackCount++)) {

         // Select the entire line
         cursor.select(QTextCursor::LineUnderCursor);

         // Create a format for highlighting (e.g., yellow background)
         QTextCharFormat fmt;
         fmt.setBackground(QColor(Qt::yellow));

         // Apply the format to the selected line
         cursor.setCharFormat(fmt);

         // scroll to position and Show the QTextEdit
         ui->recEdit->setTextCursor(cursor);
         ui->recEdit->ensureCursorVisible();
         ui->recEdit->show();

         // parse the string, send and arm timer-------------------
         std::vector<std::string> v = serializer::parseCSV(cursor.selectedText().toStdString());
         send_message(serializer::translate_script_cmd(v[0]));
         int timeout = (v.size() < 2) || v[1].empty() ? 1000 : std::stoi(v[1])*1000;
         QTimer::singleShot(timeout, this, &MainWindow::onTimerTimeout);
         qDebug() << "v.size()=" << v.size() << " : " << QString::fromStdString(v[0])
                 << " timeout:" << timeout/1000;

     }
     else {
         onPlaybackStopped("-------- done -------");
     }
}

void MainWindow::copy_to_file(QString dest) {
    // Check if the destination file exists
    if (QFile::exists(dest)) {
        // Attempt to remove the existing file
        if (!QFile::remove(dest)) {
            QMessageBox::warning(nullptr, "Error", "Failed to remove existing file: " + dest);
            return;
        }
    }

    // Copy the file to the destination
    if (inOutFileTmp.copy(dest)) {
        QMessageBox::information(nullptr, "Success", "File copied successfully!");
    } else {
        QMessageBox::warning(nullptr, "Error", "Failed to copy file: " + inOutFileTmp.errorString());
    }
}

void MainWindow::copy_from_file(QString src) {
    inOutFileTmp.close();
    // Check if the destination file exists
    if (QFile::exists(inOutFileTmp.fileName())) {
        // Attempt to remove the existing file
        if (!QFile::remove(inOutFileTmp.fileName())) {
            QMessageBox::warning(nullptr, "Error", "Failed to remove existing file: " + inOutFileTmp.fileName());
            return;
        }
    }

    QFile input(src);
    // Copy the file to the destination
    if (input.copy(inOutFileTmp.fileName())) {
//        QMessageBox::information(nullptr, "Success", "File copied successfully!");
    } else {
        QMessageBox::warning(nullptr, "Error", "Failed to copy file: " + src);
    }
}


void MainWindow::on_saveButton_clicked()
{
    if (QThread::currentThread() != qApp->thread()) {
        return;
    }
    QThread::sleep(1);  // Sleep for 1 second to switch content

    // Open the save file dialog
    QString fileName = QFileDialog::getSaveFileName(
        nullptr,                   // Parent widget
        "Save File",              // Dialog title
        "",                       // Default directory
        "Text Files (*.txt);;All Files (*)" // File filters
    );

    // Check if the user selected a file
    if (!fileName.isEmpty()) {
        copy_to_file(fileName);
    } else {
        QMessageBox::warning(nullptr, "Error", "No destination file selected.");
    }
}

void MainWindow::on_flipButton_clicked()
{
    cameraFlipped = !cameraFlipped;
}

void MainWindow::on_portName_returnPressed()
{
    SerialCommunication::set_port_name(ui->portName->text());
    ui->portName->clearFocus();
    // ui->imageLabel->setFocus();
}

void MainWindow::load_rec_edit_from_tmp() {
    if (inOutFileTmp.exists()) {
        if (inOutFileTmp.open(QIODevice::ReadOnly | QIODevice::Text)) {
            ui->recEdit->clear();
            cleanPlayEdit();

            QTextStream in(&inOutFileTmp);
            while (!in.atEnd()) {
                QString line = in.readLine();  // Read line-by-line
                ui->recEdit->append(line);
            }
            inOutFileTmp.close();
        }
        else {
            QMessageBox::warning(nullptr, "Error", "Failed to open file " + inOutFileTmp.fileName());
        }
    }
}

void MainWindow::on_loadButton_clicked()
{
    if (QThread::currentThread() != qApp->thread()) {
        return;
    }
    QThread::sleep(1);  // Sleep for 1 second to switch content

    // Open the save file dialog
    QString fileName = QFileDialog::getOpenFileName(
        nullptr,                   // Parent widget
        "Open File",              // Dialog title
        "",                       // Default directory
        "Text Files (*.txt);;All Files (*)" // File filters
    );

    // Check if the user selected a file
    if (!fileName.isEmpty()) {
        copy_from_file(fileName);
        load_rec_edit_from_tmp();
    } else {
        QMessageBox::warning(nullptr, "Error", "No file selected.");
    }
}
using namespace page_tree;
constexpr const char* kRootNodeSkipName = "Main Menu";
bool MainWindow::traversePageTreeRecursive(PageNode* currentNode, json& output_json, bool skip_print_on_enter) {
    assert(currentNode);

    // Process the current node as "OK" when visiting it initially except "Main Menu"
    if (!skip_print_on_enter && currentNode->name.compare((kRootNodeSkipName))) {
        json entry = {
            {"command", "OK"},
            {"timeout", "1"},
            {"expected", currentNode->name}
        };
        output_json.push_back(entry);
        std::cout << entry.dump() << std::endl << std::flush;
    }

    if (currentNode->children.size() <= 0) {
        // no children - return back
        return true;
    }

    // Recursively traverse through each child node
    bool _skip_print_enter = false;
    for (int index = 0; index < (int) currentNode->children.size(); index++) {
        auto child = currentNode->children[index].get();

        // Recursive call for the child node
        bool _print_return = traversePageTreeRecursive(child, output_json, _skip_print_enter);
//        // After returning from recursion, process the move back as "Rear" except Main Menu
//        if (_skip_print_return || (currentNode->children.size() == 1)) { //  && currentNode->name.compare((kRootNodeSkipName))) {
//        if (!(index < (int) currentNode->children.size()-1)) {
//            json rearEntry = {
//                {"command", "Rear"},
//                {"timeout", "1"},
//                {"expected", currentNode->name}
//            };
//            output_json.push_back(rearEntry);
//            std::cout << rearEntry.dump() << std::endl << std::flush;
//            std::cout << "-----------------------\n" << std::flush;

//        }
        // After returning from recursion, go to the next child if exist,
        if (index < (int) currentNode->children.size()-1) {
            // Process the move to the child as "Right"
            json rightEntry = {
                {"command", "Right"},
                {"timeout", "1"},
                {"expected", currentNode->children[index+1].get()->name}
            };
            output_json.push_back(rightEntry);
            std::cout << rightEntry.dump() << std::endl << std::flush;
            _skip_print_enter = true;
        }
        else if (_print_return) {
            json rearEntry = {
                {"command", "Rear"},
                {"timeout", "1"},
                {"expected", currentNode->name}
            };
            output_json.push_back(rearEntry);
            std::cout << rearEntry.dump() << std::endl << std::flush;
            std::cout << "-----------------------\n" << std::flush;
        }
    }

    // here we are at the last child - then go back to the first child
    // one by one and return to previous level
    int index = currentNode->children.size()-2;
    if (index >= 0) {
        for (; index >= 0; index--) {
            auto prechild = currentNode->children[index].get();

            // Process the move to the child as "Left"
            json leftEntry = {
                {"command", "Left"},
                {"timeout", "1"},
                {"expected", prechild->name}
            };
            output_json.push_back(leftEntry);
            std::cout << leftEntry.dump() << std::endl << std::flush;
        }
        return true;
    }
    return false;
}

void MainWindow::traversePageTree() {
    std::ofstream outfile("tmp_traverse_test.json");
    json output_json = json::array();  // Store all JSON commands here

    PageNode* root = tree.getRoot();
    if (!root) return;
    root = root->children[0].get();

    // Start the recursive traversal from the root node
    traversePageTreeRecursive(root, output_json);

    // Write the output JSON to the file
    outfile << output_json.dump(4);
    outfile.close();
}


void MainWindow::on_testButton_clicked()
{
    uint8_t result[serializer::kVelocityByteArraySize];
    serializer::serialize_velocity(result);
    serializer::deserialize_velocity(result);
    std::cout << "----------serialization-test completed-------------\n";

    tree.printTree();
    traversePageTree();
    std::cout << "----------traversePageTree-test completed-------------\n" << std::flush;
}

void MainWindow::SaveConfig() {
    QFile fout(kConfigFileName);
    if (fout.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&fout);
        out << (cameraFlipped ? "1":"0") << endl;
        out << SerialCommunication::get_port_name().toStdString().c_str() << endl;
        out << webCamera->getCameraZoom() << endl;
        fout.close();
    }
}

void MainWindow::RestoreConfig() {
    QFile fin(kConfigFileName);
    if (fin.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&fin);
        qDebug() << "---------------config loaded:--------------";
         for (int i = 0; !in.atEnd(); i++) {
             QString line = in.readLine();  // Read line-by-line
             line = line.trimmed();
             switch(i) {
             case 0:
                 cameraFlipped = line.contains("1");
                 qDebug() << "cameraFlipped: " << line;
                 break;
             case 1:
                 SerialCommunication::set_port_name(QString(line));
                 qDebug() << "port_name: " << line;
                 break;
             case 2:
                 webCamera->setCameraZoom(false, line.toUInt());
                 qDebug() << "CameraZoom: " << line;
                 break;
             }
         }
         qDebug() << "-------------------------------------------";
        fin.close();
    }
}
void MainWindow::parseAndLoadCsvPageTree() {
    if (tree.parseCSV("tabview-tree.csv")) {
//        tree.printTree();

        navigator.setRoot(tree.getRoot());
//        navigator::test_navigator(navigator);

        navigator.onEnter(); // Main Menu
        ui->navi_page->setText(navigator.onEnter().c_str()); // Bend & Rotate
    }
}
