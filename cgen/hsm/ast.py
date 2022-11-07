class Ast:
    def __init__(self):
        self.nodes = []

    def __repr__(self):
        return ''.join([str(n) for n in self.nodes])

class Node:
    def __init__(self, node_type, **kwargs):
        self.node_type = node_type
        self.indent = 0

class Comment(Node):
    def __init__(self, text, **kwargs): 
        super().__init__('comment', **kwargs)
        self.text = text

    def __repr__(self):
        indent = ' ' * self.indent
        
        text = self.text.replace('\n', f'\n{indent}')
        return f'/*{text}*/'


class Include(Node):
    def __init__(self, hname, **kwargs): 
        super().__init__('include', **kwargs)
        if '<' in hname and '>' in hname:
            self.hname = hname
        else:
            self.hname = f'"{hname}"'

    def __repr__(self):
        return f'#include {self.hname}\n'

class Ifndef(Node):
    def __init__(self, symbol, **kwargs): 
        super().__init__('ifndef', **kwargs)
        self.symbol = symbol

    def __repr__(self):
        return f'#ifndef {self.symbol}\n'

class Define(Node):
    def __init__(self, symbol, **kwargs): 
        super().__init__('define', **kwargs)
        self.symbol = symbol

    def __repr__(self):
        return f'#define {self.symbol}\n'

class Endif(Node):
    def __init__(self, **kwargs): 
        super().__init__('endif', **kwargs)

    def __repr__(self):
        return f'#endif\n'

class Block(Node):
    def __init__(self, **kwargs): 
        super().__init__('block', **kwargs)
        self.nodes = []

    def __repr__(self):
        return f'{{\n{self.nodes}\n}}'


class If(Node):
    def __init__(self, condition=None, **kwargs):
        super().__init__('if', **kwargs)
        self.condition = condition
        self.true_nodes = []
        self.false_nodes = []

    def add_true(self, node):
        self.true_nodes.append(node)

    def add_false(self, node):
        self.false_nodes.append(node)

    def __str__(self):
        true_str = ''
        for n in self.true_nodes:
            n.indent = self.indent + 4
            true_str += str(n)

        false_str = ''
        for n in self.false_nodes:
            n.indent = self.indent + 4
            false_str += str(n)

        indent = ' ' * self.indent
        true_expr = f'{indent}if({self.condition})\n{indent}{{\n{true_str}{indent}}}\n'
        if self.false_nodes:
            else_expr = f'{indent}else\n{indent}{{\n{false_str}{indent}}}\n'
            return true_expr + else_expr
        else:
            return true_expr

class Switch(Node):
    def __init__(self, variable, **kwargs):
        super().__init__('switch', **kwargs)
        self.variable = variable
        self.cases = []
        self.default = []

    def __str__(self):
        cases = []
        for n in self.cases:
            n.indent = self.indent + 4
            cases.append(str(n))

        cases_str = '\n'.join(cases)

        if self.default:
            default = []
            for d in self.default:
                d.indent = self.indent + 8
                default.append(str(d))

            default_str = ' ' * (self.indent + 4) + 'default:\n' + ''.join(default)
        else:
            default_str = ''

        indent = ' ' * self.indent
        return f'{indent}switch({self.variable})\n{indent}{{\n{cases_str}{default_str}{indent}}}\n'

    def add_case(self, case):
        self.cases.append(case)

    def add_default(self, node):
        self.default.append(node)

class Break(Node):
    def __init__(self, **kwargs): 
        super().__init__('break', **kwargs)

    def __str__(self):
        indent = ' ' * self.indent
        return f'{indent}break;\n'

class Decl(Node):
    def __init__(self, type_name, name, value, **kwargs): 
        super().__init__('decl', **kwargs)
        self.type_name = type_name
        self.name = name
        self.value = value

    def __str__(self):
        indent = ' ' * self.indent
        return f'{indent}{self.type_name} {self.name}={self.value};\n'

class Enum(Node):
    def __init__(self, name, items, **kwargs):
        super().__init__('enum', **kwargs)
        self.name = name
        self.items = items

    def __str__(self):
        indent = ' ' * self.indent

        enum = f'{indent}typedef enum {self.name}\n{indent}{{\n'
        for name, value in self.items.items():
            enum += f'{indent}    {name} = {value},\n'

        enum += f'{indent}}} {self.name};\n'

        return enum

class Assignment(Node):
    def __init__(self, name, value, **kwargs): 
        super().__init__('decl', **kwargs)
        self.name = name
        self.value = value

    def __str__(self):
        indent = ' ' * self.indent
        return f'{indent}{self.name} = {self.value};\n'

class Blank(Node):
    def __init__(self, **kwargs): 
        super().__init__('break', **kwargs)

    def __str__(self):
        return f'\n'

class Case(Node):
    def __init__(self, label, **kwargs): 
        super().__init__('case', **kwargs)
        self.label = label
        self.nodes = []

    def __str__(self):
        nodes_str = ''
        for n in self.nodes:
            n.indent = self.indent + 4
            nodes_str += str(n)

        if self.nodes[-1].node_type not in ['break', 'return']:
            nodes_str += f'{" " * (self.indent + 4)}break;\n'

        indent = ' ' * self.indent
        return f'{indent}case {self.label}:\n{nodes_str}'

    def add(self, node):
        self.nodes.append(node)

class FuncDeclaration(Node):
    def __init__(self, name, ftype='void *', params='void *self, void *e', spec='', **kwargs): 
        super().__init__('func_decl', **kwargs)
        self.name = name
        self.ftype = ftype
        self.params = params
        self.spec = spec

    def __str__(self):
        indent = ' ' * self.indent
        if self.spec:
            spec = self.spec + ' '
        else:
            spec = ''
        return f'\n{indent}{spec}{self.ftype} {self.name}({self.params});'

    __repr__ = __str__

class Func(Node):
    def __init__(self, name, ftype='void *', params='void *self, void *e', spec='', **kwargs): 
        super().__init__('func', **kwargs)
        self.name = name
        self.ftype = ftype
        self.params = params
        self.nodes = []
        self.spec = spec

    def add(self, node):
        self.nodes.append(node)

    def declaration(self):
        return FuncDeclaration(self.name, self.ftype, self.params, self.spec)

    def __str__(self):
        nodes_str = ''
        for n in self.nodes:
            n.indent = self.indent + 4
            nodes_str += str(n)

        indent = ' ' * self.indent
        if self.spec:
            spec = self.spec + ' '
        else:
            spec = ''
        return f'\n{indent}{spec}{self.ftype} {self.name}({self.params})\n{indent}{{\n{nodes_str}{indent}}}\n'

    __repr__ = __str__


class Call(Node):
    def __init__(self, func_name, params, standalone=True, **kwargs): 
        super().__init__('call', **kwargs)
        self.func_name = func_name
        self.params = params
        self.eol = ';\n' if standalone else ''

    def __str__(self):
        indent = ' ' * self.indent
        return f'{indent}{self.func_name}({self.params}){self.eol}'

class Expr(Node):
    def __init__(self, expr, **kwargs): 
        super().__init__('expr', **kwargs)
        self.expr = expr.format(**kwargs)

    def __str__(self):
        indent = ' ' * self.indent
        return f'{indent}{self.expr}\n'


class Return(Node):
    def __init__(self, value, **kwargs): 
        super().__init__('return', **kwargs)
        self.value = value

    def __str__(self):
        indent = ' ' * self.indent
        return f'{indent}return {self.value};\n'