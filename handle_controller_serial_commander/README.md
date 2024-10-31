usage:
```
python3 scripts/main.py [-h] [-s SCRIPT] [-p PORT] [-b BAUD] [-r RAND] [-t] [-d] [-g]
```

Read `SCRIPT` and send to device.

optional arguments:
```
    -h, --help            show this help message and exit
    -s SCRIPT, --script SCRIPT
                          optional, default: "tests/test_script.yaml"
    -p PORT, --port PORT  optional SERIAL_PORT, default: "/dev/ttyACM0"
    -b BAUD, --baud BAUD  optional BAUD_RATE, default: 115200.
    -r RAND, --rand RAND  optional random commands, default count: 0 (none).
    -t, --test            run in test mode.
    -d, --debug           run in debug mode.
    -g, --generate        generate tests/traverse_test.yaml

```

Note: run script from ./handle_controller_serial_commander folder

usage example:
```
python scripts/main.py -p /dev/ttyUSB0 -s tests/traverse_test.yaml
```
