import eel
import tkinter as tk
from tkinter.filedialog import askopenfilename, asksaveasfilename

class New_project:
    def __init__(self) -> None:
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
