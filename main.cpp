#include <iostream>
#include "matrix.h"
#include <chrono>
#include "cblas.h"
#include <cstring>
#pragma GCC optimize(3)
using namespace std;



int main() {
    int m = 10240,n=10240,k=10240;
    matrix m1(m,n),m2(n,k),m3(m,k);
    for (int i = 0; i < m*n; ++i) {
        m1.mContent[i]=1;
        m2.mContent[i]=1;
    }
    //m1.mContent[0] = 2;
    auto begin = chrono::steady_clock::now();
    //for (int j = 0; j < k; j+=8) {
    //    for (int i = 0; i < m; i++) {
    //            m3.dotProduct(m1,m2,i,j,n);
    //    }
    //}
    //matrixMul(m1,m2,m3,8);
    cblas_sgemm()
    /*
     * for (int l = 0; l < n; ++l) {
                //m3.mContent[i*m+j] += m1.mContent[i*m+l] * m2.mContent[l*n+j];
                M_AT(m3, i, j) += M_AT(m1, i, l) * M_AT(m2, l, j);
            }
     */
    auto end = chrono::steady_clock::now();
    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < 100; ++j) {
            cout << m3.mContent[i*100+j] << " ";
        }
        cout << endl;
    }
    cout << endl;
    cout << chrono::duration_cast<chrono::milliseconds>(end-begin).count();
    return 0;
}
