import eel
import tkinter as tk
from tkinter.filedialog import askopenfilename, asksaveasfilename

class New_module:
    def __init__(self):
        pass

    @eel.expose
    def browse_loc(self):
        root = tk.Tk()
        root.attributes("-topmost", True)
        root.withdraw()
        filepath = askopenfilename(title='Open state mechine declaration', filetypes=(('C header file', '.h'), ('State chart', '.html')))
        if not filepath:
            logging.info(f'File open canceled by user')
            return
        logging.info(f'User selected path: {filepath}')
        
    def create(self):
        pass

# nm = New_module()
# nm.browse_loc()