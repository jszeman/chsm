'''
Created on 2019. febr. 6.

@author: jszeman
'''

import re
from cgen import plantuml_reader as pr

def read_c(filename):
    with open(filename) as f:
        s = f.read()
    
    return s

def find_comments(s):
    state = 'code'
    c_start = 0
    c_end = 0
    comments = []
    for i in range(len(s)):
        if state == 'code':
            if s[i: i+2] == '/*':
                c_start = i
                state = 'block_comment'
            elif s[i: i+2] == '//':
                c_start = i
                state = 'line_comment'
        elif state == 'block_comment':
            if s[i: i+2] == '*/':
                c_end = i+2
                comments.append((c_start, c_end))
                state = 'code' 
        elif state == 'line_comment':
            if s[i] == '\n':
                c_end = i
                comments.append((c_start, c_end))
                state = 'code'
    
    return comments

def is_comment(r, comments):
    for c in comments:
        if r[0] > c[0] and r[1] <= c[1]:
            return True
        
    return False
                
def find_functions(s, comments):
    hsm_regexp = 'uint16_t\s+(\w+)\(CHsm\s+\*this\s*\,\s*const\s+CEvent\s+\*e\)(?=\s*\{)'
    
    matches = re.finditer(hsm_regexp, s)
    
    for m in matches:
        if not is_comment(m.span(), comments):
            print(m.group(1), m.span())

def make_cases(functions, transitions, states, state):
    events = []
    for f in functions:
        if state == f[0]:
            events.append(f[1])
    for f in transitions:
        if state == f[0]:
            events.append(f[1])
    
    cases = str()
    
    for e in events:
        cases += f'        case {e}:\n'
        for f in functions[(state, e)]:
            cases += f'            {f}(this, e);\n'
        if (state, e) in transitions:
            cases += f'            this->state_handler = {transitions[(state, e)]};\n'
        
        cases += f'            return;\n\n'
        
    return cases
            

def make_chsm(uml_str):
    
    fwd_declarations = []
    func_definitions = str()
    
    p = pr.PlantUML(uml_str)
    functions, transitions, states = p.state_chart.get_flat_events()
    
    for state in states:
        if state != '__top__':
            fwd_declarations.append(f'void {state}(CHsm *this, const CEvent *e);\n')
            func = f'void {state}(CHsm *this, const CEvent *e)\n{{\n    switch(e->signal) {{\n'
            func += make_cases(functions, transitions, states, state)
            func += '    }\n}\n\n'
            func_definitions += func
            
    print(''.join(fwd_declarations), '\n\n', func_definitions)
        
def get_plantuml_text(s, comment_coords):
    plantuml_pattern = '\@startuml.*\@enduml'
    
    uml_list = []
    
    for start, end in comment_coords:
        comment = s[start: end]
        m = re.search('@startuml.*@enduml', comment, re.DOTALL)
        if m:
            u_start, u_end = m.span()
            uml_list.append((u_start+start, u_end+start))
            make_chsm(m.group(0))
    
    return uml_list

if __name__ == '__main__':
    s = read_c('../../crf/test/tsrc/chsm_test_functions.c')
    c_coords = find_comments(s)
    find_functions(s, c_coords)
    uml_list = get_plantuml_text(s, c_coords)
    make_chsm(str(uml_list[0]))