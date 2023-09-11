MYFLEX=/Users/jmm01/myapps/brew/opt/flex/bin/flex
MYBISON=/Users/jmm01/myapps/brew/opt/bison/bin/bison

all: flex bison

flex:
	$(MYFLEX) mips_lexer.l

bison: 
	$(MYBISON) -d mips_parser.y -o mips_parser.c
	awk '!/mips_parser_parse/' mips_parser_tokens.h > tmpfile.h && mv tmpfile.h mips_parser_tokens.h

clean:
	rm mips_lexer.c mips_parser_tokens.h mips_parser.c