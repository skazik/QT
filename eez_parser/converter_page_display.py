import yaml

def print_page_tree(tree, level=0):
    """
    Recursively print the page tree with proper indentation.
    """
    # Print the current page's display name with indentation based on its level
    print("\t" * level + tree.get('page_name', 'Unnamed Page'))

    # Recursively print each sub-page if there are any
    sub_pages = tree.get('sub_pages', [])
    for sub_page in sub_pages:
        print_page_tree(sub_page, level + 1)

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
        if "link_1" in value:
            # Extract "link_1" and remove it from the original position
            value = value.replace("link_1_", "").strip("_")
            # Add "link_1" to the end of the modified value
            if value.endswith('",'):
                value = value[:-2] + "_link_1" + '",'
            elif value.endswith('"'):
                value = value[:-1] + "_link_1" + '"'
            else:
                value = f"{value}_link_1"

        return value

    with open(filename, 'r') as infile, open(new_filename, 'w') as outfile:
        for line in infile:
            if "page_name:" in line or "page_display_name:" in line:
                # Split at the first occurrence of ':' to separate key and value
                prefix, value = line.split(":", 1)
                # Apply adjustments to the value
                adjusted_value = adjust_string(value.strip())
                # Reconstruct the line with the adjusted value
                line = f"{prefix}: {adjusted_value}\n"
            outfile.write(line)

    return new_filename

def main():
    with open ("ui.eez-project.faked_1") as f:
        data = yaml.safe_load(f)

    # Extract the tree structure from the specified variable
    for item in data['variables']['globalVariables']:
        if item['name'] == 'main_page_info':
            tree = item['defaultValue']

#    print(f"{tree}")
    filename = "eez.extract.txt"
    with open(filename , 'w') as file:
        file.write(tree)

    new_filename = adjust_page_file(filename)
    with open(new_filename) as f:
        tree = f.read()

    # Load the page tree structure
    page_tree = yaml.safe_load(tree)

    # Print the page tree starting from the ++Startup
    sub_pages = page_tree.get('sub_pages', [])
    for sub_page in sub_pages:
        print_page_tree(sub_page)


if __name__ == "__main__":
    main()

