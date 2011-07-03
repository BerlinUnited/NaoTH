#!/usr/bin/ruby

#make the script run from any location and workaround for cygwin ruby and windows style dirs
$LOAD_PATH << File.expand_path(File.dirname(__FILE__))
$LOAD_PATH << File.expand_path(File.dirname(__FILE__)).sub(/^([A-Za-z]):/, '/cygdrive/\1')

require 'logger'
require 'optparse'
require 'translator'
require 'xabsl_parser.tab'
require 'fileutils'
require 'vcproj'
require 'ftools'

module Translator

	class XabslCompiler
		
	  def initialize
			super
			
			@fp_stb = FirstPassSyntaxTreeBuilder.new
			@sp_stb = SecondPassSyntaxTreeBuilder.new
			@tp_stb = ThirdPassSyntaxTreeBuilder.new
			@parser = XabslParser.new
			@errors = false
  	end
    
    def prepare_cc(cc)
			
			cc.pc.option_root = RootLanguageElement.new
			cc.pc.option_root.doctype = "<!DOCTYPE #{DTD_INCLUDE_ENTITY} SYSTEM \"../#{DTD_INCLUDE_FILE}\">"
			cc.pc.namespace_root = RootLanguageElement.new

			cc.pc.option_definitions_temp = RootLanguageElement.new
			
			ac_tag = AgentCollectionTag.new
			ac_tag.attr[ATTR_XMLNS] = "http://www.xabsl.de"
			ac_tag.attr[ATTR_XMLNS_XSI] = "http://www.w3.org/2001/XMLSchema-instance"
			ac_tag.attr[ATTR_XMLNS_XI] = "http://www.w3.org/2003/XInclude"
			ac_tag.add_child(TitleTag.new)
			ac_tag.add_child(PlatformTag.new)
			ac_tag.add_child(SoftwareEnvironmentTag.new)
			
			cc.pc.agent_collection_root = RootLanguageElement.new
			cc.pc.agent_collection_root.doctype = "<!DOCTYPE options [" << ENDL << "    <!ENTITY #{OPTION_DEFINITIONS_ENTITY} SYSTEM \"#{OPTION_DEFINITIONS_FILE}\">" << ENDL << "]>"
			cc.pc.agent_collection_root.add_child(ac_tag)
		end
    
    def pass(cc, stb)
			begin
				LOG.info "#{stb.class} #{cc.cu.src_filename}"
				@parser.parse(cc, stb)
        
			  rescue InternalError => error
				  #if there are errors, the internal error is most likely caused by them
				  unless errors?
            $stderr << cc.eh.visual_studio_format(error)
          
            $stderr.print(error.class.name + ENDL)				
					  error.backtrace.each do |trace|
						  $stderr.print trace
						  $stderr.print ENDL
					  end
					  exit 1
				  end
			  rescue CompilerFatal => message
			    $stderr << cc.eh.visual_studio_format(message)
			    exit 100
			  rescue CompilerMessage => message
				  cc.eh.error message
			end
    end


    def write_xabsl(output_path, cc)
    		
			filename = File.join(output_path, cc.cu.dest_fullname)
			
			if File.file?(filename) && File.stat(filename).mtime > cc.cu.src_mtime
				return 0
			end

   		#generate XML output
			LOG.info "writing #{filename}"
		
			FileUtils.mkdir_p(File.dirname(filename))
			File.open(filename, 'wb') do |dest|
				root = nil
				if cc.pc.option_declared
          root = cc.pc.option_root
				elsif cc.pc.namespace_declared
          root = cc.pc.namespace_root
				elsif cc.pc.agent_declared
          root = cc.pc.agent_collection_root
				else
          raise InternalError.new
        end				        
				
				root.to_xml Out.new(dest)
			end
			
			return 1
    end

		def write_intermediate_code(ccs, agent_cc, filename)
			FileUtils.mkdir_p(File.dirname(filename))
			LOG.info "writing #{filename}"

			option_symbols_order = []
			
			#gather option symbols from compilation contexts
			#and remember order
			ccs.each do |cc|
			
				next unless cc.pc.option_declared
				option_symbol = cc.pc.option_root.child(OptionTag).attr[ATTR_NAME]
				option_symbols_order << [cc, option_symbol]
			end
			
			option_symbols_order.sort! do |a,b|
				a[1].identifier.downcase <=> b[1].identifier.downcase
			end
			
			File.open(filename, 'wb') do |file|
				out = Out.new(file)
				title = agent_cc.pc.agent_collection_root.child(AgentCollectionTag).child(TitleTag).string
				
				out << "// Intermediate code for the agents of #{title}" << ENDL
				out << ENDL
				
				#process internal symbols
				internal_output_symbols = []
				internal_enum_symbols = []
				agent_cc.st.each do |symbol|
					case symbol
					when BooleanOutputSymbol, DecimalOutputSymbol, EnumOutputSymbol:
						if symbol.internal
							internal_output_symbols << symbol 
						end
					when EnumSymbol:
					  if symbol.internal
						  internal_enum_symbols << symbol
					  end
					end
				end
				out << "// number of internal enumerations:" << ENDL if out.verbose
				out << "#{internal_enum_symbols.size}" << ENDL
				out << "// (enumeration number-of-elements (element)*)+" << ENDL if out.verbose
				internal_enum_symbols.sort! do |a,b|
					a.identifier.downcase <=> b.identifier.downcase
				end
				internal_enum_symbols.each do |symbol|
  			  out << "#{symbol.identifier}"
					out << " #{symbol.value_symbols.size}"
					symbol.value_symbols.each do |symbol|
					  out << " #{symbol.identifier}"
					end
					out << ENDL
				end
				out << "// number of internal output symbols" << ENDL if out.verbose
				out << "#{internal_output_symbols.size}" << ENDL
				out << "// (d decimal-input-symbol | b boolean-input-symbol | e enum enumerated-input-symbol)+" << ENDL if out.verbose
				internal_output_symbols.sort! do |a,b|
					a.identifier.downcase <=> b.identifier.downcase
				end
				internal_output_symbols.each do |symbol|
					case symbol
					when BooleanOutputSymbol:
						out << "b "
					when DecimalOutputSymbol:
						out << "d "
					when EnumOutputSymbol:
						out << "e #{symbol.enum_domain.identifier} "
					else
						raise InternalError.new
					end
					out << "#{symbol.identifier}"
					out << ENDL
				end
				out << ENDL

				#process options
				out << '// number of options' << ENDL if out.verbose
				out << "#{option_symbols_order.size}" << ENDL
				out << '// names, number of parameters, parameters for all options' << ENDL if out.verbose
				
				option_symbols_order.each do |cc, option_symbol|
					out << "#{option_symbol.identifier} #{option_symbol.value_symbols.size}"
					option_symbol.value_symbols.each do |parameter_symbol|
					  case parameter_symbol.type
					  when :decimal:
						  out << " d"
					  when :boolean:
						  out << " b"
					  when :enum:
						  out << " e #{parameter_symbol.enum_domain.identifier}"
					  else
						  raise InternalError.new
					  end
						out << " #{parameter_symbol.identifier}"	
					end
					out << ENDL
				end
				out << ENDL
								
				option_symbols_order.each do |cc, option_symbol|
					cc.pc.option_root.child(OptionTag).to_ic(out)
				end
				
				#process agents
				agent_tags = Array.new(agent_cc.pc.agent_collection_root.child(AgentCollectionTag).children)
				agent_tags.delete_if do |tag|
					tag.class != AgentTag
				end
				out << "// number of agents:" << ENDL if out.verbose
				out << "#{agent_tags.size}" << ENDL
				out << "// agents: (name initial-option)+" << ENDL if out.verbose
				first = true
				agent_tags.each do |tag|
					out << " " unless first
					out << "#{tag.attr[ATTR_ID].identifier} #{tag.attr[ATTR_ROOT_OPTION].identifier}"
					first = false
				end
				out << ENDL
			end
		end

    def gather_incremental(st, all_ccs)
			changed_ccs = []
			all_ccs.each do |cc|
				
				unless FileTest.readable?(cc.cu.src_fullname)
                                        LOG.info "file not readable #{cc.cu.src_filename}, switching to non-incremental mode"
                                        throw :non_incremental 
                                end
        
                                if cc.cu.src_mtime != File.stat(cc.cu.src_fullname).mtime
					changed_ccs << cc
					LOG.info "#{cc.cu.src_filename} modified"
				end
			end
			if changed_ccs.empty?
				LOG.info "nothing to do -- should proceed to output"
			end
			
			#now gather symbols defined in changed_ccs
			symbols = []
			changed_ccs.each do |cc|
				symbols += cc.sc.from_code_declaration_count.keys
			end
		
			symbols.uniq!
			symbols.each do |symbol|
				LOG.info "uses symbol #{symbol.identifier}"
			end
		
			#now gather ccs again that use these symbols
			all_ccs.each do |cc|
				symbols.each do |symbol|
					if cc.sc.from_code_usage_count[symbol] > 0
						changed_ccs << cc
						break
					end
				end
			end
			changed_ccs.uniq!
			
			changed_ccs.each do |cc|
				cc.sc.from_code_declaration_count.keys.each do |symbol|
					symbols << symbol
				end
				LOG.info "used by #{cc.cu.src_filename}"

				if cc.pc.includes.size > 0
					LOG.info "includes detected: switching to non-incremental mode"
					throw :non_incremental 
				end
			end

			symbols.uniq!
			symbols.each do |symbol|
				LOG.info "undeclaring symbol #{symbol.identifier}"
				st.remove symbol
			end

			#generate new CompilerContexts for files that need recompiling
			all_ccs = all_ccs - changed_ccs
			ccs = []
			changed_ccs.each do |cc|
				ccs << CompilerContext.new(cc.cu, st)
				cc.cu.update_src_mtime
			end
			all_ccs += ccs
			
			return [all_ccs, ccs]
		end

		def errors?
			@error_ccs.each do |cc|
				return true if cc.errors?
			end
			return false
		end
		
		def translate(hash_value, compilation_units, write)
			@error_ccs = []
	        
	    dump_filename = File.join(write[:inc], "xabsl.#{hash_value}.dump") if write[:inc]

			agent_cc = nil
			message_count = Hash.new(0)
		  st = nil
		  marshalled_dump = nil
			options_tag = OptionsTag.new
		  option_defintions_tag = ODSTag.new
	
			ods_tag = ODSTag.new
			ods_tag.attr[ATTR_XMLNS] = "http://www.xabsl.de"
			ods_tag.attr[ATTR_XMLNS_XSI] = "http://www.w3.org/2001/XMLSchema-instance"
            
			option_definitions_root = RootLanguageElement.new
			option_definitions_root.add_child ods_tag
		    
		    					
			build_ccs = nil
			all_ccs = nil
			incremental = false
      files_written = 0
    
    	if compilation_units.size > 1
    	  $stderr.puts "please give only one filename on command line"
    		exit(1)
    	end
    	main_cu = compilation_units[0]
    
			elapsed_time = Translator.stopwatch("whole compilation run") do
    
    	  catch :non_incremental do
    	    if write[:inc]
    		    loaded = nil
      					
    			  unless FileTest.exists?(dump_filename)
    				  LOG.info "no dump file: switching to non-incremental mode"
    			    throw :non_incremental 
    		    end
      					
   				  idle_run = true
    			  File.open(dump_filename, 'rb') do |file|
    				  precheck_cus = nil
    				  begin
    				    precheck_cus = Marshal.load(file)
    				  rescue EOFError
    				    throw :non_incremental
    				  end
    				  begin
	    				  precheck_cus.each do |cu|
	    				    #here only input files are checked as (shared) output files can be generated more than once
	    				    idle_run = idle_run && (cu.src_mtime == File.stat(cu.src_fullname).mtime)
	    			    end
	    		    rescue Errno::ENOENT
	    			    idle_run = false
	    		    end
     						
  					  loaded = Marshal.load(file)
  				  end
      					
	  			  #just to be sure
		  		  if loaded[0] != hash_value
			  		  LOG.info "mismatching hash_value: switching to non-incremental mode"
				  	  throw :non_incremental 
				    end
      					
				    hash_value, st, ccs = loaded
  			    #absolutely nothing changed
				    if idle_run
					    LOG.info "ultimate shortcut. none of the input files has changed."
					    (1..rand(4)+1).each do 
							  $stdout.print "Z"	
						    (2..rand(5)+2).each do
								  $stdout.print "z"
						    end
						    $stdout.print " "
					    end
					    $stdout.puts
					    all_ccs = ccs
					    build_ccs = []
					  else
   			    	all_ccs, build_ccs = gather_incremental(st, ccs)
				    end
				    incremental = true
			    end
		    end

			  #stuff for non-incremental use
			  unless incremental

          # create symbol table
          st = SymbolTable.new
    					
			    #create compiler_contexts
			    build_ccs = []
			    all_ccs = []
			    compilation_units.each do |cu|
				    build_ccs << CompilerContext.new(cu, st)
			    end
        end
  	
        @error_ccs = Array.new(build_ccs + all_ccs).uniq
    			
				#prepare and first pass
				processed_ccs = {}
				included_ccs = {}
				double_include = false
				while !build_ccs.empty?
					
					cc = build_ccs.shift
					processed_ccs[cc.cu.src_fullname] = cc
					
					prepare_cc(cc)
					next if cc.errors?
					pass(cc, @fp_stb)
					
				        
					#keep in mind: if there are includes in the file, incremental build is disabled
					processed_filenames = {}
					cc.pc.includes.each do |filename|
						src_fullname = File.join(File.dirname(cc.cu.src_fullname), filename)
						dest_fullname = File.join(File.dirname(cc.cu.dest_fullname), filename)
						matched = 0
						cc.pc.includes.each do |filename2|
						   if  filename == filename2
						      matched += 1
						   end
					     if matched > 1 && processed_filenames[filename].nil?
						      processed_filenames[filename] = filename2
						      cc.eh.error DoubleIncludeError.new(cc, src_fullname)
						      break
					     end
						end
						
				    unless File.basename(src_fullname) =~ /^(.*)\.[xy]absl$/
		          cc.eh.error FileTypeError.new(cc, src_fullname)
		          next
	          end		
            if processed_ccs[src_fullname].nil? && included_ccs[src_fullname].nil?
							if FileTest.file?(src_fullname)
								cu_new = CompilationUnit.new
								cu_new.set_src(File.dirname(src_fullname), File.basename(src_fullname))

								# set default destination, for any file containing valid information this will be overwritten by SyntaxTreeBuilder
                cu_new.set_dest(".")							               
                                                                
								cc_new = CompilerContext.new(cu_new, st)
								included_ccs[src_fullname] = cc_new
								build_ccs << cc_new
								@error_ccs = Array.new(build_ccs + all_ccs).uniq
							else
								cc.eh.error FileOpenError.new(cc, src_fullname)
							end	
						end
					end
				end
				
				build_ccs = processed_ccs.values
				all_ccs = (all_ccs + build_ccs).uniq
				@error_ccs = Array.new(build_ccs + all_ccs).uniq

				#dump .vcproj on command
				if write[:vcproj]
					VCProj.generate_vcproj(write[:vcproj], all_ccs)
 					files_written+=1
				end
			
				
				#second pass
				build_ccs.each do |cc|
					#unless cc.errors?
					if (not cc.errors?) || write[:force]
					  pass(cc, @sp_stb)
          end
        end
        
				#third pass
				build_ccs.each do |cc|
					#unless cc.errors?
					if (not cc.errors?) || write[:force]
					  pass(cc, @tp_stb)
          end
        end

        #generate messages for unused symbols
			  st.each do |symbol|
			    unless (symbol.class <= AgentSymbol || symbol.class <= NamespaceSymbol)
				    if (symbol.usage_count <= 1)
				      message = "#{symbol.class.name} '#{symbol.identifier}' "
				      message += "in #{symbol.domain_symbol.class.name} '#{symbol.domain_symbol.identifier}' " if symbol.class <= DomainValueSymbol
				      message += "(#{symbol.namespace_symbol.identifier}) " if symbol.namespace_symbol.class <= NamespaceSymbol
				      message += "is defined but never used"
				      LOG.info message
				    end
				  end
        end
        
        #postprocessing
				build_ccs.each do |cc|
          unless errors?
            #postprocessing: generate entities for each fresh compiled option
            if cc.pc.option_declared
              cc.pc.option_root.child(OptionTag).entities += cc.sc.used_namespace_symbols
            end
          end

					#misc: count errors and warnings
					cc.eh.messages.each do |message|
						message_count[CompilerError] +=1 if message.class <= CompilerError
						message_count[CompilerWarning] +=1 if message.class <= CompilerWarning
					end
        end

				#print and count errors from symbol table
				st.errors.each do |message|
					
					if write[:vs]
						$stderr.puts cc.eh.visual_studio_format(message)
					else
						$stderr.puts cc.eh.format(message)
					end
		
					message_count[CompilerError] +=1 if message.class <= CompilerError
					message_count[CompilerWarning] +=1 if message.class <= CompilerWarning
				end

				# marshal everything if there were no errors
				# the trick to do this *here* is: we are going to modify the syntax trees below for output generation
				# but we want unmodified trees for incremental compiling
				if write[:inc] && message_count[CompilerError] == 0
					marshalled_dump = Marshal.dump([hash_value, st, all_ccs])
				end
				                
        #prepare options tag for agents tag, find agents_cc
			  file_entities = {}
			  
				unless errors?
	        all_ccs.each do |cc|
	          next if cc.errors?
  	                    
	          root = nil
	          root_counter = 0
	          if cc.pc.option_declared
	            root_counter += 1
	            root = cc.pc.option_root
  	
	            #add xincludes to options files
	      		  xinclude_tag = XIncludeTag.new
	      		  xinclude_tag.attr[ATTR_HREF] = File.relative_path(cc.cu.dest_fullname, main_cu.dest_dir)
	       		  options_tag.add_child(xinclude_tag)
							  	        	    		
	       		  raise InternalError.new if cc.pc.option_definitions_temp.children.size != 1
	       		  odTag = cc.pc.option_definitions_temp.child(ODTag)
	       		  cc.pc.option_definitions_temp.remove_child odTag
	       		  ods_tag.add_child(odTag)
	       		  
							option_symbol = cc.pc.option_root.child(OptionTag).attr[ATTR_NAME]
	        	  file_entities[option_symbol] = File.relative_path(cc.cu.dest_fullname, main_cu.dest_dir)
	          end
	      	  if cc.pc.namespace_declared
	            root_counter += 1
	            root = cc.pc.namespace_root
  	
	        	  #get symbols oder basic-behaviors tag and the id of this tag
	        	  symbol = root.children[0].attr[ATTR_ID]
  	        				
	        	  file_entities[symbol] = File.relative_path(cc.cu.dest_fullname, main_cu.dest_dir)
	          end
	       	  if cc.pc.agent_declared
	            root_counter += 1
	            root = cc.pc.agent_collection_root
  	        				
	            #identified agent cc
	            raise InternalError.new('multiple agent files encountered. i cannot handle this.') unless agent_cc.nil?    
	        	  agent_cc = cc
  	        				
  	        				 
	        	  ac_tag = cc.pc.agent_collection_root.child(AgentCollectionTag)
	        	  ac_tag.child(TitleTag).string = ac_tag.title
	        	  ac_tag.child(PlatformTag).string = ac_tag.platform
	        	  ac_tag.child(SoftwareEnvironmentTag).string = ac_tag.software_environment
			        ac_tag.add_child(options_tag)
     			  end
  	
     			  #check that other roots are really empty and the roots contain exactly 1 le
     			  raise InternalError.new if root_counter != 1
     			  raise InternalError.new if root.children.size != 1
          end
          
	     	  raise InternalError.new('agent file is not on command line.') if agent_cc.nil?
	        #ab hier ist agents_cc != nil
	        #main__cu sollte das gleiche sein
  		  end				
  				
			  all_ccs.each do |cc|
					#print errors and warnings
					cc.eh.messages.each_index do |index|
						#break if index >= 1
						message = cc.eh.messages[index]
						if write[:vs]
							$stderr.puts cc.eh.visual_studio_format(message)
						else
							$stderr.puts cc.eh.format(message)
						end
					end
				end
				
  			if write[:xabsl]
				  all_ccs.each do |cc|
	  									
					  #generate a relative filename
					  dtd_relative_filename = File.relative_path(DTD_INCLUDE_FILE, cc.cu.dest_dir)
						
						#set doctype for options (we don't care whether it's actually an option file)
						cc.pc.option_root.doctype = "<!DOCTYPE #{DTD_INCLUDE_ENTITY} SYSTEM \"#{dtd_relative_filename}\">"
					
		        #only produce output if there were no errors
		        if (write[:force] ||(!agent_cc.nil? && message_count[CompilerError] == 0))
		         	if cc == agent_cc
								#sort agent option by filename
								options_tag.children.sort! do |a,b|
									a.attr[ATTR_HREF] =~ /^(.*)\.xml$/ 
									x = $1.to_s
									b.attr[ATTR_HREF] =~ /^(.*)\.xml$/
									x <=> $1.to_s
								end
	           	end
		                
	   					files_written += write_xabsl(write[:xabsl], cc)
	          end
					end
				end
				
				if message_count[CompilerError] == 0
					
					if write[:xabsl]
	
	
						#generate option-defintions-file
						option_definitions_root.child(ODSTag).children.sort! do |a,b|
							a.attr[ATTR_NAME] <=> b.attr[ATTR_NAME]
						end
						
						filename = File.join(write[:xabsl], OPTION_DEFINITIONS_FILE)
						FileUtils.mkdir_p(File.dirname(filename))
						LOG.info "writing #{filename}"
						File.open(filename+".new", 'wb') do |dest|
							option_definitions_root.to_xml Out.new(dest)
						end
						if File.file?(filename) && File.cmp(filename+".new",filename)
							File.delete(filename+".new")
						else
							File.delete(filename) if File.file?(filename)
							File.rename(filename+".new",filename)
							files_written+=1
						end
			
						#generate dtd-file
						file_entities[OPTION_DEFINITIONS_ENTITY] = OPTION_DEFINITIONS_FILE
						filename = File.join(write[:xabsl], DTD_INCLUDE_FILE)
						FileUtils.mkdir_p(File.dirname(filename))
						
						LOG.info "writing #{filename}"
						File.open(filename+".new", 'wb') do |dest|
							DTDEntityWriter.new.write(file_entities, dest)
						end
						if File.file?(filename) && File.cmp(filename+".new",filename)
							File.delete(filename+".new")
						else
							File.delete(filename) if File.file?(filename)
							File.rename(filename+".new",filename)
							files_written+=1
						end
					end
		
	        if write[:ic]
	    		  raise InternalError.new if agent_cc.nil?
					  write_intermediate_code(all_ccs, agent_cc, write[:ic]) if write[:ic]
						files_written+=1
	        end
	                
	        if write[:api]
						filename = write[:api]
						keywords = []
						if FileTest.exists?(filename)
							File.open(filename, 'rb') do |file|
								file.each do |line|
									keywords << line.chomp unless line =~ /^\s*$/ #keine leerzeilen
								end
							end
						end
						st.each do |symbol|
							next if [AgentSymbol, NamespaceSymbol].include?(symbol.class)
							
							if [DecimalInputSymbol, BooleanInputSymbol, EnumInputSymbol, OptionSymbol, BasicBehaviorSymbol].include?(symbol.class)
								keyword = "#{SymbolContext.to_code(symbol, false)}("
								first = true
								symbol.value_symbols.each do |value_symbol|
									keyword += ", " unless first
									keyword += "#{SymbolContext.to_code(value_symbol, false)} = "
									first = false
								end
								
								keyword += ")"
								keywords << keyword
							else
								keywords << SymbolContext.to_code(symbol, false)
							end
						end
						
						Tokenizer.keywords.each do |key, value|
							keywords << key
						end

						keywords.uniq!
						keywords.sort! do |x,y|
							x.downcase <=> y.downcase
						end
						
						LOG.info "writing #{filename}"
						FileUtils.mkdir_p(File.dirname(filename))
						File.open(filename, 'wb') do |file|
							keywords.each do |keyword|
								file.puts keyword
							end
						end
						
						files_written+=1
	        end
	                
	        #dump updated cus here
					if write[:inc]
						FileUtils.mkdir_p(File.dirname(dump_filename))
						File.open(dump_filename, 'wb') do |file|
							dump_cus = []
							all_ccs.each do |cc|
								dump_cus << cc.cu	
							end
							file.write Marshal.dump(dump_cus)
							file.write marshalled_dump
						end
					end
        end		
			end
			$stdout.puts "#{build_ccs.size} compiled, #{files_written} written, #{message_count[CompilerError]} error(s), #{message_count[CompilerWarning]} warning(s), elapsed time #{elapsed_time}s"
      $stdout.puts
		end
	end


##################################################

  #profiler = Translator::Profiler.new(Translator::SymbolContext, "Translator", "SymbolContext", "from_code")


  LOG = Logger.new(STDOUT)
  LOG.datetime_format = ""
  LOG.level = Logger::WARN

  help = false
  output_path = nil
  write = Hash.new(false)
  opts = OptionParser.new do |opts|

	opts.banner = "XABSL compiler" << ENDL << ENDL
	opts.banner += "usage: #{opts.program_name} [options] file(s)"
	opts.separator ""
	opts.separator "options:"
	
	
	opts.on("-q", "--quiet", "suppresses almost any output") do
		LOG.level = Logger::FATAL
	end
	
	opts.on("-v", "--verbose", "run verbosely") do
		case LOG.level
		when Logger::INFO
			LOG.level = Logger::DEBUG
		when Logger::WARN
			LOG.level = Logger::INFO
		else
			LOG.level = Logger::WARN
		end
	end

	opts.on("-x", "--xml path", "generate xml at <path>") do |path|
		write[:xabsl] = File.expand_path(path)
	end

	opts.on("-i", "--intermediate-code filename", "generate intermediate code") do |filename|
		write[:ic] = filename
	end

	opts.on("-a", "--api-file filename", "create .api file and copy it to scite/VisualStudio locations") do |filename|
		write[:api] = filename
	end

	opts.on("-m", "--vcproj filename", "create .vcproj middle file") do |filename|
		write[:vcproj] = filename
	end

	opts.on("-I", "--incremental directory", "incremental compiling, put dumpfiles to given directory") do |directory|
		write[:inc] = directory
	end

	opts.on("-C", "--comments", "insert additional comments into generated files") do
		write[:comments] = true
	end

	opts.on("-V", "--vs-error-message-style", "produce visual studio compliant error messages") do
		write[:vs] = true
	end
	opts.on("-f", "--force-output", "will force output even if errors occured (useful when working with single files") do
		write[:force] = true
		LOG.warn "using option 'force output' - please notice that some displayed errors may be important some not"
	end
		
	opts.on_tail("-h", "--help", "show this message") do
		help = true
	end

end

#print help if requested or no filenames are supplied
filenames = opts.parse!
if help || filenames.empty?
	puts opts
	exit
end

#check file existence and readablity
errors = false
compilation_units = []
filenames.each do |src_filename|

	unless src_filename =~ /^(.*)\.[xy]absl$/
		LOG.error "file '#{src_filename}' doesn't have extension .xabsl"
		errors = true
		next
	end
	dest_filename = "#{$1.to_s}.xml"

 	unless FileTest.file?(src_filename) && FileTest.readable?(src_filename)
		LOG.error "unable to read from file '#{src_filename}'"
		errors = true
		next
	end

	cu = Translator::CompilationUnit.new
	cu.set_src(File.dirname(src_filename), File.basename(src_filename))
	cu.set_dest(".")
	compilation_units << cu

end
exit 1 if errors

#hmmm put this elsewhere
unless write.empty?

	timestamp_mtime = File.stat(TIMESTAMP_FILE).mtime

	Dir.foreach('.') do |entry|
		if entry =~ /xabsl\.[0-9A-Fa-f]+\.dump/ && FileTest.file?(entry) && File.stat(entry).mtime <= timestamp_mtime
			LOG.info "removing #{entry} because compiler timestamp is newer"
			FileUtils.rm(entry)			
		end
	end

  hash_value = "%8.8X" % [filenames.join.hash]
  translator = Translator::XabslCompiler.new
	translator.translate(hash_value, compilation_units, write)
end


#profiler.log
LOG.close

exit 1 if translator.errors?

end
