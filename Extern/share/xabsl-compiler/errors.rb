module Translator


	class ErrorHandler < CompilerContextWorker

		attr_reader :messages
		
		def initialize(compiler_context)
			super
			@messages = []
		end

		def error(message)
			#suppress output of following messages
			unless [ParameterMissing, NamingWarning].include?(message.class)
			  messages << message
				#$stderr.puts visual_studio_format(message)
		  end
		end

		def visual_studio_format(message)
			type_string = ''
			if (message.class <= CompilerWarning)
				type_string = "warning"
			elsif(message.class <= CompilerError || message.class <= RuntimeError)
				type_string = "error"
			else
				type_string = "warning"
			end
		
			filename = File.expand_path(cc.cu.src_fullname)
			filename.sub!(/\A\/cygdrive\/([a-zA-Z])/) do |match|
				"#{$1.upcase}:"
			end
			filename.gsub!(/\//, '\\')
			
      if message.respond_to?(:line_nr) &&  !message.line_nr.nil?
        line = "(#{message.line_nr})"
			else
        line = '(0)'
      end
		
			result = "#{filename}#{line} : #{type_string} X#{message.errno}: #{message.message}" << ENDL
			if message.respond_to?(:token) && !message.token.nil?
				cc.tz.get_code_context(message.token, 100).each do |string|
					result += string << ENDL
				end
			end

			#if message.respond_to?(:backtrace)
			#	$stderr.puts message.backtrace
			#end

			return result
		
		end

		def format(message)

			type_string = ''
			if (message.class <= CompilerWarning)
				type_string = "WARNING "
			elsif(message.class <= CompilerError  || message.class <= RuntimeError)
				type_string = "ERROR   "
			else
				type_string = "MESSAGE "
			end

			result = "#{type_string} X#{message.errno} "
			result += "#{File.basename(cc.cu.src_filename)}"
			result += ":#{message.line_nr}" if message.respond_to?(:line_nr) &&  !message.line_nr.nil?
			result += ENDL
			
			result += "  #{message.class.name}" << ENDL if LOG.info?
			result += "  #{message.message}" << ENDL unless message.message.nil? || message.message.empty?
			
			if !message.token.nil?
				cols = ENV['COLUMNS'].to_i
				cols = 80 if cols <= 80
				cc.tz.get_code_context(message.token, cols - 3).each do |string|
					result += "  #{string}" << ENDL
				end
			elsif !message.sample.nil?
				result += "  #{message.sample}" << ENDL
			elsif !message.line_nr.nil?
				result += "  #{cc.tz.get_code_line(message.line_nr)}" << ENDL
			end
			result += ENDL
			


			return result
		end
	
	end
	
	class CompilerMessage < RuntimeError
	
		attr_accessor :line_nr
		attr_accessor :sample
    attr_accessor :message
		attr_reader   :token
		attr_reader   :error
		attr_reader   :errno
		
		def initialize(compiler_context)
			@cc = compiler_context
		end

		def token=(token)
			if token.nil?
				$stderr.puts "token information not available"
			else
				@token = token
				@line_nr = @token.line_nr
			end
		end
	end
  
	#fatal errors stop the compiling process almost at once
	class CompilerFatal < CompilerMessage
	end
	
  class InternalError < CompilerMessage
		def initialize(message = 'internal error')
			super
			@errno = 2357
		end
	end
  
	class CompilerWarning < CompilerMessage
	end
	
	class CompilerError < CompilerMessage
	end
	
	 class DoubleIncludeError < CompilerError
		def initialize(compiler_context, filename)
			super(compiler_context)
			self.message = "File '#{filename}' is included more than once"
			@errno = 9001
		end
	end
	
	class FileOpenError < CompilerError
		def initialize(compiler_context, filename)
			super(compiler_context)
			self.message = "unable to open file '#{filename}'"
			@errno = 2375
		end
	end
        
        class FileTypeError < CompilerError
		def initialize(compiler_context, filename)
			super(compiler_context)
			self.message = "File '#{filename}' doesn't have extension .xabsl"
			@errno = 7253
		end
	end
	
	class SyntaxError < CompilerFatal
		def initialize(compiler_context)
			super(compiler_context)
			self.message = "syntax error."
			@errno = 2537
		end
	end

	class SemanticWarning < CompilerWarning
	end

	class SemanticError < CompilerError
	end

	class UnexpectedEndOfFile < SyntaxError
		def initialize(compiler_context)
			super(compiler_context)
			self.message = "unexpected end of file."
			@errno = 2555
		end
	end
		
	class TokenizerError < SyntaxError
		def initialize(compiler_context, line_nr, sample)
			super(compiler_context)
			self.line_nr = line_nr
			self.sample = sample
			self.message = "illegal characters encountered."
			@errno = 2573
		end
	end

    class UnevenStringDelimiterError < SyntaxError
        def initialize(compiler_context, delimiter)
			super(compiler_context)
			self.line_nr = line_nr
			self.sample = sample
			self.message = "uneven count of string delimiters '#{delimiter}' encountered."
			@errno = 2735
        end
    end

    class ParenthesisError < SyntaxError
        def initialize(compiler_context, left_string, left_count, right_string, right_count)
			super(compiler_context)
			self.message = "counted #{left_count} left parenthesis '#{left_string}' and #{right_count} right parenthesis."
			@errno = 2753
        end
    end

	class ClosingCommentError < SyntaxError
		def initialize(compiler_context, line_nr)
			super(compiler_context)
			self.line_nr = line_nr
			self.message = "misplaced end-of-comment detected"
			@errno = 3257
		end
	end

	class ClosingCommentMissingError < SyntaxError
		def initialize(compiler_context, line_nr)
			super(compiler_context)
			self.line_nr = line_nr
			self.message = "end-of-comment missing."
			@errno = 3275
		end
	end

	class NamespaceSeparation < SemanticError
		def initialize(compiler_context, token, symbol)
			super(compiler_context)
			self.token = token
			self.message = "namespace '#{compiler_context.sc.to_code(symbol)}' can contain either symbols or behaviors. offending declaration is '#{token}'."
			@errno = 3527
		end
	end

	class EmptyNamespaceWarning < SemanticWarning
		def initialize(compiler_context, token)
			super(compiler_context)
			self.token = token
			self.message = "namespace '#{token.to_s}' is empty."
			@errno = 3572
		end
	end
	
	class ParameterMultipleSetting < SemanticError
		def initialize(compiler_context, token)
			super(compiler_context)
			self.token = token
			self.message = "parameter '#{token.to_s}' is set more than once."
			@errno = 3725
		end
	end
	
	class ParameterMissing < SemanticWarning
		def initialize(compiler_context, call_token, missing_symbol)
			super(compiler_context)
			self.token = call_token
			self.message = "parameter '#{compiler_context.sc.to_code(missing_symbol)}' is missing in call to '#{call_token.to_s}'."
			@errno = 3752
		end
	end
	
	class DomainSymbolExpected < SemanticError
		def initialize(compiler_context, token)
			super(compiler_context)
			self.token = token
			self.message = "domain symbol expected instead of '#{token.to_s}'."
			@errno = 5237
		end
	end

	class MismatchingDomain < SemanticError
		def initialize(compiler_context, token)
			super(compiler_context)
			self.token = token
			self.message = "'#{token.to_s}' has mismatching domain."
			@errno = 5255
		end
	end
		
	class InitialStateMissing < SemanticError
		def initialize(compiler_context, token)
			super(compiler_context)
			self.token = token
			self.message = "initial state is missing on option '#{token.to_s}'."
			@errno = 5273
		end
	end

	class InitialStateMultipleDefinition < SemanticError
		def initialize(compiler_context, token)
			super(compiler_context)
			self.token = token
			self.message = "multiple initial state definitions, '#{token.to_s}'."
			@errno = 5327
		end
	end

	class MultipleDefinitions < SemanticError
		def initialize(compiler_context, token)
			super(compiler_context)
			self.token = token
			self.message = "'#{token.to_s}' is defined more than once."
			@errno = 5372
		end
	end
	
	
	class TypeMismatch < SemanticError
		def initialize(compiler_context, token)
			super(compiler_context)
			self.token = token
			self.message = "type mismatch on '#{token.to_s}'."
			@errno = 5525
		end
	end

	class EnumTypeMismatch < SemanticError
		def initialize(compiler_context, token, enum1, enum2)
			super(compiler_context)
			self.token = token
			self.message = "enumeration mismatch on '#{token.to_s}' expected '#{enum1.identifier}' found '#{enum2.identifier}'."
			@errno = 5526
		end
	end
	
	class NoMatchingSymbolFound < SemanticError
		def initialize(compiler_context, token, domain_symbol, symbol_classes)
			super(compiler_context)
			self.token = token
			self.message = "no matching symbol found for '#{token.to_s}' "
			self.message += "with domain '#{compiler_context.sc.to_code(domain_symbol)}' " unless domain_symbol.nil?
			self.message += "and symbol class in #{symbol_classes.inspect}."
			@errno = 5552
		end
	end

	class MultipleMatchingSymbolsFound < SemanticError
		def initialize(compiler_context, token, domain_symbol, symbol_classes, symbols)
			super(compiler_context)
			self.token = token
			self.message = "multiple matching symbol found for '#{token.to_s}' "
			self.message += "with domain '#{compiler_context.sc.to_code(domain_symbol)}' " unless domain_symbol.nil?
			self.message += "and symbol class in #{symbol_classes.inspect}: "
			symbols.each do |symbol|
				self.message += "#{symbol.class.name}"
				self.message += " with domain '#{compiler_context.sc.to_code(symbol.domain_symbol)}'" if symbol.class <= DomainValueSymbol && !symbol.domain_symbol.nil?
				self.message += ", "
			end
			@errno = 5723
		end
	end

	class UnexpectedLE < SemanticError
		def initialize(compiler_context, token, le, parent)
			super(compiler_context)
			self.token = token
			self.message = "unexpected language element '#{le.tag}' for '#{parent.tag}'"
			@errno = 5732
		end
	end
	
  class NamingWarning < SemanticWarning
		def initialize(compiler_context, token)
			super(compiler_context)
			self.token = token
			self.message = "file should be named like option or namespace"
			@errno = 7235
		end  
  end
  
  class DecisionTreeMustStartWithElse < SemanticWarning
		def initialize(compiler_context, token)
			super(compiler_context)
			self.token = token
			self.message = "decision tree has to start with else when the option has a common decision tree"
			@errno = 7325
		end
	end

  class DecisionTreeMustNotStartWithElse < SemanticError
		def initialize(compiler_context, token)
			super(compiler_context)
			self.token = token
			self.message = "decision tree must not start with else when the option has no common decision tree"
			@errno = 7352
		end
	end

  #unused errnos
	#7523
	#7532
	
end
