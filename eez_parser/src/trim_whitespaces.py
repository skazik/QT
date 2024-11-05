import sys
import os

def trim_file(filename):
    """Reads a file, trims each line, and returns the trimmed lines."""
    with open(filename, 'r') as file:
        trimmed_lines = [line.strip() for line in file]

    # Save trimmed content in a .trim file
    new_filename = f"{filename}.trim"
    with open(new_filename, 'w') as file:
        file.write("\n".join(trimmed_lines))

    print(f"Trimmed file saved as: {new_filename}")
    return trimmed_lines

def create_output_file(lines1, lines2):
    """Creates the output file with formatted lines from both input lists."""
    output_filename = "page_name_display_name.out"

    with open(output_filename, 'w') as outfile:
        for line1, line2 in zip(lines1, lines2):
            formatted_line = f'constexpr const std::array<const char*, 2> arr = {{"{line1}", "{line2}"}};'
            outfile.write(formatted_line + "\n")

    print(f"Combined output saved as: {output_filename}")

def main():
    if len(sys.argv) != 3:
        print("Usage: python trim_files.py <filename1> <filename2>")
        sys.exit(1)

    # Get the filenames from the command-line arguments
    filename1, filename2 = sys.argv[1], sys.argv[2]

    # Trim each file and get trimmed lines
    trimmed_lines1 = trim_file(filename1)
    trimmed_lines2 = trim_file(filename2)

    # Ensure both files have the same number of lines
    if len(trimmed_lines1) != len(trimmed_lines2):
        print("Error: The files have different numbers of lines.")
        sys.exit(1)

    # Create the output file with combined lines
    create_output_file(trimmed_lines1, trimmed_lines2)

if __name__ == "__main__":
    main()
