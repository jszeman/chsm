"""
Cgen.

Usage:
  cgen.py [options] <cfile>

Options:
  --inc_dir PATH    Include directory path [default: ../inc].
  --doc_dir PATH    Drawing directory path [default: ../doc].
  --config PATH     Config file path [default: ../doc/chsmconf.json]
"""
import eel
from pathlib import Path
from os import path
from docopt import docopt

@eel.expose
def save_state_machine(drawing: str, json_data: str):
    f = open('/home/pi/projects/mstate/cgen/templates/template.html', 'r')
    css_file = open('/home/pi/projects/mstate/web/drawing.css', 'r')
    style = css_file.read()
    template = f.read()
    output = template.format(style=style, drawing=drawing, json_data=json_data)
    f.close()
    css_file.close()
    f = open('test.html', 'w')
    f.write(output)
    f.close()
    print(output)

if __name__ == '__main__':
    args = docopt(__doc__)
    backend_path = Path(__file__).parent.absolute()
    webdir = (backend_path / '../web').absolute().resolve()
    target_file = backend_path / args['<cfile>']
    target_dir = target_file.parent
    inc_file = target_dir / args['--inc_dir'] / (target_file.stem + 'h')
    html_file = target_dir / args['--doc_dir'] / (target_file.stem + 'html')

    print(target_file, inc_file, html_file)

    eel.init(webdir)
    eel.start('main.html')