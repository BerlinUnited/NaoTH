class Translator::XabslParser

prechigh
	nonassoc UMINUS '!'
	left '*' '/' '%'
	left '+' '-'
 	nonassoc '<' '>' LE GE EQ NEQ
	left AND
	left OR
	nonassoc '?' ':'
preclow


token
LE
GE
EQ
NEQ
AND
OR
ACTION
NUMBER
TRUE
FALSE
STRING
RANGE
INTERNAL
IDENTIFIER
DOC_COMMENT
INITIAL_COMMENT
OPTION
STATE
COMMON
DECISION
TTS
STAY
DT
CDT
UMINUS
IF
ELSE
TOOX
TOSX
SORTS
INITIAL
TARGET
AGENT
FUNCTION
ENUM
INPUT
OUTPUT
BOOL
FLOAT
CONST
NAMESPACE
BEHAVIOR
INCLUDE
SYNCHRONIZED
CAPACITY
CONFLICT
'{' '}'
'(' ')'
'|' ',' ';'
'+' '-'
'*' '/'
'<' '<=' '>' '>=' '==' '!'
':' '?'

rule

xtc:
__initial_comment
__top_level
{@stb.comments(val)}
;

__initial_comment:
INITIAL_COMMENT
|
;

__top_level:
include
__top_level
|
option
|
agents
|
namespace
|
;

agents:
agent
agents
|
agent
;

include:
INCLUDE STRING ';'
{return @stb.include(val)}
;

namespace:
__doc_comment
NAMESPACE IDENTIFIER '(' STRING ')' '{'
{@stb.namespace_early(_values)}
__inner_namespace
'}'
{return @stb.namespace(val)}
;

__inner_namespace:
__inner_namespace
const
{ return val[0] + [val[1]] }
|
__inner_namespace
symbol_declaration
{ return val[0] + [val[1]] }
|
__inner_namespace
enum_symbol_declaration
{ return val[0] + [val[1]] }
|
__inner_namespace
behavior_declaration
{ return val[0] + [val[1]] }
|
{ return [] }
;


behavior_declaration:
__doc_comment
BEHAVIOR IDENTIFIER '{'
{@stb.behavior_declaration_early(_values)}
__parameter_declarations
'}'
{ return @stb.behavior_declaration(val) }
;

symbol_declaration:
__doc_comment
bool_or_float_or_enum __input_output_internal IDENTIFIER __range __measure 
{@stb.symbol_declaration_early(_values)}
symbol_parameter_declarations
';'
{return @stb.symbol_declaration(val)}
;

__input_output_internal:
INPUT
|
OUTPUT
|
INTERNAL
|
;

enum_symbol_declaration:
__doc_comment
bool_or_float_or_enum __input_output_internal 
'{'
enum_elements
'}'
';'
{ return @stb.enum_symbol_declaration(val) }
;

enum_elements:
IDENTIFIER
__enum_elements
{ return [val[0]] + val[1] }
;

__enum_elements:
__enum_elements
',' IDENTIFIER
{ return val[0] + [val[2]] }
|
{ return [] }
;

__minus:
'-'
|
;

bool_or_float_or_enum:
BOOL
|
FLOAT
|
ENUM IDENTIFIER
{ 
  return val[1] 
}
|
;

const:
__doc_comment
__float CONST IDENTIFIER '=' __minus NUMBER __measure ';'
{return @stb.const(val)}
;

__float:
FLOAT
|
;

agent:
__doc_comment
AGENT IDENTIFIER '(' STRING ',' IDENTIFIER  ')' ';'
{return @stb.agent(val)}
;

option:
__doc_comment
OPTION IDENTIFIER '{' 
{@stb.option_early(_values)}
__parameter_declarations
__common_declarations
state_declarations
'}'
{return @stb.option(val)}
;

symbol_parameter_declarations:
'('
__parameter_declarations
')'
{ return val[1] }
|
;

__parameter_declarations:
__parameter_declarations
parameter_declaration
{ return [] + val[0] + [val[1]] }
|
{ return [] };

parameter_declaration:
__doc_comment
bool_or_float_or_enum IDENTIFIER __range __measure ';'
{return @stb.parameter_declaration(val)}
;

__range:
RANGE
|
;

__measure:
STRING
|
;

state_declarations:
state_declaration
__state_declarations
{ return [val[0]] + val[1] }
;

__state_declarations:
__state_declarations
state_declaration
{ return [] + val[0] + [val[1]] }
|
{ return [] };

state_declaration:
__initial __target STATE IDENTIFIER cooperative '{'
{@stb.state_declaration_early(_values)}
decision_tree
ACTION '{'
__output_symbol_assignments_or_subsequent_declarations
'}'
'}'
{ return @stb.state_declaration(val) };

cooperative:
CAPACITY NUMBER
{ return [val[0], val[1]] }
|
SYNCHRONIZED NUMBER
{ return [val[0], val[1]] }
|
SYNCHRONIZED
{ return [val[0]] }
|;

__output_symbol_assignments_or_subsequent_declarations:
__output_symbol_assignments_or_subsequent_declarations
output_symbol_assignment
{ return val[0] + [val[1]] }
|
__output_symbol_assignments_or_subsequent_declarations
subsequent_declaration
{ return val[0] + [val[1]] }
|
{ return [] };

output_symbol_assignment:
IDENTIFIER 
{@stb.output_symbol_assignment_early(_values)}
'=' exp ';'
{ return @stb.output_symbol_assignment(val) };

#handled in state_declaration
__doc_comment:
DOC_COMMENT
|;

#handled in state_declaration
__initial:
INITIAL
|;

#handled in state_declaration
__target:
TARGET
|;

subsequent_declaration:
IDENTIFIER __option_or_behavior '(' 
{@stb.subsequent_declaration_early(_values)}
__set_parameter_list ')' ';'
{return @stb.subsequent_declaration(val)};

__option_or_behavior:
OPTION
|
BEHAVIOR
|
;

__set_parameter_list:
set_parameter __set_parameter_list_end
{return [val[0]] + val[1]}
|
{return []}
;

__set_parameter_list_end:
',' __set_parameter_list
{return val[1]}
|
{return []}
;

set_parameter:
IDENTIFIER 
{@stb.set_parameter_early(_values)}
'=' exp
{return @stb.set_parameter(val)}
;

__with_parameter_list:
with_parameter __with_parameter_list_end
{return [val[0]] + val[1]}
|
{return []}
;

__with_parameter_list_end:
',' __with_parameter_list
{return val[1]}
|
{return []}
;

with_parameter:
IDENTIFIER 
{@stb.with_parameter_early(_values)}
'=' exp
{return @stb.with_parameter(val)}
;

exp:
'!' exp
{return @stb.exp_unary(val)}
|
exp '+' {@stb.exp_binary_early(_values)} exp
{return @stb.exp_binary(val)}
|
exp '-' {@stb.exp_binary_early(_values)} exp
{return @stb.exp_binary(val)}
|
exp '*' {@stb.exp_binary_early(_values)} exp
{return @stb.exp_binary(val)}
|
exp '/' {@stb.exp_binary_early(_values)} exp
{return @stb.exp_binary(val)}
|
exp '%' {@stb.exp_binary_early(_values)} exp
{return @stb.exp_binary(val)}
|
exp NEQ {@stb.exp_binary_early(_values)} exp
{return @stb.exp_binary(val)}
|
exp EQ {@stb.exp_binary_early(_values)} exp
{return @stb.exp_binary(val)}
|
exp '<' {@stb.exp_binary_early(_values)} exp
{return @stb.exp_binary(val)}
|
exp LE {@stb.exp_binary_early(_values)} exp
{return @stb.exp_binary(val)}
|
exp '>' {@stb.exp_binary_early(_values)} exp
{return @stb.exp_binary(val)}
|
exp GE {@stb.exp_binary_early(_values)} exp
{return @stb.exp_binary(val)}
|
exp AND {@stb.exp_binary_early(_values)} exp
{return @stb.exp_binary(val)}
|
exp OR {@stb.exp_binary_early(_values)} exp
{return @stb.exp_binary(val)}
|
exp '?' exp ':' exp
{return @stb.conditional_expression(val)}
|
'(' exp ')'
{return val[1]}
|
'-' exp = UMINUS
{return @stb.exp_unary(val)}
|
NUMBER
{return @stb.exp_number(val)}
|
TRUE
{return @stb.exp_boolean(val)}
|
FALSE
{return @stb.exp_boolean(val)}
|
function
|
IDENTIFIER
{return @stb.exp_identifier(val)}
|
TOSX
{return TOSXTag.new}
|
TOOX
{return TOOXTag.new}
|
SORTS
{return SORTSTag.new}
|
CONFLICT
{return ConflictTag.new}
;


function:
IDENTIFIER 
{@stb.function_early(_values)}
'(' __with_parameter_list ')'
{return @stb.function(val)}
;

__common_declarations:
COMMON DECISION
__common_decision_tree
COMMON ACTION
__common_action
{return [val[2], val[5]]}
|
COMMON DECISION
__common_decision_tree
{return [val[2]]}
|
COMMON ACTION
__common_action
{return [val[2]]}
|;

__common_action:
'{'
__output_symbol_assignments_or_subsequent_declarations
'}'
{return @stb.common_action(val)}
|;

#CDT stuff
__common_decision_tree:
'{' cdtree '}'
{return @stb.common_decision_tree(val)}
|;

#rule with IF and optional ELSEIF
cdtree:
'{'
cdtree
'}'
{return val[1]}
|
__doc_comment
IF '(' exp ')' dtree
__cdtree_elseif_decisions
{return @stb.cdtree(val)};

__cdtree_elseif_decisions:
__doc_comment
ELSE cdtree
{return @stb.cdtree_elseif_decision(val)}
|
{return nil};

#DT stuff
decision_tree:
DECISION '{' dtree '}'
{return @stb.decision_tree(val)}
|
DECISION '{' __doc_comment ELSE dtree '}'
{return @stb.decision_tree(val)}
|
{return @stb.decision_tree(val)}
;

#rule with transition or (IF and ELSE)
dtree:
'{'
dtree
'}'
{return val[1]}
|
transition
{return [val[0]]}
|
__doc_comment
IF '(' exp ')' dtree
__doc_comment
ELSE dtree
{return @stb.dtree(val)}
;


transition:
TTS IDENTIFIER ';'
{return @stb.transition(val)}
|
STAY ';'
{return @stb.transition(val)}
;

end

---- header

require 'translator'
require 'syntax_tree_builder'

---- inner

def initialize
	super
end

#start the parsing, use stb as SyntaxTreeBuilder, tokenize and rewind on demand
def parse(cc, stb)
	@cc = cc
	@stb = stb
	@stb.compiler_context = cc
    if @cc.tz.tokenized?	
    	@cc.tz.rewind
    else
    	File.open(cc.cu.src_fullname, 'rb') do |src|
            @cc.tz.tokenize(src.read)
		end
    end
	do_parse
	@cc = nil
	@stb.compiler_context = nil
	@stb = nil
end

#return a token in the format the parser understands
def next_token
	token = @cc.tz.next_token
	return nil if token.nil?
	return [token.token_type, token]
end

def on_error(error_token_id, error_value, value_stack)
	if error_value == '$'
		raise UnexpectedEndOfFile.new(@cc)
	else
		error = SyntaxError.new(@cc)
		error.token = error_value if error_value.class <= Token
		raise error
	end
end

---- footer
