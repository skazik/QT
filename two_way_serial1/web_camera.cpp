#include "web_camera.h"

#include <QCameraInfo>
#include <QImage>
#include <QPixmap>

#include "mainwindow.h"

WebCamera::WebCamera(QObject *parent) : QObject(parent) {

    // Initialize timer to capture image
    timerCapture = new QTimer(this);
    timerCapture->setInterval(100); // 500 milliseconds
    connect(timerCapture, &QTimer::timeout, this, &WebCamera::captureImage);
    timerCapture->start();

    // Initialize the viewfinder (but do not attach it to the QWidget yet)
    viewfinder = new QCameraViewfinder();  // No parent
    camera = new QCamera(getPreferredCamera(), this); // QCameraInfo::defaultCamera(), this); // Set parent to this
    imageCapture = new QCameraImageCapture(camera, this);
    // Connect the captured signal to process the image
    connect(imageCapture, &QCameraImageCapture::imageCaptured, this, &WebCamera::processCapturedImage);

    camera->setViewfinder(viewfinder);  // Set the viewfinder here
}

WebCamera::~WebCamera() {
    qDebug() << "Cleaning up WebCamera...";
    stopCamera();  // Ensure the camera stops when the object is deleted
    delete viewfinder; // Clean up viewfinder
    delete camera; // Clean up camera
}

QCameraViewfinder* WebCamera::getViewfinder() {
    return viewfinder;  // Expose the viewfinder to be embedded into a QWidget
}

void WebCamera::startCamera() {
    if (camera) {
        camera->start();
        emit cameraStarted();
    } else {
        qDebug() << "Camera is not initialized!";
    }
}

void WebCamera::stopCamera() {
    if (camera) {
        camera->stop();
        emit cameraStopped();
    } else {
        qDebug() << "Camera is not initialized!";
    }
}

QCameraInfo WebCamera::getPreferredCamera() {
//    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
//    return cameras.isEmpty() ? QCameraInfo() : cameras.first();  // Select the first available camera
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();

    // If only one camera, return it (usually the built-in one)
    if (cameras.size() == 1) {
        return cameras.first();
    }

    // Try to find an external camera by checking the description
    for (const QCameraInfo &cameraInfo : cameras) {
        // qDebug() << cameraInfo.deviceName();
        // qDebug() << cameraInfo.description();

        if (!((cameraInfo.deviceName().contains("video0", Qt::CaseInsensitive)) ||
            (cameraInfo.description().contains("integrated", Qt::CaseInsensitive)))) {
            return cameraInfo;  // Found an external camera
        }
    }

    // Fall back to the first available camera (likely built-in)
    return cameras.first();
}

void WebCamera::captureImage() {
    camera->searchAndLock();
    imageCapture->capture(); // Capture an image from the camera
    // Unlock the camera after capturing
    camera->unlock();
}

void WebCamera::processCapturedImage(int id, const QImage &preview) {
    Q_UNUSED(id);

    bool flipped = MainWindow::getMainWinPtr()->is_camera_flipped();
    // Flip the image vertically (upside down)
    QImage flippedImage = preview.mirrored(flipped, flipped); // Horizontal=true, Vertical=true

    // Display the flipped image in the QLabel
    MainWindow:: getMainWinPtr()->on_camera_image_update(flippedImage);
}

void WebCamera::setCameraZoom(bool reset) {
    QCameraFocus *cameraFocus = camera->focus();

    if (reset) {
        cameraFocus->zoomTo(1,1);
    } else {
        cameraFocus->zoomTo(cameraFocus->opticalZoom(), cameraFocus->digitalZoom()+1);
    }
    // qDebug() << "Zoom digital" << cameraFocus->digitalZoom();
}
