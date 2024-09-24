#include "web_camera.h"

#include <QCameraInfo>
#include <QImage>
#include <QPixmap>

#include "mainwindow.h"

WebCamera::WebCamera(QObject *parent) : QObject(parent) {

#ifdef FLIPPED_IMAGE_FROM_CAMERA
    // Initialize timer to capture image
    timerCapture = new QTimer(this);
    timerCapture->setInterval(100); // 500 milliseconds
    connect(timerCapture, &QTimer::timeout, this, &WebCamera::captureImage);
    timerCapture->start();
#endif

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

    // Flip the image vertically (upside down)
    QImage flippedImage = preview.mirrored(true, true); // Horizontal=true, Vertical=true

    // Display the flipped image in the QLabel
    MainWindow:: getMainWinPtr()->on_camera_image_update(flippedImage);
}

void WebCamera::setCameraZoom(bool reset) {
    QCameraFocus *cameraFocus = camera->focus();

#ifdef CAMERA_SUPPORT_FOCUS_MANUAL
    // Check if focus mode is supported
    if (!cameraFocus) {
        qDebug() << "Camera focus is not supported on this device.";
        return;
    }
    else if (cameraFocus->isFocusModeSupported(QCameraFocus::ManualFocus)) {
        // Set the focus mode to manual
        cameraFocus->setFocusMode(QCameraFocus::ManualFocus);

        // Set focus to the minimum possible distance
        cameraFocus->setFocusPointMode(QCameraFocus::FocusPointCenter); // For fixed focus

        // If supported, some cameras allow setting custom focus distance. This value is usually
        // normalized (0.0 for minimum distance, 1.0 for infinity).
        cameraFocus->setCustomFocusPoint(QPointF(0.0, 0.0)); // Focus on the minimum distance point.

        qDebug() << "Manual Focus on the minimum distance point.";
        qDebug() << "Current Focus: " << cameraFocus->focusMode();
        return;
    }

    qDebug() << "Manual focus is not supported by this camera. Current: " << cameraFocus->focusMode();
    if (cameraFocus->isFocusModeSupported(QCameraFocus::AutoFocus)) {
        cameraFocus->setFocusMode(QCameraFocus::AutoFocus); // Set focus to QCameraFocus::MacroFocus
        qDebug() << "Set focus to QCameraFocus::AutoFocus. Current Focus: " << cameraFocus->focusMode();
        return;
    }
    qDebug() << "MacroFocus focus is not supported by this camera. Current: " << cameraFocus->focusMode();
#endif
    if (reset) {
        cameraFocus->zoomTo(1,1);
    } else {
        cameraFocus->zoomTo(cameraFocus->opticalZoom(), cameraFocus->digitalZoom()+1);
    }
    qDebug() << "Zoom digital" << cameraFocus->digitalZoom();
}
