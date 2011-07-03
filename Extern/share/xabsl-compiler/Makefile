SOURCES=constants.rb \
errors.rb \
language_elements.rb \
symbol_table.rb \
symbols.rb \
syntax_tree_builder.rb \
tokenizer.rb \
translator.rb \
translator_base.rb \
xml_parser.rb \
xabsl.rb \
vcproj.rb

.PHONY: parser clean

all: parser timestamp

timestamp: $(SOURCES) Makefile
	@touch timestamp

parser: xabsl_parser.tab.rb

xabsl_parser.tab.rb: xabsl_parser.y.rb
	@ruby racc/racc -v xabsl_parser.y.rb

clean:
	@rm -f xabsl_parser.output xabsl_parser.tab.rb
	@rm -f timestamp
