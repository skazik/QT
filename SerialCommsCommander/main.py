# This Python file uses the following encoding: utf-8
import argparse
import os


def read_file_line_by_line(file_path):
    try:
        with open(file_path, 'r') as file:
            for line in file:
                print(line.strip())
                print("-----something else hello")
    except FileNotFoundError:
        print(f"The file at {file_path} was not found.")
    except Exception as e:
        print(f"An error occurred: {e}")


def main():
    # Set up argument parser
    parser = argparse.ArgumentParser(description="Read a file line by line.")
    parser.add_argument('filename', type=str,
                        help='The path to the file you want to read')

    # Parse command-line arguments
    args = parser.parse_args()

    # Validate if file exists
    if os.path.isfile(args.filename):
        read_file_line_by_line(args.filename)
    else:
        print(f"Error: The file '{args.filename}' does not exist.")


if __name__ == "__main__":
    main()

# if __name__ == "__main__":
#    app = QApplication([])
#    # ...
#    sys.exit(app.exec_())
