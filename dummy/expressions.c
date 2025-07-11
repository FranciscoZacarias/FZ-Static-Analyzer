// Basic literals and arithmetic
int basic = 1 + 2;
int a = 1 + 2 * 3;
int declaration;
int b = (4 + 5) * 6;
int c = -a + ~b;
int d = a < b && b != c || c >= d;
int e = a ? b : c;
int* p = &a;
int f = sizeof(int);
int g = (int)3.14;

// Unary operators
int h = -1;
int i = +1;
int j = !0;
int k = ~0;
int l = ++a;
int m = --b;

// Postfix operators
int n = a++;
int o = b--;

// Function calls (declare stubs below)
int p1 = foo();
int p2 = bar(1, 2 + 3);
int p3 = baz(foo(), bar(1), 4);

// Struct member access
struct S { int x; int y; } s;
int p5 = s.x;
int p6 = s.y;

// Pointer dereference and member access
struct S* ps = &s;
int p7 = ps->x;
int p8 = (*ps).y;

// Logical and bitwise operations
int p9  = a & b;
int p10 = a | b;
int p11 = a ^ b;
int p12 = a && b;
int p13 = a || b;

// Comparison and relational operators
int p14 = a < b;
int p15 = a > b;
int p16 = a <= b;
int p17 = a >= b;
int p18 = a == b;
int p19 = a != b;

// Assignment and compound assignment
int p20 = a = 5;
int p21 = a += 1;
int p22 = b -= 2;
int p23 = c *= 3;
int p24 = d /= 4;
int p25 = e %= 5;
int p26 = f &= 6;
int p27 = g |= 7;
int p28 = h ^= 8;
int p29 = i <<= 1;
int p30 = j >>= 2;

// Comma operator
int p31 = (a = 5, b = 6, c = 7);

// Mixed complex expression
int p32 = (a + b) * (c - d / (e + f)) << 2 & 0xFF;

// Sizeof with expression
int p33 = sizeof(a + b);

// Ternary inside binary
int p34 = (a > b ? c : d) + e;

// Casts with arithmetic
int p35 = (float)(a + b) * 1.5f;
int p36 = (int)(double_val());

// Declare dummy functions and variables
int foo();
int bar(int x, int y);
int baz(int a, int b, int c);
double double_val();