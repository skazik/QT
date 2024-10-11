#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "web_camera.h"
#include <QThread>

#include <QFile>
#include <QTextStream>  // for text handling
#include <QIODevice>    // for handling input/output modes
#include <QTimer>

#include "csv_reader.h"
#include "navigator.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static MainWindow * getMainWinPtr();

    void on_serial_connect(bool connected);
    void on_serial_input(QString line);
    bool on_keyboard_input(int key);
    void on_camera_image_update(QImage image);
    bool is_camera_flipped() {return cameraFlipped;}

protected:
    void showEvent(QShowEvent *event) override;  // Override showEvent

private slots: // for serial

    void send_message(QString txt);
    void on_pushButton_up_clicked();
    void on_pushButton_down_clicked();
    void on_pushButton_left_clicked();
    void on_pushButton_ok_clicked();
    void on_pushButton_right_clicked();
    void on_pushButton_rear_clicked();

private slots: // for Camera
    void onStartCamera();  // Slot for starting the camera
    void onStopCamera();   // Slot for stopping the camera
    void onCameraStarted();  // Slot when camera has started
    void onCameraStopped();  // Slot when camera has stopped

    void on_quitButton_clicked();
    void on_resetButton_clicked();
    void on_zoomButton_clicked();
    void on_startRecord_clicked();

    void cleanPlayEdit();
    void on_playButton_clicked();
    void onTimerTimeout();
    void onPlaybackStopped(QString str = "");
    void onPlaybackStarted(QString str = "");
    void on_saveButton_clicked();
    void on_flipButton_clicked();
    void on_portName_returnPressed();

    void on_loadButton_clicked();
    void on_testButton_clicked();

    void on_captureButton_clicked();

    void on_navigator_ckeck_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    static MainWindow * pMainWindow;

    QThread *cameraThread;
    WebCamera *webCamera;
    QString kTemporaryFileName{"qfile.txt"};
    QFile inOutFileTmp{kTemporaryFileName};
    QTextStream playRecordStream;
    int lastRecTimeSeconds{0};
    int playbackCount{0};
    bool playbackInProgress{false};
    bool cameraFlipped{false};
    bool navigator_sync{false};
    csv_reader::PageTree tree;
    navigator::Navigator navigator;
};
#endif // MAINWINDOW_H
