#ifndef WEBCAMERA_H
#define WEBCAMERA_H
#include <QObject>
#include <QCamera>
#include <QCameraViewfinder>
#include <QWidget>
#include <QCameraInfo>
#include <QCameraImageCapture>
#include <QTimer>

class WebCamera : public QObject {
    Q_OBJECT

public:
    explicit WebCamera(QObject *parent = nullptr);
    ~WebCamera();

    QCameraViewfinder* getViewfinder();  // Expose the viewfinder

public slots:
    void startCamera();    // Slot to start the camera
    void stopCamera();     // Slot to stop the camera
    void setCameraZoom(bool reset = false);

private slots:
    void captureImage(); // Slot to capture the image
    void processCapturedImage(int id, const QImage &preview); // Slot to process and flip the captured image

signals:
    void cameraStarted();  // Signal to indicate camera started
    void cameraStopped();  // Signal to indicate camera stopped

private:
    QCamera *camera;          // Camera object
    QCameraViewfinder *viewfinder;  // Camera viewfinder
    QCameraInfo getPreferredCamera();  // Method to get preferred camera
    QCameraImageCapture *imageCapture;
    QTimer *timerCapture;  // Timer to trigger capture
};

#endif // WEBCAMERA_H
