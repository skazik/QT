import yaml
import json
import argparse

def build_page_tree_dict(tree, name_key):
    """
    Recursively build a dictionary from the page tree for JSON serialization.

    :param tree: The current tree or subtree to process.
    :param name_key: The key to use for the page name ('page_name' or 'page_display_name').
    :return: A dictionary representing the tree structure.
    """
    # Create the dictionary for the current page
    page_dict = {
        "   ": tree.get(name_key, 'Unnamed Page'),
        "sub_pages": []
    }

    # Recursively process each sub-page and add it to the sub_pages list
    sub_pages = tree.get('sub_pages', [])
    for sub_page in sub_pages:
        page_dict["sub_pages"].append(build_page_tree_dict(sub_page, name_key))

    return page_dict

def save_page_tree_json(tree, file, name_key):
    """
    Save the entire page tree to a JSON file.

    :param tree: The root of the page tree.
    :param file: The open file object where the JSON will be written.
    :param name_key: The key to use for the page name ('page_name' or 'page_display_name').
    """
    # Convert the tree into a dictionary starting with "pages" as the root key
    page_tree_dict = {
        "pages": [build_page_tree_dict(sub_page, name_key) for sub_page in tree.get('sub_pages', [])]
    }
    # Write the JSON to the file with indentation for readability
    json.dump(page_tree_dict, file, indent=4)

def save_page_tree_yaml(tree, file, level=0, name_key="page_display_name"):
    """
    Recursively write the page tree to a YAML file with proper indentation.

    :param tree: The current tree or subtree to process.
    :param file: The open file object where the output will be written.
    :param level: The current depth level of the tree, used for indentation.
    :param name_key: The key to use for the page name ('page_name' or 'page_display_name').
    """
    indent = "  " * level
    file.write(f"{indent}- {name_key}: {tree.get(name_key, 'Unnamed Page')}\n")

    sub_pages = tree.get('sub_pages', [])
    if sub_pages:
        file.write(f"{indent}  sub_pages:\n")
        for sub_page in sub_pages:
            save_page_tree_yaml(sub_page, file, level + 2, name_key)

def save_page_tree_csv(tree, file, level=0, name_key="page_display_name"):
    """
    Recursively write the page tree to a CSV file with commas representing levels.

    :param tree: The current tree or subtree to process.
    :param file: The open file object where the output will be written.
    :param level: The current depth level of the tree, used for indentation.
    :param name_key: The key to use for the page name ('page_name' or 'page_display_name').
    """
    line = "," * level + tree.get(name_key, 'Unnamed Page') + "\n"
    file.write(line)

    sub_pages = tree.get('sub_pages', [])
    for sub_page in sub_pages:
        save_page_tree_csv(sub_page, file, level + 1, name_key)

def save_page_tree(tree, file, level=0, name_key="page_display_name"):
    """
    Recursively write the page tree to a file with proper indentation.

    :param tree: The current tree or subtree to process.
    :param file: The open file object where the output will be written.
    :param level: The current depth level of the tree, used for indentation.
    :param name_key: The key to use for the page name ('page_name' or 'page_display_name').
    """
    file.write("\t" * level + tree.get(name_key, 'Unnamed Page') + "\n")

    sub_pages = tree.get('sub_pages', [])
    for sub_page in sub_pages:
        save_page_tree(sub_page, file, level + 1, name_key)

def print_page_tree(tree, level=0, name_key="page_display_name"):
    """
    Recursively print the page tree with proper indentation.
    """
    print("\t" * level + tree.get(name_key, 'Unnamed Page'))

    sub_pages = tree.get('sub_pages', [])
    for sub_page in sub_pages:
        print_page_tree(sub_page, level + 1, name_key)

def main():
    parser = argparse.ArgumentParser(description="Process either page name or display name.")
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("-n", "--page_name", action="store_true", help="Use 'page_name' as the identifier.")
    group.add_argument("-d", "--page_display_name", action="store_true", help="Use 'page_display_name' as the identifier.")
    args = parser.parse_args()

    name_key = "page_name" if args.page_name else "page_display_name"

    with open("ui.eez-project") as f:
        data = yaml.safe_load(f)

    for item in data['variables']['globalVariables']:
        if item['name'] == 'main_page_info':
            tree = item['defaultValue']

    with open("eez.extract.txt", 'w') as file:
        file.write(tree)

    page_tree = yaml.safe_load(tree)

    sub_pages = page_tree.get('sub_pages', [])
    for sub_page in sub_pages:
        print_page_tree(sub_page, name_key=name_key)

    with open("PageTree.txt", "w") as output_file:
        for sub_page in page_tree.get('sub_pages', []):
            save_page_tree(sub_page, output_file, name_key=name_key)

    with open("PageTree.csv", "w") as output_file:
        for sub_page in page_tree.get('sub_pages', []):
            save_page_tree_csv(sub_page, output_file, name_key=name_key)

    with open("PageTree.yaml", "w") as output_file:
        output_file.write("pages:\n")
        for sub_page in page_tree.get('sub_pages', []):
            save_page_tree_yaml(sub_page, output_file, name_key=name_key)

    with open("PageTree.json", "w") as output_file:
        save_page_tree_json(page_tree, output_file, name_key=name_key)

if __name__ == "__main__":
    main()
