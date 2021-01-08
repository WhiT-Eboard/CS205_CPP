//
// Created by 白板 on 2021/1/3.
//
#include <string>
#include <iostream>

//#include "metavEntity.h"
extern int meta_verilog_runtimes_count = 0;
extern std::string intend = "";
using namespace std;
#ifndef CPP_FINAL_PROJ_METAVERILOG_H
#define CPP_FINAL_PROJ_METAVERILOG_H

#define IF(statement) mv_if(); ([&](){(statement);})();mv_bracket();

#define END mv_end();

#define ELSE mv_else();

#define ELSEIF(statement) mv_elseif();([&](){(statement;)})();mv_bracket();

#define BEGIN //do nothing, if it can make you feel a little better :D

#define INITIAL mv_init();

#define INS(ins)

#define INIT(ins) class ins{};

#define vwrite(v, stat) if(meta_verilog_runtimes_count==1)std::cout<<"\n"<<intend<<(v.name) << "=";v.write(stat);\
if(meta_verilog_runtimes_count==1){try{ if (typeid(stat)==typeid(int)) cout<<#stat;}catch(exception e){} std::cout<<";";};

#define VAR(varname, id, bw) mv_reg<bw> varname(id)(#varname)

#define vn(varname) #varname

#define ID(var, id) varname##id

#define top(ports) gen_top();ports;gen_top2();

#define generate if(meta_verilog_runtimes_count==0){++meta_verilog_runtimes_count;intend="";return main();}else{cout<<"\nendmodule"; return 0;}

//#define Iter_0(type,name) type name##0(#name##0);
#define Iter_1(type, name) type name##1(string(#name)+string("1"));

#define Iter_2(type, name) Iter_1(type,name);type name##2(string(#name)+string("2"));

#define Iter_3(type, name) Iter_2(type,name);type name##3(string(#name)+string("3"));

#define Iter_4(type, name) Iter_3(type,name);type name##4(string(#name)+string("4"));

#define Iter_5(type, name) Iter_4(type,name);type name##5(string(#name)+string("5"));

#define Iter_6(type, name) Iter_5(type,name);type name##6(string(#name)+string("6"));

#define Iter_7(type, name) Iter_6(type,name);type name##7(string(#name)+string("7"));

#define Iter_8(type, name) Iter_7(type,name);type name##8(string(#name)+string("8"));

#define Iter_9(type, name) Iter_8(type,name);type name##9(string(#name)+string("9"));

#define Iter_10(type, name) Iter_9(type,name);type name2(string(#name)+string("10"));

#define Iter(type, name, count) Iter_##count(type,name)

#define metav_in(type,name) type name(#name,1);

#define metav_out(type,name) type name(#name,2);

//#define mv_end



void mv_if();
void mv_bracket();

void mv_end();

void mv_else();
void mv_elseif();

void gen_top();

void gen_top2();

void mv_init();
#endif //CPP_FINAL_PROJ_METAVERILOG_H
