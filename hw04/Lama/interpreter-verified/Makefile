.PHONY: all lama_runtime lama_src clean

TARGET := main

all: ${TARGET}

${TARGET}: ${TARGET}.o | lama_runtime
	gcc -O3 -g -m32 -fstack-protector-all $< ../runtime/runtime.a -o $@

${TARGET}.o: ${TARGET}.c
	gcc -O3 -g -m32 -fstack-protector-all -c $< -o $@

lama_runtime: 
	make -C ../runtime

lama_src:
	make -C ../src

clean:
	rm -f ${TARGET}
	rm -f ${TARGET}.o
	make -C ../runtime clean

