import json
import re
import logging
from datetime import datetime
from .ast import Func, If, Switch, Case, Call, Break, Return, Blank, Expr, Ast, FuncDeclaration, Include, Ifndef, Define, Endif, Comment, Decl, Assignment, Enum
from typing import List, Dict

# TODO: make sure, that there is only one exit function and it has no parameters

# Match the following patterns:
#   event
#   event / func()
#   event [guard()] / func()
#   event [guard()]
#   [guard()] / func()
#   [guard()]
EVENT_PATTERN = r'^(\s*(?P<signal>\w+)\s*)*(\[(?P<guard>\w+)\((?P<gparams>[\w,\s]*)\)\])*(\s*/\s*(?P<function>\w+)(?P<parens>\((?P<fparams>[\w,\s]*)\))*\s*)*'

class StateMachine:
    def __init__(self, data, h_file, funcs_h_file, templates, file_config,):
        self.templates = templates
        self.file_config = file_config
        self.machine_h = h_file.name
        self.funcs_h = funcs_h_file
        self.prefix = h_file.stem
        
        self.user_funcs = set()
        self.user_guards = set()
        self.user_signals = set()
        self.user_inc_funcs = set()

        self.states = self.get_states(data)                 # Extract states from the data
        self.add_transitions_to_states(self.states, data)   # Extract transition info from the data and add it to the states
        self.process_transitions(self.states)               # Calculate transition exit and entry paths and the exact end state
        self.notes = data['notes']

        top_func = self._get_user_symbols(h_file)

        logging.info(f'Extracted top_func = {top_func} from file {h_file}')

        self.top_func = self.file_config.get('top_func', top_func)

        logging.info(f'Final top_func = {self.top_func}')

        self.templates['user_func_args'] = self.templates['user_func_args_t']
        self.templates['user_func_params'] = self.templates['user_func_params_t']

        self.states['__top__']['title'] = self.top_func
        self.resolve_parent_titles(self.states)

        self.ast = self.build_ast(self.states)

        self.h_ast = self.build_user_declarations(self.user_funcs, self.user_guards, self.states)

    def _get_user_symbols(self, hpath):
        top_func = None, None

        with open(hpath, 'r') as h_file:
            h_data = h_file.read()

            m = re.search(self.templates['top_state_name'], h_data)
            if m:
                top_func = m.group('top_func')
            else:
                logging.warning(f'Could not find a state handler function declaration in file {hpath}')

        return top_func

    def build_user_declarations(self, funcs, guards, states):
        ast = Ast()

        symbol = self.funcs_h.replace('.', '_').upper()

        ast.nodes.append(Ifndef(symbol))
        ast.nodes.append(Define(symbol))
        ast.nodes.append(Blank())

        ast.nodes.append(Comment(f'Generated with CHSM v0.0.0 at {datetime.strftime(datetime.now(), "%Y.%m.%d %H.%M.%S")}'))
        ast.nodes.append(Blank())
        ast.nodes.append(Blank())

        ast.nodes.append(Include(self.machine_h))

        for i in self.templates['h_include_list']:
            ast.nodes.append(Include(i))

        for f in sorted(funcs):
            ast.nodes.append(Blank())
            comment = self.notes.get(f'{f}()', '')
            if comment:
                ast.nodes.append(Blank())
                ast.nodes.append(Comment(comment))
            ast.nodes.append(FuncDeclaration(f, 'void', self.templates['user_func_params']))

        ast.nodes.append(Blank())

        for g in sorted(guards):
            ast.nodes.append(Blank())
            comment = self.notes.get(f'{g}()', '')
            if comment:
                ast.nodes.append(Blank())
                ast.nodes.append(Comment(comment))
            ast.nodes.append(FuncDeclaration(g, self.templates['guard_return_type'], self.templates['user_func_params']))

        ast.nodes.append(Blank())
        ast.nodes.append(Blank())

        enum = Enum(f'{self.prefix}_state_id_ten', {s['title'].upper(): s['num'] for i, s in states.items() if 'num' in s})
        ast.nodes.append(enum)

        ast.nodes.append(Blank())
        ast.nodes.append(Blank())

        signal_str = f'\nSignals:\n'

        max_sig_len = max([len(s) for s in self.user_signals])

        for s in sorted(self.user_signals):
            if s not in ['entry', 'exit', 'init']:
                signal_str += f'    {s:{max_sig_len}} {self.notes.get(s, "")}\n'

        ast.nodes.append(Comment(signal_str))

        ast.nodes.append(Blank())
        ast.nodes.append(Blank())

        notes_str = f'\nOther function notes:\n'

        for f in sorted(self.user_inc_funcs):
                notes_str += f'\n{f}:\n'
                note = self.notes.get(f, '').strip()
                if note:
                    note = note.replace('\n', '\n    ')
                    notes_str += f"    {note}\n"

        ast.nodes.append(Comment(notes_str))

        ast.nodes.append(Blank())
        ast.nodes.append(Endif())

        return ast

    def get_transition_funcs(self, start, end):
        path = self.get_transition_path(start, end)
        return self.path_to_funcs(path), path[-1][0]

    def process_transitions(self, states):
        for s_id, s in states.items():
            for sig_id, sig in s['signals'].items():
                if sig_id == 'init':
                    continue
                for g in sig['guards'].values():
                    if g['target']:
                        tfuncs, target = self.get_transition_funcs(s_id, g['target'])
                        #g['transition_funcs'] = tfuncs
                        g['funcs'].extend(tfuncs)
                        g['target_title'] = states[target]['title']
        
        #The only place where transition functions are needed for an init signal is the __top__
        g = states['__top__']['signals']['init']['guards'][(None, None)]
        tfuncs, target = self.get_transition_funcs('__top__', g['target'])
        g['funcs'].extend(tfuncs)
        g['target_title'] = states[target]['title']


    def str_to_signal(self, line, target=None, target_title=None, initial=False):
        signal = None
        guard = None
        func = None
        parens = None
        fparams = None
        gparams = None

        m = re.search(EVENT_PATTERN, line)
        if m:
            signal = m.group('signal')
            guard = m.group('guard')
            func = m.group('function')
            parens = m.group('parens')
            fparams = m.group('fparams')
            gparams = m.group('gparams')

            if func and parens:
                if fparams:
                    self.user_inc_funcs.add(func)
                else:
                    self.user_funcs.add(func)

            if guard:
                if gparams:
                    self.user_inc_funcs.add(guard)
                else:
                    self.user_guards.add(guard)

            if signal:
                self.user_signals.add(signal)

        if initial:
            signal = 'init'
            guard = None
            gparams = None

        g = {
            'guard': (guard, gparams),
            'funcs': [(func, fparams)],
            'target': target,
            'target_title': target_title
        }

        s = {
            'name': signal,
            'guards': {
                g['guard']: g
            }
        }

        return s

    def add_signal_to_state(self, state, signal):
        signal_name = signal['name']

        if signal_name not in state['signals']:
            state['signals'][signal_name] = signal
        else:
            orig_signal = state['signals'][signal_name]

            for g_fn, g in signal['guards'].items():
                if g_fn  in orig_signal['guards']:
                    orig_guard = orig_signal['guards'][g_fn]

                    # Add functions to the original guard
                    orig_guard['funcs'].extend(g['funcs'])

                    # Set the target
                    if g['target']:
                        if orig_guard['target'] and orig_guard['target'] != g['target']:
                            # The old and the new guard has different targets. For now, keep the original, but drop a lin in the log
                            logging.error(f"Guard target mismatch for {g_fn} in {state['title']}: {orig_guard['target']} != {g['target']}")
                        else:
                            orig_guard['target'] = g['target']

                else:
                    orig_signal['guards'][g_fn] = g
    
    def get_states(self, data):
        """Extract state info from drawing data into a dict"""
        states = {}

        for s_id, s in data['states'].items():
            state = {
                'signals': {
                    None: {
                        'name': None,
                        'guards': {},
                    }
                },
                'parent': s['parent'],
                'children': s['children'],
                'title': s['title'],
                'type': s['type'],
                }

            if s_id.startswith('state_'):
                state['num'] = int(re.findall(r'\d+', s_id)[0])

            for line in s['text']:
                s = self.str_to_signal(line)
                self.add_signal_to_state(state, s)

            states[s_id] = state

        return states

    def resolve_parent_titles(self, states):
        for s in states.values():
            if s['parent']:
                s['parent_title'] = states[s['parent']]['title']
    
    def resolve_transition(self, tr, data):
        """Return the label, the start and the end states of a transition"""
        start_conn = data['connectors'][tr['start']]
        end_conn = data['connectors'][tr['end']]

        start_state = start_conn['parent']
        end_state = end_conn['parent']
        event = tr['label']

        return start_state, end_state, event

    def add_transitions_to_states(self, states, data):
        """Go through the transitions in the state machine, and add them
        to the start states"""

        for tr in data['transitions'].values():
            start, target, label = self.resolve_transition(tr, data)

            if states[start]['type'] == 'initial':
                start = states[start]['parent']
                states[start]['initial'] = target

                signal = self.str_to_signal(label, target=target, target_title=states[target]['title'], initial=True)
            else:
                signal = self.str_to_signal(label, target, target_title=states[target]['title'], initial=False)

            self.add_signal_to_state(states[start], signal)

    def make_call(self, func, params, standalone=False):
        fparams = f', {params}' if params else ''
        return Call(func, self.templates['user_func_args'] + fparams, standalone)

    def guard_to_ast(self, guard):
        nodes = []
        add = nodes.append

        func, param = guard['guard']

        if func:
            i = If(self.make_call(func, param))
            add(i)
            add = i.add_true

        if guard['target']:
            add(Call(self.templates['exit_children'], self.templates['func_args']))
        
        for func, param in guard['funcs']:
            if func:
                add(self.make_call(func, param, True))

        if guard['target']:
            if self.templates['set_state_id'] != "":
                add(self.make_call(self.templates['set_state_id'], guard['target_title'].upper(), True))
            add(Return(self.templates['trans_result'].format(target=guard['target_title'])))

        return nodes


    def build_case_from_signal(self, signal):
        name = signal['name']

        if name == 'init':
            name = self.templates['init_signal']
        else:
            name = f'{self.templates["signal_prefix"]}{name}'

        c = Case(name)

        for guard in signal['guards'].values():
            nodes = self.guard_to_ast(guard)
            for n in nodes:
                c.add(n)
        
        if c.nodes[-1].node_type != 'return':
            c.add(Break())

        return c

    def build_func_from_state(self, state_id, state, insert_init=False, spec=''):
        f = Func(name=state['title'], ftype=self.templates['func_return_type'], params=self.templates['func_params'], spec=spec)
        f.add(Decl('bool', self.templates['guards_only_variable'], 'true'))
        s = Switch(self.templates['switch_variable'])
        f.add(s)

        for signal, e in state['signals'].items():
            if signal in ['entry', 'exit', None]:
                continue
            elif signal == 'init' and not insert_init:
                continue

            c = self.build_case_from_signal(e)
            if c:
                s.add_case(c)

        s.add_default(Assignment(self.templates['guards_only_variable'], 'false'))

        for guard in state['signals'][None]['guards'].values():
            f.add(Blank())
            nodes = self.guard_to_ast(guard)
            for n in nodes:
                f.add(n)

        f.add(Blank())

        exit_func = 'NULL'
        efw = None
        try:
            exit_func, exit_param = state['signals']['exit']['guards'][(None, None)]['funcs'][0]
            print(f"{state['title']} exit: {exit_func}({exit_param})")

            if exit_param:
                exit_func_wrapper = f"{state['title']}_exit_func_wrapper"
                efw = Func(name=exit_func_wrapper, ftype='void', params=self.templates['user_func_params_t'], spec='static')
                efw.add(self.make_call(exit_func, exit_param, True))
                exit_func = exit_func_wrapper

        except KeyError:
            pass

        if exit_func == None:
            exit_func = 'NULL'

        if state['parent']:
            result = self.templates['parent_result'].format(parent=state['parent_title'], exit_func=exit_func)
        else:
            result = self.templates['ignored_result']

        f.add(Return(result))

        return f, efw

    def build_ast(self, states):
        ast = Ast()
        ast.nodes.append(Blank())
        for s_id, s in states.items():
            if s['type'] != 'normal':
                continue
            state_func, exit_func_wrapper = self.build_func_from_state(s_id, s, spec='static') 

            if exit_func_wrapper:
                ast.nodes.append(exit_func_wrapper)

            ast.nodes.append(state_func)
            ast.nodes.insert(0, state_func.declaration())


        top_func, _ = self.build_func_from_state(self.top_func, states['__top__'], True)
        ast.nodes.append(top_func)

        ast.nodes.insert(0, Blank())
        ast.nodes.insert(0, Include(self.funcs_h))
        ast.nodes.insert(0, Include(self.machine_h))
        for i in self.templates['c_include_list']:
            ast.nodes.insert(0, Include(i))

        
        ast.nodes.insert(0, Blank())
        ast.nodes.insert(0, Comment(f'Generated with CHSM v0.0.0 at {datetime.strftime(datetime.now(), "%Y.%m.%d %H.%M.%S")}'))

        return ast


    def get_entry_path(self, start_state_id, target_state_id):
        
        if start_state_id == target_state_id:
            return tuple()
        
        state_id = target_state_id
        path = []
        while True:
            path.append((state_id, 'entry'))
            
            super_id = self.states[state_id]['parent']
            
            if super_id == start_state_id:
                p = tuple([e for e in reversed(path)])
                return reversed(path)
            else:
                state_id = super_id
    
    def is_in(self, state_id, composit_state_id):
        if composit_state_id == '__top__':
            return True
        elif state_id == '__top__':
            return False
        
        while True:
            super_id = self.states[state_id]['parent']
            
            if super_id == '__top__':
                return False
            elif super_id == composit_state_id:
                return True
            else:
                state_id = super_id
        
    def get_LCA(self, start_state_id, target_state_id):
        """Find Lowest Common Ancestor"""
        
        if self.is_in(start_state_id, target_state_id):
            return target_state_id
        
        super_id = start_state_id
        while True:
            if self.is_in(target_state_id, super_id):
                return super_id
            else:
                super_id = self.states[super_id]['parent']
    
    def get_exit_path(self, start_state_id, target_state_id):
        path = []
        s = start_state_id
       
        while s != target_state_id:
            path.append((s, 'exit'))
            s = self.states[s]['parent']
             
        return path
    
    def get_transition_path(self, start_state_id, target_state_id):
        if target_state_id == '__top__':
            return self.get_exit_path(start_state_id, '__top__')
        else:
            lca = self.get_LCA(start_state_id, target_state_id)
            exit_path = self.get_exit_path(start_state_id, lca)
            init_path = self.get_init_path(lca, target_state_id)
            return exit_path + init_path
    
    def get_init_path(self, start_state_id, target_state_id):
        '''This func enters the target state and then enter the init state of the
        target, if defined'''
        path = []
        while True:
            entry_path = self.get_entry_path(start_state_id, target_state_id)
            path.extend(entry_path)
            
            path.append((target_state_id, 'init'))
            init_state_id = self.states[target_state_id].get('initial', None)
            if init_state_id == None:
                return path
            else:
                start_state_id = target_state_id
                target_state_id = init_state_id
    
    def path_to_funcs(self, path):
        funcs = []
        
        for step in path:
            state_id, event_id = step
            try:
                funcs.extend(self.states[state_id]['signals'][event_id]['guards'][(None, None)]['funcs'])
            except KeyError:
                pass

        return tuple(funcs)

"""
Example dictionary layout:

{   '__top__': {   'children': ['state_0', 'istate_3'],
                   'initial': 'state_0',
                   'parent': None,
                   'signals': {   None: {'guards': {}, 'name': None},
                                  'init': {   'guards': {   (None, None): {   'funcs': [(None, None)],
                                                                              'guard': (None, None),
                                                                              'target': 'state_0',
                                                                              'target_title': 's'}},
                                              'name': 'init'}},
                   'title': '__top__4',
                   'type': 'top'},

    'istate_0': {   'children': [],
                    'parent': 'state_4',
                    'signals': {   None: {'guards': {}, 'name': None},
                                   'entry': {   'guards': {   (None, None): {   'funcs': [(None, None)],
                                                                                'guard': (None, None),
                                                                                'target': None,
                                                                                'target_title': None}},
                                                'name': 'entry'},
                                   'exit': {   'guards': {   (None, None): {   'funcs': [(None, None)],
                                                                               'guard': (None, None),
                                                                               'target': None,
                                                                               'target_title': None}},
                                               'name': 'exit'}},
                    'title': 'istate_0',
                    'type': 'initial'},
    
    'state_0': {   'children': ['state_1', 'state_3', 'istate_2', 'state_6', 'state_7'],
                   'initial': 'state_1',
                   'num': 0,
                   'parent': '__top__',
                   'signals': {   None: {'guards': {}, 'name': None},
                                  'E': {   'guards': {   (None, None): {   'funcs': [(None, None)],
                                                                           'guard': (None, None),
                                                                           'target': 'state_2',
                                                                           'target_title': 's11'}},
                                           'name': 'E'},
                                  'J': {   'guards': {   (None, None): {   'funcs': [(None, None)],
                                                                           'guard': (None, None),
                                                                           'target': 'state_6',
                                                                           'target_title': 's3'}},
                                           'name': 'J'},
                                  'entry': {   'guards': {   (None, None): {   'funcs': [('s_entry', '')],
                                                                               'guard': (None, None),
                                                                               'target': None,
                                                                               'target_title': None}},
                                               'name': 'entry'},
                                  'exit': {   'guards': {   (None, None): {   'funcs': [('s_exit', '')],
                                                                              'guard': (None, None),
                                                                              'target': None,
                                                                              'target_title': None}},
                                              'name': 'exit'},
                                  'init': {   'guards': {   (None, None): {   'funcs': [('s_init', ''), (None, None)],
                                                                              'guard': (None, None),
                                                                              'target': 'state_1',
                                                                              'target_title': None}},
                                              'name': 'init'}},
                   'title': 's',
                   'type': 'normal'},
    'state_1': {   'children': ['state_2', 'istate_4'],
                   'initial': 'state_2',
                   'num': 1,
                   'parent': 'state_0',
                   'signals': {   None: {   'guards': {   ('j_guard', ''): {   'funcs': [(None, None)],
                                                                               'guard': ('j_guard', ''),
                                                                               'target': 'state_3',
                                                                               'target_title': 's2'},
                                                          ('s1_guard', ''): {   'funcs': [('s1_func', '')],
                                                                                'guard': ('s1_guard', ''),
                                                                                'target': None,
                                                                                'target_title': None}},
                                            'name': None},
                                  'ID': {   'guards': {   (None, None): {   'funcs': [('s1_func', '')],
                                                                            'guard': (None, None),
                                                                            'target': None,
                                                                            'target_title': None}},
                                            'name': 'ID'},
                                  'entry': {   'guards': {   (None, None): {   'funcs': [('s1_entry', '')],
                                                                               'guard': (None, None),
                                                                               'target': None,
                                                                               'target_title': None}},
                                               'name': 'entry'},
                                  'exit': {   'guards': {   (None, None): {   'funcs': [('s1_exit', '')],
                                                                              'guard': (None, None),
                                                                              'target': None,
                                                                              'target_title': None}},
                                              'name': 'exit'},
                                  'init': {   'guards': {   (None, None): {   'funcs': [('s1_init', ''), (None, None)],
                                                                              'guard': (None, None),
                                                                              'target': 'state_2',
                                                                              'target_title': None}},
                                              'name': 'init'}},
                   'title': 's1',
                   'type': 'normal'},
    }

"""