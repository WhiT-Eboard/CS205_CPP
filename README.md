# 基于C++的Verilog元编程

# Verilog Meta-Programing Based on C Plus Plus



> 11911307 Likangxin
>
> https://github.com/WhiT-Eboard/CS205_CPP



- 项目简介

- Verilog 介绍

  - Verilog 中的generate 语句

  - System C

- 使用

- 实现

  - 控制流
  - 变量
  - 模块
  - 运行

- 总结



## 项目简介

>  **!本项目尚处于早期开发阶段!**

本项目志在减轻计算机系，尤其是软件工程方向的学生在编写**Verilog**时所遭受的折磨

目前该项目特化面向 **SUSTech CS207/Digital Design** 以及其配套的**Minisys**开发板

项目基于原生**C++**开发，因此可以兼容所有**C++**的语法特性及外部库。可以通过编写标准C++程序来驱动产生Verilog程序。





## Verilog 介绍

**Verilog**是一种用于描述、设计电子系统的硬件描述语言（Hardware Description Language,**HDL**）

虽然**Verilog**在语法上是设计成类似与**C**语言的，但区别与**C/C++** 等高级语言，Verilog设计由模块层次结构组成。并且其缺乏抽象能力。并且硬件设计并不追求简洁，而是电路级别的实现。也因如此，编写Verilog程序是一件异常痛苦的事情。



### Verilog 中的generate 语句

在Verilog中确实拥有产生表达式的能力，即`generate`，但是该语句本身在硬件上是无法有效优化的，因此并不推荐使用。

### System C

> SystemC 是一种系统设计语言。严格讲，SystemC是一个能描述系统和硬件的C++库。和其他硬件描述语言一样，SystemC支持RTL级建模，然而SystemC最擅长的却是描述比RTL跟高层、更抽象的系统级和结构级*引自《 SystemC: From the Ground Up》*

![img](https://img2018.cnblogs.com/common/1909197/201912/1909197-20191230145020970-929705539.png)

本项目没有仿真的能力，但是能通过标准C++程序直接输出Verilog程序。这一点SystemC无法做到，而且鉴于绝大部分学生日后无需接触FPGA，而掌握SystemC又需要大量时间。故并不利于Verilog的学习。



## 使用

如同编写普通C++程序一样

```c++
#include "metaVerilog"

int main(){
    for(int i=0;i<10;++i){
        mv_reg<2> VAR(a,i);
    }
    GENERATE;
}
```

运行这段代码的输出是

```verilog
module() begin
    reg [1:0] a0;
    reg [1:0] a1;
    reg [1:0] a2;
    reg [1:0] a3;
    reg [1:0] a4;
    reg [1:0] a5;
    reg [1:0] a6;
    reg [1:0] a7;
    reg [1:0] a8;
    reg [1:0] a9;
end
```

显然，这段代码没有什么实际作用，但是随着整个程序复杂度的上升，这样的抽象能力能极大简化编写Verilog代码的难度和复杂程度，极大地降低心智负担。



## 实现

### 控制流

控制流原语的实现依赖宏和匿名函数(lambda)，以原语`IF()...END`为例：

```c++
#define IF(statement) mv_If();\ //调用mv_If()驱动产生verilog 中的if() 
	([&](){statement;})();\ //通过产生匿名函数并运行来产生条件语句
	mv_Bracket();\ //产生条件语句的反括号和 "begin"
#define END mv_End(); //产生条件语句的结尾 "end"
#define BEGIN //仅仅只为了美观
```

样例代码如下

```c++
IF(a==b) BEGIN
    c = a + b;
END
```

在编译之后

```c++
mv_If();
([&](){
    a==b;
})();
mv_Bracket();
c = a + b;
mv_End();
```

运行产生**Verilog** 代码

```verilog
if(a==b) begin
    c = a + b;
end
```



### 变量

所有的变量都继承自`metaVEntity`，目前已经实现了寄存器、线网等变量类型。以寄存器`mv_reg`为例

```c++
template<int bitwidth>
class mv_reg: public metaVEntity{
    ...
};
```

在声明变量时候需要确定模板参数：位宽(bitwidth)。以及构造器初始参数`name`。

变量重载了几乎所有的运算符，同样以`mv_reg`为例

```c++
template<typename T>
mv_reg operator +(T &r){
    int w = this->width>r.width?this->width:r.width;//二者中的最大位宽
    if (meta_verilog_runtimes_count==1){
            cout << this->name << "+" << r.name;
    }
    return *new mv_reg<w>();//返回最大位宽的变量
}
```

通过声明模板函数，来绕开不同位宽寄存器之间运算的问题。并且通过运算判别位宽，返回一个拥有最大位宽的寄存器变量。以此实现位宽的自动推断。

在全局计数器`meta_verilog_runtimes_count` 计数到 `1 ` 时，输出生成的语句。

为了方便起见，用户可以选择使用宏`REG(ins)`来直接声明一个默认位宽的变量。实现如下

```c++
#define REG(ins) mv_reg<1> ins(#ins);
```

由于**C++**不支持**反射**，所以只能通过上述的宏来实现静态获取变量名称。在其他情况下，则需要用户自己手动输入变量名称。目前尚无更好的解决方案。



### 模块

声明元**Verilog**模块需要继承`metaVMod`，并且实现独有的构造器，构造器的参数即为模组的IO端口。

```c++
class myMod: public metaVMod{
    std::string className = "myMod";
    myMod(mv_in<mv_reg<2>> &arg_in,mv_out<mv_wire<2>> &arg_out){
        ...
    }
};
```

值得一提的是，由于相同的原因，这里需要注明模组名称。模组的实现则写于构造器内部。



### 运行

如同一个常规的**C++**程序一样，在引入头文件后即可正常编写，程序入口为`main`函数。编写完毕后，在程序尾部调用宏`GENERATE`即可。

程序一共会运行两次，通过全局计数器`meta_verilog_runtimes_count`控制。在第一次运行时程序会确定所有变量的位宽，第二次运行时输出生成代码。该功能由宏`GENERATE`控制，实现如下

```c++
#define GENERATE if(meta_verilog_runtimes_count==0)\
{++meta_verilog_runtimes_count;\
return main();}\ //若控制参数为0,则递归调用main函数
else return 0;
```

由于实现了自动位宽推断，只需在关键位置声明变量位宽即可。



## 总结

由于时间所迫，大量的功能没有得以实现，并且部分代码也无法在开发板上得到验证。但不可否认，C++作为一门高级语言，其在设计DSL(domain specific language)有着得天独厚的优势。
