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
#include <boost/mpl/assert.hpp>
#include <boost/proto/core.hpp>
#include <boost/proto/context.hpp>
#include <boost/proto/traits.hpp>
#include <boost/utility/enable_if.hpp>
//#include <boost/fusion/include/vector.hpp>
//#include <boost/fusion/include/as_vector.hpp>
//#include <boost/fusion/include/joint_view.hpp>
//#include <boost/fusion/include/single_view.hpp>

namespace mpl = boost::mpl;
namespace proto = boost::proto;
using proto::_;
using proto::N;

namespace proto {

namespace tag {
		struct sum_;
	}

template<typename ParticleType, typename CONDITIONAL, typename ARG>
typename proto::result_of::make_expr<
	tag::sum_,
	ParticleType const &,
	CONDITIONAL const &,
	ARG const &
>::type const
sum_(ParticleType const & particles,CONDITIONAL const & conditional, ARG const & arg)
{
	return proto::make_expr<tag::sum_>(
			particles,
			boost::ref(conditional),
		    boost::ref(arg)
	);
}

template<typename ParticleType, typename CONDITIONAL, typename ARG>
struct sum_ :  proto::transform< sum_<ParticleType, CONDITIONAL, ARG> > {
  // types
  typedef proto::expr< tag::sum_, proto::list3< ParticleType, CONDITIONAL, ARG> >       type;
  typedef proto::basic_expr< tag::sum_, proto::list3< ParticleType, CONDITIONAL, ARG> > proto_grammar;

  // member classes/structs/unions
  template<typename Expr, typename State, typename Data>
  struct impl :
     proto::pass_through<sum_>::template impl<Expr, State, Data>
  {
  };
};
}

namespace Aboria {

template<typename Expr>
struct DataVectorExpr;

struct null {
};

template<typename Value>
struct get_particles_ref : proto::callable
{

	typedef null result_type;

	result_type operator ()(const Value& arg)
	{
		return null();
	}
};

template<int I, typename DataType>
struct get_particles_ref<DataVector<I, DataType> > : proto::callable
{

	typedef const DataType &result_type;

	result_type operator ()(const DataVector<I,DataType> &arr)
	{
		return arr.get_particles();
	}
};

template<typename ToAdd, typename Existing>
struct particles_ref_accumulate : proto::callable
{
	typedef null result_type;

	result_type operator ()(ToAdd to_add, Existing existing) {
		return null;
	}
};

template<typename ToAdd>
struct particles_ref_accumulate<ToAdd,null> : proto::callable
{
	typedef ToAdd result_type;

	result_type operator ()(ToAdd to_add, null to_add) {
		return to_add;
	}
};

template<typename Existing>
struct particles_ref_accumulate<null,Existing> : proto::callable
{
	typedef Existing result_type;

	result_type operator ()(null to_add, Existing existing) {
		return existing;
	}
};

template<typename DataType1, typename DataType2>
struct particles_ref_accumulate<DataType1,DataType2> : proto::callable
{
	typedef DataType1 result_type;

	result_type operator ()(DataType1 existing, DataType2 to_add) {
		BOOST_MPL_ASSERT(( boost::is_same< DataType1,DataType2 > ));
		if (existing != to_add) {
			throw std::runtime_error("Expression not valid: conflicting Particles data structures");
		}
		return existing;
	}
};

// A grammar which matches all the assignment operators,
// so we can easily disable them.
struct AssignOps
  : proto::switch_<struct AssignOpsCases>
{};

// Here are the cases used by the switch_ above.
struct AssignOpsCases
{
    template<typename Tag, int D = 0> struct case_  : proto::not_<_> {};

    template<int D> struct case_< proto::tag::plus_assign, D >         : _ {};
    template<int D> struct case_< proto::tag::minus_assign, D >        : _ {};
    template<int D> struct case_< proto::tag::multiplies_assign, D >   : _ {};
    template<int D> struct case_< proto::tag::divides_assign, D >      : _ {};
    template<int D> struct case_< proto::tag::modulus_assign, D >      : _ {};
    template<int D> struct case_< proto::tag::shift_left_assign, D >   : _ {};
    template<int D> struct case_< proto::tag::shift_right_assign, D >  : _ {};
    template<int D> struct case_< proto::tag::bitwise_and_assign, D >  : _ {};
    template<int D> struct case_< proto::tag::bitwise_or_assign, D >   : _ {};
    template<int D> struct case_< proto::tag::bitwise_xor_assign, D >  : _ {};
};


// This grammar describes which DataVector expressions
// are allowed;
//struct DataVectorGrammar
//  : proto::or_<
//  	  proto::when<proto::terminal, get_particles_ref<proto::_value>(proto::_value)>
//  	  , proto::when<proto::sum_<DataVectorGrammar,DataVectorGrammar>, null()>
//  	  , proto::when<proto::and_<
//              	  	  proto::nary_expr<_, proto::vararg<DataVectorGrammar> >
//            		, proto::not_<AssignOps> >
//  	  	  	  	  	, proto::fold<_, void *(), particles_ref_accumulate<get_particles_ptrs,proto::_state>(get_particles_ptrs, proto::_state) > >
//    >
//{};

// This grammar describes which DataVector expressions
// are allowed;
struct DataVectorGrammar
  : proto::or_<
  	  proto::terminal
  	  , proto::and_<
              	  	  proto::nary_expr<_, proto::vararg<DataVectorGrammar> >
            		, proto::not_<AssignOps>
  	  	  	  	   >

{};


//struct DataVectorGrammar
//		: proto::or_<
//
//		  // DataVectorTerminals return their value
//		  proto::when< proto::terminal< DataVector<_,_> >
//				, fusion::single_view<proto::_value>(proto::_value) >
//
//		  // Any other terminals return nothing ...
//		  , proto::when< proto::terminal<_>
//				, fusion::nil() >
//
//		  , proto::when<proto::sum_<DataVectorGrammar,DataVectorGrammar>, fusion::nil()>
//
//		  // For any non-terminals, concat all children values
//		  , proto::when< proto::nary_expr<_, proto::vararg<_> >
//		  	  , proto::fold<_, fusion::nil()
//					, fusion::joint_view<DataVectorGrammer,boost::add_const<proto::_state> > (DataVectorGrammer, proto::_state)
//					>
//		>
//		{};

//struct ParticlesConsistentCtx
//: proto::callable_context< ParticlesConsistentCtx, proto::null_context >
//{
//	ParticlesConsistentCtx(void *particles) {
//		particles_ptrs.push_back(particles);
//	}
//
//	typedef void * result_type;
//	template<int I, typename DataType>
//	void operator ()(proto::tag::terminal, const DataVector<I,DataType> &arr)
//	{
//		DataType *this_particles_ptr = &(arr.get_particles());
//		for (void *i: particles_ptrs) {
//			if (this_particles_ptr != i) {
//				throw std::runtime_error("Expression not valid: Particles data structure " <<
//						this_particles_ptr << " not in list of valid pointers");
//			}
//		}
//	}
//
//	template<typename Expr>
//	void operator ()(tag::sum, Expr arr)
//	{
//		DataType *this_particles_ptr = &(arr.get_particles());
//		for (void *i: particles_ptrs) {
//			if (this_particles_ptr != i) {
//				throw std::runtime_error("Expression not valid: Particles data structure " <<
//						this_particles_ptr << " not in list of valid pointers");
//			}
//		}
//	}
//
//	void *particles_ptr;
//};

  	    struct dx;

		// Here is an evaluation context that indexes into a lazy vector
		// expression, and combines the result.
		template<typename ParticlesType1, typename ParticlesType2>
		struct TwoParticleCtx
		{
			TwoParticleCtx(const Vect3d& dx, const ParticlesType1::value_type& particle1, const ParticlesType1::value_type& particle2)
			: dx_(dx),particle1_(particle1),particle2_(particle2)
			{}

			template<
			typename Expr
			// defaulted template parameters, so we can
			// specialize on the expressions that need
			// special handling.
			, typename Tag = typename proto::tag_of<Expr>::type
			, typename Arg0 = typename Expr::proto_child0
			>
			struct eval: proto::default_eval<Expr, ParticleCtx const>
			{};

			// Handle vector terminals here...
			template<typename Expr, typename I, typename ParticlesType3>
			struct eval<Expr, proto::tag::terminal, DataVector<I,ParticlesType3> >
			{
				typedef typename Elem<I::value,ParticlesType3> result_type;

				BOOST_MPL_ASSERT_RELATION(( boost::is_same< ParticlesType3,ParticlesType1 > )
											,||, (boost::is_same< ParticlesType3,ParticlesType2 >));

				result_type operator ()(Expr &expr, TwoParticleCtx const &ctx) const
				{
					return get<I::value,ParticlesType3>(ctx.particle1_);
				}
			};

			// Handle dx terminals here...
			template<typename Expr>
			struct eval<Expr, proto::tag::terminal, dx >
			{
				typedef const Vect3d& result_type;

				result_type operator ()(Expr &expr, TwoParticleCtx const &ctx) const
				{
					return ctx.dx_;
				}
			};

			const ParticlesType1::value_type& particle1_;
			const ParticlesType1::value_type& particle2_;
			const Vect3d& dx_;

		};

// Here is an evaluation context that indexes into a lazy vector
// expression, and combines the result.
template<typename ParticlesType>
struct ParticleCtx
{
	ParticleCtx(const ParticlesType::value_type& particle)
      : particle_(particle)
    {}

	template<
	        typename Expr
	        // defaulted template parameters, so we can
	        // specialize on the expressions that need
	        // special handling.
	      , typename Tag = typename proto::tag_of<Expr>::type
	      , typename Arg0 = typename Expr::proto_child0
	    >
	struct eval: proto::default_eval<Expr, ParticleCtx const>
						{};



			// Handle vector terminals here...
			template<typename Expr, typename I, typename ParticlesType2>
			struct eval<Expr, proto::tag::terminal, DataVector<I,ParticlesType2> >
			{
				typedef typename Elem<I::value,ParticlesType2> result_type;

				BOOST_MPL_ASSERT(( boost::is_same< ParticlesType,ParticlesType2 > ));

				result_type operator ()(Expr &expr, ParticleCtx const &ctx) const
				{
					return get<I::value,ParticlesType2>(ctx.particle_);
				}
			};

			// Handle sums here...
			template<typename Expr, typename Arg0>
			struct eval<Expr, proto::tag::sum_, Arg0 >
			{
				typedef typename proto::result_of::eval<Expr::proto_child0 const, ParticleCtx const>::type particles_type;
				typedef typename proto::result_of::eval<Expr::proto_child1 const, TwoParticleCtx const>::type conditional_type;
				typedef typename proto::result_of::eval<Expr::proto_child2 const, TwoParticleCtx const>::type result_type;

				BOOST_MPL_ASSERT(( boost::is_same<conditional_type,bool > ));

				result_type operator ()(Expr &expr, ParticleCtx const &ctx) const
				{
					particles_type particlesb = proto::eval(proto::child<0>(expr),ctx);
					Expr::proto_child1 conditional = proto::child<1>(expr);
					Expr::proto_child2 arg = proto::child<2>(expr);
					result_type sum = 0;
					for (auto i: particlesb.find_neighbours(particle_.get_position())) {
						TwoParticleCtx ctx(i.get<1>(),i.get<0>(),particle_);
						if (proto::eval(conditional,ctx)) {
							sum += proto::eval(arg,ctx);
						}
					}
					return sum;
				}
				};

	const ParticlesType::value_type& particle_;
};






// Tell proto how to generate expressions in the DataVectorDomain
struct DataVectorDomain
		: proto::domain<proto::generator<DataVectorExpr>, DataVectorGrammar >
		{};

// Here is DataVectorExpr, which extends a proto expr type by
// giving it an operator [] which uses the ParticleCtx
// to evaluate an expression with a given index.
template<typename Expr>
struct DataVectorExpr
		: proto::extends<Expr, DataVectorExpr<Expr>, DataVectorDomain>
{
			explicit DataVectorExpr(Expr const &expr)
			: proto::extends<Expr, DataVectorExpr<Expr>, DataVectorDomain>(expr)
			  {}

			// Use the ParticleCtx to implement subscripting
			// of a DataVector expression tree.
			template<typename ParticleType>
			typename proto::result_of::eval<Expr const, ParticleCtx const>::type
			eval( const ParticleType::value_type& particle) const
			{
				ParticleCtx<ParticleType> ctx(particle);
				return proto::eval(*this, ctx);
			}

			template<typename ParticleType1, typename ParticleType2>
			typename proto::result_of::eval<Expr const, TwoParticleCtx const>::type
			eval( const Vect3d& dx, const ParticleType1::value_type& particle1,  const ParticleType2::value_type& particle2) const
			{
				TwoParticleCtx<ParticleType1,ParticleType2> ctx(dx, particle1, particle2);
				return proto::eval(*this, ctx);
			}
};


template<typename I, typename ParticlesType>
struct DataVectorSymbolic
	: DataVectorExpr<typename proto::terminal<DataVector<I,ParticlesType> >::type> {

	typedef typename proto::terminal<DataVector<I,ParticlesType> >::type expr_type;

	explicit DataVectorSymbolic(ParticlesType &p)
	: DataVectorExpr<expr_type>( expr_type::make(DataVector<I,ParticlesType>(p)) )
	  {}


	template< typename Expr >
	DataVectorSymbolic &operator =(Expr const & expr) {
		return this->assign(proto::as_expr<DataVectorDomain>(expr));
	}

private:

	template< typename Expr >
	DataVectorSymbolic &assign(Expr const & expr)
	{
		ParticlesType &particles = proto::value(*this).get_particles();
		std::for_each(particles.begin(),particles.end(),[](ParticlesType::value_type& i) {
			set<I::value,ParticlesType>(i,expr.eval(i));
		});
		return *this;
	}
};

template<typename ParticlesType>
struct DataVectorSymbolic<mpl::int_<ID>,ParticlesType>
	: DataVectorExpr<typename proto::terminal<DataVectr<I,ParticlesType> >::type> {

	typedef typename proto::terminal<DataVector<I,ParticlesType> >::type expr_type;

	explicit DataVectorSymbolic(ParticlesType &p)
	: DataVectorExpr<expr_type>( expr_type::make(DataVector<mpl::int_<ID>,ParticlesType>(p)) )
	  {}


};



template<int I, typename ParticlesType>
DataVectorSymbolic<mpl::int_<I>,ParticlesType> get_vector(ParticlesType &p) {
	return DataVectorSymbolic<mpl::int_<I>,ParticlesType>(p);
}

}



#endif /* SYMBOLIC_H_ */
