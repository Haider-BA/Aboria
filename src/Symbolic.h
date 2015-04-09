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

#include "DataVector.h"

#include <boost/mpl/bool.hpp>
#include <boost/proto/core.hpp>
#include <boost/proto/context.hpp>
namespace mpl = boost::mpl;
namespace proto = boost::proto;
using proto::_;
using proto::N;
namespace Aboria {

template<typename Expr>
struct DataVectorExpr;

// Here is an evaluation context that indexes into a lazy vector
// expression, and combines the result.
struct DataVectorSubscriptCtx
{
	DataVectorSubscriptCtx(size_t i)
      : i_(i)
    {}

    // Use default_eval for all the operations ...
    template<typename Expr, typename Tag = typename Expr::proto_tag>
    struct eval
      : proto::default_eval<Expr, DataVectorSubscriptCtx>
    {};

    // ... except for terminals, which we index with our subscript
    template<typename Expr>
    struct eval<Expr, proto::tag::terminal>
    {
        typedef const typename proto::result_of::value<Expr>::type::value_type& result_type;

        result_type operator ()( Expr const & expr, DataVectorSubscriptCtx & ctx ) const
        {
            return proto::value( expr )[ ctx.i_ ];
        }
    };
    const int i_;
};

//// Here is an evaluation context that indexes into a Vec3
//// expression, and combines the result.
//struct DataVectorSubscriptCtx
//  : proto::callable_context< DataVectorSubscriptCtx const >
//{
//    typedef const typename Elem<I, DataType>::type& result_type;
//
//    DataVectorSubscriptCtx(int i)
//      : i_(i)
//    {}
//
//    // Index array terminals with our subscript. Everything
//    // else will be handled by the default evaluation context.
//    template<int I, typename DataType>
//    const typename Elem<I, DataType>::type& operator ()(proto::tag::terminal, const DataVector<I,DataType> &arr) const
//    {
//        return arr[this->i_];
//    }
//
//    int i_;
//};

//// Here is an evaluation context that indexes into a DataVector
//// expression and combines the result.
//struct DataVectorSubscriptCtx
//{
//    DataVectorSubscriptCtx(std::size_t i)
//      : i_(i)
//    {}
//
//    // Unless this is a DataVector terminal, use the
//    // default evaluation context
//    template<typename Expr, typename EnableIf = void>
//    struct eval
//      : proto::default_eval<Expr, DataVectorSubscriptCtx const>
//    {};
//
//    // Index DataVector terminals with our subscript.
//    template<typename Expr>
//    struct eval<
//        Expr
//      , typename boost::enable_if<
//            proto::matches<Expr, proto::terminal<DataVector<N, _> > >
//        >::type
//    >
//    {
//        typedef typename proto::result_of::value<Expr>::type::value_type result_type;
//
//        result_type operator ()(Expr &expr, DataVectorSubscriptCtx const &ctx) const
//        {
//            return proto::value(expr)[ctx.i_];
//        }
//    };
//
//    std::size_t i_;
//};


  struct DataVectorSameCtx
		  : proto::callable_context< DataVectorSameCtx, proto::null_context >
		  {
			  DataVectorSameCtx(void *particles)
    	    								  : particles(particles)
    	    								    {}

			  typedef void result_type;
			  template<int I, typename DataType>
			  void operator ()(proto::tag::terminal, const DataVector<I,DataType> &arr)
			  {
				  if(particles != &(arr.get_particles()))
				  {
					  throw std::runtime_error("LHS and RHS are not compatible");
				  }
			  }

			  void *particles;
		  };

//// Here is an evaluation context that verifies that all the
//// DataVectors in an expression have the same particle datastructure.
//struct DataVectorSameCtx
//{
//	DataVectorSameCtx(void *particles)
//      : particles(particles)
//    {}
//
//    // Unless this is a DataVector terminal, use the
//    // null evaluation context
//    template<typename Expr, typename EnableIf = void>
//    struct eval
//      : proto::null_eval<Expr, DataVintectorSameCtx const>
//    {};
//
//    // Index array terminals with our subscript. Everything
//    // else will be handled by the default evaluation context.
//    template<typename Expr>
//    struct eval<
//        Expr
//      , typename boost::enable_if<
//            proto::matches<Expr, proto::terminal<DataVector<N, _> > >
//        >::type
//    >
//    {
//        typedef void result_type;
//
//        result_type operator ()(Expr &expr, DataVectorSameCtx const &ctx) const
//        {
//            if(ctx.particles != proto::value(expr).get_particles())
//            {
//                throw std::runtime_error("LHS and RHS are not compatible");
//            }
//        }
//    };
//
//    void *particles;
//};



// This grammar describes which TArray expressions
// are allowed; namely, int and array terminalsusing namespace DataVectorOps
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
				DataVectorSubscriptCtx ctx(i);
				return proto::eval(*this, ctx);
			}
};


template<int I, typename ParticlesType>
struct DataVectorSymbolic
	: DataVectorExpr<typename proto::terminal<DataVector<I,ParticlesType> >::type> {

	typedef typename proto::terminal<DataVector<I,ParticlesType> >::type expr_type;

	explicit DataVectorSymbolic(ParticlesType& p)
	: DataVectorExpr<expr_type>( expr_type::make( DataVector<I,ParticlesType>(p) ) )
	  {}

	typename Elem<I, ParticlesType>::type &operator [](std::ptrdiff_t i)
	{
		return proto::value(*this)[i];
	}

	typename Elem<ID, ParticlesType>::type const &operator [](std::ptrdiff_t i) const
	{
		return proto::value(*this)[i];
	}

	template< typename Expr >
	DataVectorSymbolic &operator =(Expr const & expr) {
		return this->assign(proto::as_expr<DataVectorDomain>(expr));
	}

private:

	template< typename Expr >
	DataVectorSymbolic &assign(Expr const & expr)
	{
		DataVectorSameCtx same(&(proto::value(*this).get_particles()));
		proto::eval(expr, same); // will throw if the particles don't match
		for(std::size_t i = 0; i < proto::value(*this).size(); ++i) {
			proto::value(*this).set(i,expr[i]);
		}
		return *this;
	}
};


	template<typename ParticlesType>
		struct DataVectorSymbolic<ID,ParticlesType>
			: DataVectorExpr<typename proto::terminal<DataVector<ID,ParticlesType> >::type> {

			typedef typename proto::terminal<DataVector<ID,ParticlesType> >::type expr_type;

			explicit DataVectorSymbolic(ParticlesType& p)
			: DataVectorExpr<expr_type>( expr_type::make( DataVector<ID,ParticlesType>(p) ) )
			  {}

			typename Elem<ID, ParticlesType>::type &operator [](std::ptrdiff_t i)
			{
				return proto::value(*this)[i];
			}

			typename Elem<ID, ParticlesType>::type const &operator [](std::ptrdiff_t i) const
			{
				return proto::value(*this)[i];
			}
	};


template< int I, typename ParticlesType>
DataVectorSymbolic<I,ParticlesType> get_vector(ParticlesType &p) {
	return DataVectorSymbolic<I,ParticlesType>(p);
}

}



#endif /* SYMBOLIC_H_ */
