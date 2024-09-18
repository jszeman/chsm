"""
chsm_backend

Usage:
    chsm_backend.py [options] [FILE]

Options:
    -s, --server-only     Do not open the application with Chrome app mode just wait for clients at http://127.0.0.1:8000/main.html
    -c, --code-gen        Generate code and quit. Don't start the GUI.
"""
import re
import eel
import json
from pathlib import Path
from docopt import docopt
import logging
from pprint import pprint
import collections
import jinja2

import tkinter as tk
from tkinter.filedialog import askopenfilename, asksaveasfilename

from hsm import StateMachine

class HtmlException(Exception):
    pass

TOP_STATE_NAME = r'chsm_result_ten\s+(?P<top_func>\w+)\(chsm_tst\s+\*self,\s+const\s+cevent_tst\s+\*e_pst,\s+chsm_call_ctx_tst\s+\*ctx_pst\)\s*;'

class ChsmException(Exception):
    pass

def save_html(html_fname: Path, drawing: str, json_data: str):
        backend_path =     (Path(__file__).parent).absolute().resolve()
        template_dir =     (backend_path / 'templates').absolute().resolve()
        template_html =    template_dir / 'template.html'
        drawing_js =       template_dir / 'wheel.js'
        web_dir =          (backend_path / '../web').absolute().resolve()
        drawing_css =      web_dir / 'drawing.css'

        if not template_html.exists():
            logging.error(f'File not found: {template_html}')
            eel.send_event('SAVE_RESULT', {
                'result': False,
                'file': f'{template_html}',
                'message': f'File not found: {template_html}',
                })
            return

        if not drawing_js.exists():
            logging.error(f'File not found: {drawing_js}')
            eel.send_event('SAVE_RESULT', {
                'result': False,
                'file': f'{drawing_js}',
                'message': f'File not found: {drawing_js}',
                })
            return

        if not drawing_css.exists():
            logging.error(f'File not found: {drawing_css}')
            eel.send_event('SAVE_RESULT', {
                'result': False,
                'file': f'{drawing_css}',
                'message': f'File not found: {drawing_css}',
                })
            return

        with open(template_html, 'r') as tmp_html, \
             open(drawing_css, 'r') as drw_css, \
             open(drawing_js, 'r') as drw_js, \
             open(html_fname, 'w') as html:
            template = tmp_html.read()
            css = drw_css.read()
            js = drw_js.read()
            output = template.format(style=css, drawing=drawing, json_data=json_data, script=js)
            html.write(output)
            logging.info(f'Saved drawing in {html_fname}')
            eel.send_event('SAVE_RESULT', {
                'result': True,
                'filepath': f'{html_fname}',
                'filename': html_fname.name,
                'message': f'Saved drawing in {html_fname}',
                })

def open_html(html_path):
    with open(html_path, 'r') as html:
        content = html.read()
        m = re.search(r"\<pre id='chsm-json-data'\>\n(?P<json>.+)\<\/pre\>", content, re.DOTALL)
        if not m:
            logging.error(f'No JSON data was found in file: {html_path}')
            return None
        try:
            return json.loads(m.group('json'))
        except json.JSONDecodeError as e:
            logging.error(f'JSON syntax error in html file {str(html_path)}. \nError message: {str(e)}')
            return None

class Project:
    def __init__(self, h_file_path=None):
        self.backend_path =     (Path(__file__).parent).absolute().resolve()
        self.template_dir =     (self.backend_path / 'templates').absolute().resolve()

        self.h_file_path =      Path(h_file_path) if h_file_path else self._open_header_dialog()

        logging.info(f'Opening project for {self.h_file_path}')

        if not self.h_file_path.exists():
            logging.error("Selected file {self.h_file_path} doesn't exists.")
            raise ChsmException("Selected file {self.h_file_path} doesn't exists.")

        self.user_config =      self._load_user_config(self.h_file_path)
        self.file_config =      self.user_config.get(self.h_file_path.name, {})

        # This is the new way to describe what the 
        self.jobs =             self.file_config.get("jobs", [])

        if self.jobs:
             logging.info(f'Found batch job descriptor. Number of jobs: {len(self.jobs)}')
             self.html_file_path = self.h_file_path.parent / Path(self.file_config["drawing"])
        else:
            logging.info(f'There are no batch output jobs defined for {self.h_file_path}')

        self.model = self._get_model(self.html_file_path)

        #pprint(self.model, indent=4)

    def _get_default_model(self):
        self.model_json =       self.template_dir / 'model.json'

        if not self.model_json.exists():
            raise ChsmException(f'File not found: {self.model_json}')

        with open(self.model_json, 'r') as model_file:
            return json.load(model_file)

    def _get_model(self, html_path):
        if not html_path.exists():
            logging.warning(f'File not found: {html_path}\n Creating new model.')
            return self._get_default_model()

        with open(html_path, 'r') as html:
            content = html.read()
            m = re.search(r"\<pre id='chsm-json-data'\>\n(?P<json>.+)\<\/pre\>", content, re.DOTALL)
            if not m:
                logging.error(f'No JSON data was found in file: {html_path}')
                return self._get_default_model()
            try:
                return json.loads(m.group('json'))
            except json.JSONDecodeError as e:
                logging.error(f'JSON syntax error in html file {str(html_path)}. \nError message: {str(e)}')
                return self._get_default_model()

    def _load_config_from_file(self, json_path):
        with open(json_path, 'r') as cfg_file:
            return json.load(cfg_file)
        
    def _find_file(self, dir_path, f_name):
        
        for parent in dir_path.parents:
            p = parent / f'.chsm/{f_name}'
            print(f'Searching settings in {p}')
            if p.exists():
                self.cfg_file_path = p
                return p

        return None

    def _find_user_config_file(self, h_dir, h_name):
        logging.info(f'_find_user_config_file({h_dir}, {h_name})')
        if h_name:
            f_name = self._find_file(h_dir, f'{h_name}.json')
            if f_name:
                return f_name
            
        return self._find_file(h_dir, 'settings.json')


    def _load_user_config(self, hpath):
        self.user_cfg_path = self._find_user_config_file(hpath.absolute().parent, hpath.stem)
        logging.info(f'User config file: {self.user_cfg_path}')
        if self.user_cfg_path == None:
            return {}

        return self._load_config_from_file(self.user_cfg_path)

    def _open_header_dialog(self):
        root = tk.Tk()
        root.attributes("-topmost", True)
        root.withdraw()
        filepath = askopenfilename(title='Open state mechine declaration', filetypes=(('C header file', '.h'), ('State chart', '.html')))
        logging.info(f'User selected path: {filepath}')
        return Path(filepath)

    def save_html(self, drawing: str, json_data: str):
        self.model = json.loads(json_data)
        save_html(self.html_file_path, drawing, json_data)

    def _run_job(self, job):
        if not "output" in job:
            raise ChsmException("Expected an 'output' key in the job descriptor.")

        file_path = self.h_file_path.parent / Path(job.get("output", ""))
        
        if not file_path.name:
            raise ChsmException("Expected a file name in the output key.")

        if not file_path.parent.exists():
            raise ChsmException(f"Output file directory does not exists: {str(file_path.parent.resolve().absolute())}")

        if self.user_cfg_path and Path(self.user_cfg_path).parent.exists():
            user_template_path = Path(self.user_cfg_path).parent
            template_path = [self.template_dir, user_template_path]
        else:
            template_path = self.template_dir

        with open(file_path, 'w') as output_file:
            sm = StateMachine(self.model)

            env = jinja2.Environment(loader=jinja2.FileSystemLoader(template_path), trim_blocks=True, lstrip_blocks=True)
            template = env.get_template(job['template'])

            data = sm.data.copy()
            data['template_params'] = job['template_params']

            with open("states.txt", 'w') as f:
                pprint(data, f, indent=4)

            output_file.write(template.render(data=data))

    def _run_jobs(self):
        for job in self.jobs:
            self._run_job(job)

    def generate_code(self):
        if self.jobs:
            self._run_jobs()
            return

project = None

@eel.expose
def save_state_machine(drawing: str, json_data: str, filepath: str):
    if project:
        project.save_html(drawing, json_data)
    elif filepath:
        save_html(Path(filepath), drawing, json_data)
    else:
        root = tk.Tk()
        root.attributes("-topmost", True)
        root.withdraw()
        filepath = asksaveasfilename(title='Save state mechine drawing', filetypes=(('State chart', '.html'),))
        logging.info(f'User selected path: {filepath}')
        save_html(Path(filepath), drawing, json_data)


@eel.expose
def open_file():
    root = tk.Tk()
    root.attributes("-topmost", True)
    root.withdraw()
    filepath = askopenfilename(title='Open state mechine declaration', filetypes=(('C header file', '.h'), ('State chart', '.html')))
    if not filepath:
        logging.info(f'File open canceled by user')
        return

    logging.info(f'User selected path: {filepath}')

    if filepath.endswith('.h'):
        global project
        project = Project(filepath)
        eel.load_json(json.dumps(project.model), Path(filepath).name, filepath)

    elif filepath.endswith('.html'):
        model = open_html(filepath)
        if model:
            eel.load_json(json.dumps(model), Path(filepath).name, filepath)

@eel.expose
def genereate_code():
    project.generate_code()

@eel.expose
def startup():
    if project:
        eel.load_json(json.dumps(project.model), Path(args['FILE']).name, args['FILE'])

if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO, format='%(asctime)s %(filename)-20s:%(lineno)-4s %(message)s')
    args = docopt(__doc__)
    print(args)

    #project = Project('/home/pi/projects/chsm/crf/test/tinc/chsm_test_machine.h')
    #project.generate_code()

    if args['FILE']:
        p = Path(args['FILE'])
        if p.exists():
            project = Project(p)
            if args['--code-gen']:
                project.generate_code()
                quit()

    eel.init((Path(__file__).parent / '../web').absolute().resolve())

    if args['--server-only']:
        eel.start('main.html', mode=None, port=0)
    else:
        eel.start('main.html', port=0)

