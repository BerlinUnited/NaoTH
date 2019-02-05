#ifndef HUNGARIANALGORITHM_H
#define HUNGARIANALGORITHM_H

#include <forward_list>
#include "Eigen/Eigen"
#include "Tools/Debug/NaoTHAssert.h"

template<typename T>
class HungarianAlgorithm
{
public:
    static void solve(Eigen::Array<T, Eigen::Dynamic, Eigen::Dynamic>& m) {
        HungarianAlgorithm<T> h(m);
    }
private:
    HungarianAlgorithm(Eigen::Array<T, Eigen::Dynamic, Eigen::Dynamic>& m);
    bool find_uncovered_in_matrix (size_t &, size_t &) const;
    int step1();
    int step2();
    int step3();
    int step4();
    int step5();
    int step6();

    const size_t size;
    Eigen::Array<T,       Eigen::Dynamic, Eigen::Dynamic>& matrix;
    Eigen::Array<char,    Eigen::Dynamic, Eigen::Dynamic> mask_matrix;
    Eigen::Array<bool, 1, Eigen::Dynamic> row_mask;
    Eigen::Array<bool, 1, Eigen::Dynamic> col_mask;
    size_t saverow;
    size_t savecol;

    enum MASK : char
    {
        NORMAL = ' '
      , STAR   = '*' // starred,
      , PRIME  = '#' // primed.
    };
};


template<typename T>
HungarianAlgorithm<T>::HungarianAlgorithm(Eigen::Array<T, Eigen::Dynamic, Eigen::Dynamic>& m)
    : size (std::max(m.rows(), m.cols()))
    , matrix {m}
    , mask_matrix {Eigen::Array<char, Eigen::Dynamic, Eigen::Dynamic>::Constant(size,size,NORMAL)}
    , row_mask {Eigen::Array<bool, 1, Eigen::Dynamic>::Constant(size,false)}
    , col_mask {Eigen::Array<bool, 1, Eigen::Dynamic>::Constant(size,false)}
    , saverow {0}
    , savecol {0}
{
    // the input matrix has to be square
    ASSERT_MSG(matrix.rows() == matrix.cols(),
               "Input matrix has to be square!");

    // Prepare the matrix values...
    matrix.colwise() -= matrix.rowwise().minCoeff();
    matrix.rowwise() -= matrix.colwise().minCoeff();

    // Follow the steps
    int step = 1;
    while (step) {
        switch (step) {
            case 1:
                step = step1 ();    // step is always 2
            case 2:
                step = step2 ();    // step is always either 0 or 3
                break;
            case 3:
                step = step3 ();    // step in [3, 4, 5]
                break;
            case 4:
                step = step4 ();    // step is always 2
                break;
            case 5:
                step = step5 ();    // step is always 3
                break;
        }
    }

    // Store results.
    for (size_t col = 0; col < size; col++) {
        for (size_t row = 0; row < size; row++) {
            matrix (row, col) = mask_matrix (row, col) == STAR ? 0 : 1;
        }
    }
}

template<typename T>
bool HungarianAlgorithm<T>::find_uncovered_in_matrix (size_t & row, size_t & col) const
{
    for (col = 0; col < size; col++) {
        if (!col_mask(col)) {
            for (row = 0; row < size; row++) {
                if (!row_mask(row)) {
                    if (matrix(row, col) == 0) {
                        return true;
                    }
                } // if
            } // for row
        } // if
    } // for column
    return false;
}

template<typename T>
int HungarianAlgorithm<T>::step1 ()
{
    for (size_t row = 0; row < size; row++) {
        for (size_t col = 0; col < size; col++) {
            if (matrix(row, col) == 0) {
                for (size_t nrow = 0; nrow < row; nrow++) {
                    if (STAR == mask_matrix (nrow, col) ) {
                        goto next_column;
                    }
                }
                mask_matrix (row,col) = STAR;
                goto next_row;
            }
            next_column:;
        }
        next_row:;
    }
    return 2;
}

template<typename T>
int HungarianAlgorithm<T>::step2 ()
{
    size_t covercount = 0;

    for (size_t col = 0; col < size; col++) {
        for (size_t row = 0; row < size; row++) {
            if (STAR == mask_matrix (row, col) ) {
                col_mask(col) = true;
                covercount++;
            }
        }
    }
    return covercount >= size ? 0 : 3;
}

template<typename T>
int HungarianAlgorithm<T>::step3 ()
{
    // Main Zero Search
    // 1. Find an uncovered Z in the distance matrix and prime it. If no such zero exists, go to Step 5
    // 2. If No Z* exists in the row of the Z', go to Step 4.
    // 3. If a Z* exists, cover this row and uncover the column of the Z*. Return to Step 3.1 to find a new Z
    if (find_uncovered_in_matrix (saverow, savecol) ) {
        mask_matrix (saverow, savecol) = PRIME;  // Prime it.
        for (size_t ncol = 0; ncol < size; ncol++) {
            if (mask_matrix (saverow, ncol) == STAR) {
                row_mask(saverow) = true;   // Cover this row and
                col_mask(ncol) = false;     // uncover the column containing the starred zero
                return 3;                   // repeat.
            }
        }
        return 4;   // No starred zero in the row containing this primed zero.
    }

    return 5;
}

template<typename T>
int HungarianAlgorithm<T>::step4 ()
{
    // Seq contains pairs of row/column values where we have found
    // either a star or a prime that is part of the ``alternating sequence``.
    // Use saverow, savecol from step 3.
    std::forward_list<std::pair<size_t, size_t>> seq {{saverow, savecol}};

    // Increment Set of Starred Zeros
    // 1. Construct the ``alternating sequence'' of primed and starred zeros:
    //   Z0     : Unpaired Z' from Step 4.2
    //   Z1     : The Z* in the column of Z0
    //   Z[2N]  : The Z' in the row of Z[2N-1], if such a zero exists
    //   Z[2N+1]: The Z* in the column of Z[2N]
    // The sequence eventually terminates with an unpaired Z' = Z[2N] for some N.
    size_t dim [] = {0, savecol};
    const char mask [] = {STAR, PRIME};
    for (size_t i = 0; dim [i] < size; ++dim [i]) {
        if (mask_matrix (dim [0], dim [1]) == mask [i]) {
            // We have to find these two pairs: z1 and z2n.
            seq.push_front ({dim [0], dim [1]});
            i = (i + 1) & 1;    // Switch dimension.
            dim [i] = -1;       // After increment this value becames zero.
        }
    }

    for (const auto & i : seq) {
        // 2. Unstar each starred zero of the sequence.
        if (mask_matrix (i.first,i.second) == STAR) {
            mask_matrix (i.first,i.second) = NORMAL;
        }

        // 3. Star each primed zero of the sequence,
        // thus increasing the number of starred zeros by one.
        if (mask_matrix (i.first,i.second) == PRIME) {
            mask_matrix (i.first,i.second) = STAR;
        }
    }

    // 4. Erase all primes, uncover all columns and rows,
    std::replace_if (mask_matrix.data(), mask_matrix.data() + mask_matrix.size(), [](char & x){return x == PRIME;}, NORMAL);

    // reset masks
    row_mask = false;
    col_mask = false;

    // and return to Step 2.
    return 2;
}

template<typename T>
int HungarianAlgorithm<T>::step5 ()
{
    // New Zero Manufactures
    // 1. Let h be the smallest uncovered entry in the (modified) distance matrix.
    // 2. Add h to all covered rows.
    // 3. Subtract h from all uncovered columns
    // 4. Return to Step 3, without altering stars, primes, or covers.
    T h = std::numeric_limits<T>::max ();
    for (size_t col = 0; col < size; col++) {
        if (!col_mask(col)) {
            for (size_t row = 0; row < size; row++) {
                if (!row_mask(row)) {
                    if (h > matrix (row, col) && matrix(row, col) != 0) {
                        h = matrix (row, col);
                    }
                }
            }
        }
    }

    for (size_t row = 0; row < size; row++) {
        if (row_mask(row)) {
            for (size_t col = 0; col < size; col++) {
                matrix (row, col) += h;
            }
        }
    }

    for (size_t col = 0; col < size; col++) {
        if (!col_mask(col)) {
            for (size_t row = 0; row < size; row++) {
                matrix (row, col) -= h;
            }
        }
    }

    return 3;
}


#endif // HUNGARIANALGORITHM_H
