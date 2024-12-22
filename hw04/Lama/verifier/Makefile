.PHONY: all run lama_runtime lama_src regression interpreter interpreter-verified clean

TARGET := main

all: run

run: clean ${TARGET}
	./${TARGET} tests/Sort.bc

${TARGET}: | lama_runtime
	g++ -O3 -g -m32 -fstack-protector-all -o ${TARGET} ../runtime/runtime.a ${TARGET}.cpp

lama_runtime: 
	make -C ../runtime

lama_src:
	make -C ../src

regression:
	make -C ../regression

interpreter:
	make -C ../interpreter

interpreter-verified:
	make -C ../interpreter-verified

tests: lama_src lama_runtime regression interpreter interpreter-verified ${TARGET}
	make -C tests

clean:
	rm -f ${TARGET}
	make -C tests clean
