.PHONY: all run lama_runtime clean

TARGET := main

all: run

run: clean ${TARGET}
	./${TARGET} tests/Sort.bc

${TARGET}: | lama_runtime
	g++ -O3 -g -m32 -fstack-protector-all -o ${TARGET} ../runtime/runtime.a ${TARGET}.cpp

lama_runtime: 
	make -C ../runtime

clean:
	rm -f ${TARGET}
