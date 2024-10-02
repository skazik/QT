#include "web_camera.h"

#include <QCameraInfo>
#include <opencv2/opencv.hpp>
#include <QVideoFrame>
#include <QImage>


#include "mainwindow.h"

WebCamera::WebCamera(QObject *parent) : QObject(parent) {
    viewfinder = new QCameraViewfinder();
    camera = new QCamera(getPreferredCamera(), this);
    camera->setViewfinder(viewfinder);

    // Set up video probe to intercept video frames
    videoProbe = new QVideoProbe(this);

    if (videoProbe->setSource(camera)) {
        // Connect video frame captured by the probe to a processing function
        connect(videoProbe, &QVideoProbe::videoFrameProbed, this, &WebCamera::processVideoFrame);
    }
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
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();

    // If only one camera, return it (usually the built-in one)
    if (cameras.size() == 1) {
        return cameras.first();
    }

    // Try to find an external camera by checking the description
    for (const QCameraInfo &cameraInfo : cameras) {
         qDebug() << cameraInfo.deviceName() << " " << cameraInfo.description();

        if (!((cameraInfo.deviceName().contains("video0", Qt::CaseInsensitive)) ||
            (cameraInfo.description().contains("integrated", Qt::CaseInsensitive)))) {
            return cameraInfo;  // Found an external camera
        }
    }

    // Fall back to the first available camera (likely built-in)
    return cameras.first();
}

void WebCamera::processVideoFrame(const QVideoFrame &frame) {
    if (!frame.isValid()) {
        qDebug() << "Invalid frame";
        return;
    }

    QVideoFrame cloneFrame(frame);  // Clone the frame to make it mutable
    cloneFrame.map(QAbstractVideoBuffer::ReadOnly);  // Map the frame to access its data

    // Check if the frame is in YV12 format
    if (cloneFrame.pixelFormat() == QVideoFrame::Format_YV12) {
//        qDebug() << "YV12 format detected. Converting to RGB...";

        // Get frame size
        int width = cloneFrame.width();
        int height = cloneFrame.height();

        // Create an OpenCV Mat from the YV12 data
        cv::Mat yv12Frame(height + height / 2, width, CV_8UC1, (uchar*)cloneFrame.bits());

        // Convert YV12 to RGB using OpenCV
        cv::Mat rgbFrame;
        cv::cvtColor(yv12Frame, rgbFrame, cv::COLOR_YUV2RGB_YV12);

        // Convert OpenCV Mat to QImage for display in Qt
        QImage image(rgbFrame.data, rgbFrame.cols, rgbFrame.rows, rgbFrame.step, QImage::Format_RGB888);

        // Make a deep copy of the image and unmap the frame
        bool flipped = MainWindow::getMainWinPtr()->is_camera_flipped();
        QImage flippedImage = image.mirrored(flipped, flipped).copy();  // Flip image horizontally

        cloneFrame.unmap();  // Unmap the frame after processing

        // Display the flipped image in the UI label
        MainWindow::getMainWinPtr()->on_camera_image_update(flippedImage);
//        qDebug() << "Captured and displayed a flipped frame.";
    } else {
        qDebug() << "Unsupported video frame format!";
        cloneFrame.unmap();
    }
}

void WebCamera::setCameraZoom(bool reset, int digital_zoom) {
    QCameraFocus *cameraFocus = camera->focus();
    if (reset) {
        cameraFocus->zoomTo(1,1);
    } else {
        cameraFocus->zoomTo(1, digital_zoom ? digital_zoom : cameraFocus->digitalZoom()+1);
    }
    // qDebug() << "Zoom digital" << cameraFocus->digitalZoom();
}
