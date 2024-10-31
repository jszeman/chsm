import re

class ParserException(Exception):
    pass

NOSIG = ""
NOFUNC = ""
NOPARAM = ""
NOGUARD = ""
NULLFUNC = (NOFUNC, NOPARAM)

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
            return fname, "", rem

        rem = rem[1:]

        params, c_brace, rem = rem.partition(')')

        params = params.strip()
        if params == '':
            params = NOPARAM
        else:
            for p in params.split(','):
                ps = p.strip()
                if not ps.isidentifier() and not ps.isalnum():
                    raise ParserException(f'Expected  identifier as function param but found: "{p}"')

        if c_brace == '':
            raise ParserException(f'Expected ")" in "{data}"')
            
        return fname, params, rem

    def get_signal(self, data):
        signal = NOSIG
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

    def parse_one(self, data, target="", target_title="", initial=False, lca=""):
        signal = NOSIG
        guard = NOGUARD
        guard_func = NOFUNC
        guard_param = NOPARAM
        funcs = []


        # Try to read the sginal
        data = data.lstrip()
        if data[0].isidentifier():
            signal, data = self.get_signal(data)

        # If there is a guard, read it
        data = data.lstrip()
        if len(data) and data[0] == '[':
            guard_func, guard_param, data = self.get_func(data[1:])
            data = data.lstrip()
            if not len(data) or data[0] != ']':
                p = guard_param if guard_param else ''
                raise ParserException(f'Expected "]" after "{guard_func}({p})..."')
            data = data[1:]
            
            if guard_param:
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
            guard_func = NOFUNC
            guard_param = NOPARAM

        g = {   
            'guard_func': guard_func,
            'guard_param': guard_param,
            'funcs': funcs,
            'target': target,
            'target_title': target_title,
            'lca': lca
        }

        guard_id = f'{guard_func}({guard_param})' if guard_func else ''

        s = {
            'name': signal,
            'guards': {
                guard_id: g
            }
        }

        return s, data

    def empty_signal(self, target="", target_title="", initial=False, lca=""):
        signal = NOSIG

        g = {
            'guard_func': NOFUNC,
            'guard_param': NOPARAM,
            'funcs': [],
            'target': target,
            'target_title': target_title,
            'lca': lca
        }

        if initial:
            signal = 'init'
            g['guard_func'] = NOFUNC

        s = {
            'name': signal,
            'guards': {"": g}
        }

        return [s]

    def parse(self, data, target="", target_title="", initial=False, lca=""):
        self.signals = []
        while len(data):
            s, data = self.parse_one(data, target, target_title, initial, lca)
            self.signals.append(s)
            data = data.strip()
        
        if not self.signals and target:
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
    
