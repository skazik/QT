import sys

def adjust_page_file(filename):
    new_filename = f"{filename}_out"

    def adjust_string(value):
        # Apply transformations

        if value.startswith('"'):
            value = '"page_' + value[1:].lower().replace("&", "and").replace(" ", "_")
        else:
            value = "page_" + value.lower().replace("&", "and").replace(" ", "_")
        value = value.replace("_selected", "_sub")
        value = value.replace("_select", "_sub")
        value = value.replace("_segments", "_1")
        value = value.replace("_segment", "_1")
        value = value.replace("system_information", "information_sub")
        value = value.replace("page_main_menu", "page_startup")
        for link_num in range(1, 4):
            link_tag = f"link_{link_num}"

            if link_tag in value:
                # Extract the link tag and remove it from the original position
                value = value.replace(f"{link_tag}_", "").strip("_")

                # Add the link tag to the end of the modified value
                if value.endswith('",'):
                    value = value[:-2] + f"_{link_tag}" + '",'
                elif value.endswith('"'):
                    value = value[:-1] + f"_{link_tag}" + '"'
                else:
                    value = f"{value}_{link_tag}"
                break  # Stop after handling the first found link

        return value

    with open(filename, 'r') as infile, open(new_filename, 'w') as outfile:
        for line in infile:
            if "Precondition:" in line or "Postcondition:" in line:
                # Split at the first occurrence of ':' to separate key and value
                prefix, value = line.split(":", 1)
                # Apply adjustments to the value
                adjusted_value = adjust_string(value.strip())
                # Reconstruct the line with the adjusted value
                line = f"{prefix}: {adjusted_value}\n"
            outfile.write(line)

    return new_filename


# Entry point of the script
if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script_name.py <filename>")
    else:
        input_filename = sys.argv[1]
        adjust_page_file(input_filename)
