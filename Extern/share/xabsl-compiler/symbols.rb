module Translator
	

	#identifers are unique across one class of symbols
  #with the exception of states
	class Symbol

		attr_reader :identifier
		attr_reader :scope
		attr_accessor :namespace_symbol
		attr_accessor :usage_count
		
		def initialize(identifier, scope)
			super()
			@identifier = identifier
			@scope = scope
			@usage_count = 0
			
			throw "trying to instantiate an abstract class" if self.class == Symbol
		end

		def inspect
			scope_identifier = ''
			scope_identifier = ", @scope=#{@scope.identifier}>" unless @scope.nil?
			return "#{self.class.name}<@identifier=#{@identifier}#{scope_identifier}>"
		end

		#for sorting by identifier
		def <=>(symbol)
			return self.identifier <=> symbol.identifier
		end

	end
	
	class DomainValueSymbol < Symbol
	
		attr_reader :domain_symbol
		attr_reader :shortcut
		
		def qualified?
			return identifier != shortcut
		end
				
				
		def domain_symbol=(symbol)

			return if symbol.nil?
			return if @domain_symbol == symbol
				
			if @domain_symbol.nil?
				@domain_symbol = symbol
				@domain_symbol.value_symbols << self
			else
				$stderr.puts "redeclaration mismatch on #{self.inspect}: old ds #{@domain_symbol.inspect} new ds #{symbol.inspect}"
        raise InternalError.new
			end

			@shortcut = identifier.sub(/^#{Regexp.escape(@domain_symbol.identifier)}\./ , '')
		end

	end

	class NamespaceSymbol < Symbol
		attr_reader :namespace_type


    #set the type of the namespace, allowed values for type are :symbols or :behaviors
		def namespace_type=(type)
			return if @namespace_type == type
			raise InternalError.new unless @namespace_type.nil?
			raise InternalError.new unless [:symbols, :behaviors].include?(type)
			@namespace_type = type
		end
	end

	class AgentSymbol < Symbol
	end

	class StateSymbol < Symbol
	end

	class DomainSymbol < Symbol	
	
		attr_reader :value_symbols
		
		def initialize(identifier, scope)
			super
			@value_symbols = []
		end
	
	end
	
	module EnumTypeSymbol
	  attr_accessor :enum_domain
	end
	
	class ParameterSymbol < DomainValueSymbol
	  attr_accessor :type
	  include EnumTypeSymbol  
	end
		
	class DecimalInputSymbol < DomainSymbol
	end

	class InputSymbolParameterSymbol < ParameterSymbol
	end
	
	class BooleanInputSymbol < DomainSymbol
	end
		
	class ConstantSymbol < Symbol
		attr_accessor :value
	end
	  
	  
	class EnumInputSymbol < DomainSymbol
	  include EnumTypeSymbol  
	end

  class DecimalOutputSymbol < Symbol
  	attr_accessor :internal
  end
  
  class BooleanOutputSymbol < Symbol
  	attr_accessor :internal
  end

	class EnumOutputSymbol < Symbol
	  include EnumTypeSymbol  
  	attr_accessor :internal
	end
	

	class EnumSymbol < DomainSymbol
  	attr_accessor :internal
	end

	class EnumSymbolValueSymbol < DomainValueSymbol
	  include EnumTypeSymbol
	end


	class OptionSymbol < DomainSymbol
	end

	class OptionParameterSymbol < ParameterSymbol
	end


	class BasicBehaviorSymbol < DomainSymbol
	end

	class BasicBehaviorParameterSymbol < ParameterSymbol
	end
	
	
	
end
