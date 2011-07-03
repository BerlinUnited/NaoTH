module Translator

	class Token

		attr_reader :token_type;
		attr_reader :line_nr;

		def initialize(token_type, token_string, line_nr)
			@token_type = token_type
			@token_string = token_string
			@line_nr = line_nr
		end

		def raw_string
			return @token_string
		end

		def to_s
		
		    #cut away inverted commas und square backets
            if token_type == :STRING || token_type == :RANGE
                return @token_string[1..-2]
            end
            
            return @token_string            
        end

	end

	class Tokenizer < CompilerContextWorker
	
		attr_reader :keywords

		@@keywords = nil
		
		def Tokenizer.keywords
			return @@keywords unless @@keywords.nil?
		
			@@keywords = {}
			@@keywords[TOKEN_TRUE] = :TRUE
			@@keywords[TOKEN_FALSE] = :FALSE
			@@keywords[TOKEN_OPTION] = :OPTION
			@@keywords[TOKEN_ACTION] = :ACTION
			@@keywords[TOKEN_INITIAL] = :INITIAL
			@@keywords[TOKEN_TARGET] = :TARGET
			@@keywords[TOKEN_STATE] = :STATE
			@@keywords[TOKEN_COMMON] = :COMMON
			@@keywords[TOKEN_DECISION] = :DECISION
			@@keywords[TOKEN_TTS] = :TTS
			@@keywords[TOKEN_IF] = :IF
			@@keywords[TOKEN_ELSE] = :ELSE
			@@keywords[TOKEN_TOOX] = :TOOX
			@@keywords[TOKEN_TOSX] = :TOSX
			@@keywords[TOKEN_SORTS] = :SORTS
			@@keywords[TOKEN_AGENT] = :AGENT
			@@keywords[TOKEN_FUNCTION] = :FUNCTION
			@@keywords[TOKEN_ENUM] = :ENUM
			@@keywords[TOKEN_INPUT] = :INPUT
			@@keywords[TOKEN_OUTPUT] = :OUTPUT
			@@keywords[TOKEN_BOOL] = :BOOL
			@@keywords[TOKEN_FLOAT] = :FLOAT
			@@keywords[TOKEN_CONST] = :CONST
			@@keywords[TOKEN_NAMESPACE] = :NAMESPACE
			@@keywords[TOKEN_BEHAVIOR] = :BEHAVIOR
			@@keywords[TOKEN_INCLUDE] = :INCLUDE
			@@keywords[TOKEN_STAY] = :STAY
			@@keywords[TOKEN_INTERNAL] = :INTERNAL
			@@keywords[TOKEN_CAPACITY] = :CAPACITY
			@@keywords[TOKEN_SYNCHRONIZED] = :SYNCHRONIZED
			@@keywords[TOKEN_CONFLICT] = :CONFLICT
			
			return @@keywords
		end
		
		def initialize(compiler_context)
			super
			@token_index = 0
			@token_counter = 0
		end

		def rewind
			@token_index = 0
		end
		
		def next_token
			token = @stream[@token_index]
			@token_index += 1
			return token
		end

		def dump
			@stream.each do |token|
				p token.raw_string
			end
		end

		def get_code_line(line_nr)
			return @lines[line_nr]
		end

		def get_code_context(token, size)

			line_tokens = Array.new(@stream) #copy token stream
			line_tokens.delete_if do |t|
				t.line_nr != token.line_nr
			end
			
			index = line_tokens.index(token)
			regexp = "("
			(0..index-1).each do |i|
				regexp += Regexp.escape(line_tokens[i].raw_string)
				regexp += "\\s*(/\\*.*?\\*/\\s*)*"
			end
			regexp += ")(#{Regexp.escape(line_tokens[index].raw_string)})("
			(index+1..line_tokens.size-1).each do |i|
				regexp += "\\s*(/\\*.*?\\*/\\s*)*"
				regexp += Regexp.escape(line_tokens[i].raw_string)
			end
			regexp += ")"

			regexp = Regexp.new(regexp)
			match = regexp.match(@lines[token.line_nr])


			r = ''
			m = ''
			l = ''
			unless  match.nil?
				l = match[1].to_s
				r = match[index+3].to_s
				m = match[index+2].to_s
			end
			
			#rest size for left and right part
			size -= m.size
			if(size <= 0)
				l = ''
				r = ''
			else
				if size > l.size

					size -= l.size
					r = r[0..[r.size, size].min - 1]

				else
					l = l[-size..-1]
					r = ''
				end
			end
			
			return ["#{l}#{m}#{r}","#{' '*l.length}#{'^'*m.length}#{' '*r.length}"]
		
		end
		
		def push(token_type, token_string)
			token = Token.new(token_type, token_string, @line_nr)
            @token_counter[token_type] += 1
			@stream.push token
		end

		def pop
			token = @stream.pop
			@token_counter[token.token_type] -= 1
			return token
		end
		
		def tokenized?
		    return !@stream.nil?
		end
		
		def tokenize( str )

			comment_mode = :no_comment

			@token_counter = Hash.new(0)
			@stream = []
			@lines = []
			@line_nr = 1
			
			until str.empty? do
				
				#if we are within a comment...
				unless comment_mode == :no_comment
					case str
					when /\A((.|\n|\r)*?)(\*\/)/
						
						case comment_mode
						when :doc_comment:
							push(:DOC_COMMENT, $1.strip)
						when :initial_comment:
							push(:INITIAL_COMMENT, $1.strip)
						end
						str = $'

						#count newlines
						@line_nr += $1.count("\n")
						if @lines[@line_nr].nil?
							str =~ /^(.*)$/
							@lines[@line_nr] = $1.chomp
						end

						comment_mode = :no_comment
					else
						
						raise ClosingCommentMissingError.new(cc, @line_nr)
					end	
				else
			
					#consume whitespaces between tokens until end-of-line
					while str =~ /\A([\s]+|\/\/.*?[\r\n]|[\r\n])/
						str = $'

						@line_nr += $1.count("\n")
						if @lines[@line_nr].nil?
							str =~ /^(.*)$/
							@lines[@line_nr] = $1.chomp
						end
					end

					#check for keywords
					if str =~ /\A[a-zA-Z_][a-zA-Z0-9_\.\-]*/
						keyword_symbol = Tokenizer.keywords[$&]
						unless keyword_symbol.nil?

							token_match = $&
							push(keyword_symbol, token_match)
							str = $'

							next
						end
					end

					case str
					when ''
						#do nothing
					when /\A[0-9]+(\.[0-9]*)?|^[1-9]\.[0-9]*E(\+|-)?[0-9]+/
						push(:NUMBER, $&)
						str = $'
					when /\A[@]?[a-zA-Z_][a-zA-Z0-9_\.]*/
						push(:IDENTIFIER, $&)
						str = $'
					when /\A("[^"]*")/
						push(:STRING, $1.to_s)
						str = $'
					when /\A\/\*\*\*/
						str = $'
						comment_mode = :initial_comment
					when /\A\/\*\*/
						str = $'
						comment_mode = :doc_comment
					when /\A\/\*/
						str = $'
						comment_mode = :comment
					when /\A(\[[^\]]*\])/
						push(:RANGE, $1.to_s)
						str = $'
					when /\A\*\//
						raise ClosingCommentError.new(cc, @line_nr)
					else
						
						b = str[0,2]
						c = str[0,1]
						
						case b
						when '<='
						    push(:LE, b)
						    str = str[2..-1]
						when '>='
						    push(:GE, b)
						    str = str[2..-1]
						when '=='
						    push(:EQ, b)
						    str = str[2..-1]
						when '!='
						    push(:NEQ, b)
						    str = str[2..-1]
						when '&&'
						    push(:AND, b)
						    str = str[2..-1]
						when '||'
						    push(:OR, b)
						    str = str[2..-1]
                        else
    						if '%!<>[]{}();|+-*/=,?:'.index(c)
    							push(c, c)
    							str = str[1..-1]
    						else
    							dump if LOG.debug?
    							raise TokenizerError.new(cc, @line_nr, str[0..[30, str.length].min])
    						end
                        end
							
					end
				end
			end
			
			#raise ParenthesisError.new(cc, '(', @token_counter['('], ')', @token_counter[')']) unless @token_counter['('] == @token_counter[')']
			#raise ParenthesisError.new(cc, '{', @token_counter['{'], '}', @token_counter['}']) unless @token_counter['{'] == @token_counter['}']
			#raise UnevenStringDelimiterError.new(cc, '"') unless (@token_counter['"'] % 2) == 0
			
		end
	end
end
