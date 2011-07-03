require 'translator'

module Translator

	def Translator.stopwatch(message)
		LOG.info "#{message}" 
		start_time = Time.now
		yield
		elapsed_time = Time.now - start_time
		LOG.info "#{message} [#{elapsed_time}s]" 
		return elapsed_time
	end
	
	class Profiler
	
		attr_accessor :count
		attr_reader :elapsed_time
		
		def initialize(clazz, modulename, classname , methodname)
			@count = 0
			@elapsed_time = 0.0
			@name = "#{clazz.name}.#{methodname}(...)"
		
			clazz.class_eval "
				def #{classname}.profiler
					return @profiler
				end
				
				def #{classname}.profiler=(pr)
					@profiler = pr
				end		
				
				alias profiled_#{methodname} #{methodname}
				def #{methodname}(*args, &block)
					
					start_time = Time.now
					result = self.profiled_#{methodname}(*args, &block)
					#{classname}.profiler.add(Time.now - start_time)
					
					result
				end
			"
			
			clazz.profiler = self
		end

		def add(time)
			@elapsed_time += time
			@count += 1
		end

		def log
			LOG.info "#{@name} #{@count} times called, #{@elapsed_time}s in total"
		end
	end


	class Out
		
		attr_accessor :indent_string
		attr_accessor :verbose

		def initialize(out)
			@out = out
			@indent = 0
			@indent_string = "  "
			@verbose = false
		end

		#for printing indents
		def pindent(delta = 0)
			result = ''
			@indent += delta if delta < 0
			@indent.times {
				result << @indent_string
			}
			@indent += delta if delta > 0
			return result
		end

		def <<(something)
			@out << something
			return self
		end

		#suggested line break
		#vaporware
		def |(something)
			self << ENDL << something
		end

		#print with indent
		def %(something)
			self << pindent << something
		end
		
		#print and increase indent
		def >(something)
			self << pindent(1) << something
		end

		#print and decrease indent
		def <(something)
			self << pindent(-1) << something
		end

	end
	
	class RecodeOut < Out
		def <<(something)
			#recode utf8 strings to native charset
			@out << something.unpack("U*").pack("C*")
			return self
		end
	end
end

class << File

	def relative_path(path, reference_path)

		path = File.expand_path(path).sub(/^([A-Za-z]):/, '/cygdrive/\1').split(/[\\\/]+/)
		reference_path = File.expand_path(reference_path).sub(/^([A-Za-z]):/, '/cygdrive/\1').split(/[\\\/]+/)		
		
		equal_index = 0
		while(path[equal_index] == reference_path[equal_index])
			equal_index += 1
		end

		relative_path = []
		(reference_path.size - equal_index).times do
			relative_path << ".."
		end

		((equal_index)..(path.size-1)).each do |index|
			relative_path << path[index]
		end
		return File.join(*relative_path)
	end


end