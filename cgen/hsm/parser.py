import re

class ParserException(Exception):
    pass

class Parser:
    def __init__(self):
        self.funcs_w_args = set()
        self.funcs_wo_args = set()
        self.guards_wo_args = set()
        self.user_signals = set()

    def get_func(self, data):
        data = data.lstrip()
        

        m = re.match('[a-zA-z_]+\w*', data)
        if m:
            fname = m[0]
            rem = data[m.span()[1]:]
        else:
            raise ParserException(f'Expected function identifier in "{data}"')

        rem = rem.lstrip()

        if len(rem) == 0 or rem[0] != '(':
            return fname, None, rem

        rem = rem[1:]

        params, c_brace, rem = rem.partition(')')

        params = params.strip()
        if params == '':
            params = None
        else:
            for p in params.split(','):
                ps = p.strip()
                if not ps.isidentifier() and not ps.isalnum():
                    raise ParserException(f'Expected  identifier as function param but found: "{p}"')

        if c_brace == '':
            raise ParserException(f'Expected ")" in "{data}"')
            
        return fname, params, rem

    def get_signal(self, data):
        signal = None
        rem = data
        
        m = re.match('[a-zA-z_]+\w*', data)
        if m:
            signal = m[0]
            rem = data[m.span()[1]:]

            self.user_signals.add(signal)

        return signal, rem

    def get_funcs(self, data):
        funcs = []

        data = data.lstrip()
        while data[0] != '}':
            func, func_param, data = self.get_func(data)
            funcs.append((func, func_param))
   
            if func_param:
                self.funcs_w_args.add(func)
            else:
                self.funcs_wo_args.add(func)

            data = data.lstrip()
            if not len(data):
                raise ParserException('Unexpected end of input')
            
            if data[0] == ';':
                data = data[1:]
                
            data = data.lstrip()

        return funcs, data[1:]

    def parse_one(self, data, target=None, target_title=None, initial=False, lca=None):
        signal = None
        guard = (None, None)
        funcs = []


        # Try to read the sginal
        data = data.lstrip()
        if data[0].isidentifier():
            signal, data = self.get_signal(data)

        # If there is a guard, read it
        data = data.lstrip()
        if len(data) and data[0] == '[':
            guard_func, guard_params, data = self.get_func(data[1:])
            guard = (guard_func, guard_params)
            data = data.lstrip()
            if not len(data) or data[0] != ']':
                p = guard_params if guard_params else ''
                raise ParserException(f'Expected "]" after "{guard_func}({p})..."')
            data = data[1:]
            
            if guard_params:
                self.funcs_w_args.add(guard_func)
            else:
                self.guards_wo_args.add(guard_func)
            
        # Find the slash character
        data = data.lstrip()
        if len(data) and data[0] != '/':
            raise ParserException(f'Expected "/" in "{data}..."')
        
        if len(data):
            data = data[1:]
            data = data.lstrip(' \t')

            if len(data) and data[0] != '\n':
                if data[0] == '{':
                    funcs, data = self.get_funcs(data[1:])
                else:
                    func, func_param, data = self.get_func(data)
                    funcs.append((func, func_param))
                    if func_param:
                        self.funcs_w_args.add(func)
                    else:
                        self.funcs_wo_args.add(func)

        if initial:
            signal = 'init'
            guard = (None, None)

        g = {   
            'guard': guard,
            'funcs': funcs,
            'target': target,
            'target_title': target_title,
            'lca': lca
        }

        s = {
            'name': signal,
            'guards': {
                g['guard']: g
            }
        }

        return s, data

    def empty_signal(self, target=None, target_title=None, initial=False, lca=None):
        signal = None
        
        if initial:
            signal = 'init'

        g = {
            'guard': (None, None),
            'funcs': [(None, None)],
            'target': target,
            'target_title': target_title,
            'lca': lca
        }

        s = {
            'name': signal,
            'guards': {
                g['guard']: g
            }
        }

        return [s]

    def parse(self, data, target=None, target_title=None, initial=False, lca=None):
        self.signals = []
        while len(data):
            s, data = self.parse_one(data, target, target_title, initial, lca)
            self.signals.append(s)
            data = data.strip()
        
        if not self.signals:
            self.signals = self.empty_signal(target, target_title, initial, lca)

        return self.signals

if __name__ == '__main__':
    s = '''
entry/
exit/
       '''
    from pprint import pprint
    p = Parser()
    r = p.parse(s)
    pprint(r)
    pprint(p.funcs_w_args)
    pprint(p.funcs_wo_args)
    
