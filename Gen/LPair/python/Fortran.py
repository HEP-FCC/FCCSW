"""
Author  : Philip Ilten
Created : 2015-06-12

Utility to analyze FORTRAN source and generate C-bindings. The main
class, Source, does the following:

1: Open the source provided as an argument.
2: Build the source using GFORTRAN and grabbing the internal parse tree. 
3: Read the parse tree and create a Python structure representing it.

The two members of Source that are relevant are "procedures" which is
a dictionary of all the identified FORTRAN procedures and "commons"
which is a dictionary of all the identified common blocks. All keys
are the lower case name of either the procedure or common block. The
Procedure class then has two members of import, "symtrees" which is a
dictionary of all defined symbols and "commons" which is all defined
common blocks.

To generate C-interface code for a procedure call Procedure.cexpr(). The
same applies to the common blocks of class Common. See the individual
classes for further details.
"""
import copy, subprocess

class Procedure:
    """
    Class to store FORTRAN procedures, i.e. functions and subroutines.

    line:     parse tree line defining the procedure.
    name:     name of the procedure.
    commons:  dictionary of common blocks.
    symtrees: dictionary of symbol trees.
    """
    def __init__(self, line):
        self.name     = line[17:]
        self.commons  = {}
        self.symtrees = {}

    def __repr__(self): return ('{\'commons\': ' + repr(self.commons) +
                                ', \'symtrees\': ' + repr(self.symtrees) + '}')

    def cexpr(self, base = '  ', indent = '  ', width = 80):
        """
        Return the C-expression for the procedure.

        base:   the base indentation to use.
        indent: the subsequent indentiation to use.
        width:  the maximum width of the line, -1 stops line breaking.
        """
        expr = 'UNKNOWN ' + self.name + '(UNKNOWN)'
        if not self.name in self.symtrees: return expr
        sym  = self.symtrees[self.name]
        expr = (base + ('void' if sym.type == 'UNKNOWN' else sym.ctype()) +
                ' ' + self.name + '_(')
        chrs = len(expr)
        for s in sym.args:
            s = self.symtrees[s].cexpr('ARGUMENTS') + ', '
            if chrs + len(s) > width and width > 0:
                s = '\n' + base + indent + s; chrs = -1
            expr += s; chrs += len(s)
        if expr[-2] == ',': expr = expr[:-2]
        return expr + ');'

    def filter(self, include = ['VARIABLE'], exclude = [], sort = True):
        """
        Filter the symtrees by attributes and return a list sorted by size.

        include: list of attributes that the symtree must have.
        exclude: list of attributes that the symtree must not have. The
                 following is a list of some of the useful symbol attributes.
                      VARIABLE:      variable.
                      DIMENSION:     variable which is an array.
                      DUMMY:         input variable.
                      IN-COMMON:     variable included in common block.
                      SAVE:          saved variable.
                      DATA:          data type of variable.
                      IMPLICIT-TYPE: declared implicitly.
                      PROCEDURE:     procedure.
                      SUBROUTINE:    subroutine procedure.
                      FUNCTION:      function.
        sort:    sort the symbols by base size (not including dimension).
        """
        syms = [s for k, s in self.symtrees.iteritems()]
        rslt = []
        for s in syms:
            keep = False
            for i in include:
                if i in s.atts: keep = True; break
            for e in exclude:
                if e in s.atts: keep = False; break
            if keep: rslt += [s]
        if sort: rslt = sorted(rslt, key = lambda r: ('%03d' % r.size()) + 
                               r.name)
        return rslt

class Common:
    """
    Class to store FORTRAN common blocks.

    line:      parse tree line defining the common block.
    procedure: the procedure in which the common block is defined.
    name:      name of the common block.
    syms:      list of the symbol names contained in the common block.
    """
    def __init__(self, line, procedure):
        line = line.split('/')
        self.name = line[1]
        self.syms = line[2].replace(' ', '').split(',')
        self.procedure = procedure

    def __repr__(self): return repr(self.syms)

    def cexpr(self, base = '  ', indent = '  ', width = 80):
        """
        Return the C-expression for the common block.

        base:   the base indentation to use.
        indent: the subsequent indentiation to use.
        width:  the maximum width of the line, -1 stops line breaking.
        """
        expr  = base + 'extern struct {'
        chrs  = 0
        ctype = None
        for s in self.syms:
            s = self.procedure.symtrees[s]
            if s.ctype() == ctype:
                s = s.cexpr('LIST') + ', '
                if chrs + len(s) > width and width > 0:
                    s = '\n' + base + indent*2 + s; chrs = -1
                expr += s; chrs += len(s)
            else:
                if ctype != None: expr = expr[:-2] + ';'
                ctype = s.ctype()
                s = '\n' + base + indent + s.cexpr('DECLARE') + ', '
                expr += s; chrs = len(s) - 1; 
        if expr[-2] == ',': expr = expr[:-2]
        return expr + ';\n' + base + '} ' + self.name + '_;'

class Symtree:
    """
    Class to store FORTRAN symbol tree, this includes variables.

    line:   parse tree line defining the symbol.
    name:   name of the symbol.
    type:   type of the symbol, i.e. REAL 8.
    atts:   list of symbol attributes, i.e. 'VARIABLE' or 'PROCEDURE'.
    dims:   list of the symbol dimensions.
    args:   possible list of symbol names given as arguments to the symbol.
    ctypes: dictionary to translate FORTRAN types to C-types, includes base
            size.
    """
    def __init__(self, line):
        self.name = line[12:12 + line[12:].find('\'')]
        self.type = 'UNKNOWN' 
        self.atts = []
        self.dims = [1]
        self.args = []
        self.ctypes = {
            'CHARACTER':  ('char',                        1), 
            'COMPLEX 4':  ('struct {float r, i;}',        8), 
            'COMPLEX 8':  ('struct {double r, i;}',      16),
            'COMPLEX 16': ('struct {long double r, i;}', 32),
            'REAL 4':     ('float',                       4),
            'REAL 8':     ('double',                      8),
            'REAL 16':    ('long double',                16),
            'INTEGER 1':  ('signed char',                 1),
            'INTEGER 2':  ('short',                       2),
            'INTEGER 4':  ('int',                         4),
            'INTEGER 8':  ('long long int',               8),
            'LOGICAL 1':  ('signed char',                 1),
            'LOGICAL 2':  ('short',                       2),
            'LOGICAL 4':  ('int',                         4),
            'LOGICAL 8':  ('long long int',               8)
            }

    def __repr__(self): return '(' + self.type + ', ' + repr(self.dims) + ')'

    def parse(self, line):
        """
        Parse a line for the symbol tree from the parse tree.
        """
        if line.startswith('    type spec :'):
            line = line[17:-1].split()
            self.type = line[0]
            if self.type == 'CHARACTER': self.dims = [int(line[1])]
            elif self.type != 'UNKNOWN': self.type += ' ' + line[1]
        elif line.startswith('    Array spec:'):
            line = line[line.find('EXPLICIT') + 11:-2].split(' 1 ')
            self.dims = [int(c) for c in line]
        elif line.startswith('    attributes:'):
            self.atts = line[17:-1].split()
        elif line.startswith('    Formal arglist:'):
            self.args = line[20:].split()

    def size(self):
        """
        Return the variable size.
        """
        if self.type in self.ctypes: return self.ctypes[self.type][1]
        else: return -1

    def ctype(self):
        """
        Return the C-type.
        """
        if self.type in self.ctypes: return self.ctypes[self.type][0]
        else: return 'UNKNOWN'

    def cexpr(self, syntax = 'DECLARE'):
        """
        Return the C-expression for the symbol (if a variable).
        
        syntax: the type of syntax needed: DECLARE is a single line 
                declaration, ARGUMENTS is for function definition arguments,
                and LIST is for declaring a list of variables.
        """
        if syntax == 'DECLARE':
            expr = (self.ctype() + ' ' + self.name + 
                    ']'.join(['[' + str(d) for d in self.dims]) + ']')
        elif syntax == 'ARGUMENTS':
            expr = self.ctype() + ' *' + self.name
            if self.type == 'CHARACTER': expr += ', int len_' + self.name
        elif syntax == 'LIST':
            expr = self.name + ']'.join(['[' + str(d) for d in self.dims]) + ']'
        else: expr = 'UNKNOWN'
        return expr

class Source:
    """
    Analyze FORTRAN source from a file.
    
    source:     the name of the FORTRAN source file.
    target:     name of the binary object to build from the source.
    recipe:     the build recipe, the special variable $SOURCE is the source
                and $TARGET is the target.
    tree:       list of lines from the compiler's parse tree.
    procedures: dictionary of the procedures, i.e. functions and subroutines.
    commons:    dictinoary of the defined common blocks.
    """
    def __init__(self, source = None, target = '/dev/null', recipe = 
                 ['gfortran', '-fdump-fortran-original', '-w', '-c',
                  '$SOURCE', '-o', '$TARGET']):
        self.source     = source
        self.target     = target
        self.recipe     = copy.copy(recipe)
        self.tree       = []
        self.procedures = {}
        self.commons    = {}
        if type(source) == str: self.analyze()

    def __repr__(self): return repr(self.procedures)

    def tree_build(self, source = None, target = None, recipe = None):
        """
        Use the compiler debug option to build the parse tree.
        """
        if source: self.source = source
        if target: self.target = target
        if recipe: self.recipe = copy.copy(recipe)
        if ((type(self.source) == str) and (type(self.target) == str) and 
            (type(self.recipe) == list)): pass
        else: self.tree = []; return
        recipe = [r.replace('$SOURCE', self.source).
                  replace('$TARGET', self.target) for r in self.recipe]
        tree = subprocess.Popen(recipe, stdout = subprocess.PIPE,
                                stderr = subprocess.PIPE).communicate()
        if tree[1] != '': self.tree = []; print tree[1]; return
        else: self.tree = tree[0].split('\n')

    def tree_parse(self, tree = None):
        """
        Build the procedure, common block, and symtree objects using
        the parse tree obtained from the compiler.
        """
        if type(tree) != list: tree = self.tree
        code      = False
        procedure = None
        symtree   = None
        for l in tree:
            # Skip code blocks.
            if l == '------------------------------------------': code = False
            elif l == '  code:': code = True
            elif code: pass
            # Parse the procedures. 
            elif l.startswith('procedure name ='):
                procedure = Procedure(l)
                self.procedures[procedure.name] = procedure
            elif procedure == None: pass
            # Parse the common blocks.
            elif l.startswith('  common:'):
                common = Common(l, procedure)
                procedure.commons[common.name] = common
            elif l.startswith('  symtree:'):
                symtree = Symtree(l)
                procedure.symtrees[symtree.name] = symtree
            # Parse the symbol trees.
            elif symtree == None: pass
            else: symtree.parse(l)
        for k, p in self.procedures.iteritems(): self.commons.update(p.commons)
            
    def analyze(self, source = None, target = None, recipe = None):
        """
        Analyze the source.
        """
        self.tree_build(source, target, recipe); self.tree_parse()
