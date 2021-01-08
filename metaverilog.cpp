#include "metaverilog.h"
void mv_if() {
    if (meta_verilog_runtimes_count == 1) {
        cout <<"\n" <<intend << "if(";
        intend = intend + "\t";
    }
}

void mv_bracket() {
    if (meta_verilog_runtimes_count == 1) {
        cout << ") begin\n";
    }
}

void mv_end() {
    if (meta_verilog_runtimes_count == 1) {
        intend = intend.substr(0, strlen(intend.c_str()) - 1);
        cout <<"\n" << intend << "end\n";
    }
}

void mv_else() {
    if (meta_verilog_runtimes_count == 1) {
        intend = intend.substr(0, strlen(intend.c_str()) - 1);
        cout << intend << "\nelse begin\n";
        intend = intend + "\t";
    }

}

void mv_elseif() {
    if (meta_verilog_runtimes_count == 1) {
        intend = intend.substr(0, strlen(intend.c_str()) - 1);
        cout << intend << "\nelse if(";
        intend = intend + "\t";
    }
}

void gen_top(){
    if (meta_verilog_runtimes_count == 1) {
        cout << "module top(\n";
        intend = intend + "\t";
    }
}

void gen_top2(){
    if (meta_verilog_runtimes_count == 1) {
        cout <<intend <<");\n";
    }
}

void mv_init(){
    if (meta_verilog_runtimes_count == 1) {
        cout<< intend << "initial begin\n";
        intend = intend + "\t";
    }
}