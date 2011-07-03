require 'translator'

module Translator

	#stores symbols and strings within language elements.
	class AttributeHash

		def initialize
			super
			@storage = {}
			@order = []
			@declarations = {}
		end

		# returns value for key.
		def [](key)
			raise_on_undeclared(key)
			return @storage[key]
		end

		# sets value for key.
		def []=(key, value)
			raise_on_undeclared(key)
			raise_on_value(key, value)
			
			return @storage.delete(key) if value.nil?
			return @storage[key] = value
		end

		def raise_on_undeclared(key)
			if @declarations[key].nil?
				LOG.error "undeclared xml attribute '#{key}' accessed"
				raise InternalError.new
			end
		end
		
		def raise_on_value(key, value)
			return if value.nil?
			unless value.class <= @declarations[key]
				LOG.error "class mismatch on xml attribute '#{key}'"
				LOG.error "got #{value.class.name}"
				LOG.error "expected #{@declarations[key].name}"
				raise InternalError.new
			end
		end

		# imports from a string hash. 
		# declares symbols in symbol table if requested by declare(...).
		def import(attributes_string_hash, symbol_table)
			attributes_string_hash.each { |key, value|

				raise_on_undeclared(key)
				decl = @declarations[key]
			
				if  decl == String
					@storage[key] = value
				elsif decl <= Symbol
                    #simply ignore redeclarations
					symbol = symbol_table.declare(value, decl)
					@storage[key] = symbol
				else
					raise InternalError.new
				end
			}
		end

		# returns array of keys.
		def keys
			return @storage.keys
		end

		# iterates over keys in order of declaration.
		def each
			@order.each { |key|
				yield(key, @storage[key]) if @storage.key?(key)
			}
		end

		# declares a key. klass can be String or a Symbol subclass. 
		# 'required' String values are initialized with an empty String.
		def declare(key, klass = String, required = false)
			@declarations[key] = klass
			@order << key unless @order.include?(key)
			@storage[key] = '' if klass == String && required
		end
	end

	# language elements are the XABSL tags
	# base class for elements of the syntax tree(s).
	class LanguageElement

		@in_type = :any
		@out_type = :any
		@tag = nil
		class << LanguageElement #strange syntax, eeh?
			attr_reader :in_type
			attr_reader :out_type
			attr_reader :tag
		end

		#usually ignore text in XML files
		def text=(text)
		end

		def in_type
			return self.class.in_type
		end

		def out_type
			return self.class.out_type
		end

		def tag
			return self.class.tag
		end

		# AttributeHash for this language element
		attr_reader :attr
		
		# array of child language elements
		attr_reader :children
		
		# parent of this language element
		attr_accessor :parent
		
    # for error handling
		attr_reader :leftmost_token
		attr_reader :rightmost_token
		
		def initialize
			super
			@attr = AttributeHash.new
			@children = []
			@parent = nil
			@leftmost_token = nil
			@rightmost_token = nil
		end


    #set leftmost and rightmost token for a language element
		def map_tokens(array)
			array.each do |a|
				if a.class <= Token
					@leftmost_token = a if @leftmost_token.nil?
					@rightmost_token = a
				elsif a.class <= LanguageElement
					@leftmost_token = a.leftmost_token if leftmost_token.nil? && !a.leftmost_token.nil?
					@rightmost_token = a.rightmost_token unless a.rightmost_token.nil?
				elsif a.class <= Array
					map_tokens a
				end
			end
		end


		# adds le as a child to this language element
		def add_child(le, cc=nil, &block)
			if valid_child(le)
				@children << le
				le.parent = self
			else
				if !block.nil?
					block.call(le)
				elsif cc.nil?
				  $stderr << "invalid child '#{le.tag}' for '#{self.tag}'"
			 		raise InternalError.new("invalid child '#{le.tag}' for '#{self.tag}'")
				elsif le.leftmost_token.nil? && le.rightmost_token.nil?
				  $stderr << "invalid child '#{le.tag}' for '#{self.tag}'"
			 		raise InternalError.new("invalid child '#{le.tag}' for '#{self.tag}'")
			 	else
			 	  if le.leftmost_token.nil?
					  cc.eh.error UnexpectedLE.new(cc, le.rightmost_token, le, self)
					else
            cc.eh.error UnexpectedLE.new(cc, le.leftmost_token, le , self)
          end
			 	end
			end
		end

		#removes le as a child
		def remove_child(le)
			@children.delete le
			le.parent = nil
		end
		
		# method to return true if le can be added as a child
    # default is no children allowed
		def valid_child(le)
			return false
		end
		
		# returns true if this language elements has children.
		def children?
			return children.size > 0
		end

		# returns the first child with child.class == clazz
		# raises InternalError on ambuigities or if no child is found
		def child(clazz)
			result = []
			children.each do |child|
				if child.class <= clazz
					result << child
				end
			end

			raise InternalError.new unless result.size == 1
			return result[0]
		end
		
		# produces XML output for this language element.
		# this implementation should be sufficent for almost all language elements.
		def to_xml(out)
			out << out.pindent(@children.empty? ? 0 : 1)
			out << "<#{tag}"

			attr.each { |key, value|
				out << " #{key}=\"#{value.class <= Symbol ? value.identifier : escape_xml(value) }\""
			}

			if(@children.empty?)
				out << "/>#{ENDL}"
			else
				out << ">#{ENDL}"

				@children.each { |child|
					child.to_xml(out)
				}
	
				out < "</#{tag}>#{ENDL}"
			end
		end

		SPECIALS = [ /&(?!#?[\w-]+;)/u, /</u, />/u, /"/u ]
		SUBSTITUTES = ['&amp;', '&lt;', '&gt;', '&quot;']

		def escape_xml(string)
		
			copy = string.clone
			copy.gsub!( SPECIALS[0], SUBSTITUTES[0] )
            copy.gsub!( SPECIALS[1], SUBSTITUTES[1] )
            copy.gsub!( SPECIALS[2], SUBSTITUTES[2] )
            copy.gsub!( SPECIALS[3], SUBSTITUTES[3] )
		
			return copy
		end

		# produces XTC code output for this language element.
		# this method implementation differs between classes.
		def to_code(cc, out)
			raise InternalError.new("#{self.class.name}: to_code not implemented")
		end

		# shortcut, often used in to_code(...)
		def generate_children_code(cc, out)
			@children.each { |child|
				cc.bc.generate_code(child)
			}	
		end

		# delegate call to AttributesHash.
		# entry point for intercepting symbol declarations while XML parsing.
		def import(attributes_string_hash, symbol_table)
			attr.import(attributes_string_hash, symbol_table)
		end
		
		def search_upwards(le_class)
			current = self
			while true
				return current if current.class == le_class
				return nil if current.class == RootLanguageElement
				current = current.parent
			end
		end
	end

	#class used during development to cope with yet unimplemented tags
	class UnknownLanguageElement < LanguageElement

		attr_reader :debug_tag

		def initialize(debug_tag)
			super()
			@debug_tag = debug_tag
			LOG.warn "UnknownLanguageElement created for tag #{debug_tag}"
		end

		def to_code(cc, out)
			out > "/* start dummy code for #{@debug_tag} tag */#{ENDL}" if VERBOSE
			generate_children_code(cc, out)
			out < "/* end dummy code for #{@debug_tag} tag */#{ENDL}" if VERBOSE
		end
	end

	#root language elements are the root element of the syntax trees 
	class RootLanguageElement < LanguageElement

		attr_accessor :doctype

		def initialize
			super
		end
		
		def valid_child le
			return true
		end
		
		def to_code(cc, out)
			
			generate_children_code(cc, out)
		end
	
		def to_xml(out)

			out % '<?xml version="1.0" encoding="ISO-8859-1"?>' << "#{ENDL}"
			out % doctype << "#{ENDL}" unless doctype.nil?

			@children.each { |child|
				child.to_xml(out)
			}
		end
	end

	class OptionTag < LanguageElement

		attr_accessor :entities
		@tag = TAG_OPTION

		def initialize
			super
			attr.declare(ATTR_XMLNS)
			attr.declare(ATTR_XMLNS_XSI)
			attr.declare(ATTR_XSI_SCHEMALOCATION)
			attr.declare(ATTR_XMLNS_XI)
			attr.declare(ATTR_NAME, OptionSymbol)
			attr.declare(ATTR_INITIAL_STATE, StateSymbol)
			
			@entities = []
		end

		def valid_child(le)
			return [StateTag, CDTTag, CommonActionTag].include?(le.class)
		end

		def to_ic(out)
			out << "// option #{attr[ATTR_NAME].identifier}#{ENDL}" if out.verbose
			out << "// number of states#{ENDL}" if out.verbose
			
			state_count = 0
			children.each do |child|
				state_count +=1 if child.class == StateTag
			end
			
			out << "#{state_count}#{ENDL}"
			out << "// The states#{ENDL}" if out.verbose
			children.each do |child|
			  if (child.class == StateTag)
			    begin
						out << "#{child.attr[ATTR_NAME].identifier} "
						if (!child.attr[ATTR_CAPACITY].nil?)
						  out << "c #{child.attr[ATTR_CAPACITY]}#{ENDL}"
						elsif (!child.attr[ATTR_SYNCHRONIZED].nil?)
						  out << "s #{child.attr[ATTR_SYNCHRONIZED]}#{ENDL}"
	          else
	            out << "n#{ENDL}"
						end
					end
				end
			end
			out << "#{ENDL}"
			
			out << "// initial-state#{ENDL}" if out.verbose
			out << "#{attr[ATTR_INITIAL_STATE].identifier}#{ENDL}"
			children.each do |child|
				child.to_ic(out) if child.class == StateTag
			end

			out << "#{ENDL}#{ENDL}#{ENDL}"
		end


		def to_code(cc, out)
			#TODO was machen mit dem comment, etc.
			od = cc.pc.option_definitions_by_symbol[attr[ATTR_NAME]]
			throw InternalError.new if od.nil?	
			
			descr = od.attr[ATTR_DESCRIPTION]
			out % "/** #{descr} */#{ENDL}" unless (descr.nil? || descr.empty?)
			out > "#{TOKEN_OPTION} #{cc.bc[ATTR_NAME]} {#{ENDL}#{ENDL}"
			od.generate_children_code(cc, out)
			out << "#{ENDL}" if od.children.size > 0

			generate_children_code(cc, out)
			
			out < "}"
			out << " /* #{TOKEN_OPTION} #{cc.bc[ATTR_NAME]} end */"  if VERBOSE
			out << "#{ENDL}#{ENDL}"
		end

		def to_xml(out)
			out > "<#{tag}"

			attr.each { |key, value|
				out << " #{key}=\"#{value.class <= Symbol ? value.identifier : value}\""
			}

			out << ">#{ENDL}"

			symbols = []
			behaviors = []
			strings = []
			
			entities.each do |key|
				symbols << key if key.class <= NamespaceSymbol && key.namespace_type == :symbols
				behaviors << key if key.class <= NamespaceSymbol && key.namespace_type == :behaviors
				strings << key if key.class <= String
			end
			
			symbols.sort! do |a,b|
				a.identifier.downcase <=> b.identifier.downcase
			end
			behaviors.sort! do |a,b|
				a.identifier.downcase <=> b.identifier.downcase
			end
			strings.sort! do |a,b|
				a.downcase <=> b.downcase			
			end
			
			symbols.each { |entity|
				out % "&#{entity.identifier};#{ENDL}"
			}	
			behaviors.each { |entity|
				out % "&#{entity.identifier};#{ENDL}"
			}	
			strings.each { |entity|
				out % "&#{entity};#{ENDL}"
			}	
			
			@children.each { |child|
				child.to_xml(out)
			}
			
			
			out < "</#{tag}>#{ENDL}"

		end
		
	end

	class StateTag < LanguageElement

		@tag = TAG_STATE
		
		def initialize
			super
			attr.declare(ATTR_NAME, StateSymbol)
			attr.declare(ATTR_IS_TARGET_STATE)
			attr.declare(ATTR_CAPACITY)
			attr.declare(ATTR_SYNCHRONIZED)
		end
				
		def valid_child(le)
			return [TAG_SDOS, TAG_SBOS, TAG_SEOS, TAG_SBB, TAG_SO, TAG_DT].include?(le.tag)
		end

		def to_ic(out)
			out << "// state #{attr[ATTR_NAME].identifier}#{ENDL}" if out.verbose
			if  attr[ATTR_IS_TARGET_STATE] == 'true'
				out << "// is target state (1)#{ENDL}" if out.verbose
				out << "1#{ENDL}"
			else
				out << "// not a target state (0)#{ENDL}" if out.verbose
				out << "0#{ENDL}"
			end
				
			out << "// number of subsequent behavior definitions and output symbol assignments#{ENDL}" if out.verbose
			count = 0

			#include common action here
			search_upwards(OptionTag).children.each do |sibling|
					count += sibling.children.size if sibling.class == CommonActionTag
			end

			children.each do |child|
				count += 1 if child.class <= SubTag || child.class <= SOSTag
			end
			out << "#{count}#{ENDL}"

			search_upwards(OptionTag).children.each do |sibling|
				if sibling.class == CommonActionTag
					sibling.children.each do |child|
						child.to_ic(out)
					end
				end
			end

			children.each do |child|
				child.to_ic(out) if child.class <= SubTag || child.class <= SOSTag
			end


			children.each do |child|
				child.to_ic(out) if child.class == DTTag
			end
			
		end

		def to_code(cc, out)

			option = search_upwards(OptionTag)
			keywords = []
			keywords << TOKEN_INITIAL if option.attr[ATTR_INITIAL_STATE] == attr[ATTR_NAME]
			keywords << TOKEN_TARGET if attr[ATTR_IS_TARGET_STATE] == 'true'
			keywords << TOKEN_STATE

			out > "#{keywords.join(' ')} #{cc.bc[ATTR_NAME]} {#{ENDL}"

			children.each do |child|
				cc.bc.generate_code(child) if child.class == DTTag
			end

			out > "#{TOKEN_ACTION} {#{ENDL}"
			children.each do |child|
				cc.bc.generate_code(child) if child.class <= SOSTag
			end

			children.each do |child|
				cc.bc.generate_code(child) if [SBBTag, SOTag].include?(child.class)
			end
			out < "}#{ENDL}"


			out < "}"
			out << " /* #{TOKEN_STATE} #{cc.bc[ATTR_NAME]} end */" if VERBOSE
			out << "#{ENDL}#{ENDL}"
		end

	end

	class CommonActionTag < LanguageElement

		@tag = TAG_COMMON_ACTION
					
		def valid_child(le)
			return [TAG_SDOS, TAG_SBOS, TAG_SEOS, TAG_SBB, TAG_SO].include?(le.tag)
		end

		def to_ic(out)
			out << "// common action#{ENDL}" if out.verbose
	
			out << "// number of subsequent behavior definitions#{ENDL}" if out.verbose
			count = 0
			children.each do |child|
				count += 1 if child.class <= SubTag
			end
			out << "#{count}#{ENDL}"

			children.each do |child|
				child.to_ic(out) if child.class <= SubTag
			end

			out << "// number of set output symbols#{ENDL}" if out.verbose
			count = 0
			children.each do |child|
				count += 1 if child.class <= SOSTag
			end
			out << "#{count}#{ENDL}"

			children.each do |child|
				child.to_ic(out) if child.class <= SOSTag
			end
		end

		def to_code(cc, out)
			out > "#{TOKEN_COMMON} #{TOKEN_ACTION} {#{ENDL}"
			children.each do |child|
				cc.bc.generate_code(child) if child.class <= SOSTag
			end

			children.each do |child|
				cc.bc.generate_code(child) if [SBBTag, SOTag].include?(child.class)
			end
			out < "}#{ENDL}"
			out << " /* #{TAG_COMMON_ACTION} end */" if VERBOSE
			out << "#{ENDL}#{ENDL}"
		end

	end

  class SubTag < LanguageElement
  end
  
	class SBBTag < SubTag

		@tag = TAG_SBB

		def initialize
			super
			attr.declare(ATTR_REF, BasicBehaviorSymbol)
		end
		
		def valid_child le
			return le.class == SPTag
		end
		
		def to_ic(out)
			out << "// subsequent basic behavior (b)#{ENDL}" if out.verbose
			out << "a #{attr[ATTR_REF].identifier}#{ENDL}"
			out << "// number of set parameters#{ENDL}" if out.verbose
			out << "#{children.size}#{ENDL}"
			children.each do |child|
				child.to_ic(out) unless children[0].nil?
			end
		end

		def to_code(cc, out)
			
			#does an option exist with the same identifier ?
			out % "#{cc.bc[ATTR_REF]}"
			out << " #{TOKEN_BEHAVIOR}" if cc.st.get(attr[ATTR_REF].identifier, OptionSymbol)
			out <<"("

			first = true
			@children.each { |child|
				out << ", " unless first
				cc.bc.generate_code(child)
				first = false
			}

			out << ");#{ENDL}"

		end
	end

	class SOTag < SubTag

		@tag = TAG_SO

		def initialize
			super
			attr.declare(ATTR_REF, OptionSymbol)
		end

		def valid_child le
			return SPTag == le.class
		end

		def to_ic(out)
			out << "// subsequent option (o)#{ENDL}" if out.verbose
			out << "o #{attr[ATTR_REF].identifier}#{ENDL}"
			out << "// number of set parameters#{ENDL}" if out.verbose
			out << "#{children.size}#{ENDL}"
			children.each do |child|
				child.to_ic(out) unless children[0].nil?
			end
		end

		def to_code(cc, out)
			#does an behavior exist with the same identifier ?
			out % "#{cc.bc[ATTR_REF]}"
			out << " #{TOKEN_OPTION}" if cc.st.get(attr[ATTR_REF].identifier, BasicBehaviorSymbol)
			out << "("

			first = true
			@children.each { |child|
				out << ", " unless first
				cc.bc.generate_code(child)
				first = false
			}

			out << ");#{ENDL}"

		end

	end


	class SPTag < LanguageElement

		@tag = TAG_SP
		attr_accessor :in_type

		def initialize
			super
			@in_type = :any
			attr.declare(ATTR_REF, DomainValueSymbol)
		end
		
		def valid_child le
			return @in_type == :any || le.out_type == :any || @in_type == le.out_type
		end

		#late declaration dependant on parent class
		def parent=(value)
			super
			
			if parent.class == SBBTag
				attr.declare(ATTR_REF, BasicBehaviorParameterSymbol)
			elsif parent.class == SOTag
			 	attr.declare(ATTR_REF, OptionParameterSymbol)
			else
				raise InternalError.new
			end
		end

		def to_ic(out)
			out << "// set-parameter#{ENDL}" if out.verbose
			case in_type
			when :decimal
			  out << "d "
			when :boolean
			  out << "b "
			when :enum
			  out << "e #{attr[ATTR_REF].enum_domain.identifier} "
			else
			  raise InternalError.new
			end
			out << "#{attr[ATTR_REF].identifier}#{ENDL}"
			children[0].to_ic(out)
		end

		def to_code(cc, out)
			throw "only 1 child expected" unless @children.size == 1
			throw "parent should be SBBTag or SOTag but is #{parent.tag}" unless [SBBTag, SOTag].include?(parent.class)

			#skip output of prefixed '@'
			out << "#{cc.sc.to_code(attr[ATTR_REF], false)} = "
			
			generate_children_code(cc, out)
		end
	end

	class OPRTag < LanguageElement

		@tag = TAG_OPR
		attr_accessor :out_type
		attr_accessor :enum_domain

		def initialize
			super
			@out_type = :any
			attr.declare(ATTR_REF, OptionParameterSymbol)
		end

		def to_ic(out)
			out << "// option-parameter-ref#{ENDL}" if out.verbose
			out << "p #{attr[ATTR_REF].identifier}#{ENDL}"
			children.each do |child|
				child.to_ic(out)
			end			
		end
				
  		def to_code(cc, out)
			out << cc.bc[ATTR_REF]
			out << " /* #{TAG_OPR} */"  if VERBOSE
		end
	end

	class DVTag < LanguageElement

		@tag = TAG_DV
		@out_type = :decimal

		def initialize
			super
			attr.declare(ATTR_VALUE)
		end

		def valid_child le
			return false
		end

		def normalize
			float = attr[ATTR_VALUE].to_f 
			int = float.to_i

			if (float == int.to_f)
				attr[ATTR_VALUE] = int.to_s
			else
				attr[ATTR_VALUE] = float.to_s
			end
		end

		def to_ic(out)
			normalize
			out << "// decimal-value#{ENDL}" if out.verbose
			out << "v #{attr[ATTR_VALUE]}#{ENDL}"
		end

		def to_xml(out)
			normalize
			super
		end
		
		def to_code(cc, out)
			normalize
			out << attr[ATTR_VALUE]
		end
	end

	class BVTag < LanguageElement

		@tag = TAG_BV
		@out_type = :boolean

		def initialize
			super
			attr.declare(ATTR_VALUE)
		end

		def valid_child le
			return false
		end

		def to_ic(out)
			out << "// boolean-value#{ENDL}" if out.verbose
			out << "v #{attr[ATTR_VALUE]}#{ENDL}"
		end
		
		def to_code(cc, out)
			out << attr[ATTR_VALUE]
		end
	end

	class StatementContainingTag < LanguageElement

		def condition_and_statement_to_ic(out)
		
			raise InternalError.new if children.size < 2
			raise InternalError.new unless children[0].class == CondTag
			out << "// condition#{ENDL}" if out.verbose
			children[0].to_ic(out)
			statement_to_ic(out, children[1..-1])
		end

		def statement_to_ic(out, children = self.children)
			
			raise InternalError.new if children.size == 0
			
			case children[0]
			when TTSTag:
				raise InternalError.new if children.size != 1
				children[0].to_ic(out)
			when IfTag:
				out << "// if/else block (i) #{ENDL}" if out.verbose
				out << "i#{ENDL}"
				children[0].to_ic(out)
			else
				p children[0].class.name
				exit 1
			end	
			
			children[1..-1].each do |child|
				child.to_ic(out)
			end
			
			
		end	
	
	
	end

	class DTTag < StatementContainingTag

		@tag = TAG_DT

		def valid_child le
			return [IfTag, ElseTag, TTSTag].include?(le.class)
		end

		def to_ic(out)
			#include common decision tree here
			search_upwards(OptionTag).children.each do |child|
				if child.class == CDTTag
					child.to_ic(out)
					break
				end
			end
			
			out << "// state decision tree:#{ENDL}" if out.verbose
			statement_to_ic(out)
		end

		def to_code(cc, out)
			# exactly one child that is TTSTag and has a transition to self state.
			return if children.size == 1 && children[0].class == TTSTag && parent.attr[ATTR_NAME] == children[0].attr[ATTR_REF]
			
			out > "#{TOKEN_DECISION} {#{ENDL}"
			
			#begin with else if there is a common decision tree
			search_upwards(OptionTag).children.each do |child|
				if child.class == CDTTag
					out % "#{TOKEN_ELSE}#{ENDL}"
					break
				end
			end

			generate_children_code(cc, out)
			
			out < "}"
			out << " /* #{TAG_DT} end */" if VERBOSE
			out << "#{ENDL}"
		end
	end



	class IfTag < StatementContainingTag
		
		@tag = TAG_IF
		
		def valid_child le
			return [IfTag, ElseTag, CondTag, TTSTag].include?(le.class)
		end

		def to_ic(out)
						
			out << "// if statement#{ENDL}" if out.verbose
			condition_and_statement_to_ic(out)
			
		end

		def to_code(cc, out)
			throw "child #1 is not #{TAG_COND}" unless @children[0].tag == TAG_COND

			#print description of condition tag
			descr = @children[0].attr[ATTR_DESCRIPTION]
			out % "/** #{descr} */#{ENDL}" unless (descr.nil? || descr.empty?)

			out > "#{TOKEN_IF} ("
			cc.bc.generate_code(@children[0])
			out << ") {#{ENDL}"
			(1..@children.size - 1).each { |i|
				cc.bc.generate_code(@children[i])
			}
			out < "}#{ENDL}"
		end
	end
	
	class ElseTag < StatementContainingTag

		@tag = TAG_ELSE
		
		def valid_child le
			return [IfTag, ElseTag, TTSTag].include?(le.class)
		end

		def to_ic(out)
			out << "// else statement#{ENDL}" if out.verbose
			statement_to_ic(out)
		end

		def to_code(cc, out)

			out > "#{TOKEN_ELSE} {#{ENDL}"
			generate_children_code(cc, out)
			out < "}#{ENDL}"
		end
	
	end
	
	class CondTag < LanguageElement

		@tag = TAG_COND
		@in_type = :boolean

		def initialize
			super
			attr.declare(ATTR_DESCRIPTION, String)
		end

		def valid_child le
			return le.out_type == :any || in_type == le.out_type
		end
		
		def to_ic(out)
			children.each do |child|
				child.to_ic(out)
			end
		end
	
		def to_code(cc, out)
			generate_children_code(cc, out)
		end

        #hack to suppress description attributes on CE children
        #and to generate on all other even if empty
        def to_xml(out)
            if parent.class == CETag
                attr[ATTR_DESCRIPTION] = nil
            else
                attr[ATTR_DESCRIPTION] = '' if attr[ATTR_DESCRIPTION].nil?
            end
            super
        end
	end

	class TTSTag < LanguageElement

		@tag = TAG_TTS

		def initialize
			super
			attr.declare(ATTR_REF, StateSymbol)
		end
		
		def valid_child le
			return false
		end

		def to_ic(out)
			out << "// transition to state (t) #{attr[ATTR_REF].identifier}#{ENDL}" if out.verbose
			out << "t #{attr[ATTR_REF].identifier}#{ENDL}"
		end

		def to_code(cc, out)
			#current_state can be nil because of common-decision-trees
			current_state = search_upwards(StateTag)
			
			if !current_state.nil? && current_state.attr[ATTR_NAME] == attr[ATTR_REF]
				out % "#{TOKEN_STAY};#{ENDL}"
			else
				out % "#{TOKEN_TTS} #{cc.bc[ATTR_REF]};#{ENDL}"
			end
		end
	end
	
	class BinaryOperator < LanguageElement
		
		attr_reader :operator
		attr_reader :priority
		attr_reader :associative
	
		def initialize(operator, priority, associative)
			super()
			@operator = operator
			@priority = priority
			@associative = associative
		end

		def valid_child le
			return in_type == :any || le.out_type == :any || in_type == le.out_type
		end
		
		def to_code(cc, out)
	
			throw "child count is must be greater than 2" unless @children.size >= 2
			
			braces = (@children[0].class <= BinaryOperator && @children[0].priority <  self.priority)
			out << "(" if braces
			out << "/* braces needed */" if braces && VERBOSE
			cc.bc.generate_code(@children[0])
			out << ")" if braces
			
			(1..@children.size-1).each {|i|
				out << " #{operator} "
				braces = (@children[i].class <= BinaryOperator && @children[i].priority <  self.priority)
				out << "(" if braces
				out << "/* braces needed */" if braces && VERBOSE
				cc.bc.generate_code(@children[i])
				out << ")" if braces
			}
		end
	end


	class OrTag < BinaryOperator

		@tag = TAG_OR
		@in_type = :boolean
		@out_type = :boolean

		def to_ic(out)
			out << "// or (#{children.size})#{ENDL}" if out.verbose
			out << "| #{children.size}#{ENDL}"
			children.each do |child|
				child.to_ic(out)
			end			
		end
		
		def initialize
			super('||', 1 , true)
		end
	end

	class AndTag < BinaryOperator

		@tag = TAG_AND
		@in_type = :boolean
		@out_type = :boolean

		def to_ic(out)
			out << "// and (#{children.size})#{ENDL}" if out.verbose
			out << "& #{children.size}#{ENDL}"
			children.each do |child|
				child.to_ic(out)
			end			
		end
		
		def initialize
			super('&&', 2, true)
		end
	end

	class LTTag < BinaryOperator

		@tag = TAG_LT
		@in_type = :decimal
		@out_type = :boolean
		
		def to_ic(out)
			out << "// <#{ENDL}" if out.verbose
			out << "<#{ENDL}"
			children.each do |child|
				child.to_ic(out)
			end			
		end
		
		def initialize
			super('<', 3, false)
		end
	end

	class GTTag < BinaryOperator

		@tag = TAG_GT
		@in_type = :decimal
		@out_type = :boolean
		
		def to_ic(out)
			out << "// >#{ENDL}" if out.verbose
			out << ">#{ENDL}"
			children.each do |child|
				child.to_ic(out)
			end			
		end
		
		def initialize
			super('>', 3, false)
		end
	end

	class GTEQTag < BinaryOperator

		@tag = TAG_GTEQ
		@in_type = :decimal
		@out_type = :boolean
		
		def to_ic(out)
			out << "// >=#{ENDL}" if out.verbose
			out << "g#{ENDL}" 
			children.each do |child|
				child.to_ic(out)
			end			
		end
		
		def initialize
			super('>=', 3, false)
		end
	end

	class LTEQTag < BinaryOperator

		@tag = TAG_LTEQ
		@in_type = :decimal
		@out_type = :boolean
		
		def to_ic(out)
			out << "// <=#{ENDL}" if out.verbose
			out << "l#{ENDL}" 
			children.each do |child|
				child.to_ic(out)
			end			
		end
		
		def initialize
			super('<=', 3, false)
		end
	end


	class EISCTag < BinaryOperator

		@tag = TAG_EISC
		@in_type = :enum
		@out_type = :boolean
		
		def to_ic(out)
			out << "// enumerated-input-symbol-comparison#{ENDL}" if out.verbose
			out << "c#{ENDL}"
			children.each do |child|
				child.to_ic(out)
			end			
		end
		
		def initialize
			super('==', 0, false)
		end
	end

	class EQTag < BinaryOperator

		@tag = TAG_EQ
		@in_type = :decimal
		@out_type = :boolean
		
		def to_ic(out)
			out << "// ==#{ENDL}" if out.verbose
			out << "=#{ENDL}"
			children.each do |child|
				child.to_ic(out)
			end			
		end
		
		def initialize
			super('==', 3, false)
		end
	end

	class NEQTag < BinaryOperator

		@tag = TAG_NEQ
		@in_type = :decimal
		@out_type = :boolean
		
		def to_ic(out)
			out << "// !=#{ENDL}" if out.verbose
			out << "n#{ENDL}"
			children.each do |child|
				child.to_ic(out)
			end			
		end
		
		def initialize
			super('!=', 3, false)
		end
	end

	class PlusTag < BinaryOperator

		@tag = TAG_PLUS
		@in_type = :decimal
		@out_type = :decimal
		
		def to_ic(out)
			out << "// plus#{ENDL}" if out.verbose
			out << "+#{ENDL}"
			children.each do |child|
				child.to_ic(out)
			end			
		end
		
		def initialize
			super('+', 4, true)
		end
	end

	class MinusTag < BinaryOperator

		@tag = TAG_MINUS
		@in_type = :decimal
		@out_type = :decimal
	
		def to_ic(out)
			out << "// minus#{ENDL}" if out.verbose
			out << "-#{ENDL}"
			children.each do |child|
				child.to_ic(out)
			end			
		end
		
		def initialize
			super('-', 4, false)
		end
	end

	class MPLTag < BinaryOperator

		@tag = TAG_MPL
		@in_type = :decimal
		@out_type = :decimal
	
		def to_ic(out)
			out << "// multiply#{ENDL}" if out.verbose
			out << "*#{ENDL}"
			
			children.each do |child|
				child.to_ic(out)
			end			
		end
		
		def initialize
			super('*', 5, true)
		end
	end

	
	class DIVTag < BinaryOperator

		@tag = TAG_DIV
		@in_type = :decimal
		@out_type = :decimal
	
		def to_ic(out)
			out << "// divide#{ENDL}" if out.verbose
			out << "d#{ENDL}"
			children.each do |child|
				child.to_ic(out)
			end			
		end
		
		def initialize
			super('/', 5, false)
		end
	end


	class MODTag < BinaryOperator
	
		@tag = TAG_MOD
		@in_type = :decimal
		@out_type = :decimal
	
		def to_ic(out)
			out << "// modulo#{ENDL}" if out.verbose
			out << "%#{ENDL}"
			children.each do |child|
				child.to_ic(out)
			end						
		end
	
		def initialize
			super('%', 5, false)
		end
	end

	class DOSRTag < LanguageElement

		@tag = TAG_DOSR
		@out_type = :decimal

		def initialize
			super
			attr.declare(ATTR_REF, DecimalOutputSymbol)
		end
		
		def to_ic(out)
			out << "// decimal-output-symbol-ref#{ENDL}" if out.verbose
			out << "o #{attr[ATTR_REF].identifier}#{ENDL}"
		end
				
		def to_code(cc, out)
			out << "#{cc.bc[ATTR_REF]}"
			out << " /* #{TAG_DOSR} */"  if VERBOSE
		end
	end

	class BOSRTag < LanguageElement

		@tag = TAG_BOSR
		@out_type = :boolean

		def initialize
			super
			attr.declare(ATTR_REF, BooleanOutputSymbol)
		end

		def to_ic(out)
			out << "// boolean-output-symbol-ref#{ENDL}" if out.verbose
			out << "o #{attr[ATTR_REF].identifier}#{ENDL}"
		end

		def to_code(cc, out)
			out << "#{cc.bc[ATTR_REF]}"
			out << " /* #{TAG_BOSR} */"  if VERBOSE
		end
	end

	class EOSRTag < LanguageElement

    @tag = TAG_EOSR
    @out_type = :enum
    attr_accessor :enum_domain
    		
		def initialize
			super
			attr.declare(ATTR_REF, EnumOutputSymbol)
		end
				
		def to_ic(out)
			out << "// enumerated-output-symbol-ref#{ENDL}" if out.verbose
			out << "o #{attr[ATTR_REF].identifier}#{ENDL}"
		end

		def to_code(cc, out)
			out << "#{cc.bc[ATTR_REF]}"
			out << " /* #{TAG_EOSR} */"  if VERBOSE
		end
	end

	class DISRTag < LanguageElement

		@tag = TAG_DISR
		@out_type = :decimal

		def initialize
			super
			attr.declare(ATTR_REF, DecimalInputSymbol)
		end
		
		def valid_child le
			return le.class == WPTag
		end

		def to_ic(out)
			out << "// decimal-input-symbol-ref#{ENDL}" if out.verbose
			out << "i #{attr[ATTR_REF].identifier}#{ENDL}"
			out << "// number of with-parameter elements#{ENDL}" if out.verbose
			out << "#{children.size}#{ENDL}"

			children.each do |child|
				child.to_ic(out)
			end			
		end
				
		def to_code(cc, out)
			out << "#{cc.bc[ATTR_REF]}("
			first = true
			@children.each { |child|
				out << ", " unless first
				cc.bc.generate_code(child)
				first = false
			}
			out << ")"
			out << " /* #{TAG_DISR} */"  if VERBOSE
		end
	end

	class BISRTag < LanguageElement

		@tag = TAG_BISR
		@out_type = :boolean

		def initialize
			super
			attr.declare(ATTR_REF, BooleanInputSymbol)
		end
		
		def valid_child le
			return le.class == WPTag
		end

		def to_ic(out)
			out << "// boolean-input-symbol-ref#{ENDL}" if out.verbose
			out << "i #{attr[ATTR_REF].identifier}#{ENDL}"
			out << "// number of with-parameter elements#{ENDL}" if out.verbose
			out << "#{children.size}#{ENDL}"

			children.each do |child|
				child.to_ic(out)
			end			
		end

		def to_code(cc, out)
			out << "#{cc.bc[ATTR_REF]}("
			first = true
			@children.each { |child|
				out << ", " unless first
				cc.bc.generate_code(child)
				first = false
			}
			out << ")"
			out << " /* #{TAG_BISR} */"  if VERBOSE
		end
	end

	class EISRTag < LanguageElement

    @tag = TAG_EISR
    @out_type = :enum
    attr_accessor :enum_domain
    		
		def initialize
			super
			attr.declare(ATTR_REF, EnumInputSymbol)
		end
		
		def valid_child le
			return le.class == WPTag
		end
		
		def to_ic(out)
			out << "// enumerated-input-symbol-ref#{ENDL}" if out.verbose
			out << "i #{attr[ATTR_REF].identifier}#{ENDL}"
			out << "// number of with-parameter elements#{ENDL}" if out.verbose
			out << "#{children.size}#{ENDL}"

			children.each do |child|
				child.to_ic(out)
			end			
		end

		def to_code(cc, out)
			out << "#{cc.bc[ATTR_REF]}("
			first = true
			@children.each { |child|
				out << ", " unless first
				cc.bc.generate_code(child)
				first = false
			}
			out << ")"
			out << " /* #{TAG_EISR} */"  if VERBOSE
		end
	end

	class WPTag < LanguageElement

		@tag = TAG_WP
		attr_accessor :in_type

		def initialize
			super
			@in_type = :any
			attr.declare(ATTR_REF, InputSymbolParameterSymbol)
		end
				
    def valid_child le
			return @in_type == :any || le.out_type == :any || @in_type == le.out_type
		end

		def to_ic(out)
			raise "exactly 1 child expected" unless @children.size == 1
			raise "parent should be either DISRTag, BISRTag or EISRTag instead of #{parent.class.to_s}" unless (parent.class == DISRTag || parent.class == BISRTag || parent.class == EISRTag)
			
			out << "// with parameter #{attr[ATTR_REF].identifier}#{ENDL}" if out.verbose
			case in_type
			when :decimal
			  out << "d "
			when :boolean
			  out << "b "
			when :enum
			  out << "e #{attr[ATTR_REF].enum_domain.identifier} "
			else
			  raise InternalError.new
			end
			out << "#{attr[ATTR_REF].identifier}#{ENDL}"
			children[0].to_ic(out)
		end
		
		def to_code(cc, out)
			raise "exactly 1 child expected" unless @children.size == 1
			raise "parent should be either DISRTag, BISRTag or EISRTag instead of #{parent.class.to_s}" unless (parent.class == DISRTag || parent.class == BISRTag || parent.class == EISRTag)

			out << "#{cc.bc[ATTR_REF]} = "
			cc.bc.generate_code(@children[0])
		end
	end

	class CDTTag < StatementContainingTag

		@tag = TAG_CDT
		
		def valid_child le
			return [IfTag, ElseTag].include?(le.class)
		end

		def to_ic(out)
			out << "// common decision tree:#{ENDL}" if out.verbose
			statement_to_ic(out)
			out << "// (else)#{ENDL}" if out.verbose
		end

		def to_code(cc, out)
			out > "#{TOKEN_COMMON} #{TOKEN_DECISION} {#{ENDL}"

			generate_children_code(cc, out)

			out < "}"
			out << " /* #{TAG_CDT} end */" if VERBOSE
			out << "#{ENDL}#{ENDL}"
		end
	end

	class CRTag < LanguageElement

		@tag = TAG_CR
		@out_type = :decimal

		def initialize
			super
			attr.declare(ATTR_REF, ConstantSymbol)
		end

		def to_ic(out)
			out << "// constant-ref#{ENDL}" if out.verbose
			out << "c #{attr[ATTR_REF].value}#{ENDL}"
		end
		
		def to_code(cc, out)
			out << "#{cc.bc[ATTR_REF]}"
			out << " /* #{TAG_CR} */"  if VERBOSE
		end
	end

	class TOSXTag < LanguageElement

		@tag = TAG_TOSX
		@out_type = :decimal

		def initialize
			super
		end

		def to_ic(out)
			out << "// time-of-state-execution#{ENDL}" if out.verbose
			out << "s#{ENDL}"
		end

		def to_code(cc, out)
			out << TOKEN_TOSX
			out << " /* #{TAG_TOSX} */"  if VERBOSE
		end
	end

	class TOOXTag < LanguageElement

		@tag = TAG_TOOX  
		@out_type = :decimal

		def initialize
			super
		end

		def to_ic(out)
			out << "// time-of-option-execution#{ENDL}" if out.verbose
			out << "t#{ENDL}"
		end

		def to_code(cc, out)
			out << TOKEN_TOOX
			out << " /* #{TAG_TOOX} */"  if VERBOSE
		end
	end

  class SOSTag < LanguageElement
  end 
  	
	class EVTag < LanguageElement
		
		@tag = TAG_EV
    @out_type = :enum
    attr_accessor :enum_domain
		
		def initialize
			super
			attr.declare(ATTR_REF, EnumSymbolValueSymbol)
		end
		
		def valid_child le
		  return false
		end
		
		def to_ic(out)
		  out << "// enum element#{ENDL}" if out.verbose
		  out << "v #{attr[ATTR_REF].identifier} #{ENDL}"
		end
		
		def to_code(cc, out)
		  out << cc.bc[ATTR_REF]
		end
	end
	
	class SDOSTag < SOSTag

		@tag = TAG_SDOS
		@in_type = :decimal
		
		def initialize
			super
			attr.declare(ATTR_REF, DecimalOutputSymbol)
		end

		def valid_child le
			return le.out_type == :any || in_type == le.out_type
		end
		
		def to_ic(out)
			out << "// set decimal output symbol#{ENDL}" if out.verbose
			out << "d #{attr[ATTR_REF].identifier}#{ENDL}"
			children.each do |child|
				child.to_ic(out)
			end
		end

		def to_code(cc, out)
			out % cc.bc[ATTR_REF] << " = "
			generate_children_code(cc, out)
			out << ";"
			out << " /* #{TAG_SDOS} */"  if VERBOSE
			out << "#{ENDL}"
		end
	end

	class SBOSTag < SOSTag

		@tag = TAG_SBOS
		@in_type = :boolean
		
		def initialize
			super
			attr.declare(ATTR_REF, BooleanOutputSymbol)
		end

		def valid_child le
			return le.out_type == :any || in_type == le.out_type
		end
		
		def to_ic(out)
			out << "// set boolean output symbol#{ENDL}" if out.verbose
			out << "b #{attr[ATTR_REF].identifier}#{ENDL}"
			children.each do |child|
				child.to_ic(out)
			end
		end

		def to_code(cc, out)
			out % cc.bc[ATTR_REF] << " = "
			generate_children_code(cc, out)
			out << ";"
			out << " /* #{TAG_SBOS} */"  if VERBOSE
			out << "#{ENDL}"
		end
	end

	class SEOSTag < SOSTag

		@tag = TAG_SEOS
		@in_type = :enum
		
		def initialize
			super
			attr.declare(ATTR_REF, EnumOutputSymbol)
		end

		def valid_child le
			return le.out_type == :any || in_type == le.out_type
		end

		def to_ic(out)
			out << "// set enumerated output symbol#{ENDL}" if out.verbose
			out << "e #{attr[ATTR_REF].identifier}#{ENDL}"
			children.each do |child|
				child.to_ic(out)
			end
		end

		def to_code(cc, out)
			out % cc.bc[ATTR_REF] << " = "
			generate_children_code(cc, out)
			out << ";"
			out << " /* #{TAG_SEOS} */"  if VERBOSE
			out << "#{ENDL}"
		end
	end

	class NotTag < LanguageElement

		@tag = TAG_NOT
		@in_type = :boolean
		@out_type = :boolean

		def initialize
			super
		end
		
		def valid_child le
			return le.out_type == :any || in_type == le.out_type
		end

		def to_ic(out)
			out << "// not#{ENDL}" if out.verbose
			out << "!#{ENDL}"
			children.each do |child|
				child.to_ic(out)
			end			
		end
		
		def to_code(cc, out)
			throw "only 1 child expected" unless @children.size == 1

			out << "!("
			cc.bc.generate_code(@children[0])
			out << ")"
		end
	end

	class SORTSTag < LanguageElement

		@tag = TAG_SORTS
		@out_type = :boolean

		def initialize
			super
		end

		def to_ic(out)
			out << "// subsequent-option-reached-target-state#{ENDL}" if out.verbose
			out << "t#{ENDL}"
		end

		def to_code(cc, out)
			out << TOKEN_SORTS
			out << " /* #{TAG_SORTS} */"  if VERBOSE
		end
	end

	class ConflictTag < LanguageElement

		@tag = TAG_CONFLICT
		@out_type = :boolean

		def initialize
			super
		end

		def to_ic(out)
			out << "// conflict#{ENDL}" if out.verbose
			out << "x#{ENDL}"
		end

		def to_code(cc, out)
			out << TOKEN_CONFLICT
			out << " /* #{TAG_CONFLICT} */"  if VERBOSE
		end
	end

	class CETag < LanguageElement

		@tag = TAG_CE
		attr_accessor :out_type
		attr_accessor :enum_domain

		def initialize
			super
      @out_type = :any		
		end

		def valid_child le
			case @children.size
			when 0:
				return le.class == CondTag
			when 1:
				return le.class == EX1Tag && (self.out_type == :any || le.in_type == :any || self.out_type == le.in_type)
			when 2:
				return le.class == EX2Tag && (self.out_type == :any || le.in_type == :any || self.out_type == le.in_type)
			end

			return false
		end
		
		def to_ic(out)
			out << "// conditional expression (question mark operator)#{ENDL}" if out.verbose
			out << "q#{ENDL}"
			out << "// conditional expression: condition#{ENDL}" if out.verbose
			children[0].to_ic(out)
			out << "// conditional expression: expression1#{ENDL}" if out.verbose
			children[1].to_ic(out)
			out << "// conditional expression: expression2#{ENDL}" if out.verbose
			children[2].to_ic(out)
		end
		
		def to_code(cc, out)
			throw "exactly 3 children expected" unless @children.size == 3

			out << "("
			cc.bc.generate_code(@children[0])
			out << " ? "
			cc.bc.generate_code(@children[1])
			out << " : "
			cc.bc.generate_code(@children[2])
			out << ")"
		end
	end


	class EX1Tag < LanguageElement
		
		@tag = TAG_EX1
    attr_accessor :in_type
    
		def initialize
			super
			@in_type = :any
		end
		
		def valid_child le
		  @in_type = le.out_type
			return true
		end

		def to_ic(out)
			children.each do |child|
				child.to_ic(out)
			end			
		end		

		def to_code(cc, out)
			throw "exactly 1 child expected" unless @children.size == 1
			cc.bc.generate_code(@children[0])
		end
	end

	class EX2Tag < LanguageElement
		
		@tag = TAG_EX2
    attr_accessor :in_type
		
		def initialize
			super
			@in_type = :any
		end

		def valid_child le
		  @in_type = le.out_type
			return true
		end

		def to_ic(out)
			children.each do |child|
				child.to_ic(out)
			end			
		end
		
		def to_code(cc, out)
			throw "exactly 1 child expected" unless @children.size == 1
			cc.bc.generate_code(@children[0])
		end
	end

	
	class ODTag < LanguageElement

		@tag = TAG_OD

		def initialize
			super
			attr.declare(ATTR_NAME, OptionSymbol)
			attr.declare(ATTR_DESCRIPTION, String, true)
		end
		
		def valid_child le
			return le.class == DParaTag || le.class == BParaTag || le.class == EParaTag
		end
	end

	class ODSTag < LanguageElement

		@tag = TAG_ODS

		def initialize
			super
			attr.declare(ATTR_XMLNS)
			attr.declare(ATTR_XMLNS_XSI)
			attr.declare(ATTR_XSI_SCHEMALOCATION)
		end
		
		def valid_child le
			return ODTag == le.class
		end
	end

	# declaration of decimal function or option parameters
	class DParaTag < LanguageElement

		@tag = TAG_DPARA

		def initialize
			super
			attr.declare(ATTR_NAME, ParameterSymbol)
			attr.declare(ATTR_MEASURE, String, true)
			attr.declare(ATTR_RANGE, String, true)
			attr.declare(ATTR_DESCRIPTION, String, true)
		end

		def import(attributes_string_hash, symbol_table)
			super
			symbol_table.set_domain_symbol(attr[ATTR_NAME], parent.attr[ATTR_NAME])
		end
		
		#late declaration dependant on parent class
		def parent=(value)
			super
			
			if parent.class == DISTag || parent.class == BISTag || parent.class == EISTag
				attr.declare(ATTR_NAME, InputSymbolParameterSymbol)
			elsif parent.class == ODTag
			 	attr.declare(ATTR_NAME, OptionParameterSymbol)
			elsif parent.class == BBTag
			 	attr.declare(ATTR_NAME, BasicBehaviorParameterSymbol)
			else
				raise InternalError.new("unexpected parent class #{parent.class.name}")
			end
		end

		def to_code(cc, out)
			descr = attr[ATTR_DESCRIPTION]
			out % "/** #{descr} */#{ENDL}" unless (descr.nil? || descr.empty?)
			out % "float "
			out << cc.bc[ATTR_NAME]
			out << " [#{attr[ATTR_RANGE]}]" unless attr[ATTR_RANGE] =~ /^\s*$/
			out << " \"#{attr[ATTR_MEASURE]}\"" unless attr[ATTR_MEASURE] =~ /^\s*$/
			out << ";#{ENDL}"
		end
	end

	# declaration of boolean function or option parameters
	class BParaTag < LanguageElement

		@tag = TAG_BPARA

		def initialize
			super
			attr.declare(ATTR_NAME, ParameterSymbol)
			attr.declare(ATTR_DESCRIPTION, String, true)
		end

		def import(attributes_string_hash, symbol_table)
			super
			symbol_table.set_domain_symbol(attr[ATTR_NAME], parent.attr[ATTR_NAME])
		end
		
		#late declaration dependant on parent class
		def parent=(value)
			super
			
			if parent.class == DISTag || parent.class == BISTag || parent.class == EISTag
				attr.declare(ATTR_NAME, InputSymbolParameterSymbol)
			elsif parent.class == ODTag
			 	attr.declare(ATTR_NAME, OptionParameterSymbol)
			elsif parent.class == BBTag
			 	attr.declare(ATTR_NAME, BasicBehaviorParameterSymbol)
			else
				raise InternalError.new("unexpected parent class #{parent.class.name}")
			end
		end

		def to_code(cc, out)
			descr = attr[ATTR_DESCRIPTION]
			out % "/** #{descr} */#{ENDL}" unless (descr.nil? || descr.empty?)
			out % "bool "
			out << cc.bc[ATTR_NAME]
			out << ";#{ENDL}"
		end
	end

	# declaration of function or option parameters
	class EParaTag < LanguageElement

		@tag = TAG_EPARA

		def initialize
			super
			attr.declare(ATTR_NAME, ParameterSymbol)
			attr.declare(ATTR_ENUM, EnumSymbol)
			attr.declare(ATTR_DESCRIPTION, String, true)
		end

		def import(attributes_string_hash, symbol_table)
			super
			symbol_table.set_domain_symbol(attr[ATTR_NAME], parent.attr[ATTR_NAME])
		end
		
		#late declaration dependant on parent class
		def parent=(value)
			super
			
			if parent.class == DISTag || parent.class == BISTag || parent.class == EISTag
				attr.declare(ATTR_NAME, InputSymbolParameterSymbol)
			elsif parent.class == ODTag
			 	attr.declare(ATTR_NAME, OptionParameterSymbol)
			elsif parent.class == BBTag
			 	attr.declare(ATTR_NAME, BasicBehaviorParameterSymbol)
			else
				raise InternalError.new("unexpected parent class #{parent.class.name}")
			end
		end

		def to_code(cc, out)
			descr = attr[ATTR_DESCRIPTION]
			out % "/** #{descr} */#{ENDL}" unless (descr.nil? || descr.empty?)
			out % "enum "
			out << cc.bc[ATTR_ENUM]
			out << " "
			out << cc.bc[ATTR_NAME]
			out << ";#{ENDL}"
		end
	end

  class SymbolsTag < LanguageElement
	
		@tag = TAG_SYMBOLS
		
		def initialize
			super
			attr.declare(ATTR_XMLNS)
			attr.declare(ATTR_XMLNS_XSI)
			attr.declare(ATTR_XSI_SCHEMALOCATION)
			attr.declare(ATTR_ID, NamespaceSymbol)
			attr.declare(ATTR_TITLE)
			attr.declare(ATTR_DESCRIPTION, String, true)
		end

		def valid_child le
			return [DISTag, DOSTag, ETag, EISTag, EOSTag, BISTag, BOSTag, ConstantTag].include?(le.class)
		end

		def to_code(cc, out)
			descr = attr[ATTR_DESCRIPTION]
			out % "/** #{descr} */#{ENDL}" unless (descr.nil? || descr.empty?)
			out > "namespace #{cc.bc[ATTR_ID]}(\"#{attr[ATTR_TITLE]}\") {#{ENDL}#{ENDL}"
			generate_children_code(cc, out)
			out < "}#{ENDL}#{ENDL}"
		end
	end

	class DISTag < LanguageElement
	
		@tag = TAG_DIS

		def initialize
			super
			attr.declare(ATTR_NAME, DecimalInputSymbol)
			attr.declare(ATTR_DESCRIPTION, String, true)
			attr.declare(ATTR_MEASURE, String, true)
			attr.declare(ATTR_RANGE, String, true)
		end
		
		def valid_child le
			return le.class == DParaTag || le.class == BParaTag || le.class == EParaTag
		end

		def to_code(cc, out)
			descr = attr[ATTR_DESCRIPTION]
			out % "/** #{descr} */#{ENDL}" unless (descr.nil? || descr.empty?)
			out % "#{TOKEN_FLOAT} #{TOKEN_INPUT} #{cc.bc[ATTR_NAME]}"
			out << " [#{attr[ATTR_RANGE]}]" unless (attr[ATTR_RANGE].nil? || attr[ATTR_RANGE].empty?)
			out << " \"#{attr[ATTR_MEASURE]}\"" unless (attr[ATTR_MEASURE].nil? || attr[ATTR_MEASURE].empty?)
			if children.size > 0
			  out > "#{ENDL}("
  			is_first = true
			  children.each do |child|
				  out << "," unless is_first
				  out << "#{ENDL}"
				  out % ""
				  cc.bc.generate_code(child)
				  is_first = false
			  end
			  out << "#{ENDL})"
			  out < ")"
			end
  	  out << ";#{ENDL}#{ENDL}"
		end
	end

	class DOSTag < LanguageElement
	
		@tag = TAG_DOS

		def initialize
			super
			attr.declare(ATTR_NAME, DecimalOutputSymbol)
			attr.declare(ATTR_DESCRIPTION, String, true)
			attr.declare(ATTR_MEASURE, String, true)
			attr.declare(ATTR_RANGE, String, true)
			attr.declare(ATTR_INTERNAL)
		end
		
		def to_code(cc, out)

			descr = attr[ATTR_DESCRIPTION]
			out % "/** #{descr} */#{ENDL}" unless (descr.nil? || descr.empty?)
			out % "#{TOKEN_FLOAT} "
			out << "#{TOKEN_INTERNAL} " if attr[ATTR_INTERNAL] == 'true'
			out << "#{TOKEN_OUTPUT} #{cc.bc[ATTR_NAME]}"
			out << " [#{attr[ATTR_RANGE]}]" unless (attr[ATTR_RANGE].nil? || attr[ATTR_RANGE].empty?)
			out << " \"#{attr[ATTR_MEASURE]}\"" unless (attr[ATTR_MEASURE].nil? || attr[ATTR_MEASURE].empty?)
			out << ";#{ENDL}#{ENDL}"
		end
	end

	class EISTag < LanguageElement
	
		@tag = TAG_EIS
		
		def initialize
			super
			attr.declare(ATTR_NAME, EnumInputSymbol)
			attr.declare(ATTR_ENUM, EnumSymbol)
			attr.declare(ATTR_DESCRIPTION, String, true)
		end

		def valid_child le
			return le.class == DParaTag || le.class == BParaTag || le.class == EParaTag
		end
		
		def to_code(cc, out)
			descr = attr[ATTR_DESCRIPTION]
			out % "/** #{descr} */#{ENDL}" unless (descr.nil? || descr.empty?)
			out % "#{TOKEN_ENUM} #{cc.bc[ATTR_ENUM]} #{TOKEN_INPUT} #{cc.bc[ATTR_NAME]}"
			if children.size > 0
			  out > "#{ENDL}("
  			is_first = true
			  children.each do |child|
				  out << "," unless is_first
				  out << "#{ENDL}"
				  out % ""
				  cc.bc.generate_code(child)
				  is_first = false
			  end
			  out << "#{ENDL})"
			  out < ")"
			end
  	  out << ";#{ENDL}#{ENDL}"
		end

	end

	class EnumTag < LanguageElement
	
		@tag = TAG_ENUM
		
		def initialize
			super
		  attr.declare(ATTR_NAME, EnumSymbolValueSymbol)
		end
		
		def to_code(cc, out)
			out << cc.bc[ATTR_NAME]
		end
			
		def import(attributes_string_hash, symbol_table)
			super
			symbol_table.set_domain_symbol(attr[ATTR_NAME], parent.attr[ATTR_NAME])
		end
	end

	class EOSTag < LanguageElement
	
		@tag = TAG_EOS
		
		def initialize
			super
			attr.declare(ATTR_NAME, EnumOutputSymbol)
			attr.declare(ATTR_ENUM, EnumSymbol)
			attr.declare(ATTR_DESCRIPTION, String, true)
			attr.declare(ATTR_INTERNAL)
		end
		
		def valid_child le
			return false
		end
		
		def to_code(cc, out)
			is_first = true
			descr = attr[ATTR_DESCRIPTION]
			out % "/** #{descr} */#{ENDL}" unless (descr.nil? || descr.empty?)
			out % "#{TOKEN_ENUM} #{cc.bc[ATTR_ENUM]} "
			out << "#{TOKEN_INTERNAL} " if attr[ATTR_INTERNAL] == 'true'
			out << "#{TOKEN_OUTPUT} #{cc.bc[ATTR_NAME]};#{ENDL}#{ENDL}"
		end
	end
	
	class ETag < LanguageElement
	
		@tag = TAG_E
		
		def initialize
			super
			attr.declare(ATTR_NAME, EnumSymbol)
			attr.declare(ATTR_DESCRIPTION, String, true)
			attr.declare(ATTR_INTERNAL)
		end
		
		def valid_child le
			return EnumTag == le.class
		end
		
		def to_code(cc, out)
			is_first = true
			descr = attr[ATTR_DESCRIPTION]
			out % "/** #{descr} */#{ENDL}" unless (descr.nil? || descr.empty?)
			out > ""
			out << "#{TOKEN_INTERNAL} " if attr[ATTR_INTERNAL] == 'true'
			our << "#{TOKEN_ENUM} #{cc.bc[ATTR_NAME]} {"
			children.each do |child|
				out << "," unless is_first
				out << "#{ENDL}"
				out % ""
				cc.bc.generate_code(child)
				is_first = false
			end
			out << "#{ENDL}"
			out < "};#{ENDL}#{ENDL}"
		end
	end
	
	class BISTag < LanguageElement

		@tag = TAG_BIS
	
		def initialize
			super
			attr.declare(ATTR_NAME, BooleanInputSymbol)
			attr.declare(ATTR_DESCRIPTION, String, true)
		end
	
		def valid_child le
			return le.class == DParaTag || le.class == BParaTag || le.class == EParaTag
		end
		
		def to_code(cc, out)
			descr = attr[ATTR_DESCRIPTION]
			out % "/** #{descr} */#{ENDL}" unless (descr.nil? || descr.empty?)
			out % "#{TOKEN_BOOL} #{TOKEN_INPUT} #{cc.bc[ATTR_NAME]}"
			if children.size > 0
			  out > "#{ENDL}("
  			is_first = true
			  children.each do |child|
				  out << "," unless is_first
				  out << "#{ENDL}"
				  out % ""
				  cc.bc.generate_code(child)
				  is_first = false
			  end
			  out << "#{ENDL})"
			  out < ")"
			end
  	  out << ";#{ENDL}#{ENDL}"
		end

	end

	class BOSTag < LanguageElement

		@tag = TAG_BOS
	
		def initialize
			super
			attr.declare(ATTR_NAME, BooleanOutputSymbol)
			attr.declare(ATTR_DESCRIPTION, String, true)
			attr.declare(ATTR_INTERNAL)
		end
	
		def to_code(cc, out)

			descr = attr[ATTR_DESCRIPTION]
			out % "/** #{descr} */#{ENDL}" unless (descr.nil? || descr.empty?)
			out % "#{TOKEN_BOOL} "
			out << "#{TOKEN_INTERNAL} " if attr[ATTR_INTERNAL] == 'true'
			out << "#{TOKEN_OUTPUT} #{cc.bc[ATTR_NAME]};#{ENDL}#{ENDL}"
		end

	end

	class ConstantTag < LanguageElement
	
		@tag = TAG_CONST

		def initialize
			super
			attr.declare(ATTR_NAME, ConstantSymbol)
			attr.declare(ATTR_VALUE)
			attr.declare(ATTR_MEASURE, String, true)
			attr.declare(ATTR_DESCRIPTION, String, true)
		end

		def import(attributes_string_hash, symbol_table)
			super
			#save constant value within symbol. this value is accessed while generating IC.
			attr[ATTR_NAME].value = attr[ATTR_VALUE] 
		end		

		def to_code(cc, out)

			descr = attr[ATTR_DESCRIPTION]
			measure = attr[ATTR_MEASURE]
			out % "/** #{descr} */#{ENDL}" unless (descr.nil? || descr.empty?)
			out % "#{TOKEN_FLOAT} #{TOKEN_CONST} #{cc.bc[ATTR_NAME]} = #{attr[ATTR_VALUE]}"
			out << " \"#{measure}\"" unless (measure.nil? || measure.empty?)
			out << ";#{ENDL}#{ENDL}"
		end
		
	end

	class BBSTag < LanguageElement

		@tag = TAG_BBS

		def initialize
			super
			attr.declare(ATTR_XMLNS)
			attr.declare(ATTR_XMLNS_XSI)
			attr.declare(ATTR_XSI_SCHEMALOCATION)
			attr.declare(ATTR_ID, NamespaceSymbol)
			attr.declare(ATTR_TITLE)
			attr.declare(ATTR_DESCRIPTION, String, true)
		end

		def valid_child le
			return BBTag == le.class
		end

		def to_code(cc, out)
			descr = attr[ATTR_DESCRIPTION]
			out % "/** #{descr} */#{ENDL}" unless (descr.nil? || descr.empty?)
			out > "namespace #{cc.bc[ATTR_ID]}(\"#{attr[ATTR_TITLE]}\") {#{ENDL}#{ENDL}"
			generate_children_code(cc, out)
			out < "}#{ENDL}#{ENDL}"
		end
	end

	class BBTag < LanguageElement
	
		@tag = TAG_BB
		
		def initialize
			super
			attr.declare(ATTR_NAME, BasicBehaviorSymbol)
			attr.declare(ATTR_DESCRIPTION, String, true)
		end
	
		def valid_child le
			return le.class == DParaTag || le.class == BParaTag || le.class == EParaTag
		end
	
		def to_code(cc, out)

			descr = attr[ATTR_DESCRIPTION]
			out % "/** #{descr} */#{ENDL}" unless (descr.nil? || descr.empty?)
			out > "#{TOKEN_BEHAVIOR} #{cc.bc[ATTR_NAME]} {#{ENDL}"
			generate_children_code(cc, out)
			out < "}#{ENDL}#{ENDL}"
		end
	end

	#TODO title, platform, software-environment tags
	class AgentCollectionTag < LanguageElement
	
		@tag = TAG_AGENT_COLLECTION
		attr_accessor :includes
		attr_accessor :title
		attr_accessor :platform
		attr_accessor :software_environment

		def initialize
			super
			attr.declare(ATTR_XMLNS)
			attr.declare(ATTR_XMLNS_XSI)
			attr.declare(ATTR_XSI_SCHEMALOCATION)
			attr.declare(ATTR_XMLNS_XI)
			@includes = []
		end

		def to_code(cc, out)
			unless @title.nil? || @platform.nil? || @software_environment.nil?
				out > "/***#{ENDL}"
				out % "Title: #{@title}#{ENDL}" unless @title.nil?
				out % "Platform: #{@platform}#{ENDL}" unless @platform.nil?
				out % "Software-Environment: #{@software_environment}#{ENDL}" unless @software_environment.nil?
				out < "*/#{ENDL}#{ENDL}"			
			end
			
			@includes.each do |filename|
				out % "include \"#{filename}\";#{ENDL}"
			end
			out << "#{ENDL}" unless @includes.empty?
			
			generate_children_code(cc, out)
		end
		
		def valid_child le
			return [AgentTag, OptionsTag, TitleTag, PlatformTag, SoftwareEnvironmentTag].include?(le.class)
		end
		
	end
	
	class AgentTag < LanguageElement
	
		@tag = TAG_AGENT

		def initialize
			super
			attr.declare(ATTR_ID, AgentSymbol)
			attr.declare(ATTR_TITLE)
			attr.declare(ATTR_DESCRIPTION, String, true)
			attr.declare(ATTR_ROOT_OPTION, OptionSymbol)
		end

		def to_code(cc, out)
			descr = attr[ATTR_DESCRIPTION]
			out % "/** #{descr} */#{ENDL}" unless (descr.nil? || descr.empty?)
			out % "#{TOKEN_AGENT} #{cc.bc[ATTR_ID]}(\"#{attr[ATTR_TITLE]}\", #{cc.bc[ATTR_ROOT_OPTION]});#{ENDL}"
			out % "#{ENDL}"
		end
	end

	class OptionsTag < LanguageElement
		@tag = TAG_OPTIONS
		
		def valid_child le
			return le.class == XIncludeTag
		end

		def to_xml(out)
			out % "&options;#{ENDL}"
			super
		end
		
		def to_code(cc, out)
		end
	end

	class StringTag < LanguageElement

		attr_accessor :string
		
		def initialize(string = nil)
			super()
			@string = string
		end
		
		def to_xml(out)
			if @string.nil? || @string.empty?
				out % "<#{tag}/>#{ENDL}"
			else
				out % "<#{tag}>#{@string}</#{tag}>#{ENDL}"
			end
		end
		
		def to_code(cc, out)
		end
	end

	class TitleTag < StringTag
		@tag = TAG_TITLE

		def text=(text)
			@string = text
			parent.title = text unless parent.nil?
		end

	end
	
	class PlatformTag <  StringTag
		@tag = TAG_PLATFORM

		def text=(text)
			@string = text
			parent.platform = text unless parent.nil?
		end


	end

	class SoftwareEnvironmentTag <  StringTag
		@tag = TAG_SOFTWARE_ENVIRONMENT

		def text=(text)
			@string = text
			parent.software_environment = text unless parent.nil?
		end


	end
	
	class XIncludeTag < LanguageElement
		@tag = TAG_XINCLUDE
		
		def initialize
			super
			attr.declare(ATTR_HREF)
		end
	end
end
