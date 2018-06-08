# 词法分析

## 类别

* KEY 关键字

```C

// 数据类型
void
char
int
union
struct
typedef

//控制类型
if
for
else
break
while
return
continue

//类型长度
sizeof
```

* IDENTIFIER 标识符

```C
"[_a-zA-Z][_a-zA-Z0-9]*$"
```

* INT_CONST 整数常量

```C
//十进制
[0-9]+

//十六进值
((0x)|(0X))[a-zA-Z0-9]*
```

* CHAR_CONST 字符常量

```C
//普通字符
'?'

//转义字符
'\t'

//利用十六进制
'\x30'
'\X30'

//利用8进制
'\077'
```

* STRING_CONST 字符串常量

```C
"ABC"

"\x30\n\tABC\077"
```

* OPERATION 操作符

```C
'('
')'
';'
','
'='
'{'
'}'
'['
']'
'*'
'|'
'^'
'&'
'<'
'>'
'+'
'-'
'/'
'%'
'~'
'!'
'.'
'||'
'&&'
'=='
'!='
'<='
'>='
'<<'
'>>'
'++'
'--'
'->'
'+='
'-='
'*='
'/='
'%='
'|='
'^='
'>>='
'<<='
```

## 输入

被编译 C 语言文件的字符缓冲区

## 输出

输出是 Token 序列的数组

```C
class Token {
 public:
  // Token type five kind
  Type type_;
  // location
  size_t col_;
  size_t row_;
  // literal value
  std::string literal_;
  // if is int
  int int_val_;
  // if is char
  char char_val_;
  // if is str
  std::string str_val_;
};
```

Token 中包括 type (类别), row col (文件中的坐标), literal (词片段), int_val (如果是 int,则 int 值 ), char_val (如果是 char,则 char 值), str_val (如果是字符串,则字符串值)

## 测试

对 test/test.c 文件进行测试

![test.c部分截图](../pic/test_test_c_part.png)

输出的部分 Token 序列包括

```C
// 行号 列号 原始串
5, 1: typedef
5, 9: struct
5, 16: Elem
5, 21: Elem
5, 25: ;
6, 1: typedef
6, 9: struct
6, 16: Node
6, 21: Node
6, 25: ;
8, 1: struct
8, 8: Elem
8, 13: {
9, 3: union
9, 9: {
10, 5: int
10, 9: int_val
10, 16: ;
11, 5: char
11, 10: char_val
11, 18: ;
12, 3: }
12, 4: ;
13, 1: }
13, 2: ;
15, 1: struct
15, 8: Node
15, 13: {
16, 3: int
16, 7: id
16, 9: ;
17, 3: char
17, 8: *
17, 9: value
17, 14: ;
18, 3: Elem
18, 8: elem
18, 12: ;
19, 1: }
19, 2: ;
```