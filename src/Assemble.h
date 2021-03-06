/*

Copyright (c) 2005-2016, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of Aboria.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of the University of Oxford nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


#ifndef ASSEMBLE_H_
#define ASSEMBLE_H_

#include <type_traits>
#include "Symbolic.h"


#ifdef HAVE_EIGEN
#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET
#include <Eigen/Sparse>
#include <Eigen/Dense>
#endif


namespace Aboria {

#ifdef HAVE_EIGEN

template <typename Derived, typename Expr, 
         typename = typename std::enable_if<detail::is_bivariate<Expr>::value >::type>
void assemble(Eigen::DenseBase<Derived>& matrix, const Expr& expr) {
    typedef typename detail::symbolic_helper<Expr>::particles_a_type particles_a_type;
    typedef typename detail::symbolic_helper<Expr>::particles_b_type particles_b_type;
    typedef typename particles_b_type::double_d double_d;
    typedef typename particles_b_type::position position;

    const particles_a_type& a = fusion::at_c<0>(detail::get_labels()(expr,fusion::nil_())).get_particles();
    const particles_b_type& b = fusion::at_c<1>(detail::get_labels()(expr,fusion::nil_())).get_particles();

    const size_t na = a.size();
    const size_t nb = b.size();

    //matrix.resize(na,nb);

    CHECK((matrix.rows() == na) && (matrix.cols() == nb), "matrix size is not compatible with expression.")

    for (size_t i=0; i<na; ++i) {
        for (size_t j=0; j<nb; ++j) {
            double_d dx = a.correct_dx_for_periodicity(get<position>(b[j])-get<position>(a[i]));
            matrix(i,j) = eval(expr,dx,a[i],b[j]);
        }
    }
}

template <typename Scalar, typename Expr, typename ifExpr,
         typename = typename std::enable_if<detail::is_bivariate<Expr>::value >::type>
void assemble(Eigen::SparseMatrix<Scalar>& matrix, const Expr& expr, const ifExpr& if_expr) {
    typedef typename detail::symbolic_helper<Expr>::particles_a_type particles_a_type;
    typedef typename detail::symbolic_helper<Expr>::particles_b_type particles_b_type;
    typedef typename particles_b_type::double_d double_d;
    typedef typename particles_b_type::position position;

    const particles_a_type& a = fusion::at_c<0>(detail::get_labels()(expr,fusion::nil_())).get_particles();
    const particles_b_type& b = fusion::at_c<1>(detail::get_labels()(expr,fusion::nil_())).get_particles();

    const size_t na = a.size();
    const size_t nb = b.size();

    matrix.resize(nb,na);

    CHECK((matrix.cols() == na) && (matrix.rows() == nb), "matrix size is not compatible with expression.")

    //std::vector<size_t> cols_sizes(na);
    typedef Eigen::Triplet<Scalar> triplet_type;
    std::vector<triplet_type> tripletList;
    tripletList.reserve(na*5); 

    for (size_t i=0; i<na; ++i) {
        //cols_sizes[i] = 0;
        typename particles_a_type::const_reference ai = a[i];
        for (auto pairj: b.get_neighbours(get<position>(ai))) {
            const double_d & dx = std::get<1>(pairj);
            typename particles_b_type::const_reference bj = std::get<0>(pairj);
            if (eval(if_expr,dx,ai,bj)) {
                const size_t j = &get<position>(bj)-get<position>(b).data();
                tripletList.push_back(triplet_type(i,j,eval(expr,dx,ai,bj)));
                //++cols_sizes[i];
            }
        }
    }
    matrix.setFromTriplets(tripletList.begin(),tripletList.end());
}

#endif

}

#endif //ASSEMBLE_H_
