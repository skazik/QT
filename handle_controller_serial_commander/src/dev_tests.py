from page_tree import traverse_pagetree_yaml
from path_finder import PathFinder
from translations import Translator


def test_and_validate(ptree, test, generate, find=False, extract_and_save=False):
    # for testing & generation of test files
    # please don't delete, used to supress ruff error of unused ptree
    if not test:
        return

    if generate:
        traverse_pagetree_yaml(ptree.root, Translator().get_root_name())

    if find:
        path_finder_test = PathFinder()
        start_name = "page_elbow_1_sub"
        end_name = "page_reset_position"

        for _i in range(2):
            output_yaml = path_finder_test.traverse_pagetree_path(
                ptree.root, Translator().get_root_name(), start_name, end_name
            )
            for entry in output_yaml:
                if "Command" in entry:
                    print(f"- Command: {entry['Command']}", flush=True)
                if "Postcondition" in entry:
                    print(f"  Postcondition: {entry['Postcondition']}", flush=True)
            print(
                f"--- test path_finder from {start_name} to {end_name} done ---\n",
                flush=True,
            )
            # Swap start_name and end_name for the next iteration
            start_name, end_name = end_name, start_name

    if extract_and_save:
        ptree.print_and_save_tree("tmp._ui.extract.out.yaml")
