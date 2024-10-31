.PHONY: all run_tests clean

all: run_tests

run_tests: .. | ../../regression
	python3 test.py

..:
	make -C ..

../../regression:
	make -C ../../regression

clean:
	rm -f test[0-9][0-9][0-9].bc
	rm -rf temp
