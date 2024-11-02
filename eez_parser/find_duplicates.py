import os
from collections import defaultdict

def pre_collect_cpp_files(root_dir):
    cpp_files = defaultdict(list)

    # Walk through all files in the directory and its subdirectories
    for dirpath, _, files in os.walk(root_dir):
        for file in files:
            if file.endswith(".hpp"):
                # Add file path to list with filename as key
                cpp_files[file].append(os.path.join(dirpath, file))

    return cpp_files

def pre_find_duplicates(cpp_files):
    duplicates = {name: paths for name, paths in cpp_files.items() if len(paths) > 1}

    if duplicates:
        print("Duplicate .hpp filenames found:")
        for name, paths in duplicates.items():
            print(f"{name}:")
            for path in paths:
                print(f"  - {path}")
    else:
        print("No duplicate .hpp filenames found.")

def pre_main():
    root_dir = "."  # Replace with your desired root directory
    cpp_files = pre_collect_cpp_files(root_dir)
    pre_find_duplicates(cpp_files)


import os
from collections import defaultdict

def collect_cpp_files(root_dir):
    cpp_files = defaultdict(list)

    # Walk through all files in the directory and its subdirectories
    for dirpath, _, files in os.walk(root_dir):
        for file in files:
            if file.endswith(".hpp"):
                # Add file path to list with filename as key
                cpp_files[file].append(os.path.join(dirpath, file))

    return cpp_files

def find_duplicates(cpp_files):
    duplicates = {name: paths for name, paths in cpp_files.items() if len(paths) > 1}
    return duplicates

def print_statistics(duplicates):
    # Calculate occurrences for each duplicate and filter only those repeated more than 3 times
    duplicate_counts = {name: len(paths) for name, paths in duplicates.items() if len(paths) > 3}

    # Sort by count in descending order
    sorted_duplicates = sorted(duplicate_counts.items(), key=lambda item: item[1], reverse=True)

    if sorted_duplicates:
        print("Duplicate .hpp filenames found (more than 3 occurrences):")
        for name, count in sorted_duplicates:
            print(f"- {name} - found {count} duplications")
    else:
        print("No .hpp filenames with more than 3 duplications found.")

def main():
    pre_main()
    root_dir = "."  # Replace with your desired root directory
    cpp_files = collect_cpp_files(root_dir)
    duplicates = find_duplicates(cpp_files)
    print_statistics(duplicates)

if __name__ == "__main__":
    main()
