import io
import os
import re
import sys

class Batch:
    def __init__(self, bat_file = None):
        self.chsm_path = None
        self.bat_file_path = None

    def create(type, path):
    root_exists = os.path.exists(path)
    if root_exists and os.path.isdir(path) and len(os.listdir(path)) != 0:
        print(
            f"Error - directory exists and is not empty:\n{path}",
            file=sys.stderr,
        )
        exit(1)
    if type != "":
        with contextlib.redirect_stdout(io.StringIO()):
            d = get_substitutes(type, os.path.basename(path))
    else:
        d = get_substitutes(None, os.path.basename(path))
    project = {
        **root_dir(d),
        "cmake": cmake_dir(d),
        "test": test_dir(d),
        "include": include_dir(d),
        "source": (d["type_id"] != "h", source_dir, d),
        "example": (d["exclude_examples"] == "n", example_dir, d),
    }
    if not root_exists:
        os.mkdir(path)
    write_dir(path, project)
    git_init(path)
    print_tips(d)

def main():
    print("Hello")

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        pass