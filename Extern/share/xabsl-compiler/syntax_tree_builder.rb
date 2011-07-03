module Translator

	class SyntaxTreeBuilder < CompilerContextWorker

		def initialize
			super(nil)
		end

		def debug_val(val)
			val.each_index do |i|
				$stderr.puts "#{i} #{val[i]}"
			end
			exit
		end
	
		def assume(val, expected)
			
			return unless PARANOIA
			
			val.each_index do |i|
				v = val[i]
				e = expected[i]
				
				unless expected[i].nil?
					if v.class <= Token 
						if  v.token_type != e
							LOG.error "got token #{v.inspect}"
							LOG.error "expected token type #{e.inspect}"
							raise InternalError.new
						end
					elsif e.class <= Class
						unless  v.class <= e
							LOG.error "got #{v.inspect}"
							LOG.error "expected class #{e.name}"
							raise InternalError.new
						end
					end
				end
				
			end
		end

		def compiler_context=(compiler_context)
			@compiler_context = compiler_context
		end

		def option val
		end

		def option_early val
		end

		def parameter_declaration val
		end

		def state_declaration val
		end
		
		def state_declaration_early val
		end

		def subsequent_declaration_early val
		end

		def subsequent_declaration val
		end

		def output_symbol_assignment_early val
		end
		
		def output_symbol_assignment val
		end

		def exp_number(val)
		end
		
		def exp_boolean(val)
	  end

		def exp_identifier val
		end

		def decision_tree val
		end

		def dtree val
		end

		def gtree val
		end
		
		def common_decision_tree val
		end

		def common_action val
		end

		def cdtree val
		end
		
		def cdtree_elseif_decision val
		end
		
		def transition val
		end

    def exp_binary_early val
    end
    
		def exp_binary val
		end

		def set_parameter_early val
		end
		
		def set_parameter val
		end
		
		def with_parameter_early val
		end

		def with_parameter val
		end

		def comments val
		end

		def function_early val
		end

		def function val
		end

		def conditional_expression val
		end

		def exp_unary val
		end
		
		def agent val
		end
		
		def const val
		end
		
		def symbol_declaration_early val
		end

		def symbol_declaration val
		end
		
		def enum_symbol_declaration val
		end
			
		def behavior_declaration_early val
		end

		def behavior_declaration val
		end
	
		def namespace val
		end

		def namespace_early val
		end
		
		def include val
		end
	end

	class FirstPassSyntaxTreeBuilder < SyntaxTreeBuilder

		def initialize
			super
		end

		def include(val)
			pc.includes << val[1].to_s
		end
		
		def enum_symbol_declaration val
			unless val[1].nil? || val[1].token_type == :FLOAT || val[1].token_type == :BOOL
				symbol = cc.sc.from_code_declare(val[1], EnumSymbol)
			else
				error = SyntaxError.new(cc)
				error.token = val[1]
				eh.error error
			end
		end
  end
  
	class SecondPassSyntaxTreeBuilder < SyntaxTreeBuilder

		def initialize
			super
		end

		def enum_symbol_declaration val
      			
			unless val[1].nil? || val[1].token_type == :FLOAT || val[1].token_type == :BOOL
				symbol = cc.sc.from_code(val[1], EnumSymbol)
			end			
		
			if [nil, :symbols].include?(pc.namespace_symbol.namespace_type)
				pc.namespace_symbol.namespace_type = :symbols
			else
				eh.error NamespaceSeparation.new(cc, val[3], pc.namespace_symbol)
			end

			tag = ETag.new

			tag.attr[ATTR_NAME] = symbol
			tag.attr[ATTR_DESCRIPTION] = val[0].to_s unless val[0].nil?

			unless val[2].nil?
				if val[2].token_type == :INTERNAL
					tag.attr[ATTR_INTERNAL] = "true" 
					symbol.internal = true
				else
					error = SyntaxError.new(cc)
					error.token = val[2]
					eh.error error
				end
			end
			
			val[4].each do |token|
				sub_tag = EnumTag.new
				tag.add_child(sub_tag, cc)
				sub_tag.attr[ATTR_NAME] = sc.from_code_declare(token, EnumSymbolValueSymbol, symbol, nil, true)
			end
			return tag
		end

		def parameter_declaration val

			symbol = nil
			if(!pc.option_symbol.nil?)
				symbol = cc.sc.from_code_declare(val[2], OptionParameterSymbol, pc.option_symbol)
			elsif(!pc.input_symbol.nil?)
				symbol = cc.sc.from_code_declare(val[2], InputSymbolParameterSymbol, pc.input_symbol, nil, true)
			elsif(!pc.behavior_symbol.nil?)
				symbol = cc.sc.from_code_declare(val[2], BasicBehaviorParameterSymbol, pc.behavior_symbol, nil, true)
			else
				raise InternalError.new
			end
			
			if val[1].nil? || val[1].token_type == :FLOAT
			  symbol.type = :decimal
  			tag = DParaTag.new
			elsif val[1].token_type == :BOOL
			  symbol.type = :boolean
  			tag = BParaTag.new
			else
			  symbol.type = :enum
			  symbol.enum_domain = cc.sc.from_code(val[1], EnumSymbol)
  			tag = EParaTag.new
  			tag.attr[ATTR_ENUM] = symbol.enum_domain
			end

			tag.map_tokens val
      
			tag.attr[ATTR_DESCRIPTION] = val[0].to_s unless val[0].nil?
			tag.attr[ATTR_NAME] = symbol
			if symbol.type == :decimal
			  tag.attr[ATTR_RANGE] = val[3].to_s unless val[3].nil?
			  tag.attr[ATTR_MEASURE] = val[4].to_s unless val[4].nil?
			else
				unless val[3].nil?
					error = SyntaxError.new(cc)
					error.token = val[3]
					error.message = "must not give range for boolean/enumerated parameter"
					eh.error error
				end
				unless val[4].nil?
					error = SyntaxError.new(cc)
					error.token = val[4]
					error.message = "must not give measure for boolean/enumerated parameter"
					eh.error error
				end
			end
						
			return tag
		end

		def option_early val
			symbol = cc.sc.from_code_declare(val[-2], OptionSymbol)
			
			pc.option_symbol = symbol
			pc.option_declared = true
			symbol.namespace_symbol = OPTION_DEFINITIONS_ENTITY
		end
		
		def option val

			odTag = ODTag.new
			pc.option_definitions_temp.add_child(odTag, cc)
			
			odTag.attr[ATTR_DESCRIPTION] = val[0].to_s unless val[0].nil?
			odTag.attr[ATTR_NAME] = pc.option_symbol

			#add ParaTags
			val[5].each do |paraTag|
				
				odTag.add_child(paraTag, cc)
			end
			pc.option_symbol = nil
		end

		def state_declaration_early val
			sc.from_code_declare(val[-3], StateSymbol)
		end
		
		def agent val
			sc.from_code_declare(val[2], AgentSymbol)
			pc.agent_declared = true
		end

		def const val
			
			if [nil, :symbols].include?(pc.namespace_symbol.namespace_type)
				pc.namespace_symbol.namespace_type = :symbols
			else
				eh.error NamespaceSeparation.new(cc, val[3], pc.namespace_symbol)
			end

			symbol = sc.from_code_declare(val[3], ConstantSymbol)
			tag = ConstantTag.new
			tag.attr[ATTR_NAME] = symbol
			tag.attr[ATTR_DESCRIPTION] = val[0].to_s unless val[0].nil?
			tag.attr[ATTR_MEASURE] = val[7].to_s unless val[7].nil?
			symbol.value = val[6].to_s
			symbol.value = "-#{symbol.value}" unless val[5].nil? 
			tag.attr[ATTR_VALUE] = symbol.value
			return tag
		end
		
		def symbol_declaration_early val
			if val[-5].nil? || val[-5].token_type == :FLOAT
			  if val[-4].nil? || val[-4].token_type == :INPUT
			    pc.input_symbol = cc.sc.from_code_declare(val[-3], DecimalInputSymbol)
			  elsif val[-4].token_type == :OUTPUT || val[-4].token_type == :INTERNAL
			    pc.input_symbol = cc.sc.from_code_declare(val[-3], DecimalOutputSymbol)
			  else
  				raise InternalError.new
        end			  
			elsif val[-5].token_type == :BOOL
			  if val[-4].nil? || val[-4].token_type == :INPUT
			    pc.input_symbol = cc.sc.from_code_declare(val[-3], BooleanInputSymbol)
			  elsif val[-4].token_type == :OUTPUT || val[-4].token_type == :INTERNAL
			    pc.input_symbol = cc.sc.from_code_declare(val[-3], BooleanOutputSymbol)
        else
          raise InternalError.new
        end			  
      else
			  if val[-4].nil? || val[-4].token_type == :INPUT
			    pc.input_symbol = cc.sc.from_code_declare(val[-3], EnumInputSymbol)
			  elsif val[-4].token_type == :OUTPUT || val[-4].token_type == :INTERNAL
			    pc.input_symbol = cc.sc.from_code_declare(val[-3], EnumOutputSymbol)
        else
          raise InternalError.new
        end			  
      end			  
      		
			if [nil, :symbols].include?(pc.namespace_symbol.namespace_type)
				pc.namespace_symbol.namespace_type = :symbols
			else
				eh.error NamespaceSeparation.new(cc, val[-3], pc.namespace_symbol)
			end
		end
		
		def symbol_declaration val
			tag = nil
			if val[1].nil? || val[1].token_type == :FLOAT
			  if val[2].nil? || val[2].token_type == :INPUT
				  tag = DISTag.new
			  elsif val[2].token_type == :OUTPUT || val[2].token_type == :INTERNAL
				  tag = DOSTag.new
        else
          raise InternalError.new
        end			  
        symbol = pc.input_symbol
        tag.attr[ATTR_RANGE] = val[4].to_s unless val[4].nil?
				tag.attr[ATTR_MEASURE] = val[5].to_s unless val[5].nil?
			  pc.input_symbol = nil
			elsif val[1].token_type == :BOOL
			  if val[2].nil? || val[2].token_type == :INPUT
				  tag = BISTag.new
			  elsif val[2].token_type == :OUTPUT || val[2].token_type == :INTERNAL
				  tag = BOSTag.new
        else
          raise InternalError.new
        end			  
				unless val[4].nil?
					error = SyntaxError.new(cc)
					error.token = val[4]
					error.message = "must not give range for boolean symbol"
					eh.error error
				end
				unless val[5].nil?
					error = SyntaxError.new(cc)
					error.token = val[5]
					error.message = "must not give measure for boolean symbol"
					eh.error error
				end
				symbol = pc.input_symbol
			  pc.input_symbol = nil
      else
			  if val[2].nil? || val[2].token_type == :INPUT
				  tag = EISTag.new
			  elsif val[2].token_type == :OUTPUT || val[2].token_type == :INTERNAL
				  tag = EOSTag.new
        else
          raise InternalError.new
        end			  
				unless val[4].nil?
					error = SyntaxError.new(cc)
					error.token = val[4]
					error.message = "must not give measure for enumerated symbol"
					eh.error error
				end
				unless val[5].nil?
					error = SyntaxError.new(cc)
					error.token = val[5]
					error.message = "must not give range for enumerated symbol"
					eh.error error
				end
				symbol = pc.input_symbol
				symbol.enum_domain = cc.sc.from_code(val[1], EnumSymbol)
		    tag.attr[ATTR_ENUM] = symbol.enum_domain
			  pc.input_symbol = nil
      end	
      tag.attr[ATTR_NAME] = symbol
      
      if val[2].token_type == :INTERNAL
  			tag.attr[ATTR_INTERNAL] = "true"
   			symbol.internal = true
      end	
      
      unless val[7].nil?
	      if val[2].token_type == :INPUT
	  			val[7].each do |sub_tag|
		  			tag.add_child(sub_tag, cc)
				  end
				else
					error = SyntaxError.new(cc)
					error.token = val[7]
					eh.error error
				end
			end      		  
			tag.attr[ATTR_DESCRIPTION] = val[0].to_s unless val[0].nil?
			return tag
		end
		
		def namespace_early val
			pc.namespace_symbol = cc.sc.from_code_declare(val[-5], NamespaceSymbol)
			pc.namespace_declared = true
		end
		
		def namespace val
			tag = nil
			dir = ""
			case pc.namespace_symbol.namespace_type
			when :symbols
				tag = SymbolsTag.new
				dir = "Symbols"
				#tag.attr[ATTR_XSI_SCHEMALOCATION] = "http://www.xabsl.de"
			when :behaviors
				tag = BBSTag.new
				dir = "BasicBehaviors"
				#tag.attr[ATTR_XSI_SCHEMALOCATION] = "http://www.xabsl.de"
			when nil
				eh.error EmptyNamespaceWarning.new(cc, val[2])
			else
				raise InternalError.new
			end

			unless tag.nil?
				tag.attr[ATTR_ID] = pc.namespace_symbol
				tag.attr[ATTR_TITLE] = val[4].to_s
  			tag.attr[ATTR_DESCRIPTION] = val[0].to_s unless val[0].nil?
				tag.attr[ATTR_XMLNS] = 'http://www.xabsl.de'
				tag.attr[ATTR_XMLNS_XSI] = 'http://www.w3.org/2001/XMLSchema-instance'

				#inner namespace thingies
				val[8].each do |sub_tag|
					tag.add_child(sub_tag, cc)
				end
				
				pc.namespace_root.add_child(tag, cc)
			end
				
			if pc.namespace_symbol.identifier != File.basename(cc.cu.src_filename).sub(/\..*?$/, '')
        eh.error NamingWarning.new(cc, val[2])
			end
      if cc.cu.dest_filename =~ /^.*(\..*)$/ 
      	ext = $1 
      else 
      	ext = ".xml"
      end
      cc.cu.set_dest(dir, pc.namespace_symbol.identifier + ext) 

			pc.namespace_symbol = nil
		end
		
		def behavior_declaration_early val
			pc.behavior_symbol = cc.sc.from_code_declare(val[-2], BasicBehaviorSymbol)
			if [nil, :behaviors].include?(pc.namespace_symbol.namespace_type)
				pc.namespace_symbol.namespace_type = :behaviors
			else
				eh.error NamespaceSeparation.new(cc, val[-2], pc.namespace_symbol)
			end
		end
		
		def behavior_declaration val

			tag = BBTag.new
			tag.attr[ATTR_NAME] = pc.behavior_symbol
			tag.attr[ATTR_DESCRIPTION] = val[0].to_s unless val[0].nil?
					
			val[5].each do |sub_tag|
				tag.add_child(sub_tag, cc)
			end

			pc.behavior_symbol = nil
			return tag
		end
	end

	class ThirdPassSyntaxTreeBuilder < SyntaxTreeBuilder

		def initialize
			super
		end

		
		def comments val
			if pc.agent_declared
				agent_collection_tag = pc.agent_collection_root.child(AgentCollectionTag)
				agent_collection_tag.title = $1.to_s if val[0].to_s =~ /Title:\s*(.*?)\s*$/i
				agent_collection_tag.platform = $1.to_s if val[0].to_s =~ /Platform:\s*(.*?)\s*$/i
				agent_collection_tag.software_environment = $1.to_s if val[0].to_s =~ /Software-Environment:\s*(.*?)\s*$/i
			end
		end

		
		def option_early val
			pc.option_symbol = cc.sc.from_code(val[-2], OptionSymbol)
    end

		def option val
		
			oTag = OptionTag.new
			pc.option_root.add_child(oTag, cc)

			oTag.attr[ATTR_NAME] = pc.option_symbol
			oTag.attr[ATTR_XMLNS] = 'http://www.xabsl.de'
			oTag.attr[ATTR_XMLNS_XSI] = 'http://www.w3.org/2001/XMLSchema-instance'
			#oTag.attr[ATTR_XSI_SCHEMALOCATION] = "http://www.xabsl.de"
			
			#common decision tree/action
			unless val[6].nil?
				val[6].each { |cTag|
					oTag.add_child(cTag, cc)
				}
			end
								
			#set initial state from cc
			if pc.initial_state_symbol.nil?
				cc.eh.error InitialStateMissing.new(cc, val[2])
				pc.initial_state_symbol = st.declare('__initial', StateSymbol);
			else
				oTag.attr[ATTR_INITIAL_STATE] = pc.initial_state_symbol
			end

			#add StateTags
			val[7].each { |stateTag|
				oTag.add_child(stateTag, cc)
			}

      if pc.option_symbol.identifier != File.basename(cc.cu.src_filename).sub(/\..*?$/, '')
        eh.error NamingWarning.new(cc, val[2])
      end
      if cc.cu.dest_filename =~ /^.*(\..*)$/ 
      	ext = $1 
      else 
      	ext = ".xml"
      end
      cc.cu.set_dest("Options", pc.option_symbol.identifier + ext) 

			pc.option_symbol = nil
			pc.initial_state_symbol = nil
		end

		def parameter_declaration val
			return nil
		end

		def state_declaration_early val
			pc.state_symbol = cc.sc.from_code(val[-3], StateSymbol)
		end


		def state_declaration val
			
			tag = StateTag.new		
			
			#initial
			unless val[0].nil?
				unless pc.initial_state_symbol.nil?
					cc.eh.error InitialStateMultipleDefinition.new(cc, val[3])
				end
				pc.initial_state_symbol = pc.state_symbol
			end
			
			#target
			tag.attr[ATTR_IS_TARGET_STATE] = "true" unless val[1].nil?
			tag.attr[ATTR_NAME] = pc.state_symbol
			
			unless val[4].nil?
			  if val[4][0].token_type == :CAPACITY
			  	tag.attr[ATTR_CAPACITY] = val[4][1].to_s
			  elsif val[4][0].token_type == :SYNCHRONIZED
			  	tag.attr[ATTR_SYNCHRONIZED] = (val[4][1].nil?) ? "0" : val[4][1].to_s 
			  end
			end

			#SO / SBB / output symbol decls
			val[10].each { |sosTag|
				tag.add_child(sosTag, cc)
			}
					
			#decisiton tree
			tag.add_child(val[7], cc)
			pc.state_symbol = nil

			return tag
		end
				
		def subsequent_declaration_early val
			raise InternalError.new unless pc.subsequent_dpc.nil?
			raise InternalError.new unless pc.subsequent_type.nil?
			
			symbols = nil
			if val[-2].nil?
				symbols = [OptionSymbol, BasicBehaviorSymbol]
			elsif val[-2].token_type == :OPTION
				symbols = [OptionSymbol]
			elsif val[-2].token_type == :BEHAVIOR
				symbols = [BasicBehaviorSymbol]
			else			
				raise InternalError.new
			end
						
			symbol = cc.sc.from_code(val[-3], symbols)
			case symbol
			when OptionSymbol:
				pc.set_subsequent(DomainParameterContext.new(symbol, cc), :option)
			when BasicBehaviorSymbol:
				pc.set_subsequent(DomainParameterContext.new(symbol, cc), :behavior)
			else
				raise InternalError.new
			end
		end


		def subsequent_declaration val
			tag = nil
			symbol = nil
				
			case pc.subsequent_type
			when :option
				tag = SOTag.new
				symbol = cc.sc.from_code(val[0], OptionSymbol)
				
			when :behavior
				tag = SBBTag.new
				symbol = cc.sc.from_code(val[0], BasicBehaviorSymbol)
			else
				raise InternalError.new
			end
				
			pc.subsequent_dpc.check_used_value_symbols(val[0])

			tag.attr[ATTR_REF] = symbol

			#parameters
			val[4].each { |aTag|
				tag.add_child(aTag, cc)
			}

			pc.set_subsequent(nil, nil)
			return tag
		end

    def set_parameter_early val
			case pc.subsequent_type
			when :option
				symbol = cc.sc.from_code(val[-1], OptionParameterSymbol, pc.subsequent_dpc.domain_symbol)
			when :behavior
				symbol = cc.sc.from_code(val[-1], BasicBehaviorParameterSymbol, pc.subsequent_dpc.domain_symbol)
			else
				raise InternalError.new
			end
			pc.enum_domains.push(symbol.enum_domain) if symbol.type == :enum
    end

		def set_parameter val
			symbol = nil
			case pc.subsequent_type
			when :option
				symbol = cc.sc.from_code(val[0], OptionParameterSymbol, pc.subsequent_dpc.domain_symbol)
			when :behavior
				symbol = cc.sc.from_code(val[0], BasicBehaviorParameterSymbol, pc.subsequent_dpc.domain_symbol)
			else
				raise InternalError.new
			end

      pc.enum_domains.pop if symbol.type == :enum			
			pc.subsequent_dpc.use_value_symbol(symbol, val[0])
			
			tag = SPTag.new
      tag.map_tokens val
     
			tag.attr[ATTR_REF] = symbol
			tag.in_type = symbol.type
			tag.add_child(val[3], cc)
			return tag
		end

    def with_parameter_early val
			symbol = cc.sc.from_code(val[-1], InputSymbolParameterSymbol, pc.function_dpcs.last.domain_symbol)
			pc.enum_domains.push(symbol.enum_domain) if symbol.type == :enum
    end
    
		def with_parameter val
			tag = WPTag.new
			
			domain_context = pc.function_dpcs.last
			symbol = cc.sc.from_code(val[0], InputSymbolParameterSymbol, domain_context.domain_symbol)
			pc.enum_domains.pop if symbol.type == :enum
			domain_context.use_value_symbol(symbol, val[0])
			tag.attr[ATTR_REF] = symbol
			tag.in_type = symbol.type
			tag.add_child(val[3], cc)
			return tag
		end
						
		def conditional_expression val
			tag = CETag.new
			tag.map_tokens val
      cond_tag = CondTag.new
			ex1_tag = EX1Tag.new
			ex1_tag.map_tokens [val[2]]
			ex2_tag = EX2Tag.new
			ex2_tag.map_tokens [val[4]]

			cond_tag.add_child(val[0], cc)
			ex1_tag.add_child(val[2], cc)
			ex2_tag.add_child(val[4], cc)
			
			tag.out_type = ex1_tag.in_type
			
			tag.add_child(cond_tag, cc)
			tag.add_child(ex1_tag, cc)
			tag.add_child(ex2_tag, cc)

			if tag.out_type == :enum			
				#domain checking
				cc.eh.error MismatchingDomain.new(cc, val[3]) unless val[2].enum_domain == val[4].enum_domain
				tag.enum_domain = val[4].enum_domain
			end

			return tag
		end

    def output_symbol_assignment_early val
			symbol = cc.sc.from_code(val[-1], [DecimalOutputSymbol, BooleanOutputSymbol, EnumOutputSymbol])
			pc.enum_domains.push(symbol.enum_domain) if symbol.class == EnumOutputSymbol
    end
    
		def output_symbol_assignment val 

			symbol = cc.sc.from_code(val[0], [DecimalOutputSymbol, BooleanOutputSymbol, EnumOutputSymbol])
					
			case symbol #the tag that defined the symbol name
			when DecimalOutputSymbol:
				tag = SDOSTag.new
			when BooleanOutputSymbol:
				tag = SBOSTag.new
			when EnumOutputSymbol:
        pc.enum_domains.pop
			  #domain checking
			  cc.eh.error MismatchingDomain.new(cc, val[2]) unless val[3].out_type == :enum && symbol.enum_domain == val[3].enum_domain
			  tag = SEOSTag.new
			end
			
		  tag.attr[ATTR_REF] = symbol
		  tag.add_child(val[3], cc)
			return tag
		end

		def exp_number(val)
			tag = DVTag.new
			tag.map_tokens val
      
      tag.attr[ATTR_VALUE] = val[0].to_s
			
			return tag
		end
		
		def exp_boolean(val)
		  tag = BVTag.new
		  tag.map_tokens val
		  
		  tag.attr[ATTR_VALUE] = val[0].to_s
		  
		  return tag
	  end

		def decision_tree val
			tag = DTTag.new

			#if no decision tree is given, build one for decision-tree{tts(self);}
			if val[0].nil?
				tts_tag = TTSTag.new
				tts_tag.attr[ATTR_REF] = pc.state_symbol
				tag.add_child(tts_tag, cc)
											
			else
			  # starting with else
			  if (val.size == 6)
					cc.eh.error DecisionTreeMustNotStartWithElse.new(cc, val[0]) unless pc.has_common_decision_tree
					
					# description comment for first if can be before starting else
					# belatedly add description to correct tag
					ifTag = val[-2][0]
					unless val[2].nil? || ifTag.class != IfTag
					  raise InternalError.new unless ifTag.children[0].class == CondTag
            ifTag.children[0].attr[ATTR_DESCRIPTION] = val[2].to_s
          end
			  else
					cc.eh.error DecisionTreeMustStartWithElse.new(cc, val[0]) if pc.has_common_decision_tree
			  end
			  			  
				#if/else
				val[-2].each { |aTag|
					tag.add_child(aTag, cc)
  			}
			end
			
			return tag
		end
	
		def dtree val
			
			ifTag = IfTag.new
			tags = [ifTag]

			condTag = CondTag.new
			condTag.map_tokens val
      condTag.attr[ATTR_DESCRIPTION] = val[0].to_s # nil.to_s = "" --> always generate a description tag

			ifTag.add_child(condTag, cc)
			condTag.add_child(val[3],cc)

			#dtree or transition
			val[5].each { |aTag|
				ifTag.add_child(aTag, cc)
			}

			#else
			
			# description comment for first if after else can be before else
			# belatedly add description to correct tag
			ifTag = val[8][0]
			unless val[6].nil? || ifTag.class != IfTag
			  raise InternalError.new unless ifTag.children[0].class == CondTag
        ifTag.children[0].attr[ATTR_DESCRIPTION] = val[6].to_s
      end
      
		  elseTag = ElseTag.new
		  val[8].each { |aTag|
			  elseTag.add_child(aTag, cc)
		  }
		  tags << elseTag
			return tags
		end

		def common_decision_tree val
			tag = CDTTag.new
			tag.map_tokens val[1..1]
			pc.has_common_decision_tree = true
      
			#if/else
			val[1].each { |aTag|
				tag.add_child(aTag, cc)
			}
			
			return tag
		end
		
		def common_action val

			tag = CommonActionTag.new
			
			#SO / SBB / output symbol decls
			val[1].each { |sosTag|
				tag.add_child(sosTag, cc)
			}
					
			return tag
		end

		def cdtree val

			ifTag = IfTag.new
            ifTag.map_tokens val
			tags = [ifTag]

			condTag = CondTag.new
			condTag.map_tokens val
			condTag.attr[ATTR_DESCRIPTION] = val[0].to_s # nil.to_s = "" --> always generate a description tag

			ifTag.add_child(condTag, cc)
			condTag.add_child(val[3], cc)

			#cdtree or transition
			val[5].each { |aTag|
				ifTag.add_child(aTag, cc)
			}
			
			tags << val[6] unless val[6].nil?
			
			return tags
		end
		
		def cdtree_elseif_decision val
			tag = ElseTag.new
			tag.map_tokens val
			
			# description comment for first if after else can be before else
			# belatedly add description to correct tag
			ifTag = val[2][0]
			unless val[0].nil? || ifTag.class != IfTag
			  raise InternalError.new unless ifTag.children[0].class == CondTag
              ifTag.children[0].attr[ATTR_DESCRIPTION] = val[0].to_s
            end

			val[2].each {|aTag|
				tag.add_child(aTag, cc)
			}

			return tag
		end
				
		def transition val

			tag = TTSTag.new
			tag.map_tokens val
      
			case val[0].token_type
			when :TTS
				tag.attr[ATTR_REF] = cc.sc.from_code(val[1], StateSymbol)
			when :STAY #transition to current state symbol
				tag.attr[ATTR_REF] = pc.state_symbol			
			else
				raise InternalError.new
			end
		
			return tag
		end

		def exp_unary val
			
			tag = nil
			case val[0].to_s
			when '!'
				tag = NotTag.new
        tag.map_tokens val
			when '-' #translate -(...) --> 0-(...)

				#shortcut for -(5) --> -5
				if val[1].class <= DVTag
					tag = val[1]
          tag.map_tokens val
					tag.attr[ATTR_VALUE] = (-(tag.attr[ATTR_VALUE].to_f)).to_s
					return tag
				end

				dv_tag = DVTag.new
				dv_tag.attr[ATTR_VALUE] = '0'

				dv_tag.map_tokens val

				tag = MinusTag.new
        tag.map_tokens val
				tag.add_child(dv_tag, cc)
			else
				raise InternalError.new("unary handler missing for #{val[0].to_s}")
			end
			
      tag.add_child(val[1], cc)
			
			return tag
		end

    def exp_binary_early val
      if val[-1].to_s == '==' || val[-1].to_s == '!='
        if val[-2].out_type == :enum
          pc.enum_domains.push(val[-2].attr[ATTR_REF].enum_domain)
        end
      end
    end
    
		def exp_binary val
			tag = nil
			case val[1].to_s
			when '*'
				tag = MPLTag.new
			when '/'
				tag = DIVTag.new
			when '%'
				tag = MODTag.new
			when '+'
				tag = PlusTag.new
			when '-'
				tag = MinusTag.new
			when '>'
				tag = GTTag.new
			when '<'
				tag = LTTag.new
			when '<='
				tag = LTEQTag.new
			when '>='
				tag = GTEQTag.new
			when '==', '!='
        if val[0].out_type == :enum
          pc.enum_domains.pop
        end
				if (val[0].out_type == :enum && val[3].out_type == :enum)
					
					#domain check
				  cc.eh.error MismatchingDomain.new(cc, val[1]) unless val[0].enum_domain == val[3].enum_domain

					tag = EISCTag.new
					tag.map_tokens val

			    tag.add_child(val[0], cc)
			    tag.add_child(val[3], cc)
					
					if val[1].to_s == '!='
						not_tag = NotTag.new
						not_tag.map_tokens val
            not_tag.add_child(tag, cc)
						
						return not_tag
					
					end
					
					return tag
				end

				if val[1].to_s == '!='
					tag = NEQTag.new
				elsif val[1].to_s == '=='
					tag = EQTag.new
				end
								
			when '&&'
				tag = AndTag.new
			when '||'
				tag = OrTag.new
			else
				raise InternalError.new("binary missing for #{val[1].to_s}")
			end
      
			tag.map_tokens val
			unless tag.in_type == val[0].out_type && tag.in_type == val[3].out_type
				cc.eh.error TypeMismatch.new(cc, val[1])
			end

			# flatten trees on identical, associative tag classes
			# this is only allowed on "and" and "or" operators as they have a variable parameter count in intermediate code
			#if tag.associative
			case tag
			when OrTag, AndTag:
				[0,3].each {|i|
					child = val[i]
					if(tag.class == child.class)
						copy = Array.new(child.children)
						copy.each {|grandchild|
							child.remove_child grandchild
							tag.add_child(grandchild, cc)
						}
						
					else
						tag.add_child(child, cc)
					end
				}
			else
				tag.add_child(val[0], cc)
				tag.add_child(val[3], cc)
			end
					
			return tag
		end

		def exp_identifier val

      if pc.enum_domains.last.nil?
        symbol = cc.sc.from_code(val[0], [ConstantSymbol, OptionParameterSymbol, DecimalInputSymbol, BooleanInputSymbol, EnumInputSymbol, DecimalOutputSymbol, BooleanOutputSymbol, EnumOutputSymbol])
      else
			  symbol = cc.sc.from_code(val[0], [EnumSymbolValueSymbol], pc.enum_domains.last) do
			    cc.sc.from_code(val[0], [ConstantSymbol, OptionParameterSymbol, DecimalInputSymbol, BooleanInputSymbol, EnumInputSymbol, DecimalOutputSymbol, BooleanOutputSymbol, EnumOutputSymbol])
			  end
			end
			tag = nil
			
			case symbol #the tag that defined the symbol name
			when DecimalInputSymbol:
				tag = DISRTag.new
				tag.attr[ATTR_REF] = symbol
			when BooleanInputSymbol:
				tag = BISRTag.new
				tag.attr[ATTR_REF] = symbol
			when EnumInputSymbol:
				tag = EISRTag.new
				tag.attr[ATTR_REF] = symbol
				tag.enum_domain = symbol.enum_domain
			when DecimalOutputSymbol:
				tag = DOSRTag.new
				tag.attr[ATTR_REF] = symbol
			when BooleanOutputSymbol:
				tag = BOSRTag.new
				tag.attr[ATTR_REF] = symbol
			when EnumOutputSymbol:
				tag = EOSRTag.new
				tag.attr[ATTR_REF] = symbol
				tag.enum_domain = symbol.enum_domain
			when ConstantSymbol:
				tag = CRTag.new
				tag.attr[ATTR_REF] = symbol
			when OptionParameterSymbol:
				tag = OPRTag.new
				tag.attr[ATTR_REF] = symbol
				tag.out_type = symbol.type
				if symbol.type == :enum
					tag.enum_domain = symbol.enum_domain
				end
			when EnumSymbolValueSymbol:
				tag = EVTag.new
				tag.attr[ATTR_REF] = symbol
			  tag.enum_domain = symbol.enum_domain
			else
				raise InternalError.new
			end

      tag.map_tokens val

			return tag
		end

		def function_early val
			pc.function_dpcs.push(DomainParameterContext.new(cc.sc.from_code(val[-1], [DecimalInputSymbol, BooleanInputSymbol, EnumInputSymbol]), cc))
  	  pc.enum_domains.push(nil)
		end

		def function val
		  pc.enum_domains.pop
		  case pc.function_dpcs.last.domain_symbol
		  when DecimalInputSymbol:
			  tag = DISRTag.new
			when BooleanInputSymbol:
			  tag = BISRTag.new
			when EnumInputSymbol:
				tag = EISRTag.new
			  tag.enum_domain = pc.function_dpcs.last.domain_symbol.enum_domain
			else
			  raise InternalError.new
			end
			 
			tag.map_tokens val
			tag.attr[ATTR_REF] = pc.function_dpcs.last.domain_symbol
			
			#parameter
			val[3].each { |aTag|
				tag.add_child(aTag, cc)
			}
			fc = pc.function_dpcs.pop
			fc.check_used_value_symbols(val[0])
			
			return tag
		end

		def agent(val)
			assume(val, [:DOC_COMMENT, :AGENT, :IDENTIFIER, nil, :STRING, nil, :IDENTIFIER])

			agentTag = AgentTag.new
			agentTag.map_tokens val
      
			agentTag.attr[ATTR_DESCRIPTION] = val[0].to_s unless val[0].nil?
			agentTag.attr[ATTR_ID] = cc.sc.from_code(val[2], AgentSymbol)
			agentTag.attr[ATTR_TITLE] = val[4].to_s
			agentTag.attr[ATTR_ROOT_OPTION] = cc.sc.from_code(val[6], OptionSymbol)
			
			pc.agent_collection_root.child(AgentCollectionTag).add_child(agentTag, cc)
			
		end

	end
end
