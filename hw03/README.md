## Задание

Написать статический анализатор частот вхождений последовательностей из 1-2 параметризованных байткодов в программах на Lama

Исходный код, тесты корректности и производительности: https://github.com/PLTools/Lama

## Решение

Код анализатора: [analyzer/main.cpp](Lama/analyzer/main.cpp)

## Запуск тестов

Перед запуском тестов нужно выполнить [инструкции по сборке самой Lama](https://github.com/PLTools/Lama)

```bash
cd Lama/analyzer
make
```

<details>
<summary>full stdout</summary>

```
31 times: DROP; 
28 times: DUP; 
21 times: ELEM; 
16 times: CONST 1; 
13 times: CONST 1; ELEM; 
12 times: CONST 0; 
11 times: DROP; DUP; 
11 times: DUP; CONST 1; 
10 times: DROP; DROP; 
8 times: CONST 0; ELEM; 
8 times: LD arg 0; 
7 times: DUP; CONST 0; 
7 times: ELEM; DROP; 
6 times: END; 
5 times: JMP 776; 
4 times: DUP; DUP; 
4 times: SEXP cons 2; 
3 times: DUP; ARRAY 2; 
3 times: ARRAY 2; 
3 times: ELEM; ST local 0; 
3 times: ST local 0; 
3 times: CALL Larray 2; JMP 776; 
3 times: ST local 0; DROP; 
3 times: JMP 350; 
3 times: LD local 0; 
3 times: CALL 351 1; 
3 times: LD local 3; 
3 times: CALL Larray 2; 
2 times: BEGIN 1 0; 
2 times: ELEM; CONST 1; 
2 times: LD local 1; 
2 times: TAG cons 2; 
2 times: CALL 43 1; 
2 times: DUP; TAG cons 2; 
2 times: BEGIN 2 0; 
2 times: JMP 116; 
2 times: CALL 151 1; 
2 times: BINOP ==; 
2 times: SEXP cons 2; CALL Larray 2; 
2 times: END; BEGIN 1 0; 
2 times: ELEM; CONST 0; 
2 times: CALL 351 1; DUP; 
2 times: BINOP -; 
1 times: JMP 262; DUP; 
1 times: BEGIN 1 1; LINE 18; 
1 times: CJMPNZ 651; DROP; 
1 times: LINE 28; 
1 times: CJMPNZ 428; DROP; 
1 times: ELEM; DUP; 
1 times: CALL 43 1; SEXP cons 2; 
1 times: CJMPZ 106; 
1 times: DROP; LD local 5; 
1 times: CJMPZ 106; LD arg 0; 
1 times: DROP; LINE 19; 
1 times: LINE 29; 
1 times: CALL 117 1; 
1 times: LD local 3; LD local 4; 
1 times: LD local 5; 
1 times: LINE 31; 
1 times: BEGIN 1 0; LINE 28; 
1 times: DROP; JMP 336; 
1 times: CJMPNZ 392; 
1 times: LINE 11; LD local 2; 
1 times: LD arg 0; LD arg 0; 
1 times: CONST 1000; 
1 times: CONST 0; LINE 13; 
1 times: ARRAY 2; CJMPNZ 651; 
1 times: ST local 3; DROP; 
1 times: LINE 28; LD arg 0; 
1 times: ST local 1; 
1 times: LINE 9; 
1 times: BEGIN 1 1; 
1 times: CJMPZ 274; DUP; 
1 times: LINE 20; 
1 times: CONST 1; BINOP ==; 
1 times: END; BEGIN 1 6; 
1 times: JMP 350; FAIL 18 9; 
1 times: FAIL 18 9; JMP 350; 
1 times: FAIL 11 17; JMP 776; 
1 times: JMP 350; END; 
1 times: BEGIN 1 6; 
1 times: DROP; JMP 386; 
1 times: LINE 7; 
1 times: BEGIN 1 6; LINE 7; 
1 times: TAG cons 2; CJMPNZ 392; 
1 times: ST local 5; DROP; 
1 times: CJMPNZ 280; 
1 times: DROP; JMP 748; 
1 times: CJMPNZ 428; 
1 times: JMP 386; 
1 times: TAG cons 2; CJMPNZ 428; 
1 times: BINOP -; END; 
1 times: CJMPZ 191; DUP; 
1 times: CONST 1; BINOP -; 
1 times: JMP 262; 
1 times: ST local 1; DROP; 
1 times: DROP; JMP 262; 
1 times: CJMPNZ 280; DROP; 
1 times: CJMPNZ 197; 
1 times: BINOP ==; CJMPZ 274; 
1 times: JMP 350; DUP; 
1 times: CJMPZ 191; 
1 times: CONST 0; BINOP ==; 
1 times: JMP 748; 
1 times: DROP; LINE 20; 
1 times: LD arg 0; DUP; 
1 times: LINE 19; LD local 0; 
1 times: LD local 0; CALL 151 1; 
1 times: CALL 151 1; JMP 350; 
1 times: CJMPZ 614; 
1 times: LINE 7; LD arg 0; 
1 times: LINE 18; LD arg 0; 
1 times: ARRAY 2; CJMPNZ 280; 
1 times: JMP 336; 
1 times: BINOP >; CJMPZ 614; 
1 times: JMP 776; LINE 11; 
1 times: LINE 1; LD arg 0; 
1 times: LINE 11; 
1 times: ELEM; SEXP cons 2; 
1 times: CALL 351 1; CONST 1; 
1 times: LD local 0; SEXP cons 2; 
1 times: LD local 3; LD local 0; 
1 times: JMP 729; 
1 times: LD local 1; LD local 3; 
1 times: CONST 1; LINE 10; 
1 times: LINE 10; 
1 times: LD local 2; 
1 times: CONST 0; BINOP >; 
1 times: BINOP >; 
1 times: CALL 777 2; CONST 0; 
1 times: LD local 1; CALL 777 2; 
1 times: CALL 777 2; 
1 times: LINE 10; LD local 1; 
1 times: LD local 3; LD local 1; 
1 times: DROP; LINE 9; 
1 times: ELEM; ST local 1; 
1 times: ELEM; ST local 2; 
1 times: ST local 2; 
1 times: LINE 2; LD arg 1; 
1 times: LD arg 1; BINOP -; 
1 times: LD arg 0; LINE 2; 
1 times: LINE 2; 
1 times: BEGIN 2 0; LINE 1; 
1 times: LINE 1; 
1 times: LD arg 0; CALL Larray 2; 
1 times: LINE 13; LD arg 0; 
1 times: LINE 13; 
1 times: DROP; CONST 0; 
1 times: JMP 776; DUP; 
1 times: JMP 776; FAIL 11 17; 
1 times: JMP 776; END; 
1 times: FAIL 11 17; 
1 times: LD local 4; SEXP cons 2; 
1 times: LD local 4; 
1 times: LD local 5; LD local 3; 
1 times: ST local 4; DROP; 
1 times: ST local 4; 
1 times: ELEM; ST local 5; 
1 times: DROP; JMP 729; 
1 times: DUP; DROP; 
1 times: CJMPNZ 651; 
1 times: LD local 2; CALL 351 1; 
1 times: CALL 151 1; END; 
1 times: SEXP cons 2; JMP 116; 
1 times: LINE 22; 
1 times: JMP 116; END; 
1 times: END; BEGIN 2 0; 
1 times: JMP 748; DUP; 
1 times: LD arg 0; CJMPZ 106; 
1 times: BEGIN 2 0; LINE 29; 
1 times: LINE 20; LD local 0; 
1 times: BINOP ==; CJMPZ 191; 
1 times: CONST 0; JMP 116; 
1 times: LINE 9; LD local 3; 
1 times: BEGIN 1 0; LINE 22; 
1 times: LD arg 0; CONST 1; 
1 times: CJMPNZ 197; DROP; 
1 times: ST local 5; 
1 times: JMP 116; CONST 0; 
1 times: CALL 117 1; END; 
1 times: CALL 43 1; CALL 117 1; 
1 times: LINE 29; LINE 31; 
1 times: LINE 22; LINE 24; 
1 times: LINE 19; 
1 times: CONST 1000; CALL 43 1; 
1 times: BINOP -; CALL 43 1; 
1 times: CJMPNZ 392; DROP; 
1 times: ELEM; ST local 3; 
1 times: JMP 776; JMP 776; 
1 times: ST local 3; 
1 times: JMP 386; DUP; 
1 times: ARRAY 2; CJMPNZ 197; 
1 times: JMP 729; DUP; 
1 times: ST local 2; DROP; 
1 times: LD arg 0; CALL 351 1; 
1 times: FAIL 18 9; 
1 times: CJMPZ 614; CONST 1; 
1 times: LINE 18; 
1 times: LINE 31; CONST 1000; 
1 times: END; BEGIN 1 1; 
1 times: LD arg 0; CALL 151 1; 
1 times: SEXP cons 2; CALL 351 1; 
1 times: CJMPZ 274; 
1 times: LD arg 1; 
1 times: ELEM; ST local 4; 
1 times: JMP 336; DUP; 
1 times: LINE 24; LD arg 0; 
1 times: LINE 24; 
1 times: LD local 0; JMP 350; 
```

</details>