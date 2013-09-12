#ifndef FF_LU_MATRIX_H_H
#define FF_LU_MATRIX_H_H
#include <sys/stat.h>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <iostream>

typedef enum {
    row_major,
    col_major,
    block
} MatrixStorage;

template<MatrixStorage MS, size_t BS>
struct pos_trait {
    static int 	pos(int m, int n, int i, int j)
    {
        assert(0);
        return 0;
    }
};

template<size_t BS>
struct pos_trait<row_major, BS>
{
    static int 	pos(int m, int n, int i, int j)
    {
        return i*m + j;
    }
};
template<size_t BS>
struct pos_trait<col_major, BS>
{
    static int 	pos(int m, int n, int i, int j)
    {
        return j*n + i;
    }
};

template<size_t BS>
struct pos_trait<block, BS>
{
    static int 	pos(int m, int n, int i, int j)
    {
        int bi = i%BS;
        int bj = j%BS;
        int gi = i/BS;
        int gj = j/BS;
        return ((m/BS)*gi + gj)*BS*BS + bi * BS + bj;
    }
};




template<class Ty, MatrixStorage MS, size_t BS=0>
class matrix_impl {
public:
    typedef Ty	elem_t;
    const static MatrixStorage ms = MS;
    const static size_t block_size = BS;
    matrix_impl(int m, int n)
        :m_pElements(nullptr)
        ,m_m(m), m_n(n)
    {
        m_pElements = new elem_t[m_m * m_n];
    }

    template <class M1>
    matrix_impl(M1 & a)
        : m_m(a.M())
        , m_n(a.N())
        , m_pElements(nullptr) {
        m_pElements = new elem_t[m_m * m_n];
        for(int i = 0; i < m_m; i++)
            for(int j = 0; j < m_n; j++)
                at(i, j) = a(i, j);
    }

    template<class T>
    matrix_impl<Ty, MS, BS> & operator = (const T & t) = delete;

    virtual ~matrix_impl()
    {
        if(m_pElements)
        {
            delete[] m_pElements;
        }
        m_pElements = nullptr;
    }

    elem_t &		operator()(int i, int j)
    {
        int pos = pos_trait<MS, BS>::pos(m_m, m_n, i, j);
        return m_pElements[pos];
    }
    const elem_t &		operator()(int i, int j) const
    {
        int pos = pos_trait<MS, BS>::pos(m_m, m_n, i, j);
        return m_pElements[pos];
    }

    int		M() const {
        return m_m;
    }
    int		N() const {
        return m_n;
    }
protected:
    elem_t &		at(int i, int j)
    {
        int pos = pos_trait<MS, BS>::pos(m_m, m_n, i, j);
        return m_pElements[pos];
    }
protected:
    elem_t * m_pElements;
    int 	m_m, m_n; //A[m][n]
};//end class matrix_impl


template<class Ty, size_t BS>
class SingleBlock {
public:
    SingleBlock( const Ty * p): m_pElements(const_cast<Ty *>(p)) {}
    Ty &		operator()(int i, int j)
    {
        return m_pElements[i*BS + j];
    }
    const Ty &		operator()(int i, int j) const
    {
        return m_pElements[i*BS + j];
    }
    int		M() const {
        return BS;
    }
    int		N() const {
        return BS;
    }

protected:
    Ty * m_pElements;
};

template <class M>
void print(const M & m)
{
    for(int i = 0; i < m.M(); ++i)
    {
        std::cout<<std::endl;
        for(int j = 0; j < m.N(); ++j)
        {
            std::cout<<m(i, j) <<"  ";
        }
    }
}

template<class Ty, size_t BS>
SingleBlock<Ty, BS> get_block(const matrix_impl<Ty, block, BS> & m, int i, int j)
{
    return SingleBlock<Ty, BS>(&m(i*BS, j*BS));
}

template<class Ty, size_t BS1>
void 	set_block(matrix_impl<Ty, block, BS1> & m, int i, int j, const matrix_impl<Ty, row_major, 0> & a)
{
    std::memcpy((void *)&m(i*BS1,j*BS1), (void *)&a(0, 0), BS1 * BS1*sizeof(Ty));
}

template<class Ty, size_t BS1, class M2>
void 	set_block(matrix_impl<Ty, block, BS1> & m, int i, int j, const M2 & a)
{
    for(int i = 0; i< BS1; i++)
        for(int j = 0; j < BS1; j++)
            m(i, j) = a(i, j);
}



template<class M1, class M2>
void	LUDecompose(const M1 & m1, M2 & res)
{
    for(int j = 0; j <m1.N(); j++)
        res(0,j) = m1(0,j);
    for(int i = 1; i < m1.M(); i++)
        res(i, 0) = m1(i, 0)/res(0, 0);
    for(int k = 1; k < m1.N(); k++)
    {
        for(int j = k; j<m1.M(); j++)
        {
            double sum = 0;
            for(int m = 0; m <= k-1; m++)
                sum += res(k, m) * res(m, j);
            res(k, j) = m1(k, j) - sum;
        }

        for(int i = k+1; i < m1.M(); ++i)
        {
            double sum = 0;
            for(int m = 0; m <= k-1; m++)
                sum += res(i, m) * res(m, k);
            res(i, k) = (m1(i, k) - sum)/res(k,k);
        }
    }
}


template<class M1, class M2>
void    invL(const M1 & m1, M2 & res)
{
    for(int i = 0; i<m1.M(); ++i)
    {
        for(int j = 0; j<m1.N(); ++j)
        {
            if(j < i)
            {
                double sum = 0;
                for(int k = j; k<=i-1; ++k)
                    sum += m1(i,k) * res(k, j);
                res(i, j) = 0-sum;
            }
            else if(j == i)
                res(i, j) = 1;
            else
                res(i, j) = 0;
        }
    }
}
template<class M1, class M2>
void    invU(const M1 & m1, M2 & res)
{
    for(int i = m1.M() -1 ; i>=0; --i)
    {
        res(i, i) = 1/m1(i, i);
        for(int j = m1.N()-1; j>=0; --j)
        {
            if(j >i)
            {
                double sum = 0;
                for(int k = i+1; k <= j; ++k)
                {
                    sum += m1(i, k) * res(k, j);
                }
                res(i, j) = -sum / m1(i, i);
            }
            else if(j == i){}
            else
                res(i, j) = 0;
        }
    }
    
}

template<class M1, class M2, class M3>
void mul(const M1 & m1, const M2 & m2, M3 & res) {

    for(int i = 0; i< m1.M(); i++)
    {
        for(int j = 0; j< m2.N(); j++)
        {
            double sum = 0;
            for(int k = 0; k < m1.N(); k++)
                sum += m1(i, k) * m2(k, j);
            res(i, j) = sum;
        }
    }
    /*
    std::cout<<"****************mul:"<<std::endl;
    std::cout<<"m1:";
    print(m1);
    std::cout<<"\nm2:";
    print(m2);
    std::cout<<"\nres:";
    print(res);
    std::cout<<"\n*****************mul end!"<<std::endl;
    */
}

template<class M1, class M2, class M3>
void sub(const M1 & m1, const M2 & m2, M3 & res) {
    for(int i = 0; i < m1.M(); ++i)
        for(int j = 0; j< m1.N(); ++j)
            res(i, j) = m1(i, j) - m2(i, j);
}

template<class M1>
void initMatrixForLU(M1 & matrix)
{
    int m = matrix.M();
    matrix_impl<typename M1::elem_t, row_major, 0> l(m, m);
    matrix_impl<typename M1::elem_t, row_major, 0> u(m, m);
    std::srand(std::time(0)); // use current time as seed for random generator
    for(int i = 0; i < m; i++)
    {
        for(int j = 0; j< m; j++)
        {
            if(i>j)
            {
                l(i, j) = 1.0 * (rand()%(m*m));
                u(i, j) = 0;
            }
            else if(i == j)
            {
                l(i, j) = 1;
                u(i, j) = 1.0 * (rand()%(m*m));
            }
            else
            {
                l(i, j) = 0;
                u(i, j) = 1.0 * (rand()%(m*m));
            }
        }
    }

    mul(l, u, matrix);
}
#endif