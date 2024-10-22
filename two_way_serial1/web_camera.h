#ifndef WEBCAMERA_H
#define WEBCAMERA_H
#include <QObject>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraInfo>
#include <QVideoProbe>
#include <QVideoFrame>
#include <QImage>
#include <opencv2/opencv.hpp>

class WebCamera : public QObject {
    Q_OBJECT

public:
    explicit WebCamera(QObject *parent = nullptr);
    ~WebCamera();

    QCameraViewfinder* getViewfinder();  // Expose the viewfinder

public slots:
    void startCamera();    // Slot to start the camera
    void stopCamera();     // Slot to stop the camera
    void setCameraZoom(bool reset = false, int digital_zoom = 0);
    int  getCameraZoom();
    void saveLastFrame();

private slots:
    void processVideoFrame(const QVideoFrame &frame); // Process frames directly
    void updateFrame();

signals:
    void cameraStarted();  // Signal to indicate camera started
    void cameraStopped();  // Signal to indicate camera stopped

private:
    QCamera *camera{nullptr};       // Camera object
    QCameraViewfinder *viewfinder;  // Camera viewfinder
    QCameraInfo getPreferredCamera();  // Method to get preferred camera
    QVideoProbe *videoProbe; // Probe to capture video frames
    QImage flippedImage;

    cv::VideoCapture cap;
    QTimer *cap_timer;
    double zoom_factor{1.0};
    cv::Mat getZoomedFrame(const cv::Mat &frame);
};

#endif // WEBCAMERA_H
