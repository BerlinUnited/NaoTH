from __future__ import annotations

from typing import List

from ..allocation import Allocation
from ..nodes.expressions import Variable
from ..nodes.misc import Node
from ..writer import Writer
from ..naoth_helpers import print_predict_function, print_get_center_function, print_get_radius_function, \
    print_get_confidence_function


class CHeaderNode(Node):
    """
    Class for writing everything in a C file that is above the first CNN node, e.g. the MeanNode. Should
    be the root node of the complete graph.
    """
    __instance: CHeaderNode = None
    intel_intr_includes = '''
#include <emmintrin.h>
#include <pmmintrin.h>
#include <tmmintrin.h>
#include <immintrin.h>
#include <xmmintrin.h>

'''
    import_statement = '#include "{class_name}.h"\n'
    func_def = 'void {class_name}::cnn{id}(float {out_var_name})\n{{\n'

    intel_intr_required = False

    out_var: Variable = None
    out_var_name = ''

    var_decls: List[Variable] = []
    pointer_decls: List[Variable] = []
    const_decls: List[Variable] = []

    def __init__(self, id, in_dim, weights_method, class_name):
        """
        Initialize the node.
        :param id: An identifier that is added to the function name, see func_def.
        :param in_dim: The three dimensional length of the input: H x W x C
        :param weights_method: The method how the weights are stored and initialized.
                               'direct': The weights are written into the C file.
                               'stdio': The weights are read using ANSI C stdio.
        """
        super().__init__()

        self.id = id
        self.in_dim = in_dim
        self.out_var = Allocation.allocate_var('float', 'x', in_dim)
        self.out_var.decl_written = True
        self.out_dim = in_dim
        self.class_name = class_name
        self.weights_method = weights_method
        if weights_method == 'stdio':
            self.direct = False
            self.stdio = True
        elif weights_method == 'direct':
            self.direct = True
            self.stdio = False
        else:
            raise Exception('Unknown weights method.')
        CHeaderNode.__instance = self

        self.reset()

    @staticmethod
    def instance() -> CHeaderNode:
        """
        Gives the single instance of this class.
        :return: The instance.
        """
        return CHeaderNode.__instance

    def reset(self):
        """
        Clear all variables.
        :return:
        """
        self.var_decls = []
        self.pointer_decls = []
        self.const_decls = []

    def write_c(self):
        """
        Override the base write_c method. This method is called when the WriteC action writes the C code file.
        It is called before the child nodes are visited.
        :return: None.
        """
        self.snippet = ''
        if self.id is None:
            self.id = ''

        # Remove the last added variable from any list for declarations, we will declare it in the
        # function declaration
        self.scores_var = CFooterNode.instance().in_var
        # TODO self.scores_var is the scores variable that is defined in the header file
        try:
            self.var_decls.remove(self.scores_var)
        except Exception:
            pass
        try:
            self.pointer_decls.remove(self.scores_var)
        except Exception:
            pass
        try:
            self.const_decls.remove(self.scores_var)
        except Exception:
            pass

        # Name and definition of the input variable for the C function definition. Variable is call 'out' because
        # it is not only the input but also the output of this layer and following nodes search for 'out_var'.
        self.out_var_name = str(self.out_var) + ''.join(['[' + str(i) + ']' for i in self.in_dim])

        self.snippet += self.import_statement

        # Add header for Intel intrinsics
        if self.intel_intr_required:
            self.snippet += self.intel_intr_includes

        # Write all constants, primarily weights including the init data.
        for v in self.const_decls:
            self.snippet += v.get_def(self.direct).replace('{', '{{').replace('}', '}}')

        self.snippet += "\n"
        self.snippet += self.func_def
        # Now write all variable definitions. That are primarily the outputs of each layer.
        for v in self.var_decls:
            self.snippet += '\t' + v.get_def().replace('{', '{{').replace('}', '}}')
        # And now all pointer definitions.
        for v in self.pointer_decls:
            self.snippet += '\t' + v.get_pointer_decl().replace('{', '{{').replace('}', '}}')

        super().write_c()
        Writer.cur_depth += 1


class CFooterNode(Node):
    """
    Class for writing everything in a C file that is below the last CNN node. Should
    be the last node of the path of 'next' edges.
    """
    __instance: CFooterNode = None

    @staticmethod
    def instance() -> CFooterNode:
        """
        Gives the single instance of this class.
        :return: The instance.
        """
        return CFooterNode.__instance

    snippet = '}}'

    def __init__(self, exe_return_filename, weights_method, prev_node, class_name):
        """
        Initialize the node.
        :param exe_return_filename: Name of file to write test results in.
        :param weights_method: The method how the weights are stored and initialized.
                               'direct': The weights are written into the C file.
                               'stdio': The weights are read using ANSI C stdio.
        :param prev_node: The previous node.
        """
        super().__init__(prev_node)
        CFooterNode.__instance = self
        dim = CHeaderNode.instance().in_dim
        id = CHeaderNode.instance().id
        self.in_dim = prev_node.out_dim
        self.in_var = prev_node.out_var
        self.x_dim = dim[0]
        self.y_dim = dim[1]
        self.class_name = class_name
        if len(dim) > 2:
            self.z_dim = dim[2]
        else:
            self.z_dim = 1
        self.version = "5"
        if id is None:
            self.id = ''
        else:
            self.id = id
        self.exe_return_filename = exe_return_filename
        if weights_method == 'stdio':
            self.weights_init = 'init_weights();'
        elif weights_method == 'direct':
            self.weights_init = ''
        else:
            raise Exception('Unimplemented')

    def write_c(self):
        """
        Write the code of this node.
        :return: None.
        """

        self.snippet += '\n'
        self.snippet += print_predict_function(self.class_name).replace('{', '{{').replace('}', '}}')
        self.snippet += print_get_radius_function(self.class_name).replace('{', '{{').replace('}', '}}')
        self.snippet += print_get_center_function(self.class_name).replace('{', '{{').replace('}', '}}')
        self.snippet += print_get_confidence_function(self.class_name).replace('{', '{{').replace('}', '}}')

        Writer.cur_depth -= 1
        super().write_c()
