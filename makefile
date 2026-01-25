# Target: assembler
assembler: assembler.o first_pass.o pre_proc.o structs.o second_pass.o translation_unit.o pre_proc_errors.o first_pass_errors.o second_pass_errors.o free_table.o
	gcc -Wall -ansi -pedantic assembler.o first_pass.o pre_proc.o structs.o second_pass.o translation_unit.o pre_proc_errors.o first_pass_errors.o second_pass_errors.o free_table.o -o assembler

# Compile assembler.c into assembler.o
assembler.o: assembler.c assembler.h
	gcc -c -Wall -ansi -pedantic assembler.c -o assembler.o

# Compile first_pass.c into first_pass.o
first_pass.o: first_pass.c assembler.h
	gcc -c -Wall -ansi -pedantic first_pass.c -o first_pass.o

# Compile pre_proc.c into pre_proc.o
pre_proc.o: pre_proc.c assembler.h
	gcc -c -Wall -ansi -pedantic pre_proc.c -o pre_proc.o

# Compile structs.c into structs.o
structs.o: structs.c assembler.h
	gcc -c -Wall -ansi -pedantic structs.c -o structs.o

# Compile second_pass.c into second_pass.o
second_pass.o: second_pass.c assembler.h
	gcc -c -Wall -ansi -pedantic second_pass.c -o second_pass.o

# Compile translation_unit.c into translation_unit.o
translation_unit.o: translation_unit.c assembler.h
	gcc -c -Wall -ansi -pedantic translation_unit.c -o translation_unit.o

# Compile pre_proc_errors.c into pre_proc_errors.o
pre_proc_errors.o: pre_proc_errors.c assembler.h
	gcc -c -Wall -ansi -pedantic pre_proc_errors.c -o pre_proc_errors.o

# Compile first_pass_errors.c into first_pass_errors.o
first_pass_errors.o: first_pass_errors.c assembler.h
	gcc -c -Wall -ansi -pedantic first_pass_errors.c -o first_pass_errors.o

# Compile second_pass_errors.c into second_pass_errors.o
second_pass_errors.o: second_pass_errors.c assembler.h
	gcc -c -Wall -ansi -pedantic second_pass_errors.c -o second_pass_errors.o

# Compile free_table.c into free_table.o
free_table.o: free_table.c assembler.h
	gcc -c -Wall -ansi -pedantic free_table.c -o free_table.o

# Execute assembler
exec: assembler
	./assembler

# Clean compiled files
clean:
	rm -f *.o assembler
