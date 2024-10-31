## Задание

Написать простой итеративный интерпретатор стековой машины языка Lama

Исходный код, тесты корректности и производительности: https://github.com/JetBrains-Research/Lama

## Решение

[interpreter/main.c](Lama/interpreter/main.c)

Запустить тесты корректности и производительности:

```bash
cd Lama/interpreter
make tests
```

<details>
<summary>stdout</summary>

```
mmvpm@compute-vm-2-4-20-ssd-1730209407248:~/virtual-machines-course/hw02/Lama/interpreter$ make tests
make -C ../src
make[1]: Entering directory '/home/mmvpm/virtual-machines-course/hw02/Lama/src'
dune build ./Driver.exe
<... omitted ...>
make[2]: Leaving directory '/home/mmvpm/virtual-machines-course/hw02/Lama/regression'
python3 test.py
test001 passed
test002 passed
<... omitted ...>
test110 passed
Regression tests passed!
Elapsed time on "Sort.bc": 3.097 seconds
make[1]: Leaving directory '/home/mmvpm/virtual-machines-course/hw02/Lama/interpreter/tests'
```

</details>
