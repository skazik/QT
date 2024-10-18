usage: main.py [-h] [-p PORT] [-b BAUD] filename

Read CMD script and send to device.

positional arguments:
  filename              The path to the script file

optional arguments:
  -h, --help            show this help message and exit
  -p PORT, --port PORT  optional SERIAL_PORT, default: "/dev/ttyACM0"
  -b BAUD, --baud BAUD  optional BAUD_RATE, default: 115200.
  
  
usage example:
python main.py -p /dev/ttyUSB0 test_script_file.txt

Files in the folder:

