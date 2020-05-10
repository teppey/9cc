#!/bin/bash

assert() {
    expected="$1"
    input="$2"

    ./sobacc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 '0;'
assert 42 '42;'
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5;"
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 10 '-10+20;'
assert 8 '-(3+5)*-1;'
assert 9 '+10+-1;'
assert 0 '1==0;'
assert 1 '0==0;'
assert 1 '1==1;'
assert 0 '1!=1;'
assert 1 '1!=0;'
assert 1 '3<5;'
assert 0 '5<3;'
assert 1 '3<=5;'
assert 0 '5<=3;'
assert 1 '5<=5;'
assert 1 '4>2;'
assert 0 '2>4;'
assert 1 '2<=4;'
assert 0 '4<=2;'
assert 1 '4<=4;'
assert 0 '1+2==3-4;'
assert 1 '(1>2)+(2<3);'
assert 3 'a = 3;'
assert 22 'b = 5 * 6 - 8;'
assert 14 'a = 3; b = 5 * 6 - 8; a + b / 2;'
assert 1 'foo = 1;'
assert 5 'bar = 2 + 3;'
assert 10 'foo = 2; bar = 2 + 3; foo * bar;'
assert 0 '1 == 0;'
assert 1 '1+1 == 2;'
assert 5 'return 5; return 8;'
assert 14 'a = 3; b = 5 * 6 - 8; return a + b / 2;'
assert 1 'if (3 == 3) return 1;'
assert 2 'if (3 == 4) return 1; else return 2;'
assert 0 'a = 0; while (a > 1) a = a + 1; return a;'
assert 3 'a = 0; while (a < 3) a = a + 1; return a;'
assert 20 'a = 0; while (a < 3) if (a == 2) a = a * 10; else a = a + 1; return a;'
assert 1 'for (;;) return 1;'
assert 1 'for (a = 1;;) return a;'
assert 2 'for (;0;) return 1; return 2;'
assert 7 'for (i = 0; i < 7; i = i + 1) 0; return i;'
assert 11 'a = 0; for (;; a = a + 1) if (a > 10) return a;'
assert 4 'a = 0; for (i = 0; i < 2; i = i + 1) for (j = 0; j < 2; j = j + 1) a = a + 1; return a;'

echo OK
