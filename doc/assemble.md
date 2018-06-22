# Mips汇编生成

## 寄存器使用

```mips

# 用于返回起始点的返回地址寄存器
$ra

# 栈指针寄存器
$sp

# 用于返回值
$v0
$v1

# 用于传递参数
$a0

# 临时寄存器
$t0,$t1,$t2
```

## 指令使用

```mips

# 第一类
not Rdest, Rsrc

# 第二类
add Rdest, Rsrc1, Src2

# 第三类
beqz Rsrc, label # 等于0时跳转

# 第四类
ble Rsrc1, Src2, label # 小于等于则跳转

# 存储
usw Rsrc, address # 存储32位 可能未对齐 unaligned

sb Rsrc, address # 存储8位


# 加载
ulw Rdest, address # 加载32位 可能未对齐 unaligned

li Rdest, imm # 加载立即数

lb Rdest, address # 加载8位

la Rdest, address # 将地址加载进寄存器


# 其它
jr Rsrc # 跳转到寄存器,用于函数返回

j label # 跳转到label

jal label # 跳转并链接,相当于调用函数

syscall # 系统调用 $v0 用于系统调用编号

# 使用的系统调用
exit 17
malloc 9
print int 1
print string 4
putchar 11
getchar 12
```