import eel
import tkinter as tk
from tkinter.filedialog import askopenfilename, asksaveasfilename, askdirectory
from pathlib import Path
from docopt import docopt
import logging

from .directory_generator import DirGen
from .cmake_generator import CmakeGen
from .batch_generator import Batch

class New_module:
    def __init__(self):
        pass
        
@eel.expose
def generate_module(name : str, 
                    version : str, 
                    description : str,
                    module_location : str, 
                    batch_location : str, 
                    cmake_target_mode_selector : str,
                    batch_cb :str,
                    cmake_cb : str):
    print("Generate module!")
    print(name)
    print(module_location)
    print(batch_location)
    dg.generate_modul_dirs(module_location, name, version, description)
    print("Generate module directories!")
    if(batch_cb):
        bg.generate_batch_file(batch_location, module_location, name)
    print("Generate module batch file!")
    if(cmake_cb):
        if(cmake_target_mode_selector == "library_opt"):
            cg.generate_cmake_files(module_location, name, "library")
        elif(cmake_target_mode_selector == "executable_opt"):
            cg.generate_cmake_files(module_location, name, "executable")
    eel.successful_generate()

@eel.expose
def browse_dir_path():
    root = tk.Tk()
    root.attributes("-topmost", True)
    root.withdraw()
    filepath = askdirectory()
    if not filepath:
        logging.info(f'File open canceled by user')
        return
    logging.info(f'User selected path: {filepath}')
    path = Path(filepath).parent
    print(path)
    eel.set_choosen_path_to_input_text(filepath)

@eel.expose
def create_project():
    eel.start('new_module/new_module.html', port=0, mode='None') 

nm = New_module()
dg = DirGen()
bg = Batch((Path(__file__).parent / '../../').absolute().resolve())
cg = CmakeGen()
