import yaml
import json

#comment - current: usage of 'page_display_name' replaced to use 'page_name'
def build_page_tree_dict(tree):
    """
    Recursively build a dictionary from the page tree for JSON serialization.

    :param tree: The current tree or subtree to process.
    :return: A dictionary representing the tree structure.
    """
    # Create the dictionary for the current page
    page_dict = {
        "page_name": tree.get('page_name', 'Unnamed Page'),
        "sub_pages": []
    }

    # Recursively process each sub-page and add it to the sub_pages list
    sub_pages = tree.get('sub_pages', [])
    for sub_page in sub_pages:
        page_dict["sub_pages"].append(build_page_tree_dict(sub_page))

    return page_dict

def save_page_tree_json(tree, file):
    """
    Save the entire page tree to a JSON file.

    :param tree: The root of the page tree.
    :param file: The open file object where the JSON will be written.
    """
    # Convert the tree into a dictionary starting with "pages" as the root key
    page_tree_dict = {
        "pages": [build_page_tree_dict(sub_page) for sub_page in tree.get('sub_pages', [])]
    }
    # Write the JSON to the file with indentation for readability
    json.dump(page_tree_dict, file, indent=4)

def save_page_tree_yaml(tree, file, level=0):
    """
    Recursively write the page tree to a YAML file with proper indentation.

    :param tree: The current tree or subtree to process.
    :param file: The open file object where the output will be written.
    :param level: The current depth level of the tree, used for indentation.
    """
    # Create the indentation for the current level
    indent = "  " * level
    # Write the current page's display name with indentation
    file.write(f"{indent}- page_name: {tree.get('page_name', 'Unnamed Page')}\n")

    # Check if there are sub_pages and add them recursively
    sub_pages = tree.get('sub_pages', [])
    if sub_pages:
        file.write(f"{indent}  sub_pages:\n")
        for sub_page in sub_pages:
            save_page_tree_yaml(sub_page, file, level + 2)

def save_page_tree_csv(tree, file, level=0):
    """
    Recursively write the page tree to a CSV file with commas representing levels.

    :param tree: The current tree or subtree to process.
    :param file: The open file object where the output will be written.
    :param level: The current depth level of the tree, used for indentation.
    """
    # Create the line with commas representing the level
    line = "," * level + tree.get('page_name', 'Unnamed Page') + "\n"
    file.write(line)

    # Recursively write each sub-page if there are any
    sub_pages = tree.get('sub_pages', [])
    for sub_page in sub_pages:
        save_page_tree_csv(sub_page, file, level + 1)

def save_page_tree(tree, file, level=0):
    """
    Recursively write the page tree to a file with proper indentation.

    :param tree: The current tree or subtree to process.
    :param file: The open file object where the output will be written.
    :param level: The current depth level of the tree, used for indentation.
    """
    # Write the current page's display name with indentation based on its level
    file.write("\t" * level + tree.get('page_name', 'Unnamed Page') + "\n")

    # Recursively write each sub-page if there are any
    sub_pages = tree.get('sub_pages', [])
    for sub_page in sub_pages:
        save_page_tree(sub_page, file, level + 1)

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

def main():
    with open("ui.eez-project") as f:
        data = yaml.safe_load(f)

    # Extract the tree structure from the specified variable
    for item in data['variables']['globalVariables']:
        if item['name'] == 'main_page_info':
            tree = item['defaultValue']

#    print(f"{tree}")
    with open("eez.extract.txt", 'w') as file:
        file.write(tree)


    # Load the page tree structure
    page_tree = yaml.safe_load(tree)

    # Print the page tree starting from the ++Startup
    sub_pages = page_tree.get('sub_pages', [])
    for sub_page in sub_pages:
        print_page_tree(sub_page)

    # Write the page tree starting from the first level to the specified file
    with open("PageTree.txt", "w") as output_file:
        for sub_page in page_tree.get('sub_pages', []):
            save_page_tree(sub_page, output_file)

    # Write the page tree to a CSV file starting from the first level
    with open("PageTree.csv", "w") as output_file:
        for sub_page in page_tree.get('sub_pages', []):
            save_page_tree_csv(sub_page, output_file, level=0)

    # Write the page tree to a YAML file starting from the first level
    with open("PageTree.yaml", "w") as output_file:
        output_file.write("pages:\n")
        for sub_page in page_tree.get('sub_pages', []):
            save_page_tree_yaml(sub_page, output_file, level=0)

    # Write the page tree to a JSON file
    with open("PageTree.json", "w") as output_file:
        save_page_tree_json(page_tree, output_file)

if __name__ == "__main__":
    main()

