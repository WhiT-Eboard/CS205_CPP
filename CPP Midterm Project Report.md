# CPP Midterm Project Report

> 11911307 likangxin

## Content

- Configuration
- Matrix Class
- The Simplest Implement
- First try to optimize
- Optimize index and others
- Using AVX2
- Blocking
- Multi Threads

- Some possible methods



## Configuration

All the tests of this report (if not specify) are done by this CPU:

```
model name	: AMD Ryzen 5 4500U with Radeon Graphics
microcode	: 0x8600102
cpu MHz		: 1396.768
cache size	: 512 KB
siblings	: 6
cpu cores	: 6
```

The operation system is `Ubuntu 20.04.1 LTS` and the GCC version is `gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)`



##  Matrix Class

This class is defined in the  `matrix.h` 

 ```c++
class matrix{
public:
    int row,col;
    float *mContent;

    matrix(int row,int col);

    void dotProduct(matrix &first_matrix,matrix &second_matrix,int i,int j,int k);
    

};
 ```



## The Simplest Implement

Same as the  dot product, the brute force way to do matrix multiplication looks like this

```c++
	matrix m1(m,n),m2(n,k),m3(m,k);
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < k; ++j) {
            for (int l = 0; l < n; ++l) {
                m3.mContent[i*m+j] += m1.mContent[i*m+l] * m2.mContent[l*n+j];
            }
        }
    }
```

This will do `m*n*k` times of multiplications and adding, the time complexity reaches `O(n^3)`. For the matrix of  size of `10000*10000` . It's almost impossible to get the result  within an hour.  



## First try to optimize

First we analyze the problem: the size of my CPU cache is `512 KB` which is `524288 B`. It can hold about  `65536`  float numbers.  So some of the memory have to load many times into the cache from RAM. When the matrix is small, this effect can be ignored.  But IO cost cannot be  neglected if we want to do larger calculation. 

For convenience , we first define a macro 

```c++
#define M_AT(m,i,j) (m.mContent[i*m.row+j])
```

And we then define a member function of class matrix:

```c++
void matrix::dotProduct(matrix &first_matrix, matrix &second_matrix, int i, int j, int k) {
    this->mContent[i * this->row + j] += M_AT(first_matrix, i, k) * M_AT(second_matrix, k, j);
    this->mContent[i * this->row + j + 1] += M_AT(first_matrix, i, k) * M_AT(second_matrix, k, j + 1);
    this->mContent[i * this->row + j + 2] += M_AT(first_matrix, i, k) * M_AT(second_matrix, k, j + 2);
    this->mContent[i * this->row + j + 3] += M_AT(first_matrix, i, k) * M_AT(second_matrix, k, j + 3);
}
```

Then our code will be like this

```c++
	matrix m1(m,n),m2(n,k),m3(m,k);    
	for (int j = 0; j < k; j+=4) {
        for (int i = 0; i < m; ++i) {
            for (int l = 0; l < n; ++l) {
                m3.dotProduct(m1,m2,i,j,l);
            }
        }
    }
```

Notice that we change the position of `i` and  `j`  , since the matrix is stored as an array. This will make reuse of the  `m1[i,k]`  which can reduce cache use. And by calculate `m3[i,j],m3[i,j+1],m3[i,j+2],m3[i,j+3]` together,  we will have less loop. All the improvement above will speed up the program a little bit. But not obviously. It takes no less than an hour to run `m1*m2` of size `10000*10000`. 



 ## Optimize index and others

To do this, we first have to change the matrix class, especially the `M_AT`. 

```c++
#define M_AT(m,i,j) (m.mContent[j*m.row+i])
```

By doing so, we are now store the matrix in an array that columns are contentious. Looks like this

```
Before:
{[0,0],[0,1],[0,2]...[i,j],[i,j+1]...}

After:
{[0,0],[1,0],[2,0]...[i,j],[i+1,j]...}
```

Then we take a look at our code   

```c++
void matrix::dotProduct(matrix &first_matrix, matrix &second_matrix, int i, int j, int k) {
    this->mContent[i * this->row + j] += M_AT(first_matrix, i, k) * M_AT(second_matrix, k, j);
    this->mContent[i * this->row + j + 1] += M_AT(first_matrix, i, k) * M_AT(second_matrix, k, j + 1);
    this->mContent[i * this->row + j + 2] += M_AT(first_matrix, i, k) * M_AT(second_matrix, k, j + 2);
    this->mContent[i * this->row + j + 3] += M_AT(first_matrix, i, k) * M_AT(second_matrix, k, j + 3);
}
```

We notice that  `M_AT(first_matrix, i, k)`  is  used four times and be tread from cache four times, and we have to re-calculate the index of  `second_matrix`  and  `m3[i,j]`. And since we changed the way of storage matrix. We change the code to :

```c++
void matrix::dotProduct(matrix &first_matrix, matrix &second_matrix, int i, int j, int k) {
    auto smPtr_0 = &M_AT(second_matrix, 0, j);
    auto smPtr_1 = &M_AT(second_matrix, 0, j + 1);
    auto smPtr_2 = &M_AT(second_matrix, 0, j + 2);
    auto smPtr_3 = &M_AT(second_matrix, 0, j + 3);
    float fm,m3_0=0,m3_1=0,m3_2=0,m3_3=0;
    for (int l = 0; l < k; ++l) {
        fm = M_AT(first_matrix, i, l);
        m3_0 += fm * *smPtr_0++;
        m3_1 += fm * *smPtr_1++;
        m3_2 += fm * *smPtr_2++;
        m3_3 += fm * *smPtr_3++;
    }
    this->mContent[j * this->row + i] += m3_0;
    this->mContent[(j+1) * this->row + i] += m3_1;
    this->mContent[(j+2) * this->row + i] += m3_2;
    this->mContent[(j+3) * this->row + i] += m3_3;
}
```

This will  slightly increase the speed of our program, still it cannot  run the `10000*10000` matrix in an acceptable time.



## Using AVX2

From the last assignment we know that instruction set is very powerful when calculating something very large. So we try to use avx2 to optimize our code. Since `__mm256`  can handle `8 float` number at a time, instead of calculating  `1x4` vector,  we will now calculating  `1x8`  vector of the matrix we want in one run.  The `dotProduct`  will be look like this:

```c++
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
        fm = _mm256_set_ps(*(fmPtr_0 + count * k), *(fmPtr_1 + count * k), *(fmPtr_2 + count * k), *(fmPtr_3 + count * k),
                           *(fmPtr_4 + count * k), *(fmPtr_5 + count * k), *(fmPtr_6 + count * k), *(fmPtr_7 + count * k));
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

```

And the program will be look like this:

```c++
for (int j = 0; j < k; j+=8) {
        for (int i = 0; i < m; i++) {
                m3.dotProduct(m1,m2,i,j,n);
        }
    }
```

Basically, in the inner loop, we calculate vector `m1[i,j...j+8]` with sub matrix  `m2[i...i+8,j...j+8]` together. And store it in a register `m3_v_r` , which represent  the  vector we want to get.  Then we move the first vector 8 digits right and the second matrix 8 digits down, till they reach the edge of the matrix.

> For convenience, we simply think two matrices are both eight times large.



## Blocking

Code look like this

```c++
int matrixMul(matrix &first_matrix, matrix &second_matrix, matrix &result_Matrix, int L2) {
    int m = first_matrix.row, n = first_matrix.col, k = second_matrix.col;
    int step_1 = 512, step_2 = 64;
    if (n != second_matrix.row)
        return -1;
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
```

By separate the matrix, we can use cache more efficiently. We no longer to load the whole matrix so the efficiency will be improved.



## Multi Threads

Same as the last assignment, we can use OpenMp to make full use of our CPU, just add one line code:

```c++
int matrixMul(matrix &first_matrix, matrix &second_matrix, matrix &result_Matrix, int L2) {
    int m = first_matrix.row, n = first_matrix.col, k = second_matrix.col;
    int step_1 = 512, step_2 = 64;
    if (n != second_matrix.row)
        return -1;
#pragma omp parallel for num_threads(6)// Use omp
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
```

This greatly improve our speed. Till now we can try to calculate `10000*10000` matrix. The result is `114094`. About 100sec.  Far away from the `Openblas`



