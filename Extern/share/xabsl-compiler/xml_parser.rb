module Translator
	
	# parser callback with a stack of open tags.
	class StackParserCallback

		include REXML::StreamListener

		attr_reader :le_classes_by_tag
		attr_reader :root
		attr_reader :symbol_table

		def initialize
			super
			@stack = nil
			@root = nil
			@symbol_table = nil
			@le_classes_by_tags = {}
		end

		# sets root, symbol_tables and clears parser stack.
		def prepare(root, symbol_table)
			@root = root
			@symbol_table = symbol_table
			@stack = [@root]
		end

		# registers an expected language element class for xml parsing.
		def add_le_class le_class
			@le_classes_by_tags[le_class.tag] = le_class
		end

		# when a new tag starts: creates a new language elmement with attributes and pushes it onto the stack
		def tag_start(name, attributes)
			
			@current = @stack.last

			le_class = @le_classes_by_tags[name]
			if le_class.nil?
				le = UnknownLanguageElement.new(name)
			else
				le = le_class.new
			end
			@stack.push le

			#then put it into the tree
			@current.add_child(le)

			#and after that import attributes
			le.import(attributes, @symbol_table)
		end

		# when tag ends: pops of a language element from the stack
		def tag_end(name)
			@current = @stack.pop
		end

		# when text is encountered: send text to language element.
		# used for platform, software environment, title tags
		def text(text)
			@stack.last.text = text unless @current.nil? || text =~ /\A\s*\z/		
		end
		
		# REXML::StreamListener provided a function stub with only 2 parameters which caused an error
		def entitydecl a,b,c
		end
	end

	# this parser callback cares for XML entities found and is used for agents.xml.
	class AgentCollectionParserCallback < StackParserCallback
		attr_reader :entities
	
		def prepare(root, symbol_table)
			super
			@entities = {}
		end
	
		def entitydecl(a, b, c)
			@entities[a] = c
		end
		
		#hack for REXML. in some versions entitydecl is delivered as notationdecl
		def notationdecl(n)
		    entitydecl(*n)
		end

	end


	# parser callback implementation for parsing option-definitions files
	# creates an mapping between ODTags and OptionSymbols
	class OptionDefinitonsParserCallback < StackParserCallback

		attr_reader :option_definitions_by_symbol

		def initialize
			super
			@option_definitions_by_symbol = {}
		end
		
		def tag_start(name, attributes)
			super

			last = @stack.last
			if (last.class <= ODTag)
				@option_definitions_by_symbol[last.attr[ATTR_NAME]] = last
			end
		end
	
	end

	# small parser for dtd files.	
	class DTDEntityScanner

		def scan(src)
			filename_by_entity = {}
			entity_by_filename = {}
			entity_order = []
			src.each do |line|
				if line =~ /<!ENTITY\s+([A-Za-z0-9_.\-]*)\s+SYSTEM\s+"([^"]*)">/
					filename = $2.to_s
					entity = $1.to_s
					filename_by_entity[entity] = filename
					entity_by_filename[filename] = entity
					entity_order << entity
				end
			end
			return [filename_by_entity , entity_by_filename, entity_order]
		end
	end
	
	# the counterpart to the dtd scanner.
	class DTDEntityWriter
		
		def initialize
			super
		end
		
		def write(entities, dest)
		
			dest.puts "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>"

			symbols = []
			behaviors = []
			strings = []
			
			keys = entities.keys
			keys.each do |key|
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
			
			
			symbols.each do |key|
				write_single(dest, key.identifier, entities[key]) 
			end
			behaviors.each do |key|
				write_single(dest, key.identifier, entities[key]) 
			end
			strings.each do |key|
				write_single(dest, key, entities[key]) 
			end
		end

		def write_single(dest, a , b)
			dest.puts "<!ENTITY #{a} SYSTEM \"#{b}\">"
		end		
	end
end
