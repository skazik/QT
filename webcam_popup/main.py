import os
import sys
import subprocess
import signal
from PyQt5.QtWidgets import QApplication, QLabel, QVBoxLayout, QWidget
from PyQt5.QtGui import QImage, QPixmap
from PyQt5.QtCore import Qt, QTimer

os.environ['OPENCV_LOG_LEVEL'] = 'ERROR'

import cv2

def get_camera_descriptions():
    # Run the v4l2-ctl command to list all devices
    result = subprocess.run(['v4l2-ctl', '--list-devices'], stdout=subprocess.PIPE, text=True)
    output = result.stdout

    # Print the raw output from v4l2-ctl
    print("Camera devices listed by v4l2-ctl:\n", flush=True)
    print(output, flush=True)  # This will print the full device list as given by v4l2-ctl

    # Parse the output into camera descriptions and their corresponding /dev/video* paths
    cameras = []
    for line in output.split('\n'):
        if line.startswith('\t/dev/video'):
            cameras[-1]['device'] = line.strip()  # Add the /dev/videoX path to the last camera in the list
        elif line:
            cameras.append({'description': line.strip(), 'device': None})

    return cameras

def get_available_cameras():
    cameras = []
    # Dynamically check which cameras can be opened using OpenCV
    for i in range(5):  # Assuming there won't be more than 10 cameras
        cap = cv2.VideoCapture(i)
        if cap.isOpened():
            cameras.append(i)
            cap.release()  # Release the camera after checking
#        else:
#            print(f"can't open /dev/video{i}")
    return cameras

def get_camera_list_with_description():
    # Get camera descriptions from v4l2-ctl
    camera_descriptions = get_camera_descriptions()

    # Print the available cameras and their descriptions
    print("Available cameras:", flush=True)
    for cam in camera_descriptions:
        if cam['device']:
            print(f"Camera: {cam['description']} at {cam['device']}", flush=True)

    # Get available camera indexes from OpenCV
    available_camera_indexes = get_available_cameras()

    print("Available camera indexes (usable by OpenCV):", flush=True)
    print(available_camera_indexes, flush=True)

#    class SuppressWarnings:
#        def __enter__(self):
#            self._stderr = sys.stderr
#            sys.stderr = open(os.devnull, 'w')

#        def __exit__(self, exc_type, exc_value, traceback):
#            sys.stderr.close()
#            sys.stderr = self._stderr

#    # Usage
#    with SuppressWarnings():
#        import cv2
#        cap = cv2.VideoCapture(0)

class WebcamWindow(QWidget):
    def __init__(self):
        super().__init__()

        # Set window properties: resizable and always on top
        self.setWindowFlags(Qt.WindowStaysOnTopHint)  # This flag ensures the window stays on top
        self.setWindowTitle("Webcam Feed")
        self.resize(320, 240)  # Set initial window size

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
        self.timer.start(50)

    def select_camera(self):
        # Get available camera indexes
        available_cameras = get_available_cameras()

        # Check for external camera first, then built-in camera
        available_cameras.reverse()
        for cam_index in available_cameras:
            if cam_index > 3:
                continue
            cap = cv2.VideoCapture(cam_index)
            if cap.isOpened():
                print(f"Using camera {cam_index}", flush=True)
                return cap
        print("No camera found.", flush=True)
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
    print("Exiting...", flush=True)
    QApplication.quit()

if __name__ == "__main__":
    # Suppress all warnings and only show errors
#    print(cv2.__version__)

    get_camera_list_with_description()

    app = QApplication(sys.argv)

    # Create the webcam window and display it
    window = WebcamWindow()
    window.show()

    # Catch the Ctrl+C signal to gracefully exit
    signal.signal(signal.SIGINT, signal_handler)

    sys.exit(app.exec_())
