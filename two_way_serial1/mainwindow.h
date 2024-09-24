#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "web_camera.h"
#include <QThread>

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

    void on_serial_input(QString line);
    void on_keyboard_input(int key);
    void on_camera_image_update(QImage image);

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

private:
    Ui::MainWindow *ui;
    static MainWindow * pMainWindow;

    QThread *cameraThread;
    WebCamera *webCamera;
};
#endif // MAINWINDOW_H
