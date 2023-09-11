CC = gcc -Werror
INSTALL_PATH=./bin
OBJS = data_memory.o instr_memory.o \
	instructions.o mips_lexer.o mips_parser.o mips_pipeline.o pipeline.o \
	register_file.o symbol_table.o update.o utils.o

TESTS = test_basic_program test_jump_instruction test_forwarding_simple test_forwarding_alternating test_forwarding_loads test_branch_false test_branch_true

all: mips_pipeline

test: mips_pipeline
	@for p in $(TESTS); do \
		echo "Running and comparing $$p to trace:"; \
		$(INSTALL_PATH)/mips_pipeline ../Tests/$$p.s; \
		diff --ignore-space-change --brief ../Tests/$$p.html ../Traces/$$p.html; \
		echo "\n"; \
	done
	@rm -f ../Tests/*.html

clean:
	rm ./*.o
	rm -f ../Tests/*.html
	rm bin/*

mips_pipeline: $(OBJS)
	@mkdir -p ./bin
	$(CC) $^ -o ./bin/$@

data_memory.o: data_memory.c data_memory.h error_handling.h utils.h
	$(CC) -c $<

instr_memory.o: instr_memory.c instr_memory.h instructions.h \
  error_handling.h symbol_table.h utils.h
	$(CC) -c $<

instructions.o: instructions.c instructions.h register_file.h utils.h
	$(CC) -c $<

mips_lexer.o: mips_lexer.c instructions.h register_file.h \
  mips_parser_tokens.h
	$(CC) -c $<

mips_parser.o: mips_parser.c instructions.h data_memory.h instr_memory.h \
  symbol_table.h mips_parser.h mips_parser_tokens.h error_handling.h \
  utils.h
	$(CC) -c $<

mips_pipeline.o: mips_pipeline.c pipeline.h instr_memory.h instructions.h \
  data_memory.h register_file.h
	$(CC) -c $<

pipeline.o: pipeline.c pipeline.h instr_memory.h instructions.h \
  data_memory.h register_file.h mips_parser.h symbol_table.h \
  error_handling.h utils.h update.h
	$(CC) -c $<

register_file.o: register_file.c register_file.h error_handling.h
	$(CC) -c $<

symbol_table.o: symbol_table.c symbol_table.h
	$(CC) -c $<

update.o: update.c update.h pipeline.h instr_memory.h instructions.h \
  data_memory.h register_file.h utils.h error_handling.h
	$(CC) -c $<

utils.o: utils.c utils.h
	$(CC) -c $<

