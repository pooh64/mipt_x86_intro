.PHONY: .FORCE folders sasm sasm_x64lib exec exec_mmap

SASM_OBJ_LIST = ./debug/obj/main.o  	./debug/obj/database.o 	\
		./debug/obj/parse.o 	./debug/obj/x64.o	\
	   	./debug/obj/serrno.o

folders: .FORCE
	mkdir -p ./debug
	mkdir -p ./debug/obj
	mkdir -p ./debug/test
	mkdir -p ./debug/x64lib

sasm: .FORCE folders sasm_x64lib
	gcc -c -o ./debug/obj/main.o		./sasm/main.c
	gcc -c -o ./debug/obj/database.o 	./sasm/database.c
	gcc -c -o ./debug/obj/parse.o 		./sasm/parse.c
	gcc -c -o ./debug/obj/x64.o 		./sasm/x64.c
	gcc -c -o ./debug/obj/serrno.o		./public/serrno.c
	gcc -o    ./debug/sasm $(SASM_OBJ_LIST)

sasm_x64lib: .FORCE folders
	nasm -o ./debug/x64lib/inp.x64 ./sasm/x64lib/inp.nasm
	nasm -o ./debug/x64lib/out.x64 ./sasm/x64lib/out.nasm

exec: .FORCE folders
	gcc -o ./debug/exec ./exec/main.c

exec_mmap: .FORCE folders
	gcc -o ./debug/exec_mmap ./exec_mmap/main.c

