.PHONY: all run_tests interpreter regression clean

all: run_tests

run_tests: interpreter regression
	python3 test.py

interpreter:
	make -C ..

regression:
	make -C ../../regression

clean:
	rm -f test[0-9][0-9][0-9].bc
	rm -rf temp
