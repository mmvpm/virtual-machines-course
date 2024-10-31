## Задание

Написать простой итеративный интерпретатор стековой машины языка Lama

Исходный код, тесты корректности и производительности: https://github.com/PLTools/Lama

## Решение

Код интерпретатора: [interpreter/main.c](Lama/interpreter/main.c)

Запуск тестов корректности и производительности: [interpreter/tests/test.py](Lama/interpreter/tests/test.py). Скрипт запускает написанный интерпретатор на тестах из каталога [/regression](Lama/regression), а также меряет время его работы на файле [Sort.bc](Lama/interpreter/tests/Sort.bc) (байткод исходника [Sort.lama](Lama/interpreter/tests/Sort.lama))

## Запуск тестов

Перед запуском тестов нужно выполнить [инструкции по сборке самой Lama](https://github.com/PLTools/Lama)

```bash
cd Lama/interpreter
make tests
```

<details>
<summary>full stdout</summary>

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
[iterative] Elapsed time on "Sort.bc": 3.151 seconds
[recursive] Elapsed time on "Sort.lama": 6.315 seconds
make[1]: Leaving directory '/home/mmvpm/virtual-machines-course/hw02/Lama/interpreter/tests'
```

</details>

## Производительность

Итеративный интерпретатор: 3.151 seconds
Рекурсивный интерпретатор: 6.315 seconds