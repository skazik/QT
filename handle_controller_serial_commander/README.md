usage:
```
python3 serial_comms_tool.py [-h] -c CMD -u UI [-p PORT] [-b BAUD] [-r RAND] [-i INTERVAL] [-t] [-d] [-g]

```

Read `CMD` <command script> and send to device.

arguments:
```
    -h, --help            show this help message and exit
    -c CMD, --cmd CMD     (Required) Path to the yaml command script file (e.g., 'tests/test_script')
    -u UI, --ui UI        (Required) Path to the UI file (e.g., '<path_to>ui.eez-project')
    -p PORT, --port PORT  Optional SERIAL_PORT, default: "/dev/ttyACM0"
    -b BAUD, --baud BAUD  Optional BAUD_RATE, default: 115200.
    -r RAND, --rand RAND  Optional random commands, default count: 0 (none).
    -i INTERVAL, --interval INTERVAL
                          Optional interval between sending CMD to device, default 1 second.
    -t, --test            Run in test mode.
    -d, --debug           Run in debug mode.
    -g, --generate        Generate tests/traverse_test
    -f, --find            Test path_finder.
```

Note: export path to additional sources, for example:
```
export PYTHONPATH="$HOME/flxbot-firmware/tools/handle_controller_serial_commander/src:$PYTHONPATH"
```

usage example:
```
python3 scripts/serial_comms_tool.py -s tests/test_script -u <path_to>/ui.eez-project
```
