from string import ascii_lowercase, ascii_letters, ascii_uppercase, digits
from random import choice, choices, randint

from pycparser import c_ast, parse_file

function_decs = []
function_calls = []
typedefs = []
variable_decs = []
variable_uses = []

new_function_names = {}
new_variable_names = {}
new_type_names = {}

used_names = []


def get_random_string() -> str:
    first_char = choice(ascii_letters)
    remaining_chars = ''.join(choices(ascii_uppercase + ascii_lowercase + digits, k=randint(3, 10)))
    result = first_char + remaining_chars
    if result in used_names:
        return get_random_string()
    used_names.append(result)
    return result


class FuncDefVisitor(c_ast.NodeVisitor):
    def visit_FuncDef(self, node):
        print('Declared function %s' % node.decl.name)
        function_decs.append(node)


class FuncCallVisitor(c_ast.NodeVisitor):
    def visit_FuncCall(self, node):
        print('Called function %s' % node.name.name)
        function_calls.append(node)


class TypeDeclVisitor(c_ast.NodeVisitor):
    def visit_Decl(self, node):
        if isinstance(node.type, c_ast.FuncDecl):
            return
        print('Declared type %s' % node.type.type.names[0])
        typedefs.append(node)


class VariableDeclVisitor(c_ast.NodeVisitor):
    def visit_Decl(self, node):
        if isinstance(node.type, c_ast.FuncDecl) or node.name == '_Value':
            return
        print('Declared variable %s' % node.name)
        variable_decs.append(node)


class VariableUseVisitor(c_ast.NodeVisitor):
    def visit_ID(self, node):
        print('Used ID %s' % node.name)
        variable_uses.append(node)


def show_func_defs(filename):
    # Note that cpp is used. Provide a path to your own cpp or
    # make sure one exists in PATH.
    ast = parse_file(filename, use_cpp=True,
                     cpp_args=r'-Iutils/fake_libc_include')

    v = FuncDefVisitor()
    vc = FuncCallVisitor()
    vt = TypeDeclVisitor()
    vv = VariableDeclVisitor()
    vu = VariableUseVisitor()

    v.visit(ast)
    vc.visit(ast)
    vt.visit(ast)
    vv.visit(ast)
    vu.visit(ast)


def get_all_function_names(ast):
    pass


def main():
    show_func_defs("p_main.c")
    new_type_names = {f"{i}": {"old_name": typedefs[i].type.type.names[0], "new_name": get_random_string()} for i in
                      range(len(typedefs))}
    new_variable_names = {f"{i}": {"old_name": variable_decs[i].name, "new_name": get_random_string()} for i in
                          range(len(variable_decs))}
    new_function_names = {f"{i}": {"old_name": function_decs[i].decl.name, "new_name": get_random_string()} for i in
                          range(len(function_decs))}
    pass


if __name__ == "__main__":
    main()
