

module Translator

	class Node
	
		attr_accessor :name
		attr_reader :children
		attr_accessor :type
		attr_accessor :relative_name
	
		def initialize
			@children = []
			@type = nil
		end
	
		def get_or_create(child_name)
			@children.each do |child|
				return child if child.name == child_name
			end
			
			new_child = Node.new
			new_child.name = child_name
			@children << new_child
			
			return new_child
		end
		
		def sort!
			@children.sort! do |a, b|
				result = nil
				result = 1 if a.type == :file && b.type == :folder
				result = -1 if a.type == :folder && b.type == :file
				if a.type == :file && b.type == :file
					
					result = File.basename(a.relative_name).downcase <=> File.basename(b.relative_name).downcase
				end
				result = a.name.downcase <=> b.name.downcase if a.type == :folder && b.type == :folder
				result
			end
			@children.each do |child|
				child.sort!
			end
		end
	
		def print(out)
			
			case type
			when :file
				out << "<File RelativePath=\"#{relative_name.gsub(/\//,'\\')}\">\r\n"
				out << "</File>\r\n"
			
			when :folder
				
				unless name.nil?
					out << "<Filter\r\n"
					out << "Name=\"#{name}\"\r\n"
					out << "Filter=\"\">\r\n"
				end
				children.each do |child|
					child.print(out)
				end
				out << "</Filter>\r\n" unless name.nil?

			end
		end
		
		
	end


	class VCProj
		
		def VCProj.readMiddle(filename,root)
		   File.open(filename, 'ab')
		   input = IO.readlines(filename)
		   input.each do |line|                 
		      if (line =~ /RelativePath/)
		         line = line.gsub('\\','/')
		         path =  line[line.index('/Src/')..line.rindex('l"')]
			 relative_name = path.sub(/^.*\/Src\//, '../Src/')
					folder_names = path.sub(/^.*\/Modules\//, '').split('/')
			 current = root
		         while folder_names.size > 0
			    folder_name = folder_names.shift
			    current = current.get_or_create(folder_name)
			    if folder_names.size == 0 #was last
			      current.type = :file
			      current.relative_name = relative_name
			    else
			      current.type = :folder
			    end
		         end
		      end 
		   end
		end
		
		def VCProj.generate_vcproj(filename, ccs)
		        
			FileUtils.mkdir_p(File.dirname(filename))
			root = Node.new
			root.type = :folder
			VCProj.readMiddle(filename,root)
			
			ccs.each do |cc|
				path = File.expand_path(cc.cu.src_fullname)
				if(path =~ /^.*\/Src\// && path =~ /^.*\/Modules\//)
					relative_name = path.sub(/^.*\/Src\//, '../Src/')
					folder_names = path.sub(/^.*\/Modules\//, '').split('/')

					current = root
					while folder_names.size > 0
						folder_name = folder_names.shift
						current = current.get_or_create(folder_name)
						if folder_names.size == 0 #was last
							current.type = :file
							current.relative_name = relative_name
						else
							current.type = :folder
						end
					end
				end
			end			
			
			root.sort!
			FileUtils.mkdir_p(File.dirname(filename))
			File.open(filename, "w") do |file|
				root.print(file)
			end
 
		end
	end
end