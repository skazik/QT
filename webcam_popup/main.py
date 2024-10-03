import sys
import cv2
import numpy as np
import signal
from PyQt5.QtWidgets import QApplication, QLabel, QVBoxLayout, QWidget
from PyQt5.QtGui import QImage, QPixmap
from PyQt5.QtCore import Qt, QTimer

class WebcamWindow(QWidget):
    def __init__(self):
        super().__init__()

        # Set window properties: resizable and always on top
        self.setWindowFlags(Qt.WindowStaysOnTopHint)  # This flag ensures the window stays on top
        self.setWindowTitle("Webcam Feed")
        self.resize(400, 250)  # Set initial window size

        # Create a label to display the webcam video
        self.label = QLabel(self)
        self.label.setScaledContents(True)  # Ensure the video scales when resizing the window

        # Set layout
        layout = QVBoxLayout()
        layout.addWidget(self.label)
        self.setLayout(layout)

        # Set up a timer to refresh the video feed
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.update_frame)

        # Find a valid camera (external first, then fallback to built-in)
        self.cap = self.select_camera()

        # Start capturing video from the selected webcam
        self.timer.start(30)

    def select_camera(self):
        # Try to open external camera (index 1)
        for camera_index in [1, 0]:  # Check external camera first (1), then built-in (0)
            cap = cv2.VideoCapture(camera_index)
            if cap.isOpened():
                print(f"Using camera {camera_index}")
                return cap
        print("No camera found.")
        sys.exit()  # Exit if no camera is available

    def update_frame(self):
        ret, frame = self.cap.read()
        if ret:
            # Resize frame to fit the window size
            frame = cv2.resize(frame, (self.label.width(), self.label.height()), interpolation=cv2.INTER_AREA)

            # Convert the frame to RGB
            frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            height, width, channels = frame.shape

            # Convert the frame to QImage
            bytes_per_line = channels * width
            q_image = QImage(frame.data, width, height, bytes_per_line, QImage.Format_RGB888)

            # Display the image in the label
            self.label.setPixmap(QPixmap.fromImage(q_image))

    def closeEvent(self, event):
        # Release the webcam when the window is closed
        self.cap.release()

# Gracefully handle Ctrl+C
def signal_handler(sig, frame):
    print("Exiting...")
    QApplication.quit()

if __name__ == "__main__":
    app = QApplication(sys.argv)

    # Create the webcam window and display it
    window = WebcamWindow()
    window.show()

    # Catch the Ctrl+C signal to gracefully exit
    signal.signal(signal.SIGINT, signal_handler)

    sys.exit(app.exec_())
