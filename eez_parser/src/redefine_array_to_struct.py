import sys
import re

def convert_line_for_def(line):
    # Step 2: Replace 'constexpr const std::array<const char*, 2>' with 'constexpr auto'
    line = line.replace('constexpr const std::array<const char*, 2>', 'constexpr auto')

    # Step 3: Extract and transform the field name
    match = re.search(r'\{ *"([^"]+)"', line)
    if match:
        raw_name = match.group(1)
        # Apply transformations to create the new field name
        field_name = 'k' + ''.join(word.capitalize() for word in raw_name.split('_')) + 'Names'
        # Step 3f: Replace 'arr' with the new field_name
        line = re.sub(r'\barr\b', field_name, line)

    # Step 4: Add 'TabviewPageNames{' with no space before '{'
    line = re.sub(r'(\s*)\{', r'\1TabviewPageNames{', line)

    return line

def convert_line_for_inc(line):
    # Add 'extern' at the beginning
    line = 'extern ' + line

    # Replace 'auto' with 'TabviewPageNames'
    line = line.replace('auto', 'TabviewPageNames')

    # Remove everything starting from ' =' and add ';' at the end
    line = re.sub(r' =.*', ';', line)

    return line

def process_file(filename):
    def_filename = f"{filename}.def"
    inc_filename = f"{filename}.inc"

    with open(filename, 'r') as file:
        lines = file.readlines()

    # Convert each line for the .def file
    def_lines = [convert_line_for_def(line) for line in lines]

    # Write the output to the .def file
    with open(def_filename, 'w') as file:
        file.writelines(def_lines)

    # Convert each line from the .def content for the .inc file
    inc_lines = [convert_line_for_inc(line) for line in def_lines]

    # Write the output to the .inc file
    with open(inc_filename, 'w') as file:
        file.writelines(inc_lines)

    print(f"Output saved to {def_filename} and {inc_filename}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script.py <filename>")
    else:
        process_file(sys.argv[1])
