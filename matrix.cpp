#include "matrix.h"



/*
float vector_mul(float *a,float *b,size_t size) {
    size_t eight_times_size = div(size,8).quot;
    eight_times_size *=8;
    float total=0;
#pragma omp parallel for num_threads(32) reduction(+:total)
    for (int k = 0; k < eight_times_size; k+=8) {
        total+= _mm256_dp_ps(_mm256_loadu_ps(a + k), _mm256_loadu_ps(b + k), 0xff)[0];
        total+= _mm256_dp_ps(_mm256_loadu_ps(a + k), _mm256_loadu_ps(b + k), 0xff)[4];
    }
    for (int i = eight_times_size; i < size; ++i) {
        total += a[i]*b[i];
    }
    return total;
}
 */
matrix::matrix(int row, int col) {
    this->row = row;
    this->col = col;
    this->mContent = new float[row * col]();
    memset(this->mContent, 0, sizeof(float) * row * col);
}



void matrix::dotProduct(matrix &first_matrix, matrix &second_matrix, int i, int j, int k) {
    float *smPtr_0, *smPtr_1, *smPtr_2, *smPtr_3, *smPtr_4, *smPtr_5, *smPtr_6, *smPtr_7,
            *fmPtr_0, *fmPtr_1, *fmPtr_2, *fmPtr_3, *fmPtr_4, *fmPtr_5, *fmPtr_6, *fmPtr_7;
    __m256 fm, sm_v_0, sm_v_1, sm_v_2, sm_v_3, sm_v_4, sm_v_5, sm_v_6, sm_v_7, m3_v, m3_v_r;
    int count=0;
    m3_v_r = _mm256_setzero_ps();

    smPtr_0 = &M_AT(second_matrix, 0, j);
    smPtr_1 = &M_AT(second_matrix, 0, j + 1);
    smPtr_2 = &M_AT(second_matrix, 0, j + 2);
    smPtr_3 = &M_AT(second_matrix, 0, j + 3);
    smPtr_4 = &M_AT(second_matrix, 0, j + 4);
    smPtr_5 = &M_AT(second_matrix, 0, j + 5);
    smPtr_6 = &M_AT(second_matrix, 0, j + 6);
    smPtr_7 = &M_AT(second_matrix, 0, j + 7);

    fmPtr_0 = &M_AT(first_matrix, i, 0);
    fmPtr_1 = &M_AT(first_matrix, i, 1);
    fmPtr_2 = &M_AT(first_matrix, i, 2);
    fmPtr_3 = &M_AT(first_matrix, i, 3);
    fmPtr_4 = &M_AT(first_matrix, i, 4);
    fmPtr_5 = &M_AT(first_matrix, i, 5);
    fmPtr_6 = &M_AT(first_matrix, i, 6);
    fmPtr_7 = &M_AT(first_matrix, i, 7);

    for (int l = 0; l < k; l += 8) {
        fm = _mm256_set_ps(*(fmPtr_0 ++), *(fmPtr_1 ++), *(fmPtr_2 ++), *(fmPtr_3 ++),
                           *(fmPtr_4 ++), *(fmPtr_5 ++), *(fmPtr_6 ++), *(fmPtr_7 ++));
        sm_v_0 = _mm256_loadu_ps(smPtr_0 + l);
        sm_v_1 = _mm256_loadu_ps(smPtr_1 + l);
        sm_v_2 = _mm256_loadu_ps(smPtr_2 + l);
        sm_v_3 = _mm256_loadu_ps(smPtr_3 + l);
        sm_v_4 = _mm256_loadu_ps(smPtr_4 + l);
        sm_v_5 = _mm256_loadu_ps(smPtr_5 + l);
        sm_v_6 = _mm256_loadu_ps(smPtr_6 + l);
        sm_v_7 = _mm256_loadu_ps(smPtr_7 + l);

        m3_v = _mm256_dp_ps(fm, sm_v_0, 0xff);
        m3_v_r[0] += m3_v[0] + m3_v[4];
        m3_v = _mm256_dp_ps(fm, sm_v_1, 0xff);
        m3_v_r[1] += m3_v[0] + m3_v[4];
        m3_v = _mm256_dp_ps(fm, sm_v_2, 0xff);
        m3_v_r[2] += m3_v[0] + m3_v[4];
        m3_v = _mm256_dp_ps(fm, sm_v_3, 0xff);
        m3_v_r[3] += m3_v[0] + m3_v[4];
        m3_v = _mm256_dp_ps(fm, sm_v_4, 0xff);
        m3_v_r[4] += m3_v[0] + m3_v[4];
        m3_v = _mm256_dp_ps(fm, sm_v_5, 0xff);
        m3_v_r[5] += m3_v[0] + m3_v[4];
        m3_v = _mm256_dp_ps(fm, sm_v_6, 0xff);
        m3_v_r[6] += m3_v[0] + m3_v[4];
        m3_v = _mm256_dp_ps(fm, sm_v_7, 0xff);
        m3_v_r[7] += m3_v[0] + m3_v[4];
        ++count;
    }
    this->mContent[j * row + i] = m3_v_r[0];
    this->mContent[(j + 1) * row + i] = m3_v_r[1];
    this->mContent[(j + 2) * row + i] = m3_v_r[2];
    this->mContent[(j + 3) * row + i] = m3_v_r[3];
    this->mContent[(j + 4) * row + i] = m3_v_r[4];
    this->mContent[(j + 5) * row + i] = m3_v_r[5];
    this->mContent[(j + 6) * row + i] = m3_v_r[6];
    this->mContent[(j + 7) * row + i] = m3_v_r[7];


}

int matrixMul(matrix &first_matrix, matrix &second_matrix, matrix &result_Matrix, int L2) {
    int m = first_matrix.row, n = first_matrix.col, k = second_matrix.col;
    int step_1 = 512, step_2 = 64;
    if (n != second_matrix.row)
        return -1;
#pragma omp parallel for num_threads(6)
    for (int j = 0; j < k; j += step_1) {
        for (int i = 0; i < m; i += step_2) {
            mulKernel(first_matrix, second_matrix, result_Matrix, i, n, j, step_1, step_2);
        }
    }
}

void mulKernel(matrix &first_matrix, matrix &second_matrix, matrix &result_Matrix, int m, int n, int k, int s1, int s2) {
    for (int j = k; j < k + s1; j += 8) {
        for (int i = m; i < m + s2; ++i) {
            result_Matrix.dotProduct(first_matrix, second_matrix, i, j, n);
        }
    }
}

/*
 *  auto smPtr_0 = &M_AT(second_matrix, 0, j);
    auto smPtr_1 = &M_AT(second_matrix, 0, j + 1);
    auto smPtr_2 = &M_AT(second_matrix, 0, j + 2);
    auto smPtr_3 = &M_AT(second_matrix, 0, j + 3);
    float fm_0,fm_1,fm_2,fm_3,
            m3_00=0,m3_01=0,m3_02=0,m3_03=0;
            //m3_10=0,m3_11=0,m3_12=0,m3_13=0,
            //m3_20=0,m3_21=0,m3_22=0,m3_23=0,
            //m3_30=0,m3_31=0,m3_32=0,m3_33=0;
    for (int l = 0; l < k; ++l) {
        fm_0 = M_AT(first_matrix, i, l);
        m3_00 += fm_0 * *smPtr_0++;
        m3_01 += fm_0 * *smPtr_1++;
        m3_02 += fm_0 * *smPtr_2++;
        m3_03 += fm_0 * *smPtr_3++;

        fm_1 = M_AT(first_matrix, i+1, l);
        m3_10 += fm_1 * *smPtr_0;
        m3_11 += fm_1 * *smPtr_1;
        m3_12 += fm_1 * *smPtr_2;
        m3_13 += fm_1 * *smPtr_3;

        fm_2 = M_AT(first_matrix, i+2, l);
        m3_20 += fm_2 * *smPtr_0;
        m3_21 += fm_2 * *smPtr_1;
        m3_22 += fm_2 * *smPtr_2;
        m3_23 += fm_2 * *smPtr_3;

        fm_3 = M_AT(first_matrix, i+3, l);
        m3_30 += fm_3 * *smPtr_0++;
        m3_31 += fm_3 * *smPtr_1++;
        m3_32 += fm_3 * *smPtr_2++;
        m3_33 += fm_3 * *smPtr_3++;

}
this->mContent[j * this->row + i] += m3_00;
this->mContent[(j+1) * this->row + i] += m3_01;
this->mContent[(j+2) * this->row + i] += m3_02;
this->mContent[(j+3) * this->row + i] += m3_03;

this->mContent[j * this->row + i+1] += m3_10;
this->mContent[(j+1) * this->row + i+1] += m3_11;
this->mContent[(j+2) * this->row + i+1] += m3_12;
this->mContent[(j+3) * this->row + i+1] += m3_13;

this->mContent[j * this->row + i+2] += m3_20;
this->mContent[(j+1) * this->row + i+2] += m3_21;
this->mContent[(j+2) * this->row + i+2] += m3_22;
this->mContent[(j+3) * this->row + i+2] += m3_23;

this->mContent[j * this->row + i+3] += m3_30;
this->mContent[(j+1) * this->row + i+3] += m3_31;
this->mContent[(j+2) * this->row + i+3] += m3_32;
this->mContent[(j+3) * this->row + i+3] += m3_33;









  float *smPtr_0,*smPtr_1,*smPtr_2,*smPtr_3,*smPtr_4,*smPtr_5,*smPtr_6,*smPtr_7,
    *fmPtr_0,*fmPtr_1,*fmPtr_2,*fmPtr_3,*fmPtr_4,*fmPtr_5,*fmPtr_6,*fmPtr_7;
    __m256 fm,sm_v_0,sm_v_1,sm_v_2,sm_v_3,sm_v_4,sm_v_5,sm_v_6,sm_v_7;

    smPtr_0 = &M_AT(second_matrix, 0, j);
    smPtr_1 = &M_AT(second_matrix, 0, j+1);
    smPtr_2 = &M_AT(second_matrix, 0, j+2);
    smPtr_3 = &M_AT(second_matrix, 0, j+3);
    smPtr_4 = &M_AT(second_matrix, 0, j+4);
    smPtr_5 = &M_AT(second_matrix, 0, j+5);
    smPtr_6 = &M_AT(second_matrix, 0, j+6);
    smPtr_7 = &M_AT(second_matrix, 0, j+7);

    fmPtr_0 = &M_AT(first_matrix, i, 0);
    fmPtr_1 = &M_AT(first_matrix, i, 1);
    fmPtr_2 = &M_AT(first_matrix, i, 2);
    fmPtr_3 = &M_AT(first_matrix, i, 3);
    fmPtr_4 = &M_AT(first_matrix, i, 4);
    fmPtr_5 = &M_AT(first_matrix, i, 5);
    fmPtr_6 = &M_AT(first_matrix, i, 6);
    fmPtr_7 = &M_AT(first_matrix, i, 7);


    for (int l = 0; l < k; l+=8) {
        fm = _mm256_set_ps(*fmPtr_0++,*fmPtr_1++,*fmPtr_2++,*fmPtr_3++,*fmPtr_4++,*fmPtr_5++,*fmPtr_6++,*fmPtr_7++);
        sm_v_0 = _mm256_loadu_ps(smPtr_0+l);
        sm_v_1 = _mm256_loadu_ps(smPtr_1+l);
        sm_v_2 = _mm256_loadu_ps(smPtr_2+l);
        sm_v_3 = _mm256_loadu_ps(smPtr_3+l);
        sm_v_4 = _mm256_loadu_ps(smPtr_4+l);
        sm_v_5 = _mm256_loadu_ps(smPtr_5+l);
        sm_v_6 = _mm256_loadu_ps(smPtr_6+l);
        sm_v_7 = _mm256_loadu_ps(smPtr_7+l);


 fm = _mm256_set_ps(M_AT(first_matrix, i, 0),
                       M_AT(first_matrix, i, 1),
                       M_AT(first_matrix, i, 2),
                       M_AT(first_matrix, i, 3),
                       M_AT(first_matrix, i, 4),
                       M_AT(first_matrix, i, 5),
                       M_AT(first_matrix, i, 6),
                       M_AT(first_matrix, i, 7));

                        m3_v += fm * sm_v_1;
        m3_v += fm * sm_v_2;
        m3_v += fm * sm_v_3;
        m3_v += fm * sm_v_4;
        m3_v += fm * sm_v_5;
        m3_v += fm * sm_v_6;
        m3_v += fm * sm_v_7;
    }
 */


