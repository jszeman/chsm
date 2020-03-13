"""
Cgen.

Usage:
  cgen.py [options] <cfile>

Options:
  --inc_dir PATH    Include directory path [default: ../inc].
  --doc_dir PATH    Drawing directory path [default: ../doc].
  --config PATH     Config file path [default: ../doc/chsmconf.json]
"""
import re
import eel
import json
from pathlib import Path
from docopt import docopt

class HtmlException(Exception):
  pass

class File:
  def __init__(self, file_path, inc_dir, doc_dir):
    self.c_file_path =    (Path.cwd() / file_path).absolute().resolve()
    self.c_file_dir =     self.c_file_path.parent
    self.h_file_path =    (self.c_file_dir / inc_dir / (self.c_file_path.stem + '.h')).resolve()
    self.html_file_path = (self.c_file_dir / doc_dir / (self.c_file_path.stem + '.html')).resolve()

  def __repr__(self):
    return f'c: {self.c_file_path}, h: {self.h_file_path}, html: {self.html_file_path}'

  def write_html(self, content):
    with open(self.html_file_path, 'w') as html:
      html.write(content)

  def read_html(self):
    with open(self.html_file_path, 'r') as html:
      content = html.read()
      m = re.search("\<pre id='chsm-json-data'\>\\n(?P<json>.+)\<\/pre\>", content, re.DOTALL)
      if not m:
        return None

      try:
        data = m.group('json')
        j = json.loads(data)
        return data
      except json.decoder.JSONDecodeError as e:
        raise(HtmlException(str(e))) 

      

class Project:
  def __init__(self):
    self.backend_path =   Path(__file__).parent.absolute()
    self.web_dir =        (self.backend_path / '../web').absolute().resolve()
    self.template_dir =   (self.backend_path / 'templates').absolute().resolve()

    self.template_html =  self.template_dir / 'template.html'
    self.drawing_js =  self.template_dir / 'wheel.js'
    self.drawing_css = self.web_dir / 'drawing.css'
    self.files = []

  def __repr__(self):
    retval = f'backend: {self.backend_path} web_dir: {self.web_dir}, templates: {self.template_dir}\nFiles:\n'
    for f in self.files:
      retval += f'    {repr(f)}\n'
    
    return retval

  def save_html(self, drawing: str, json_data: str):
    with open(self.template_html, 'r') as tmp_html, \
         open(self.drawing_css, 'r') as   drw_css, \
         open(self.drawing_css, 'r') as   drw_css, \
         open(self.drawing_js, 'r') as    drw_js:
      template = tmp_html.read()
      css = drw_css.read()
      js = drw_js.read()
      output = template.format(style=css, drawing=drawing, json_data=json_data, script=js)
      for f in self.files:
        f.write_html(output)

  def add_file(self, file_path, inc_dir='../inc', doc_dir='../doc'):
    self.files.append(File(file_path, inc_dir, doc_dir))

  def open_html(self):
    eel.load_files({f.c_file_path.name:f.read_html() for f in self.files})

project = Project()

@eel.expose
def save_state_machine(drawing: str, json_data: str):
  project.save_html(drawing, json_data)

@eel.expose
def open_state_machine():
  return project.open_html()

if __name__ == '__main__':
    args = docopt(__doc__)

    project.add_file(args['<cfile>'], args['--inc_dir'], args['--doc_dir'])

    eel.init(project.web_dir)
    eel.start('main.html')