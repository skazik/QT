usage:
```
python3 main.py [-h] [-s SCRIPT] [-p PORT] [-b BAUD] [-r RAND] [-t]
```

Read `SCRIPT` and send to device.

optional arguments:
```
  -h, --help            show this help message and exit
  -s SCRIPT, --script SCRIPT
                        optional, default: "test_script.yaml"
  -p PORT, --port PORT  optional SERIAL_PORT, default: "/dev/ttyACM0"
  -b BAUD, --baud BAUD  optional BAUD_RATE, default: 115200.
  -r RAND, --rand RAND  optional random commands, default: 0 (none).
  -t, --test            Run in test mode.
  -d, --debug           Run in debug mode.
```

usage example:
```
python main.py -p /dev/ttyUSB0 -s test_script.txt
```
