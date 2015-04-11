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
#include <boost/proto/traits.hpp>
#include <boost/utility/enable_if.hpp>
namespace mpl = boost::mpl;
namespace proto = boost::proto;
using proto::_;
using proto::N;

namespace Aboria {

template<typename Expr>
struct DataVectorExpr;

// Here is an evaluation context that indexes into a lazy vector
// expression, and combines the result.
template<typename ParticlesType>
struct DataVectorSubscriptCtx
{
	DataVectorSubscriptCtx(size_t i, const ParticlesType& particles)
      : i_(i),particles_(particles)
    {}

	template<
	        typename Expr
	        // defaulted template parameters, so we can
	        // specialize on the expressions that need
	        // special handling.
	      , typename Tag = typename proto::tag_of<Expr>::type
	      , typename Arg0 = typename Expr::proto_child0
	    >
	struct eval: proto::default_eval<Expr, DataVectorSubscriptCtx const>
						{};



			// Handle vector terminals here...
			template<typename Expr, int I, typename ParticlesType>
			struct eval<Expr, proto::tag::terminal, DataVector<I,ParticlesType> >
			{
				typedef typename proto::result_of::value<Expr>::type::value_type result_type;

				result_type operator ()(Expr &expr, DataVectorSubscriptCtx const &ctx) const
				{
					return proto::value(expr)[ctx.i_];
				}
	};

			// Handle sums here...
			template<typename Expr, typename Arg0>
			struct eval<Expr, tag::sum, Arg0 >
			{
				typedef typename proto::result_of::value<Expr>::type::value_type result_type;

				result_type operator ()(Expr &expr, DataVectorSubscriptCtx const &ctx) const
				{
					auto particlesb = proto::child<0>(expr);
					auto conditional = proto::child<1>(expr);
					auto arg = proto::child<2>(expr);
					double sum = 0;
					for (auto i: particlesb.find_neighbours(particles_[ctx.i_].get_position())) {
						if (conditional.eval_particle(i)) {
							sum += arg.eval_particle(i);
						}
					}
					return sum;
				}
				};

    const int i_;
    const ParticlesType& particles_;
};



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



// This grammar describes which DataVector expressions
// are allowed;
struct DataVectorGrammar
		: proto::or_<
		  proto::terminal<_>
		, proto::plus< DataVectorGrammar, DataVectorGrammar >
		, proto::minus< DataVectorGrammar, DataVectorGrammar >
		, proto::multiplies< DataVectorGrammar, DataVectorGrammar >
		, proto::divides< DataVectorGrammar, DataVectorGrammar >
		>
		{};

	template<typename PARTICLES, typename CONDITIONAL, typename ARG>
		  typename proto::result_of::make_expr<
			proto::tag::function,
			sum_fun,        // First child (by value)
			PARTICLES const &,
			CONDITIONAL const &,
			ARG const &
		  >::type const
		  sum(PARTICLES const & particles, CONDITIONAL const & conditional, ARG const & arg)
	{
		return proto::make_expr<proto::tag::function>(
				sum_fun()    // First child (by value)
				, boost::ref(particles)   // Second child (by reference)
				, boost::ref(conditional)
				, boost::ref(arg)
		);
	}
		// Define a lazy pow() function for the calculator EDSL.
		// Can be used as: pow< 2 >(_1)
		template< int Exp, typename Arg >
		typename proto::result_of::make_expr<
		    proto::tag::function  // Tag type
		  , sum_fun< Exp >        // First child (by value)
		  , Arg const &           // Second child (by reference)
		>::type const
		pow(Arg const &arg)
		{
		    return proto::make_expr<proto::tag::function>(
		        pow_fun<Exp>()    // First child (by value)
		      , boost::ref(arg)   // Second child (by reference)
		    );
		}



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
			template<typename ParticleType>
			typename proto::result_of::eval<Expr const, DataVectorSubscriptCtx const>::type
			eval_particle_index( std::size_t i , const ParticleType& particles) const
			{
				DataVectorSubscriptCtx<ParticleType> ctx(i, particles);
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
			proto::value(*this).set(i,expr.eval_particle_index(i,proto::value(*this).get_particles()));
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
};


template< int I, typename ParticlesType>
DataVectorSymbolic<I,ParticlesType> get_vector(ParticlesType &p) {
	return DataVectorSymbolic<I,ParticlesType>(p);
}

}



#endif /* SYMBOLIC_H_ */
