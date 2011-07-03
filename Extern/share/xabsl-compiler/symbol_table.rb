
module Translator
	
	class SymbolTable
	
		def initialize
			super
			@prepared = false
			@table = {}
			@shortcuts = {}
		end

		def each(&block)
			@table.each_value(&block)
		end

		def dump
			@table.each { |key, symbol|
				puts "#{symbol.inspect}"
			}  
		end


		#removes a symbol completely from symbol table
		#used when compiling incremental
		def remove(symbol)
			check = @table.delete([symbol.identifier, symbol.class, symbol.scope])
			throw InternalError.new unless check == symbol
			
			@shortcuts[symbol.identifier] -= [symbol]
		end

		def get(identifier, symbol_class, scope = nil, &block)
			
			symbol = @table[[identifier, symbol_class, scope]]
			if symbol.nil? && !block.nil?
				symbol = block.call
			end
			
			return symbol
		end

		def get_all_regexp(regexp)
			result = []

			@table.each { |key, value|
				result << value if key[0] =~ regexp
			}
			
			return result
		end

		# function to intercept domain symbol information for shortcut generation.
		def set_domain_symbol(symbol, domain_symbol)

			raise InternalError.new unless symbol.class <= DomainValueSymbol

			symbol.domain_symbol = domain_symbol
			symbol.enum_domain = domain_symbol if symbol.class <= EnumTypeSymbol
			
			shortcut = symbol.shortcut
			symbols = @shortcuts[shortcut]
			if symbols.nil?
				@shortcuts[shortcut] = [symbol]
			else
				symbols << symbol unless symbols.include?(symbol)
			end
		end

		# returns array of shortcut symbols for given identifier.
		def get_shortcuts(identifier)
			result = @shortcuts[identifier]
			return [] if result.nil?
			return Array.new(result)
		end
		
		# declares and returns a symbol with given identifier and requested class and scope.
		# scope is (only) used to disabiguate state symbols and should be set to the corresponding option symbol.
		# otherwise scope is nil.
		# yields block on symbol redefinition
		def declare(identifier, symbol_class, scope = nil, &block)
			
			key = [identifier, symbol_class, scope]
			
			symbol = @table[key]
			
			if symbol.nil?
				symbol = symbol_class.new(identifier, scope)
				@table[key] = symbol

				symbols = @shortcuts[identifier]
				if symbols.nil?
					@shortcuts[identifier] = [symbol]
				else
					symbols << symbol unless symbols.include?(symbol)
				end
			else
        block.call unless block.nil?
			end
			return symbol
		end
	
		# checks symbol table and returns array of error messages.
		def errors
			result = []
			@table.each do |key, symbol|
				result << InternalError.new("no domain symbol set on #{key.inspect}") if symbol.class <= DomainValueSymbol && symbol.domain_symbol.nil?
			end
			return result
		end

	end
	
	
	#uesd by SymbolContext to validate the function parameters in xtc code
	class DomainParameterContext
	
		attr_reader :domain_symbol
		attr_reader :used_value_symbols
		attr_reader :compiler_context
		
		def initialize(domain_symbol, compiler_context)
			super()
			
			@domain_symbol = domain_symbol
			@compiler_context = compiler_context
			@used_value_symbols = []
		end
		
		
		def use_value_symbol(symbol, token)
			
			if used_value_symbols.include?(symbol)
				compiler_context.eh.error ParameterMultipleSetting.new(@compiler_context, token)
			end
			
			unless symbol.class <= DomainValueSymbol
				compiler_context.eh.error DomainSymbolExpected.new(@compiler_context, token)
			end
			
			unless symbol.domain_symbol == domain_symbol
				compiler_context.eh.error MismatchingDomain.new(@compiler_context, token)
			end
			
			# mark symbol as used for MissingParameter warning
			@used_value_symbols << symbol	
		end
		
		# issue a MissingParameter warning on unused domain value symbols.
		def check_used_value_symbols(token)
			if used_value_symbols.size < @domain_symbol.value_symbols.size
				(@domain_symbol.value_symbols - used_value_symbols).each {|symbol|
					compiler_context.eh.error ParameterMissing.new(compiler_context, token, symbol)
				}
				
			end
		end
	end

	# class provides short access methods to members of compiler context.
	class CompilerContextWorker
		def initialize(compiler_context)
			@compiler_context = compiler_context
		end

		def cc
			return @compiler_context
		end

		def cu
			return @compiler_context.compilation_unit
		end
	
		def st
			return @compiler_context.symbol_table
		end

		def sc
			return @compiler_context.symbol_context
		end

		def tz
			return @compiler_context.tokenizer
		end

		def pc
			return @compiler_context.parser_context
		end

		def bc
			return @compiler_context.builder_context
		end

		def eh
			return @compiler_context.error_handler
		end
	end
	
	class CompilerContext < CompilerContextWorker
			
		attr_reader :compilation_unit
		attr_reader :symbol_table
		attr_reader :tokenizer
		attr_reader :parser_context
		attr_reader :symbol_context
		attr_reader :builder_context
		attr_reader :error_handler

		def initialize(compilation_unit, symbol_table)
			
			super(self)
			@compilation_unit = compilation_unit
			@symbol_table = symbol_table
			@tokenizer = Tokenizer.new(self)
			@parser_context = ParserContext.new(self)
			@symbol_context = SymbolContext.new(self)
			@builder_context = BuilderContext.new(self)
			@error_handler = ErrorHandler.new(self)
		end
		
		def id
			return File.expand_path(@compilation_unit.src_filename)		
		end
		
		def warnings?
			eh.messages.each do |message|
				return true if message.class <= CompilerWarning
			end
			return false
		end

		def errors?
			eh.messages.each do |message|
				return true if message.class <= CompilerError
			end
			return false
		end
	end		

	class CompilationUnit
		attr_reader :src_fullname
		attr_reader :src_filename
		attr_reader :src_dir
		attr_reader :src_mtime
		attr_reader :dest_fullname
		attr_reader :dest_filename
		attr_reader :dest_dir
		
		def initialize()
		end

		def set_src(working_directory, filename)
			@src_filename = filename
			@src_dir = working_directory
			@src_fullname = File.join(@src_dir, @src_filename)
      update_src_mtime
		end
		
		def update_src_mtime
			@src_mtime = File.stat(@src_fullname).mtime
		end

		def set_dest(working_directory, filename = nil)
		
			if filename.nil?
				case @src_filename
				
				when /^(.*)\.[xy]absl$/
					@dest_filename = "#{$1}.xml"
				when /^(.*)\.xml$/
					@dest_filename = "#{$1}.xabsl"
				else
					raise InternalError.new
				end
			else
			  @dest_filename = filename
			end
		
			@dest_dir = working_directory unless working_directory.nil?
			@dest_fullname = File.join(@dest_dir, @dest_filename)
		end

	end

	class BuilderContext < CompilerContextWorker

		def start_generate_code(le, out)
			@stack = []
			@out = out
			generate_code(le)
		end
		
		def generate_code(le)
			@stack.push le
			le.to_code(cc, @out)
			@stack.pop
		end
		
		#convinience method
		def[](attribute_name)
			return sc.to_code(@stack.last.attr[attribute_name])
		end
	end
	
	class SymbolContext < CompilerContextWorker
		
		attr_reader :from_code_declaration_count
		attr_reader :from_code_usage_count
		attr_reader :from_code_namespace_symbol_usage_count
		
		def initialize(cc)
			super
			@from_code_declaration_count = Hash.new(0)
			@from_code_usage_count = Hash.new(0)
			@from_code_namespace_symbol_usage_count = Hash.new(0)
		end

		#transforms symbol to xtc identifier
		def to_code(symbol, at_prefix_on_option_parameter = true)
			return SymbolContext.to_code(symbol, at_prefix_on_option_parameter)
		end
		
		
		def SymbolContext.to_code(symbol, at_prefix_on_option_parameter = true)
			
			identifier = symbol.identifier
			if symbol.class <= DomainValueSymbol
				if symbol.class == OptionParameterSymbol && at_prefix_on_option_parameter
					subst = '@'
				else
					subst = ''
				end
				
				raise InternalError.new if symbol.domain_symbol.nil?
				identifier = identifier.sub(/^#{Regexp.escape(symbol.domain_symbol.identifier)}\./ , subst) if symbol.qualified?
			end
		
			return identifier
		end

		
		def from_code_declare(token, symbol_class, domain_symbol = nil, identifier = nil, qualified = true)
			raise InternalError.new if symbol_class <= DomainValueSymbol && domain_symbol.nil?
			symbol = nil
			
			identifier = token.to_s if identifier.nil?
			
			#cut away the @prefix
			if symbol_class == OptionParameterSymbol
				raise InternalError.new if domain_symbol.nil?
				#option parameter symbols *have* to be marked as qualified
				raise InternalError.new unless qualified 
				identifier = identifier.sub(/^@/, "#{domain_symbol.identifier}.")
			elsif [InputSymbolParameterSymbol, BasicBehaviorParameterSymbol].include?(symbol_class)
				raise InternalError.new if domain_symbol.nil?
				identifier = "#{domain_symbol.identifier}.#{identifier}" if qualified
			elsif [EnumSymbolValueSymbol].include?(symbol_class)
				raise InternalError.new if domain_symbol.nil?
				identifier = "#{domain_symbol.identifier}.#{identifier}" if qualified
			 	
			elsif symbol_class <= DomainValueSymbol
				# this code does not yet support declaration of DomainValueSymbols,
				# because of the prefix handling
				# raise InternalError.new
				raise InternalError.new if domain_symbol.nil?
				identifier = "#{domain_symbol.identifier}.#{identifier}" if qualified
			end

			
			#StateSymbols get the option_symbol as scope
      scope = nil
			if symbol_class == StateSymbol
				raise InternalError.new if pc.option_symbol.nil?
        	scope = pc.option_symbol
			end
      symbol = st.declare(identifier, symbol_class, scope) do
      	cc.eh.error MultipleDefinitions.new(cc, token)
      end
			
			unless domain_symbol.nil?
				st.set_domain_symbol(symbol, domain_symbol)
			end
			
			#current symbol is declared within namespace
			unless pc.namespace_symbol.nil?
				symbol.namespace_symbol = pc.namespace_symbol
				@from_code_namespace_symbol_usage_count[symbol.namespace_symbol] += 1
			end
			
			@from_code_usage_count[symbol] += 1
			@from_code_declaration_count[symbol] += 1
			if @from_code_declaration_count[symbol] > 1
				eh.error MultipleDefinitions.new(cc, token)
			end
			
			return symbol
		end
		
		#used to track used namespace symbols for entity list generation
		def used_namespace_symbols
			return @from_code_namespace_symbol_usage_count.keys
		end
		
		
		
		#returns symbol from identifier
		#
		#returned symbol is subclasses of at least one element of symbol_classes
		#if !domain_symbol.nil? the returned symbol is a DomainValueSymbol with this domain symbol
		def from_code(token, symbol_classes, domain_symbol = nil, &block)
		  
			identifier = token.to_s
			
			#separate-sausage for option parameters
			if identifier =~ /^@/
				
				raise InternalError.new if pc.option_symbol.nil?
				identifier = identifier.sub(/^@/, "#{pc.option_symbol.identifier}.")
				return st.get(identifier, OptionParameterSymbol) {
					eh.error NoMatchingSymbolFound.new(cc, token, domain_symbol, symbol_classes)
					from_code_declare(token, OptionParameterSymbol, pc.option_symbol, identifier)
				}
			end
		
			symbol_classes = [symbol_classes] unless symbol_classes.class <= Array
			symbols = get_fuzzy_fast(identifier, symbol_classes, domain_symbol)
			
			#if we found exactly one symbol the world is in perfect order
			#otherwise give error message
			symbol = nil
			case symbols.size
			when 1:
				symbol = symbols[0]
			when 0:
			  if !block.nil?
			    return block.call
			  else
				  #declare symbol to prevent stopping the compile process
				  #SENSE?
				  eh.error NoMatchingSymbolFound.new(cc, token, domain_symbol, symbol_classes)
			    symbol = from_code_declare(token, symbol_classes[0], domain_symbol)
        end			
			else
				#return the first symbol to get on with compiling
				#SENSE?
				eh.error  MultipleMatchingSymbolsFound.new(cc, token, domain_symbol, symbol_classes, symbols)
				symbol = symbols[0]
			end

			@from_code_usage_count[symbol] += 1
			@from_code_namespace_symbol_usage_count[symbol.namespace_symbol] += 1 unless symbol.namespace_symbol.nil?
			symbol.usage_count += 1
			return symbol
		end


		def get_fuzzy_fast(identifier, symbol_classes, domain_symbol)

			symbols = st.get_shortcuts(identifier)
			#delete symbols with wrong symbol class
			symbols.delete_if { |symbol|
				result = true
				
				symbol_classes.each {|symbol_class|
					if symbol.class <= symbol_class
						result = false
					end
				}
				
				result
			}
			
			#delete symbols out of scope
			symbols.delete_if { |symbol|
				!symbol.scope.nil? && symbol.scope != pc.option_symbol
			}

			#delete symbols with wrong domain_symbol
			unless domain_symbol.nil?
				symbols.delete_if {|symbol|
					(not symbol.class <= DomainValueSymbol) || symbol.domain_symbol != domain_symbol
				}
			else
			  symbols.delete_if {|symbol|
			    symbol.class <= DomainValueSymbol
			  }
			end

			return symbols
		end

		
		#very imperformant due to regular expressions and speed >=O(#symbols)
		def get_fuzzy(identifier, symbol_classes, domain_symbol)

			regexp = /(^|\.)#{Regexp.escape(identifier)}$/
			symbols = st.get_all_regexp(regexp)

			#delete symbols with wrong symbol class
			symbols.delete_if { |symbol|
				result = true
				
				symbol_classes.each {|symbol_class|
					if symbol.class <= symbol_class
						result = false
					end
				}
				
				result
			}
			
			#delete symbols out of scope
			symbols.delete_if { |symbol|
				!symbol.scope.nil? && symbol.scope != pc.option_symbol
			}
			
			#delete symbols with wrong domain_symbol
			unless domain_symbol.nil?
				symbols.delete_if {|symbol|
					symbol.domain_symbol != domain_symbol
				}
			end
			
			#double check by domain_value substitution or identity
			result = []
			symbols.each {|symbol|
				if symbol.identifier == identifier
					result << symbol
				elsif symbol.class <= DomainValueSymbol
					result << symbol if symbol.identifier == "#{symbol.domain_symbol.identifier}.#{identifier}"
				end
			}
			
			return result
		end
		
	end
	
	
	
	# represents a the location in code where a symbol occurs
	# and is used to resolve symbol abbrevs. and ambiguities
	# performs semantic checking
	class ParserContext < CompilerContextWorker

		attr_accessor :option_root
		attr_accessor :agent_collection_root
		attr_accessor :namespace_root
		attr_accessor :option_definitions_temp
		
		attr_accessor :option_definitions_by_symbol
		
		attr_accessor :option_symbol
		attr_accessor :state_symbol
		attr_accessor :input_symbol
		attr_accessor :initial_state_symbol
	
		attr_accessor :namespace_symbol
		attr_accessor :behavior_symbol
		
		attr_reader :function_dpcs
		attr_reader :subsequent_dpc
		attr_reader :subsequent_type
		attr_reader :includes

		attr_accessor :option_declared
		attr_accessor :agent_declared
		attr_accessor :namespace_declared
		
		attr_accessor :enum_domains
		
		attr_accessor :has_common_decision_tree
		
		def initialize(compiler_context)
			super
			@function_dpcs = []
			@enum_domains = []
			@option_definitions_by_symbol = {}
			@includes = []
			@option_declared = false
			@agent_declared = false
			@namespace_declared = false
			@has_common_decision_tree = false
		end

		def set_subsequent(dpc, type)
			
			raise InternalError.new unless [:option, :behavior, nil].include?(type)
			
			@subsequent_dpc = dpc
			@subsequent_type = type
		end

	end
end
