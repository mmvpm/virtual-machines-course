.PHONY: all lama_runtime lama_src tests clean

TARGET := main

all: ${TARGET}

${TARGET}: ${TARGET}.o | lama_runtime
	gcc -O3 -g -m32 -fstack-protector-all $< ../runtime/runtime.a -o $@

${TARGET}.o: ${TARGET}.c
	gcc -O3 -g -m32 -fstack-protector-all -c $< -o $@

tests: lama_src lama_runtime ${TARGET}
	make -C tests

lama_runtime: 
	make -C ../runtime

lama_src:
	make -C ../src

clean:
	rm -f ${TARGET}
	rm -f ${TARGET}.o
	make -C tests clean
	make -C ../runtime clean

