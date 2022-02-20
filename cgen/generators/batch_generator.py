import io
import os
import re
import sys

class Batch:
    def __init__(self, chsm_path):
        self.chsm_path = chsm_path

    batch_file_template = """
@echo off

cd {chsm_root_path}
py.exe cgen\chsm_backend.py {module_path}/{name}/inc/{name}.h
"""

    def createFile(self, file_loc, file_name, content):
        file_dir = os.path.join(file_loc, file_name)
        try:
            if not os.path.exists(file_dir):
                fd_name = open(file_dir, "a+")
                fd_name.write(content)
                fd_name.close()  
        except OSError:
            pass

    def generate_batch_file(self, batch_path, module_path, module_name):
        self.createFile(os.path.join(str(batch_path), str(module_name)), (str(module_name) + "_start.bat"), self.batch_file_template.format(name=module_name, module_path=module_path, chsm_root_path=self.chsm_path) )
        
