# This Python file uses the following encoding: utf-8

def translate_script_cmd(input_str):
    # Convert input to lowercase
    input_str = input_str.lower()

    # Define a simple hash function (can use Python's hash if needed)
    def hash_string(s):
        return hash(s)

    # Simulated 'translate_key_to_cmd' function to return string
    def translate_key_to_cmd(key):
        data_to_send = bytearray([0x42, 0x67, 0x6E,  # 'Bgn'
                                  0x0A,              # '\n' type 10 String
                                  0x71,              # 'q' CRC8
                                  0x2A,              # '42' cmd type
                                  0x72,              # 0x72 'r'
                                  0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D,
                                  0x00, 0x00])       # CRC16

        key_mapping = {
            'Key_Up': 'U',
            'Key_Down': 'D',
            'Key_Left': 'L',
            'Key_Right': 'R',
            'Key_OK': 'K',
            'Key_Rear': 'r'
        }
#        print(key_mapping.get(key, ''))
        data_to_send[6] = ord(key_mapping.get(key, ''))
        return data_to_send

    # Calculate the hash for comparison
    if input_str in ["u", "up"]:
        return translate_key_to_cmd('Key_Up')
    elif input_str in ["d", "down", "dn"]:
        return translate_key_to_cmd('Key_Down')
    elif input_str in ["l", "left", "lt"]:
        return translate_key_to_cmd('Key_Left')
    elif input_str in ["ri", "right", "rt"]:
        return translate_key_to_cmd('Key_Right')
    elif input_str in ["k", "ok"]:
        return translate_key_to_cmd('Key_OK')
    elif input_str in ["re", "rb", "rear", "rbutton", "rearbutton"]:
        return translate_key_to_cmd('Key_Rear')
    else:
        return ""
