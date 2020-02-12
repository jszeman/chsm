'''
Created on 2019. febr. 7.

@author: jszeman
'''

import re
import pprint
import logging
import threading
import queue

uml = r'''
@startuml
state sdraw {
  sdraw: entry/ draw_window()
  sdraw: exit/ close_window()
  sdraw: update/ draw_model()

  state select {
    state select1
    [*] -> select1
    
    select1: exit/ select1_exit()
  }
  select : entry/ select_entry()
  select : exit/ select_exit()
  select : btn_press / save_start_coords()
  select : btn_release / find_selected_objects()
  select --> selected: [objects_selected()]
  select: o_pressed/ select_object() 
  
  selected: entry/ slected_entry()
 
  state draw_state {
    draw_state: entry/ new_state()
    draw_state: exit/
    draw_state: init/ init_drawing()
    draw_state: esc_pressed/ delete_new_state()
    draw_state --> select: esc_pressed



    [*] --> select_corner1
    select_corner1: entry/ move_state()
    select_corner1: mouse_move/ move_state()
    select_corner1 -> select_corner2: btn_release
    select_corner1: btn_release/ save_corner1_coords()

    select_corner2 -> select: btn_release
    select_corner2: mouse_move/ resize_state()

  }

  select --> draw_state: s_pressed
}

[*] --> select
sdraw --> [*]: close
@enduml
'''

# May God forgive us all for regular expressions
INITIAL_PATTERN = '\[\*\]\s+-+>\s+(?P<target_state>\w+)(\s*:\s+(?P<event>\w+)\s*)*\Z'
END_PATTERN = '(?P<source_state>\w+)\s+-+>\s+\[\*\](\s*:\s+(?P<event>[\w\s]+)\s*)*\Z'
TRANSITION_PATTERN = '(?P<source_state>\w+)\s*-+>\s+(?P<target_state>\w+)(\s*:\s+(?P<event>[\w\s]+)\s*)*\Z'
GUARD_PATTERN = '(?P<source_state>\w+)\s+-+>\s+(?P<target_state>\w+)(\s*:\s+\[(?P<guard>[\w\s]+)\(\)\]\s*)*\Z'
STATE_PATTERN = 'state\s+("(?P<long_name>[\w\s\\\]+)"\s+as\s+)?(?P<state>\w+)\s*(?P<composit_start>\{)?\s*\Z'
COMPOSIT_END_PATTERN = '\}\s*\Z'
EVENT_PATTERN = '(?P<state>\w+)\s*:\s*(?P<event>\w+)\s*/\s*(?P<function>\w+)\(\)\s*\Z'

class UmlGuard:
    def __init__(self, target_state_id, cond):
        self.target_state_id = target_state_id
        self.condition = cond
            
    def __str__(self):
        return f'\t[{self.condition}()] --> {self.target_state_id}'

class UmlEvent:
    def __init__(self, event_id, target_state_id=None, func=None):
        self.event_id = event_id
        self.target_state_id = target_state_id
        self.func = func
    
    def update(self, event):
        if self.target_state_id == None:
            self.target_state_id = event.target_state_id
        
        if self.func == None:
            self.func = event.func
            
    def __str__(self):
        if self.func:
            func = f': {self.func}()'
        else:
            func = ''
            
        if self.target_state_id:
            target = f' --> {self.target_state_id}'
        else:
            target = ''    
        return f'\t{self.event_id}{func}{target}'
        
class UmlState:
    def __init__(self, state_id, _super=None, long_name=None):
        self.state_id = state_id
        self._super = _super
#         self.init_state_id = None
        self.long_name = long_name
        self.events = {'entry': UmlEvent('entry'), 'exit': UmlEvent('exit')}
        self.functions = {}
        self.guards = []
    
    def get_super(self):
        return self._super
    
    def get_init_state_id(self):
        try:
            return self.events['init'].target_state_id
        except KeyError:
            return None
    
    def add_event(self, event):
        if event.event_id not in self.events:
            self.events[event.event_id] = event
        else:
            self.events[event.event_id].update(event)
            
    def add_guard(self, guard):
        self.guards.append(guard)
        
    def __str__(self):
        state =  f'state: {self.state_id}, super: {self._super}, long: {self.long_name}\n'
        
        if len(self.events):
            events = '\n'.join([str(e) for e in self.events.values()]) + '\n'
        else:
            events = ''
            
        if len(self.guards):
            guards = '\n'.join([str(g) for g in self.guards]) + '\n'
        else:
            guards = ''
        
        return state + events + guards            

class UmlStateChart:
    def __init__(self):
        self.states = {}
        self.actual_super = [None]
        self.add_state('__top__')
        self.actual_super = ['__top__']
        self.top = self.states['__top__'] #shorthand to top

    def set_init_event(self, event):
        self.states[self.actual_super[-1]].add_event(event)
        
    def add_event(self, state_id, event):
        self.states[state_id].add_event(event)
        
    def add_guard(self, state_id, guard):
        self.states[state_id].add_guard(guard)
        
    def add_state(self, state_id, long_name=None):
        if state_id not in self.states:
            self.states[state_id] = UmlState(state_id, self.actual_super[-1], long_name=long_name)
        
    def enter_composit_state(self, state_id):
        self.actual_super.append(state_id)
        
    def leave_composit_state(self):
        self.actual_super.pop()
        
    def __str__(self):
        states = '\n'.join([str(self.states[key]) for key in self.states])
        
        return states
    
    def get_entry_path(self, start_state_id, target_state_id):
        logging.debug(f'get_entry_path({start_state_id}, {target_state_id})')
        
        if start_state_id == target_state_id:
            return tuple()
        
        state = self.states[target_state_id]
        path = []
        while True:
            state_id = state.state_id
            path.append((state_id, 'entry'))
            
            super_id = state.get_super()
            
            if super_id == start_state_id:
                p = tuple([e for e in reversed(path)])
                logging.debug(f'    return {p}')
                return reversed(path)
            else:
                state = self.states[super_id]
    
    def is_in(self, state_id, composit_state_id):
        if composit_state_id == '__top__':
            return True
        elif state_id == '__top__':
            return False
        
        while True:
            super_id = self.states[state_id].get_super()
            
            if super_id == '__top__':
                return False
            elif super_id == composit_state_id:
                return True
            else:
                state_id = super_id
        
    def get_LCA(self, start_state_id, target_state_id):
        '''Find Lowest Common Ancestor'''
        logging.debug(f'get_LCA: {start_state_id}, {target_state_id}')
        
        if self.is_in(start_state_id, target_state_id):
            logging.debug(f' LCA = {target_state_id}')
            return target_state_id
        
        super_id = start_state_id
        while True:
            if self.is_in(target_state_id, super_id):
                logging.debug(f' LCA = {super_id}')
                return super_id
            else:
                super_id = self.states[super_id].get_super()
    
    def get_exit_path(self, start_state_id, target_state_id):
        path = []
        s = start_state_id
        logging.debug(f'get_exit_path: {start_state_id}, {target_state_id}')
       
        while s != target_state_id:
            path.append((s, 'exit'))
            s = self.states[s].get_super()
             
        return path
    
    def get_transition_path(self, start_state_id, target_state_id):
        logging.debug(f'get_transition_path: {start_state_id}, {target_state_id}')
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
        logging.debug(f'get_init_path {start_state_id}, {target_state_id}')
        path = []
        while True:
            entry_path = self.get_entry_path(start_state_id, target_state_id)
            path.extend(entry_path)
            
            init_state_id = self.states[target_state_id].get_init_state_id()
            if init_state_id == None:
                return path
            else:
                path.append((target_state_id, 'init'))
                start_state_id = target_state_id
                target_state_id = init_state_id
                
    
    def make_initial_func(self):
        return self.get_init_path('__top__', self.top.events['init'].target_state_id)
    
    def path_to_funcs(self, path):
        funcs = []
        
        for step in path:
            state_id, event_id = step
            func = self.states[state_id].events[event_id].func
            if func:
                funcs.append(func)
                
        return tuple(funcs)
    
#     def get_state_guards(self, state_id):
#         state = self.states[state_id]
#         
#         return [(g.condition, g.target_state_id) for g in state.guards]
    
    def get_state_events(self, state_id):
        state = self.states[state_id]
        functions = {}
        transitions = {}
        
        for e in state.events.values():
            target_id = e.target_state_id
            if target_id:
                transition_path = self.get_transition_path(state_id, target_id)
                transition = self.path_to_funcs(transition_path)
                transitions[(state_id, e.event_id)] = transition_path[-1][0]
            else:
                transition = tuple()
                
            if e.func:
                functions[(state_id, e.event_id)] = (e.func, ) + transition
            else:
                functions[(state_id, e.event_id)] = transition
        
                
        return functions, transitions
    
    def get_state_guards(self, state_id):
        state = self.states[state_id]
        guards = []
        while state.state_id != '__top__':
            guards.extend([(g.condition, g.target_state_id, self.path_to_funcs(self.get_transition_path(state_id, g.target_state_id))) for g in state.guards])
            state = self.states[state.get_super()]
        
        return guards

    
    def get_flat_events(self):
        functions = {}
        transitions = {}
        parents = {}
        guards = {}
        
        for state_id in self.states:
            f, t = self.get_state_events(state_id)
            functions.update(f)
            transitions.update(t)
            parents[state_id] = self.states[state_id].get_super()
            
            g = self.get_state_guards(state_id)
            guards[state_id] = g
        
        return functions, transitions, parents, guards
        

class PlantUML:
    def __init__(self, uml_str):
        self.state_chart = UmlStateChart()
        
        self.uml = (l for l in uml_str.splitlines())
        
        for line in self.uml:
            self.process_pu_line(line)
            
#         pprint.pprint(self.state_chart.get_flat_events(), indent=4)
        
    def make_flat_machine(self, user_object):
        functions, transitions, parents, guards = self.state_chart.get_flat_events()
        fm = UmlFlatMachine(user_object, functions, transitions, parents, guards)
        return fm

    def process_pu_line(self, line):
        m = re.search(INITIAL_PATTERN, line)
        if m:
            target_state = m.group('target_state')
            event = UmlEvent(event_id='init', target_state_id=target_state)
            self.state_chart.set_init_event(event)
            return
        
        m = re.search(TRANSITION_PATTERN, line)
        if m:
            source_state = m.group('source_state')
            target_state = m.group('target_state')
            event = UmlEvent(event_id=m.group('event'), target_state_id=target_state) 
            self.state_chart.add_state(source_state)
            self.state_chart.add_state(target_state)
            self.state_chart.add_event(source_state, event)
            return
        
        m = re.search(GUARD_PATTERN, line)
        if m:
            source_state = m.group('source_state')
            target_state = m.group('target_state')
            guard = UmlGuard(target_state_id=target_state, cond=m.group('guard')) 
            self.state_chart.add_state(source_state)
            self.state_chart.add_state(target_state)
            self.state_chart.add_guard(source_state, guard)
            return
        
        m = re.search(STATE_PATTERN, line)
        if m:
            state = m.group('state')
            long_name = m.group('long_name')
            composit = m.group('composit_start')
            self.state_chart.add_state(state, long_name)
            if composit:
                self.state_chart.enter_composit_state(state)
            return

        
        if re.search(COMPOSIT_END_PATTERN, line):
            self.state_chart.leave_composit_state()
            return
            
        m = re.search(EVENT_PATTERN, line)
        if m:
            state = m.group('state')
            func = m.group('function')
            event = UmlEvent(event_id=m.group('event'), func=func) 
            self.state_chart.add_state(state)
            self.state_chart.add_event(state, event)
            return
        
        m = re.search(END_PATTERN, line)
        if m:
            source_state = m.group('source_state')
            target_state = '__top__'
            event = UmlEvent(event_id=m.group('event'), target_state_id=target_state) 
            self.state_chart.add_state(source_state)
            self.state_chart.add_event(source_state, event)
            return

class UmlFlatMachine:
    def __init__(self, user_object, uml_str):
        self.actual_state_id = '__top__'
        self.user_object = user_object
        self.event_q = queue.Queue()
        self.lock = threading.Lock()
        
        p = PlantUML(uml_str)
        self.functions, self.transitions, self.parents, self.guards = p.state_chart.get_flat_events()
    
    
    def dispatch(self, event_id, **cargo):
        
        # Try to acquire the lock. If failed, just put the event into the queue and return
        
        lock_acquired = self.lock.acquire(blocking=False)
        
        if not lock_acquired:
            self.event_q.put((event_id, cargo))
            return
        
        # Process the event passed to us
        self.process_event(event_id, **cargo)
        
        # Process events that arrived during processing
        while True:
            try:
                event_id, cargo = self.event_q.get_nowait()
            except queue.Empty:
                break
            
#             print((self.actual_state_id, event_id, cargo))
            self.process_event(event_id, **cargo)
            
        self.lock.release()
    
    def process_event(self, event_id, **cargo):
        state_id = self.actual_state_id
        
        funcs = []
        
        while True:
            key = (state_id, event_id)
            
            func = key in self.functions                            
            trans = key in self.transitions                         
            event_handled = func or trans                           
            
            if not event_handled:                                   # if the event is not handled in this state
                funcs.extend(self.functions[(state_id, 'exit')])    # add the state exit function to the list
                state_id = self.parents[state_id]                   # go to the parent state
                if state_id == '__top__':                           # return if the parent is the top
                    break
                
            else:                                                   # if the event is handled in this state
                if trans:                                           # if the event causes a state transition
                    funcs.extend(self.functions[key])               # start the func chain with the exit functions collected from child states
                    self.actual_state_id = self.transitions[key]    # change the state
                else:                                               # if the event is handled inside the state (no transition)
                    funcs = self.functions[key]                     # throw out the exit functions, use only the handler in this state
                
                for f in funcs:                                     # execute all functions in the handler list
                    getattr(self.user_object, f)(event_id, **cargo)
                    
                break
        
        for guard in self.guards[self.actual_state_id]:             # go through all the guards associated with this state (this includes guards of the superstates as well)
            cond, targer_state_id, funcs = guard                   # unpack the guard
            if getattr(self.user_object, cond)(event_id, **cargo):  # call the conditional function and check if it returns True:
                for f in funcs:                                     # execute all functions in the handler list
                    getattr(self.user_object, f)(event_id, **cargo)
                self.actual_state_id = targer_state_id              # switch the state
                return                                              # only the first guard that evaluates true will be executed
            

class TestObject:
    def __init__(self):
        self._objects_selected = False
    def event_handler(self, event_id, **cargo):
        print(event_id, cargo)
        
    def select_object(self, event_id, **cargo):
        print(f'select_object {event_id}, {cargo}')
        self._objects_selected = True
        
    def objects_selected(self, event_id, **cargo):
        return self._objects_selected
        
    def __getattr__(self, name):
        print(name, end=' ')
        return self.event_handler

if __name__ == '__main__':
    to = TestObject()
#     p = PlantUML(uml)
    fm = UmlFlatMachine(to, uml_str=uml)
    print('    ', fm.actual_state_id)
    fm.dispatch('init')
    print('    ', fm.actual_state_id)
    fm.dispatch('btn_press')
    print('    ', fm.actual_state_id)
    fm.dispatch('s_pressed')
    print('    ', fm.actual_state_id)
    fm.dispatch('esc_pressed')
    print('    ', fm.actual_state_id)
    fm.dispatch('o_pressed')
    print('    ', fm.actual_state_id)
    