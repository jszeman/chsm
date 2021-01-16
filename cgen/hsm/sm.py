
import json
import re
import logging
from datetime import datetime
from pprint import pprint
from .ast import Func, If, Switch, Case, Call, Break, Return, Blank, Expr, Ast, FuncDeclaration, Include, Ifndef, Define, Endif, Comment, Decl, Assignment, Enum
from typing import List, Dict

# Match the following patterns:
#   event
#   event / func()
#   event [guard()] / func()
#   event [guard()]
#   [guard()] / func()
#   [guard()]
EVENT_PATTERN = r'^(\s*(?P<signal>\w+)\s*)*(\[(?P<guard>\w+)\((?P<gparams>[\w,\s]*)\)\])*(\s*/\s*(?P<function>\w+)(?P<parens>\((?P<fparams>[\w,\s]*)\))*\s*)*'

class Event:
    def __init__(self, signal, guard=None, func=None, fparams=None, gparams=None, target=None):
        self.signal = signal
        self.func = func
        self.guard = guard
        self.target = target
        self.fparams = fparams
        self.gparams = gparams
        self.target_title = None
        self.trans_guard = None
        self.trans_func = None
        self.trans_funcs = None
        self.trans_fparams = None
        self.trans_gparams = None

    def __repr__(self):
        fargs = f'({self.fparams})' if self.fparams else ''
        gargs = f'({self.gparams})' if self.fparams else ''
        return f'Event({self.signal}, guard={self.guard}{gargs}, func={self.func}{fargs}, target={self.target}, tguard={self.trans_guard}, tfunc={self.trans_func}, {self.trans_funcs})'
"""
class Function:
    def __init__(self, name: str, params: str = None):
        self.name: str = name
        self.params: str = params

    def __repr__(self):
        return f'Function({self.name}, {self.params})'

class Transition:
    def __init__(self, target: str):
        self.target: str = target
        self.funcs: List[Function] = []

    def add_func(self, func: Function):
        self.funcs.append(func)

    def __repr__(self):
        return f'Transition({self.target}): ' + ' '.join([str(f) for f in self.funcs]) 

class Guard:
    def __init__(self, guard: Function, func: Function = None, target: str = None):
        self.guard: Function = guard
        self.func: Function = func
        self.trans: Transition = Transition(target)
        self.trans.add_func(func)

    def __repr__(self):
        return f'Guard({self.guard}, {self.func}): {self.trans}'

class Signal:
    def __init__(self, name: str, target: str = None, func: Function = None):
        self.name: str = name
        self.func: Function = func
        self.guards: List[Guard] = {}
        self.trans: Transition = Transition(target)
        self.trans.add_func(func)

    def __repr__(self):
        return f'Signal({self.name}, {self.func}): {self.trans} ' + ' '.join([str(g) for g in self.guards]) 

class State:
    def __init__(self, title: str, num: int):
        self.title: str = title
        self.num: int = num
        self.signals: Dict[Signal] = {}
        self.guards: List[Guard] = []

    def __repr__(self):
        return f'State({self.title}, {self.num}): {self.trans} ' + ' '.join([str(g) for g in self.guards]) 
"""
class StateMachine:
    def __init__(self, data, h_file, funcs_h_file, templates, file_config,):
        #pprint(data, indent=4)
        self.templates = templates
        self.file_config = file_config
        self.machine_h = h_file.name
        self.funcs_h = funcs_h_file
        self.prefix = h_file.stem
        
        self.user_funcs = set()
        self.user_guards = set()
        self.user_signals = set()
        self.user_inc_funcs = set()

        self.states = self.get_states(data)
        self.add_transitions_to_states(self.states, data)
        pprint(self.states, indent=4, width=150)

        '''
        self.process_transitions(self.states)
        self.notes = data['notes']

        #pprint(self.states, indent=4)

        top_func = self._get_user_symbols(h_file)

        logging.info(f'Extracted top_func = {top_func} from file {h_file}')

        self.top_func = self.file_config.get('top_func', top_func)

        logging.info(f'Final top_func = {self.top_func}')

        self.templates['user_func_args'] = self.templates['user_func_args_t']
        self.templates['user_func_params'] = self.templates['user_func_params_t']

        self.states['__top__']['title'] = self.top_func
        self.resolve_parent_titles(self.states)

        #pprint(self.states, indent=4)
        self.ast = self.build_ast(self.states)

        self.h_ast = self.build_user_declarations(self.user_funcs, self.user_guards, self.states)
        '''

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
            for e in s['events'].values():
                if e.target:
                    e.trans_funcs, e.target = self.get_transition_funcs(s_id, e.target)
                    e.target_title = states[e.target]['title']

            for g in s['guards'].values():
                if g.target:
                    g.trans_funcs, g.target = self.get_transition_funcs(s_id, g.target)
                    g.target_title = states[g.target]['title']

    def str_to_signal(self, line):
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

        g = {
            'guard': (guard, gparams),
            'funcs': [(func, fparams)],
            'target': None
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
        """Extract state info from drawing data into a dict like this:
        {
            'signals': {
                None: {
                    'name': None,
                    'guards': {
                        ('guard_fn1', '5, 8'): {
                            'guard': ('guard_fn1', '5, 8'),
                            'funcs': [('func1', None), ('func2', 'CONST_MACRO, 9')],
                            'target': 'state_5'
                        },
                    },
                },
                'SIG_A': {
                    'name': 'SIG_A',
                    'guards': {
                        ('guard_fn2', '5, 8'): {
                            'guard': ('guard_fn1', '5, 8'),
                            'funcs': [('func1', None), ('func2', 'CONST_MACRO, 9')],
                            'target': 'state_5'
                        },
                        ('guard_fn3', '5, 8'): {
                            'guard': ('guard_fn3', '5, 8'),
                            'funcs': [('func5', None), ('func6', 'CONST_MACRO, 9')],
                            'target': 'state_8'
                        },
                    },
                },
            },
            'parent': 'state_2',
            'children': ['state_13', 'state_14'],
            'title': 's_idle',
            'type': 'normal' / 'initial',
            }
        }
        """
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

            signal, guard, func, fparams, gparams = self.decode_line(label)

            if states[start]['type'] == 'initial':
                start = states[start]['parent']
                states[start]['initial'] = target

                signal = 'init'
                guard = None
                func = None

            if signal:
                if signal not in states[start]['events']:
                    states[start]['events'][signal] = Event(signal)
                e = states[start]['events'][signal]
            elif guard:
                while guard in states[start]['guards']:
                    guard = guard + ' '
                e = Event(None)
                states[start]['guards'][guard] = e

            e.target = target
            e.target_title = states[target]['title']
            e.trans_guard = guard
            e.trans_func = func
            e.trans_fparams = fparams
            e.trans_gparams = gparams


    def build_case_from_event(self, event):
        signal = event.signal
        if signal == 'init':
            signal = self.templates['init_signal']
        else:
            signal = f'{self.templates["signal_prefix"]}{signal}'

        c = Case(signal)
        if event.func:
            fparams = f', {event.fparams}' if event.fparams else ''
            f = Call(event.func, self.templates['user_func_args'] + fparams)
            if event.guard:
                gparams = f', {event.gparams}' if event.gparams else ''
                i = If(Call(event.guard, self.templates['user_func_args'] + gparams, False))
                i.add_true(f)
                c.add(i)
            else:
                c.add(f)

        if event.target:
            add = c.add
            if event.trans_guard:
                gparams = f', {event.trans_gparams}' if event.trans_gparams else ''
                i = If(Call(event.trans_guard, self.templates['user_func_args'] + gparams, False))
                c.add(i)
                add = i.add_true

            f = Call(self.templates['exit_children'], self.templates['func_args'])
            add(f)

            if event.trans_func:
                fparams = f', {event.trans_fparams}' if event.trans_fparams else ''
                f = Call(event.trans_func, self.templates['user_func_args'] + fparams)
                add(f)

            for f in event.trans_funcs:
                if f.func:
                    fparams = f', {f.fparams}' if f.fparams else ''
                    add(Call(f.func, self.templates['user_func_args'] + fparams))
            
            add(Call(self.templates['set_state_id'], self.templates['user_func_args'] + f', {event.target_title.upper()}'))

            add(Return(self.templates['trans_result'].format(target=event.target_title)))
        
        if c.nodes[-1].node_type != 'return':
            c.add(Break())
            #c.add(Return(self.templates['handled_result']))

        return c

    def build_if_from_guard(self, guard, g):
        if g.target:
            gparams = f', {g.trans_gparams}' if g.trans_gparams else ''
        else:
            gparams = f', {g.gparams}' if g.gparams else ''
            
        i = If(Call(guard.strip(), self.templates['user_func_args'] + gparams, False))
        if g.func:
            fparams = f', {g.fparams}' if g.fparams else ''
            i.add_true(Call(g.func, self.templates['user_func_args'] + fparams))

        if g.target:
            f = Call(self.templates['exit_children'], self.templates['func_args'])
            i.add_true(f)

            if g.trans_func:
                fparams = f', {g.trans_fparams}' if g.trans_fparams else ''
                f = Call(g.trans_func, self.templates['user_func_args'] + fparams)
                i.add_true(f)

            if g.trans_funcs:
                for f in g.trans_funcs:
                    if f.func:
                        fparams = f', {f.fparams}' if f.fparams else ''
                        i.add_true(Call(f.func, self.templates['user_func_args'] + fparams))
            
            
            i.add_true(Call(self.templates['set_state_id'], self.templates['user_func_args'] + f', {g.target_title.upper()}'))
            i.add_true(Return(self.templates['trans_result'].format(target=g.target_title)))

            
        return i

    def build_func_from_state(self, state_id, state, insert_init=False, spec=''):
        f = Func(name=state['title'], ftype=self.templates['func_return_type'], params=self.templates['func_params'], spec=spec)
        f.add(Decl('bool', self.templates['guards_only_variable'], 'true'))
        s = Switch(self.templates['switch_variable'])
        f.add(s)

        for signal, e in state['events'].items():
            if signal in ['entry', 'exit']:
                continue
            elif signal == 'init' and not insert_init:
                continue

            c = self.build_case_from_event(e)
            if c:
                s.add_case(c)

        s.add_default(Assignment(self.templates['guards_only_variable'], 'false'))

        for guard, g in state['guards'].items():
            f.add(Blank())
            i = self.build_if_from_guard(guard, g)
            f.add(i)

        f.add(Blank())

        exit_func = 'NULL'
        try:
            exit_func = state['events']['exit'].func
        except KeyError:
            pass

        if exit_func == None:
            exit_func = 'NULL'

        if state['parent']:
            result = self.templates['parent_result'].format(parent=state['parent_title'], exit_func=exit_func)
        else:
            result = self.templates['ignored_result']

        f.add(Return(result))

        return f

    def build_ast(self, states):
        ast = Ast()
        ast.nodes.append(Blank())
        for s_id, s in states.items():
            if s['type'] != 'normal':
                continue
            state_func = self.build_func_from_state(s_id, s, spec='static') 
            ast.nodes.append(state_func)
            ast.nodes.insert(0, state_func.declaration())

        ast.nodes.append(self.build_func_from_state(self.top_func, states['__top__'], True))

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
                
    
    def make_initial_func(self):
        return self.get_init_path('__top__', self.states['__top__']['events']['init'].target)
    
    def path_to_funcs(self, path):
        funcs = []
        
        for step in path:
            state_id, event_id = step
            try:
                func = self.states[state_id]['events'][event_id]
            except KeyError:
                func = None # The requested event was not implemented in the actual state
            
            if func:
                funcs.append(func)
        
        return tuple(funcs)