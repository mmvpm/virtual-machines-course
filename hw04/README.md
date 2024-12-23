## Задание

Написать верификатор байткодов Lama программ

Исходный код, тесты корректности и производительности: https://github.com/PLTools/Lama

## Решение

Код верификатора: [verifier/main.cpp](Lama/verifier/main.cpp)

## Запуск тестов

Перед запуском тестов нужно выполнить [инструкции по сборке самой Lama](https://github.com/PLTools/Lama)

```bash
eval $(opam env --switch=lama --set-switch)
cd Lama/verifier
make tests
```

### Тесты корректности и сравнение производительности

```
test001 passed
test002 passed
...
test046 passed
test050 passed
terminate called after throwing an instance of 'std::runtime_error'
  what():  ERROR: stack size is out of bounds: -1

Aborted (core dumped)
test054 failed on verifier
test059 passed
test063 passed
...
test110 passed
Failed: test054
[verifier] Elapsed time on "Sort.bc": 0.002 seconds
[iterative] Elapsed time on "Sort.bc": 3.491 seconds
[iterative-verified] Elapsed time on "Sort.bc": 3.139 seconds
[recursive] Elapsed time on "Sort.lama": 6.943 seconds
```

Видим ускорение на ~ 0.35 секунды по сравнению с версией без верификатора
