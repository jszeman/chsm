import eel

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

    eel.init('../web')
    eel.start('main.html')