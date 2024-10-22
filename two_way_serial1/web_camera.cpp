#include "web_camera.h"

#include <QCameraInfo>
#include <opencv2/opencv.hpp>
#include <QVideoFrame>
#include <QImage>
#include <QDir>         // For handling file paths
#include <QDateTime>
#include <QList>

#include "mainwindow.h"

WebCamera::WebCamera(QObject *parent) : QObject(parent) {
    viewfinder = new QCameraViewfinder();

    auto cameraInfo = getPreferredCamera();
    if (cameraInfo.isNull()) {
        cap.open(2, cv::CAP_V4L2);
        assert(cap.isOpened());

        // Set up the QTimer to grab frames and update the QLabel
        cap_timer = new QTimer(this);
        connect(cap_timer, &QTimer::timeout, this, &WebCamera::updateFrame);
        cap_timer->start(100); // Refresh every xxx milliseconds
    }
    else {
        camera = new QCamera(cameraInfo, this);
        camera->setViewfinder(viewfinder);

        // Set up video probe to intercept video frames
        videoProbe = new QVideoProbe(this);

        if (videoProbe->setSource(camera)) {
            // Connect video frame captured by the probe to a processing function
            connect(videoProbe, &QVideoProbe::videoFrameProbed, this, &WebCamera::processVideoFrame);
        }
    }
}

WebCamera::~WebCamera() {
    qDebug() << "Cleaning up WebCamera...";
    if (camera) {
        stopCamera();  // Ensure the camera stops when the object is deleted
        delete viewfinder; // Clean up viewfinder
        delete camera; // Clean up camera
    }
    else if (cap.isOpened()) {
        cap.release();
    }

}

QCameraViewfinder* WebCamera::getViewfinder() {
    return viewfinder;  // Expose the viewfinder to be embedded into a QWidget
}

void WebCamera::startCamera() {
    if (camera) {
        camera->start();
        emit cameraStarted();
    }
}

void WebCamera::stopCamera() {
    if (camera) {
        camera->stop();
        emit cameraStopped();
    }
}

QCameraInfo WebCamera::getPreferredCamera() {
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();

    if (!cameras.size()) {
        qDebug() << "no cameras!!!!!";
        return QCameraInfo();
    }

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

cv::Mat WebCamera::getZoomedFrame(const cv::Mat &frame) {
    if (zoom_factor <= 1.0) {
        return frame.clone(); // No zoom, return the original frame
    }

    int width = frame.cols;
    int height = frame.rows;
    int new_width = static_cast<int>(width / zoom_factor);
    int new_height = static_cast<int>(height / zoom_factor);

    int x_start = (width - new_width) / 2;
    int y_start = (height - new_height) / 2;

    // Define the region of interest (ROI) and crop it
    cv::Rect roi(x_start, y_start, new_width, new_height);
    cv::Mat cropped_frame = frame(roi);

    // Resize the cropped frame back to the original size for the zoom effect
    cv::Mat zoomed_frame;
    cv::resize(cropped_frame, zoomed_frame, frame.size());

    return zoomed_frame;
}

void WebCamera::updateFrame() {
    cv::Mat frame;
    cap >> frame; // Capture a new frame from the camera
    if (!frame.empty()) {

        frame = getZoomedFrame(frame);

        // Convert the frame from BGR (OpenCV default) to RGB
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

        // Convert the cv::Mat to QImage
        QImage image(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

        // Make a deep copy of the image and unmap the frame
        bool flipped = MainWindow::getMainWinPtr()->is_camera_flipped();
        flippedImage = image.mirrored(flipped, flipped).copy();

        // Display the flipped image in the UI label
        MainWindow::getMainWinPtr()->on_camera_image_update(flippedImage);
    }
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
        flippedImage = image.mirrored(flipped, flipped).copy();

        cloneFrame.unmap();  // Unmap the frame after processing

        // Display the flipped image in the UI label
        MainWindow::getMainWinPtr()->on_camera_image_update(flippedImage);
//        qDebug() << "Captured and displayed a flipped frame.";
    } else {
        qDebug() << "Unsupported video frame format!";
        cloneFrame.unmap();
    }
}

int  WebCamera::getCameraZoom(){
    return camera ? camera->focus()->digitalZoom() : static_cast<uint8_t>(zoom_factor*10);
}

void WebCamera::setCameraZoom(bool reset, int digital_zoom) {
    if (camera) {
        QCameraFocus *cameraFocus = camera->focus();
        if (reset) {
            cameraFocus->zoomTo(1,1);
        } else {
            cameraFocus->zoomTo(1, digital_zoom ? digital_zoom : cameraFocus->digitalZoom()+1);
        }
        // qDebug() << "Zoom digital" << cameraFocus->digitalZoom();
    }
    else if (reset) {
        zoom_factor = 1.0;
    } else if (digital_zoom > 0) {
        zoom_factor = (double)digital_zoom/10;
    } else {
        zoom_factor += 0.2;
    }
}

void WebCamera::saveLastFrame() {
    // Ensure we have a valid frame to capture
    if (!flippedImage.isNull()) {

        // Get current date and time
        QString currentTime = QDateTime::currentDateTime().toString("MMdd_HHmmss");

        // Define the directory where you want to save the image
        QString dirPath = QDir::homePath() + "/flx/ui/";

        // Generate a unique filename based on the current date and time
        QString uniqueFilename = dirPath + "ui_image_" + currentTime + ".jpg";

        // Save the last frame (the flipped image or current frame) to the predefined path
        if (flippedImage.save(uniqueFilename)) {
            qDebug() << "Image saved successfully to:" << uniqueFilename;
        } else {
            qDebug() << "Failed to save the image.";
        }
    } else {
        qDebug() << "No frame available to capture.";
    }
}
