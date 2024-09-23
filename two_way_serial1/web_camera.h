#ifndef WEBCAMERA_H
#define WEBCAMERA_H

#include <QCamera>
#include <QCameraViewfinder>
#include <QObject>
#include <QWidget>
#include <QCameraInfo>

class WebCamera : public QObject {
    Q_OBJECT

public:
    explicit WebCamera(QObject *parent = nullptr);
    ~WebCamera();

    QCameraViewfinder* getViewfinder();  // Expose the viewfinder

public slots:
    void startCamera();    // Slot to start the camera
    void stopCamera();     // Slot to stop the camera

signals:
    void cameraStarted();  // Signal to indicate camera started
    void cameraStopped();  // Signal to indicate camera stopped

private:
    QCamera *camera;          // Camera object
    QCameraViewfinder *viewfinder;  // Camera viewfinder
    QCameraInfo getPreferredCamera();  // Method to get preferred camera
};

#endif // WEBCAMERA_H
