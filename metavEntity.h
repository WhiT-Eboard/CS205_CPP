//
// Created by 白板 on 2021/1/4.
//
#include "string"
#include "iostream"
using namespace std;
#ifndef CPP_FINAL_PROJ_METAVENTITY_H
#define CPP_FINAL_PROJ_METAVENTITY_H
//if constexpr a()

//extern int meta_verilog_runtimes_count=0;


template<int bit_width>
class metaVEntity{
public:
    int width = bit_width;
    //string varName;
    string name = "";


    metaVEntity(){};
    metaVEntity(int w){
        this->width =w;
    }

    template<typename T>
    metaVEntity operator + (T &r){
        int const w = this->width>r.width? this->width:r.width;
        if (meta_verilog_runtimes_count==1){
            cout <<this->name << "+" << r.name;
        }
        return *new metaVEntity<bit_width>(w);
    }

    template<typename T>
    metaVEntity operator -(T &r){
        const int w = this->width>r.width? this->width:r.width;
        if (meta_verilog_runtimes_count==1){
            cout <<this->name << "-" << r.name;
        }
        return *new metaVEntity<bit_width>(w);
    }

    template<typename T>
    metaVEntity operator *(T &r){
        const int w = this->width>r.width? this->width:r.width;
        if (meta_verilog_runtimes_count==1){
            cout <<this->name << "*" << r.name;
        }
        return *new metaVEntity<bit_width>(2*w);
    }

    template<typename T>
    metaVEntity operator /(T &r){
        const int w = this->width>r.width? this->width:r.width;
        if (meta_verilog_runtimes_count==1){
            cout <<this->name << "/" << r.name;
        }
        return *new metaVEntity<bit_width>(w);
    }

    template<typename T>
    bool operator >(T &r){
        //const int w = this->width>r.width? this->width:r.width;
        if (meta_verilog_runtimes_count==1){
            cout <<this->name << ">" << r.name;
        }
        return false;
    }

    template<typename T>
    bool operator !(){
        //const int w = this->width>r.width? this->width:r.width;
        if (meta_verilog_runtimes_count==1){
            cout <<"!" <<this->name;
        }
        return false;
    }

    template<typename T>
    bool operator ~(){
        //const int w = this->width>r.width? this->width:r.width;
        if (meta_verilog_runtimes_count==1){
            cout <<"~"<<this->name;
        }
        return false;
    }

    template<typename T>
    bool operator <(T &r){
        //const int w = this->width>r.width? this->width:r.width;
        if (meta_verilog_runtimes_count==1){
           cout <<this->name << ">" << r.name;
        }
        return false;
    }

    template<typename T>
    metaVEntity operator &(T &r){
        //const int w = this->width>r.width? this->width:r.width;
        if (meta_verilog_runtimes_count==1){
            cout <<this->name << "&" << r.name;
        }
        return *new metaVEntity<bit_width>();
    }

    template<typename T>
    metaVEntity operator ^(T &r){
        //const int w = this->width>r.width? this->width:r.width;
        if (meta_verilog_runtimes_count==1){
            //cout <<this->name << "^" << r.name;
        }
        return *new metaVEntity<bit_width>();
    }

    template<typename T>
    metaVEntity operator |(T &r){
        //const int w = this->width>r.width? this->width:r.width;
        if (meta_verilog_runtimes_count==1){
            cout <<this->name << "|" << r.name;
        }
        return *new metaVEntity<bit_width>();
    }


    template<typename T>
    metaVEntity& operator =(const T &r){
        const int w = this->width>r.width? this->width:r.width;
        if (this->width<r.width){
            this->width = r.width;
        }
        if (meta_verilog_runtimes_count==1){
            cout <<"\n" << intend <<this->name << "=" << r.name;
        }
        return *this;
    }

    template<typename T>
    void write(T &r){
        if (this->width<r.width){
            this->width = r.width;
            //cout<< r.width;
        }
        /*
        if (meta_verilog_runtimes_count==1){
            cout <<"\n" << intend <<this->name << "=" << r.name;
        }
         */
    }

    void write(int n){

    }
};
#endif //CPP_FINAL_PROJ_METAVENTITY_H
