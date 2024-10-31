.PHONY: all lama tests clean

TARGET := main

all: ${TARGET}

${TARGET}: ${TARGET}.o | lama
	gcc -O3 -g -m32 -fstack-protector-all $< ../runtime/runtime.a -o $@

${TARGET}.o: ${TARGET}.c
	gcc -O3 -g -m32 -fstack-protector-all -c $< -o $@

tests: ${TARGET}
	make -C tests

lama: 
	make -C ../runtime

clean:
	rm -f ${TARGET}
	rm -f ${TARGET}.o
	make -C tests clean
	make -C ../runtime clean

