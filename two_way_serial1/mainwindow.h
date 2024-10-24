#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "web_camera.h"
#include <QThread>

#include <QFile>
#include <QTextStream>  // for text handling
#include <QIODevice>    // for handling input/output modes
#include <QTimer>

#include "utils.h"

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
    bool is_camera_flipped() {return cameraFlipped;}
    // camera - should be public
    void on_camera_image_update(QImage image);

protected:
    void showEvent(QShowEvent *event) override;  // Override showEvent

private slots: // for serial
    void send_message(QString txt);
    void on_pushButton_up_clicked()     { on_keyboard_input(Qt::Key_Up);}
    void on_pushButton_down_clicked()   { on_keyboard_input(Qt::Key_Down);}
    void on_pushButton_right_clicked()  { on_keyboard_input(Qt::Key_Right);}
    void on_pushButton_left_clicked()   { on_keyboard_input(Qt::Key_Left);}
    void on_pushButton_ok_clicked()     { on_keyboard_input(Qt::Key_Enter);}
    void on_pushButton_rear_clicked()   { on_keyboard_input(Qt::Key_End);}

private slots: // for Camera
    void onStartCamera();  // Slot for starting the camera
    void onStopCamera();   // Slot for stopping the camera

    void on_quitButton_clicked();
    void on_startRecord_clicked();
    void on_resetButton_clicked() { webCamera->setCameraZoom(true); }
    void on_zoomButton_clicked() { webCamera->setCameraZoom(); }

    void cleanPlayEdit();
    void on_playButton_clicked();
    void onTimerTimeout();
    void onPlaybackStopped(QString str = "");
    void onPlaybackStarted(QString str = "");
    void on_saveButton_clicked();
    void on_flipButton_clicked();
    void on_portName_returnPressed();
    void load_rec_edit_from_tmp();

    void on_loadButton_clicked();
    void on_testButton_clicked();

    void on_captureButton_clicked() { webCamera->saveLastFrame(); }
    void on_navigator_ckeck_toggled(bool checked) { navigator_sync = checked; }
    void on_radioButton_toggled(bool checked) { serial_debug = checked; }

private:
    enum eLoadsource_t {
        LOAD_CSV,
        LOAD_YAML
    };

    void SaveConfig();
    void RestoreConfig();
    void parseAndLoadPageTree(eLoadsource_t from);

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
    PageTree tree;
    Navigator navigator;
    bool serial_debug{false};

    static inline const QVector<QString> page_names = {
        "Bend & Rotate",
        "LED Lights",
        "Shape Selection",
        "Joint Control",
        "Setup",
        "Information"
    };
};

#endif // MAINWINDOW_H
