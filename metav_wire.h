//
// Created by 白板 on 2021/1/8.
//
#include "metavEntity.h"
#ifndef CPP_FINAL_PROJ_METAV_WIRE_H
#define CPP_FINAL_PROJ_METAV_WIRE_H
template<int w>
class metav_wire : public metaVEntity<w> {
public:
    metav_wire(){};
    metav_wire(string name){
        this->name = name;
        if (meta_verilog_runtimes_count==1){
            cout <<intend <<"wire [" << this->width-1 <<":0] " << this->name <<";" <<endl;
        }
    };

    metav_wire(string name,int cn){
        this->name = name;
        if (meta_verilog_runtimes_count==1){
            if (cn == 1){
                cout <<intend<<"input wire ["<< this->width-1 <<":0] " << this->name <<";" <<endl;
            }
            if (cn == 2){
                cout <<intend<<"output wire ["<< this->width-1 <<":0] " << this->name <<";" <<endl;
            }
        }
    }
};
#endif //CPP_FINAL_PROJ_METAV_WIRE_H
