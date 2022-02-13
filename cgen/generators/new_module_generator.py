import eel
import tkinter as tk
from tkinter.filedialog import askopenfilename, asksaveasfilename
from pathlib import Path
from docopt import docopt
import logging

class New_module:
    def __init__(self):
        pass
        
    def create(self):
        pass

@eel.expose
def generate_module(name : str, version : str, description : str, location : str):
    print("Generate module!")
    pass

@eel.expose
def browse_loc():
    root = tk.Tk()
    root.attributes("-topmost", True)
    root.withdraw()
    filepath = askopenfilename(title='Open state mechine declaration', filetypes=(('C header file', '.h'), ('State chart', '.html')))
    if not filepath:
        logging.info(f'File open canceled by user')
        return
    logging.info(f'User selected path: {filepath}')

@eel.expose
def create_project():
    eel.start('new_project/new_project.html', port=0, mode='None') 

nm = New_module()

eel.init((Path(__file__).parent / '../../web').absolute().resolve())
create_project()