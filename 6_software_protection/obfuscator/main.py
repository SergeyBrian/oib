from string import ascii_lowercase, ascii_letters, ascii_uppercase, digits
from random import choice, choices, randint

from pycparser import c_ast, parse_file, c_generator

function_decs = []
function_calls = []
typedefs = []
variable_decs = []
variable_uses = []

new_function_names = []
new_variable_names = []
new_type_names = []

new_names = []

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
        if node.decl.name == 'main':
            return
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


class FuncNameReplaceVisitor(c_ast.NodeVisitor):
    def visit_FuncDef(self, node):
        node.decl.name = find_field_by_value(new_function_names, "old_name", node.decl.name)


class FuncCallNameReplaceVisitor(c_ast.NodeVisitor):
    def visit_FuncCall(self, node):
        pass


class TypeNameReplaceVisitor(c_ast.NodeVisitor):
    def visit_Decl(self, node):
        node.type.type.names[0] = find_field_by_value(new_type_names, "old_name", node.type.type.names[0])


class IDNameReplaceVisitor(c_ast.NodeVisitor):
    def visit_ID(self, node):
        node.name = find_field_by_value(new_variable_names, "old_name", node.name)


def show_func_defs(ast):
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


def find_field_by_value(dicts, field, value):
    for d in dicts:
        if d.get(field) == value:
            return d.get("new_name")
    return None


def get_all_function_names(ast):
    pass


def generate_new_names():
    global new_type_names
    global new_variable_names
    global new_function_names

    new_type_names = [{"old_name": typedefs[i].type.type.names[0], "new_name": get_random_string()} for i in
                      range(len(typedefs))]
    new_variable_names = [{"old_name": variable_decs[i].name, "new_name": get_random_string()} for i in
                          range(len(variable_decs))]
    new_function_names = [{"old_name": function_decs[i].decl.name, "new_name": get_random_string()} for i in
                          range(len(function_decs))]


class ReplaceVisitor(c_ast.NodeVisitor):
    def __init__(self, new_function_names, new_variable_names, new_type_names):
        self.new_function_names = new_function_names
        self.new_variable_names = new_variable_names
        self.new_type_names = new_type_names

    def visit_FuncCall(self, node):
        for item in new_names:
            if node.name == item['old_name']:
                node.name = item['new_name']
        if hasattr(node, "body"):
            for node_item in node.body.block_items:
                self.visit(node_item)

    def visit_FuncDef(self, node):
        if hasattr(node, "body"):
            for node_item in node.body.block_items:
                self.visit(node_item)
        self.visit(node.decl)

    def visit_TypeDecl(self, node):
        for item in new_names:
            if node.declname == item["old_name"]:
                node.declname = item["new_name"]
        self.visit(node.type)

    def visit_IdentifierType(self, node):
        for item in new_names:
            if node.names[0] == item["old_name"]:
                node.names[0] = item["new_name"]

    def visit_Decl(self, node):
        for item in new_names:
            if node.name == item['old_name']:
                node.name = item['new_name']

        self.visit(node.type)

    def custom_visit(self, node):
        if isinstance(node, c_ast.FileAST):
            for node_item in node.ext:
                self.visit(node_item)
        if hasattr(node, "body"):
            for node_item in node.body:
                self.visit(node_item)


class ReplaceIDVisitor(c_ast.NodeVisitor):
    def visit_ID(self, node):
        for item in new_names:
            if node.name == item["old_name"]:
                node.name = item["new_name"]


def replace_names(ast):
    visitor = ReplaceVisitor(new_function_names, new_variable_names, new_type_names)
    visitor.custom_visit(ast)
    id_visitor = ReplaceIDVisitor()
    id_visitor.visit(ast)


def print_ast(ast):
    code = ''
    for t in new_type_names:
        code += f"typedef {t['old_name']} {t['new_name']};"

    code += '\n'

    generator = c_generator.CGenerator()
    for node in ast.ext:
        if 'fake' in str(node.coord):
            continue
        code += generator.visit(node)
    print(code)


def main():
    global new_names

    filename = "p_main.c"

    ast = parse_file(filename, use_cpp=False)

    show_func_defs(ast)
    generate_new_names()

    new_names = new_function_names + new_variable_names + new_type_names
    replace_names(ast)

    print_ast(ast)


if __name__ == "__main__":
    main()
