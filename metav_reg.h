//
// Created by 白板 on 2021/1/7.
//
#include "metavEntity.h"
#ifndef CPP_FINAL_PROJ_METAV_REG_H
#define CPP_FINAL_PROJ_METAV_REG_H

template<int w>
class metav_reg : public metaVEntity<w> {
public:
    metav_reg(){};
    metav_reg(string name){
        this->name = name;
        if (meta_verilog_runtimes_count==1){
            cout <<intend <<"reg [" << this->width-1 <<":0] " << this->name <<";" <<endl;
        }
    };

    metav_reg(string name,int cn){
        this->name = name;
        if (meta_verilog_runtimes_count==1){
            if (cn == 1){
                cout <<intend<<"input reg ["<< this->width-1 <<":0] " << this->name <<";" <<endl;
            }
            if (cn == 2){
                cout <<intend<<"output reg ["<< this->width-1 <<":0] " << this->name <<";" <<endl;
            }
        }
    }
};
#endif //CPP_FINAL_PROJ_METAV_REG_H
