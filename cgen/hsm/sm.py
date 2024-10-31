import json
import re
import logging
from datetime import datetime
import pprint
from copy import deepcopy
from .parser import Parser, ParserException, NOSIG, NOFUNC, NOPARAM, NOGUARD, NULLFUNC

# TODO: make sure, that there is only one exit function and it has no parameters

# Match the following patterns:
#   event
#   event / func()
#   event [guard()] / func()
#   event [guard()]
#   [guard()] / func()
#   [guard()]
EVENT_PATTERN = r'^(\s*(?P<signal>\w+)\s*)*(\[(?P<guard>\w+)\((?P<gparams>[\w,\s]*)\)\])*(\s*/\s*(?P<function>\w+)(?P<parens>\((?P<fparams>[\w,\s]*)\))*\s*)*'

VERSION_STRING = 'Generated with CHSM v0.0.2'

SYS_SIGNALS = ('entry', 'exit', 'init')

class StateMachine:
    def __init__(self, data):
        
        self.user_funcs = set()
        self.user_guards = set()
        self.user_signals = set()
        self.user_inc_funcs = set()

        data['states']['__top__']['parent'] = ""
        self.states = self.get_states(data)                 # Extract states from the data
        self.add_transitions_to_states(self.states, data)   # Extract transition info from the data and add it to the states
        self.add_parent_signals(self.states)
        self.process_transitions(self.states)               # Calculate transition exit and entry paths and the exact end state
        
        self.delete_non_leaf_states(self.states)

        self.notes = data['notes']

        self.resolve_parent_titles(self.states)

        self.remove_unnecessary_transitions(self.states)

        self.remove_initial_states(self.states)

        self.data = {
            'states': self.states,
            'user_funcs': self.user_funcs,
            'user_guards': self.user_guards,
            'user_signals': self.user_signals,
            'user_inc_funcs': self.user_inc_funcs,
            'notes': self.notes,
        }

    def get_transition_funcs(self, start, end, lca):
        path = self.get_transition_path(start, end, lca)
        return self.path_to_funcs(path), path[-1][0]

    def process_transitions(self, states):
        for s_id, s in states.items():
            if s['children']:
                continue
            for sig_id, sig in s['signals'].items():
                if sig_id == 'init':
                    continue
                for g in sig['guards'].values():
                    if g['target']:
                        tfuncs, target = self.get_transition_funcs(s_id, g['target'], g['lca'])
                        #g['transition_funcs'] = tfuncs
                        g['funcs'].extend(tfuncs)
                        g['target_title'] = states[target]['title']
                        g['target_type'] = states[target]['type']
                        if 'params' in states[target]:
                            g['target_params'] = states[target]['params']
        
        #The only place where transition functions are needed for an init signal is the __top__
        g = states['__top__']['sys_signals']['init']['guards'][NOGUARD]
        tfuncs, target = self.get_transition_funcs('__top__', g['target'], g['lca'])
        g['funcs'].extend(tfuncs)
        g['target_title'] = states[target]['title']

    def delete_non_leaf_states(self, states):
        for s_id in [*states.keys()]:
            s = states[s_id]
            if s['children'] and s['parent']:
                del states[s_id]
            elif s['parent']:
                s['parent'] = '__top__'

    def add_parent_signals(self, states):
        for s_id, s in states.items():
            if not s['children'] and s['type'] == 'normal':
                self.add_parent_signals_to_state(s_id, states)

    def add_parent_signals_to_state(self, state_id, states):
        """
            Take a state, go through its parent states and add signal handlers
            that are not already in the original state.
            If the signal already has some handlers in the original state, then
            go through the guards one by one and copy those that are not in the
            original state.
        """
        signals = states[state_id]['signals']

        parent_id = states[state_id]['parent']
        while parent_id:
            p_signals = states[parent_id]['signals']
            for sig_id, sig in p_signals.items():
                if sig_id in ('entry', 'exit', 'init'):
                    continue

                if not sig_id in signals:
                    signals[sig_id] = deepcopy(sig)

                else:
                    guards = signals[sig_id]['guards']
                    for guard_id, p_guard in sig['guards'].items():
                        if not guard_id in guards:
                            guards[guard_id] = deepcopy(p_guard)

            parent_id = states[parent_id]['parent']
    

    def add_signal_to_state(self, state, signal):
        signal_name = signal['name']
            
        if signal_name in SYS_SIGNALS:
            if not state['sys_signals'][signal_name]:
                state['sys_signals'][signal_name] = signal
            else:
                orig_signal = state['sys_signals'][signal_name]
                orig_signal['guards']['']['funcs'].extend(signal['guards']['']['funcs'])
                orig_signal['guards']['']['target'] = signal['guards']['']['target']
                orig_signal['guards']['']['target_title'] = signal['guards']['']['target_title']
        elif signal_name not in state['signals']:
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
    
    def process_state_title(self, title):
        """Parse the title string. A state title can be a function identifier
        or a function call.
        Return: (function_name, params, is_call), where:
            function: name of the function or id
            params: parameter string,
            is_call: True, if the title is actually a function call."""
        title = title.lstrip()

        m = re.match('[a-zA-z_]+\w*', title)
        if m:
            fname = m[0]
            rem = title[m.span()[1]:]
        else:
            raise ParserException(f'Expected function identifier in "{title}"')

        rem = rem.lstrip()

        if len(rem) == 0 or rem[0] != '(':
            return fname, "", False

        rem = rem[1:]

        params, c_brace, rem = rem.partition(')')

        params = params.strip()
        if params == '':
            params = ""
        else:
            for p in params.split(','):
                ps = p.strip()
                if not ps.isidentifier() and not ps.isalnum():
                    raise ParserException(f'Expected  identifier as function param but found: "{p}"')

        if c_brace == '':
            raise ParserException(f'Expected ")" in "{title}"')
            
        return fname, params, True

    def get_states(self, data):
        """Extract state info from drawing data into a dict"""
        states = {}

        for s_id, s in data['states'].items():

            title, params, is_call = self.process_state_title(s['title'])

            state = {
                'signals': {},
                'sys_signals': {
                    'entry': {},
                    'exit': {},
                    'init': {}
                },
                'guards': [],
                'parent': s['parent'],
                'children': s['children'],
                'title': title,
                'type': s['type'],
                }
            
            if is_call:
                state['type'] = 'call'
                state['params'] = params
            else:
                if s_id.startswith('state_'):
                    state['num'] = int(re.findall(r'\d+', s_id)[0])

            txt = '\n'.join(s['text'])
            p = Parser()
            try:
                sigs = p.parse(txt)
            except ParserException as e:
                logging.info(f'Exception {str(e)} in state "{s["title"]}". Text:\n{txt}')

            if state['title'] == 's':
                print(state['title'])

            for sig in sigs:
                self.add_signal_to_state(state, sig)

            self.user_inc_funcs.update(p.funcs_w_args)
            self.user_funcs.update(p.funcs_wo_args)
            self.user_signals.update(p.user_signals)
            self.user_guards.update(p.guards_wo_args)

            states[s_id] = state

        return states

    def resolve_parent_titles(self, states):
        for s in states.values():
            if s['parent']:
                s['parent_title'] = states[s['parent']]['title']

    def remove_initial_states(self, states):
        keys = tuple(states.keys())
        for k in keys:
            if states[k]['type'] == 'initial':
                del states[k]

    def remove_unnecessary_transitions(self, states):
        """Remove transition targets pointing to the current state."""
        for state in states.values():
            for signal in state['signals'].values():
                for guard in signal['guards'].values():
                    if guard["target_title"] == state['title']:
                        guard['target'] = ""
                        guard["target_title"] = ""
    
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

            if target:
                if self.is_in(start, target):
                    lca = target
                else:
                    lca = self.get_LCA(start, target)
            else:
                lca = ""

            p = Parser()

            if states[start]['type'] == 'initial':
                start = states[start]['parent']
                states[start]['initial'] = target

                signals = p.parse(label, target=target, target_title=states[target]['title'], initial=True)
            else:
                signals = p.parse(label, target, target_title=states[target]['title'], initial=False, lca=lca)

            try:
                if signals:
                    self.add_signal_to_state(states[start], signals[0])
            except:
                print(signals)
                raise

            self.user_inc_funcs.update(p.funcs_w_args)
            self.user_funcs.update(p.funcs_wo_args)
            self.user_signals.update(p.user_signals)
            self.user_guards.update(p.guards_wo_args)

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
        
        #if self.is_in(start_state_id, target_state_id):
        #    return target_state_id
        
        super_id = start_state_id
        while True:
            if self.is_in(target_state_id, super_id):
                return super_id
            else:
                super_id = self.states[super_id]['parent']
    
    def get_exit_path(self, start_state_id, target_state_id):
        path = []
        s = start_state_id

        if s == "__top__":
            return path
       
        while s != target_state_id:
            path.append((s, 'exit'))
            s = self.states[s]['parent']
             
        return path
    
    def get_transition_path(self, start_state_id, target_state_id, lca):
        if target_state_id == '__top__':
            return self.get_exit_path(start_state_id, '__top__')
        else:
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
            init_state_id = self.states[target_state_id].get('initial', "")
            if init_state_id == "":
                return path
            else:
                start_state_id = target_state_id
                target_state_id = init_state_id
    
    def path_to_funcs(self, path):
        funcs = []
        for step in path:
            state_id, event_id = step
            try:
                funcs.extend(self.states[state_id]['sys_signals'][event_id]['guards'][NOGUARD]['funcs'])
            except KeyError as e:
                pass

        funcs = [f for f in funcs if f != NULLFUNC]

        return tuple(funcs)