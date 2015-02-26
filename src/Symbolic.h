/*
 * Symbolic.h
 * 
 * Copyright 2015 Martin Robinson
 *
 * This file is part of Aboria.
 *
 * Aboria is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Aboria is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Aboria.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Created on: 5 Feb 2015
 *      Author: robinsonm
 */

#ifndef SYMBOLIC_H_
#define SYMBOLIC_H_

#include "Particles.h"

#include <boost/proto/core.hpp>
#include <boost/proto/context.hpp>
namespace proto = boost::proto;
using proto::_;

namespace Aboria {

template<typename Expr>
struct DataVectorExpr;

// Here is an evaluation context that indexes into a DataVector
// expression and combines the result.
struct DataVectorSubscriptCtx
{
    DataVectorSubscriptCtx(std::size_t i)
      : i_(i)
    {}

    // Unless this is a DataVector terminal, use the
    // default evaluation context
    template<typename Expr, typename EnableIf = void>
    struct eval
      : proto::default_eval<Expr, DataVectorSubscriptCtx const>
    {};

    // Index DataVector terminals with our subscript.
    template<typename Expr>
    struct eval<
        Expr
      , typename boost::enable_if<
            proto::matches<Expr, proto::terminal<DataVector<_, _> > >
        >::type
    >
    {
        typedef typename proto::result_of::value<Expr>::type::value_type result_type;

        result_type operator ()(Expr &expr, DataVectorSubscriptCtx const &ctx) const
        {
            return proto::value(expr)[ctx.i_];
        }
    };

    std::size_t i_;
};

// Here is an evaluation context that verifies that all the
// DataVectors in an expression have the same particle datastructure.
struct DataVectorSameCtx
{
	DataVectorSameCtx(void *particles)
      : particles(particles)
    {}

    // Unless this is a DataVector terminal, use the
    // null evaluation context
    template<typename Expr, typename EnableIf = void>
    struct eval
      : proto::null_eval<Expr, DataVectorSameCtx const>
    {};

    // Index array terminals with our subscript. Everything
    // else will be handled by the default evaluation context.
    template<typename Expr>
    struct eval<
        Expr
      , typename boost::enable_if<
            proto::matches<Expr, proto::terminal<DataVector<_, _> > >
        >::type
    >
    {
        typedef void result_type;

        result_type operator ()(Expr &expr, DataVectorSameCtx const &ctx) const
        {
            if(ctx.particles != proto::value(expr).get_particles())
            {
                throw std::runtime_error("LHS and RHS are not compatible");
            }
        }
    };

    void *particles;
};



// This grammar describes which TArray expressions
// are allowed; namely, int and array terminals
// plus, minus, multiplies and divides of TArray expressions.
struct DataVectorGrammar
		: proto::or_<
		  proto::terminal<_>
		, proto::plus< DataVectorGrammar, DataVectorGrammar >
		, proto::minus< DataVectorGrammar, DataVectorGrammar >
		, proto::multiplies< DataVectorGrammar, DataVectorGrammar >
		, proto::divides< DataVectorGrammar, DataVectorGrammar >
		>
		{};




// Tell proto how to generate expressions in the DataVectorDomain
struct DataVectorDomain
		: proto::domain<proto::generator<DataVectorExpr>, DataVectorGrammar >
		{};

// Here is DataVectorExpr, which extends a proto expr type by
// giving it an operator [] which uses the DataVectorSubscriptCtx
// to evaluate an expression with a given index.
template<typename Expr>
struct DataVectorExpr
		: proto::extends<Expr, DataVectorExpr<Expr>, DataVectorDomain>
{
			explicit DataVectorExpr(Expr const &expr)
			: proto::extends<Expr, DataVectorExpr<Expr>, DataVectorDomain>(expr)
			  {}

			// Use the DataVectorSubscriptCtx to implement subscripting
			// of a DataVector expression tree.
			typename proto::result_of::eval<Expr const, DataVectorSubscriptCtx const>::type
			operator []( std::size_t i ) const
			{
				DataVectorSubscriptCtx const ctx(i);
				return proto::eval(*this, ctx);
			}
};

// Define a trait type for detecting DataVector terminals, to
// be used by the BOOST_PROTO_DEFINE_OPERATORS macro below.
template<typename T>
struct IsDataVector
		: mpl::false_
		  {};

template<int I, typename DataType>
struct IsDataVector<DataVector<I,DataType> >
		: mpl::true_
		  {};

namespace DataVectorOps
{
    // This defines all the overloads to make expressions involving
    // std::DataVector to build expression templates.
    BOOST_PROTO_DEFINE_OPERATORS(IsDataVector, DataVectorDomain)

    typedef DataVectorSubscriptCtx const CDataVectorSubscriptCtx;

    // Assign to a DataVector from some expression.
    template<int I, typename DataType, typename Expr>
    DataVector<I, DataType> &operator =(DataVector<I, DataType> &arr, Expr const &expr)
    {
        DataVectorSameCtx const same(arr.get_particles());
        proto::eval(proto::as_expr<DataVectorDomain>(expr), same); // will throw if the particles don't match
        for(std::size_t i = 0; i < arr.size(); ++i) {
            arr[i] = proto::as_expr<DataVectorDomain>(expr)[i];
        }
        return arr;
    }

    // Add-assign to a DataVector from some expression.
    template<int I, typename DataType, typename Expr>
    DataVector<I, DataType> &operator +=(DataVector<I, DataType> &arr, Expr const &expr)
    {
    	DataVectorSameCtx const same(arr.get_particles());
    	proto::eval(proto::as_expr<DataVectorDomain>(expr), same); // will throw if the particles don't match
    	for(std::size_t i = 0; i < arr.size(); ++i) {
    		arr[i] += proto::as_expr<DataVectorDomain>(expr)[i];
    	}
    	return arr;
    }
}


}



#endif /* SYMBOLIC_H_ */
