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

assert_func() {
    file="$1"
    expected="$2"
    input="$3"

    cc -o "$file.o" -c "$file"
    ./sobacc "$input" > tmp.s
    cc -o tmp tmp.s "$file.o"
    actual="$(./tmp)"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 'main() { return 0; }'
assert 42 'main() { return 42; }'
assert 21 'main() { return 5+20-4; }'
assert 41 'main() { return 12 + 34 - 5; }'
assert 47 'main() { return 5+6*7; }'
assert 15 'main() { return 5*(9-6); }'
assert 4 'main() { return (3+5)/2; }'
assert 10 'main() { return -10+20; }'
assert 8 'main() { return -(3+5)*-1; }'
assert 9 'main() { return +10+-1; }'
assert 0 'main() { return 1==0; }'
assert 1 'main() { return 0==0; }'
assert 1 'main() { return 1==1; }'
assert 0 'main() { return 1!=1; }'
assert 1 'main() { return 1!=0; }'
assert 1 'main() { return 3<5; }'
assert 0 'main() { return 5<3; }'
assert 1 'main() { return 3<=5; }'
assert 0 'main() { return 5<=3; }'
assert 1 'main() { return 5<=5; }'
assert 1 'main() { return 4>2; }'
assert 0 'main() { return 2>4; }'
assert 1 'main() { return 2<=4; }'
assert 0 'main() { return 4<=2; }'
assert 1 'main() { return 4<=4; }'
assert 0 'main() { return 1+2==3-4; }'
assert 1 'main() { return (1>2)+(2<3); }'
assert 3 'main() { return a = 3; }'
assert 22 'main() { return b = 5 * 6 - 8; }'
assert 14 'main() { a = 3; b = 5 * 6 - 8; return a + b / 2; }'
assert 1 'main() { return foo = 1; }'
assert 5 'main() { return bar = 2 + 3; }'
assert 10 'main() { foo = 2; bar = 2 + 3; return foo * bar; }'
assert 0 'main() { return 1 == 0; }'
assert 1 'main() { return 1+1 == 2; }'
assert 5 'main() { return 5; return 8; }'
assert 14 'main() { a = 3; b = 5 * 6 - 8; return a + b / 2; }'
assert 1 'main() { if (3 == 3) return 1; }'
assert 2 'main() { if (3 == 4) return 1; else return 2; }'
assert 0 'main() { a = 0; while (a > 1) a = a + 1; return a; }'
assert 3 'main() { a = 0; while (a < 3) a = a + 1; return a; }'
assert 20 'main() { a = 0; while (a < 3) if (a == 2) a = a * 10; else a = a + 1; return a; }'
assert 1 'main() { for (;;) return 1; }'
assert 1 'main() { for (a = 1;;) return a; }'
assert 2 'main() { for (;0;) return 1; return 2; }'
assert 7 'main() { for (i = 0; i < 7; i = i + 1) 0; return i; }'
assert 11 'main() { a = 0; for (;; a = a + 1) if (a > 10) return a; }'
assert 4 'main() { a = 0; for (i = 0; i < 2; i = i + 1) for (j = 0; j < 2; j = j + 1) a = a + 1; return a; }'
assert 1 'main() { { return 1; } }'
assert 2 'main() { { a = 1; return a + 1; } }'
assert 5 'main() { a = 2; b = 3; if (a > 0) { a = a * a; b = a + 1; } return b; }'
assert 20 'main() { a = b = 0; while (a < 10) { b = b + 1; a = a + 1; } return a + b; }'
assert 1 'main() { {} return 1; }'
assert 8 'fib(n) { if (n <= 2) return 1; return fib(n - 2) + fib(n - 1); } main() { return fib(6); }'

assert_func ./testfunc/foo.c "OK" 'main() { foo(); }'
assert_func ./testfunc/foo1.c "3" 'main() { foo(3); }'
assert_func ./testfunc/foo2.c "7" 'main() { foo(3, 4); }'
assert_func ./testfunc/foo3.c "12" 'main() { foo(3, 4, 5); }'
assert_func ./testfunc/foo4.c "18" 'main() { foo(3, 4, 5, 6); }'
assert_func ./testfunc/foo5.c "25" 'main() { foo(3, 4, 5, 6, 7); }'
assert_func ./testfunc/foo6.c "33" 'main() { foo(3, 4, 5, 6, 7, 8); }'

echo OK
