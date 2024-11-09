import sys
import os
#  this script is stand alone - not used in this project
#  this script replace     modified_content = content.replace('\n', '\\n').replace('"', '\\"')
#  after extraction of YAML - like tree to be added to ui.eez.project
#  Note: extraction is made with main script in ~/flx/flxbot-firmware/tools/handle_controller_serial_commander

#  sequence of operation:
#  ~/flx/flxbot-firmware/tools/handle_controller_serial_commander/main.py - uncoment create tmp from csv
#  copy tmp here
#  run this py to replace \n \"
#  cat reformatted.tmp  | xclip -sel clip
#  gedit ui.eez-project
#  find         "type": "struct:page_info",    "defaultValue":
#  replace all in "" save
#  use it as input for this project main.py validator- convertor


def replace_newlines_and_quotes(filename):
    # Read the content of the input file
    with open(filename, 'r') as file:
        content = file.read()

    # Replace all occurrences of \n with \\n and " with \"
    modified_content = content.replace('\n', '\\n').replace('"', '\\"')

    # Prepare the output filename as reformatted.<original_filename>
    directory, original_filename = os.path.split(filename)
    output_filename = os.path.join(directory, f"reformatted.{original_filename}")

    # Write the modified content to the output file
    with open(output_filename, 'w') as file:
        file.write(modified_content)

    print(f"Reformatted content saved to: {output_filename}")

# Entry point of the script
if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script_name.py <filename>")
    else:
        input_filename = sys.argv[1]
        replace_newlines_and_quotes(input_filename)
