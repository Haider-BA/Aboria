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
#include "Vector.h"


#include <boost/mpl/bool.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/proto/core.hpp>
#include <boost/proto/context.hpp>
#include <boost/proto/traits.hpp>
#include <boost/proto/transform.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/ice.hpp>
#include <type_traits>
#include <tuple>
//#include <boost/fusion/include/vector.hpp>
//#include <boost/fusion/include/as_vector.hpp>
//#include <boost/fusion/include/joint_view.hpp>
//#include <boost/fusion/include/single_view.hpp>

namespace mpl = boost::mpl;
namespace proto = boost::proto;
using proto::_;
using proto::N;





namespace Aboria {

template<typename I>
struct label_ {
	typedef I depth;
};


template<typename Expr>
struct DataVectorExpr;

template<typename Expr>
struct LabelExpr;

template<typename Expr>
struct GeometryExpr;



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
// are allowed;#include <tuple>
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
  	  proto::terminal<_>
  	  , proto::and_<
              	  	  proto::nary_expr<_, proto::vararg<DataVectorGrammar> >
            		, proto::not_<AssignOps>
  	  	  	  	   >
  >

{};

  struct LabelGrammar
    : proto::or_<
        proto::when< proto::terminal<_>
                    , proto::_value >
        , proto::when< 
                    _ 
    				, LabelGrammar(proto::_right) >
      >
  {};

    struct SubscriptGrammar
       : proto::when< proto::terminal<label_<_> >
                       , proto::_value >
     {};

	template <typename T>
	  struct geometry_ {
		typedef T result_type;

        template <typename... A>
		result_type operator()(const A&... args) const
		{
			return T(args...);
		}


	};


    template <typename... T>
    struct particles_list_ {
        typedef mpl::int_<sizeof...(T)> order;

        generator_(const T&... args):args(args){};

        std::tuple< const T&... > args;
    };

/*
    template<>
    struct generator_<int> {
        typedef mpl::int_<0> order;

        generator_(unsigned int n):n(n){};

        int n;
    };
    */

	template <typename T>
	  struct geometries_ {
		typedef T result_type;
		
		//result_type operator()(const Vect3d& centre, const double radius, const bool in) const
        template <typename... A>
		result_type operator()(const A&... args) const
		{
			return T(args...);
		}
	};


      template <typename T>
      struct accumulate_ {
          typedef T functor_type;
          accumulate_() {};
          accumulate_(const T& functor):functor(functor) {};
          T functor;
      };

    struct GeometryGrammar
    	: proto::function< proto::terminal< geometry_<_> >, DataVectorGrammar, DataVectorGrammar, DataVectorGrammar>
		{}; 

	struct GeometriesGrammar
    	: proto::function< proto::terminal< geometries_<_> >, LabelGrammar, DataVectorGrammar>
		{}; 


    //TODO: should make a "conditional" grammar to match 2nd arguement
	struct AccumulateGrammar
    	: proto::function< proto::terminal< accumulate_<_> >, LabelGrammar, DataVectorGrammar, DataVectorGrammar>
		{}; 


	struct AccumulateInitGrammar
    	: proto::function< proto::terminal< accumulate_<_> >, LabelGrammar, DataVectorGrammar, DataVectorGrammar, DataVectorGrammar>
		{}; 


      template <typename T>
	  struct vector_ {
		typedef Vector<T,3> result_type;
		
		result_type operator()(const T arg1,const T arg2,const T arg3) const
		{
			return result_type(arg1,arg2,arg3);
		}
	};

  	    struct dx_ {};
  	  struct normal_ {};

  	  template<typename ParticlesType>
  	  struct ParticleCtx;

	    // Here is an evaluation context that indexes into a lazy vector
		// expression, and combines the result.
		template<typename ParticlesType1, typename ParticlesType2>
		struct TwoParticleCtx
		{
			TwoParticleCtx(const Vect3d& dx, const typename ParticlesType1::value_type& particle1, const typename ParticlesType2::value_type& particle2)
			: dx_(dx),particle1_(particle1),particle2_(particle2)
			{}

			template<
			typename Expr
			// defaulted template parameters, so we can
			// specialize on the expressions that need
			// special handling.
			, typename Tag = typename proto::tag_of<Expr>::type
	        , typename Enable = void
			>
			struct eval: proto::default_eval<Expr, TwoParticleCtx const>
			{};


			template<typename Expr>
			struct eval<Expr, proto::tag::terminal,
                typename boost::enable_if<proto::matches<Expr, proto::terminal<DataVector<_,_> > > >::type 
                >
			{

                typedef typename proto::result_of::value<Expr>::type::variable_type variable_type;
                typedef const typename proto::result_of::value<Expr>::type::value_type& result_type;

				result_type operator ()(Expr &expr, TwoParticleCtx const &ctx) const
				{
					return get<variable_type>(ctx.particle1_);
				}
			};

			// Handle subscripted expressions here...
			template<typename Expr>
			struct eval<Expr, proto::tag::subscript,
				typename boost::enable_if<proto::matches<typename proto::result_of::child_c<Expr,1>::type,SubscriptGrammar> >::type 
                >
			{

				typedef typename proto::result_of::child_c<Expr, 1>::type subscript_type;

				BOOST_MPL_ASSERT(( proto::matches< subscript_type, SubscriptGrammar > ));

				typedef typename boost::result_of<SubscriptGrammar(subscript_type)>::type result_of_subscript_grammar;
				typedef typename std::remove_reference<result_of_subscript_grammar>::type::depth subscript_depth;

				BOOST_MPL_ASSERT_RELATION( subscript_depth::value , < , 2);

				typedef typename proto::result_of::child_c<Expr,0>::type expr_to_subscript;


                typedef typename mpl::vector<ParticlesType1,ParticlesType2> ParticlesTypes;

                typedef typename mpl::at<ParticlesTypes,subscript_depth>::type ParticlesType;

			    typedef typename proto::result_of::eval<expr_to_subscript const, ParticleCtx<ParticlesType> const>::type result_type;
				
                result_type operator ()(Expr &expr, TwoParticleCtx const &ctx) const
				{
                    auto particles = std::tie(ctx.particle1_,ctx.particle2_);
					ParticleCtx<ParticlesType> const single_ctx(std::get<subscript_depth::value>(particles));
					return proto::eval(proto::child_c<0>(expr), single_ctx);
					//return proto::child_c<0>(expr).eval<ParticlesType1>(ctx.particle1_);
				}
			};


			// Handle dx terminals here...
			template<typename Expr>
			struct eval<Expr, proto::tag::terminal, 
                typename boost::enable_if<proto::matches<Expr, proto::terminal<dx_> > >::type 
                >
			{
				typedef const Vect3d& result_type;

				result_type operator ()(Expr &expr, TwoParticleCtx const &ctx) const
				{
					return ctx.dx_;
				}
			};

			// Handle normal terminals here...
			template<typename Expr>
			struct eval<Expr, proto::tag::terminal,
                typename boost::enable_if<proto::matches<Expr, proto::terminal<normal_> > >::type 
                >
			{
				typedef double result_type;

				result_type operator ()(Expr &expr, TwoParticleCtx const &ctx) const
				{
					return const_cast<typename ParticlesType1::value_type&>(ctx.particle1_).rand_normal();
				}
			};

			const typename ParticlesType1::value_type& particle1_;
			const typename ParticlesType2::value_type& particle2_;
			const Vect3d& dx_;

		};

// Here is an evaluation context that indexes into a lazy vector
// expression, and combines the result.
template<typename ParticlesType>
struct ParticleCtx
{
	ParticleCtx(const typename ParticlesType::value_type& particle)
      : particle_(particle)
    {}

	template<
	        typename Expr
	        // defaulted template parameters, so we can
	        // specialize on the expressions that need
	        // special handling.
	      , typename Tag = typename proto::tag_of<Expr>::type
	      , typename Enable = void
	    >
	struct eval: proto::default_eval<Expr, ParticleCtx const>
						{};



	// Handle unlabeled vector terminals here...
	template<typename Expr>
	struct eval<Expr, proto::tag::terminal, 
        typename boost::enable_if<proto::matches<Expr, proto::terminal<DataVector<_,_> > > >::type 
	> 
    {
        typedef typename proto::result_of::value<Expr>::type::variable_type variable_type;
        typedef const typename proto::result_of::value<Expr>::type::value_type& result_type;
				
        result_type operator ()(Expr &expr, ParticleCtx const &ctx) const
	    {
	        return get<variable_type>(ctx.particle_);
	    }
	};

	// Handle normal terminals here...
	template<typename Expr>
	struct eval<Expr, proto::tag::terminal,
        typename boost::enable_if<proto::matches<Expr, proto::terminal<normal_ > > >::type 
	    >
	{
	    typedef double result_type;

	    result_type operator ()(Expr &expr, ParticleCtx const &ctx) const
	    {
            //TODO: get better (parallel) random number generator
	        return const_cast<typename ParticlesType::value_type&>(ctx.particle_).rand_normal();
	    }
	};

	// Handle subscripts here...
	template<typename Expr>
	struct eval<Expr, proto::tag::subscript,
        typename boost::enable_if<proto::matches<typename proto::result_of::child_c<Expr,1>::type,SubscriptGrammar> >::type 
	    >
	{
        typedef typename proto::result_of::child_c<Expr, 1>::type subscript_type;
	    typedef typename boost::result_of<SubscriptGrammar(subscript_type)>::type result_of_subscript_grammar;
	    typedef typename std::remove_reference<result_of_subscript_grammar>::type::depth subscript_depth;

	    BOOST_MPL_ASSERT_RELATION( subscript_depth::value, == , 0 );

	    typedef typename proto::result_of::child_c<Expr, 0>::type ExprToSubscript;

	    typedef typename proto::result_of::eval<ExprToSubscript const, ParticleCtx<ParticlesType> const>::type result_type;

	    result_type operator ()(Expr &expr, ParticleCtx const &ctx) const
    	{
            return proto::child_c<0>(expr).eval(ctx.particle_);
		}
	};

    template<typename Expr>
    struct eval<Expr, proto::tag::bitwise_or,
        typename boost::enable_if<proto::matches<typename proto::result_of::child_c<Expr,1>::type,GeometriesGrammar> >::type 
    >
    {

        typedef Vect3d result_type;

	    result_type operator ()(Expr &expr, ParticleCtx const &ctx) const
	    {
	        typedef typename proto::result_of::child_c<Expr,1>::type geometry_expr_type;
			typedef typename proto::result_of::child_c<geometry_expr_type,0>::type geometry_functor_terminal_type;
			typedef typename proto::result_of::value<geometry_functor_terminal_type>::type geometry_functor_type;
			typedef typename geometry_functor_type::result_type geometry_type;
			geometry_expr_type geometry_expr = proto::child_c<1>(expr);
			Vect3d vector = proto::eval(proto::child_c<0>(expr),ctx);
			typedef typename proto::result_of::child_c<geometry_expr_type,1>::type label_expr_type;
			typedef typename proto::result_of::child_c<geometry_expr_type,2>::type arg1_expr_type;
			typedef typename boost::result_of<LabelGrammar(label_expr_type)>::type particles_type_ref;
			typedef typename std::remove_reference<particles_type_ref>::type particles_type;
			particles_type_ref particlesb = LabelGrammar()(proto::child_c<1>(geometry_expr));
			arg1_expr_type arg1_expr = proto::child_c<2>(geometry_expr);
            //std::cout << "doing reflect for particle "<<get<id>(ctx.particle_)<<std::endl;
            bool keep_going = true;
			while (keep_going) {
			    keep_going = false;	
                //std::cout << "searching around position = "<<vector + get<position>(ctx.particle_)<<std::endl;
			    for (auto i: particlesb.get_neighbours(vector + get<position>(ctx.particle_))) {
                    //std::cout << "doing neighbour "<<get<id>(std::get<0>(i))<<std::endl;
				    TwoParticleCtx<ParticlesType,particles_type> ctx2(std::get<1>(i),ctx.particle_,std::get<0>(i));
				    geometry_type geometry(vector-ctx2.dx_,proto::eval(arg1_expr,ctx2),true);
                    //std::cout <<"result of evaluating geometry is "<<geometry<<std::endl;
					if (reflect_once(Vect3d(0,0,0),vector,geometry)) 
                    {
                        keep_going = true;
                    }
			    }
		    }
	        return vector;
	    }
	};


    // Handle sums here...
    template<typename Expr>
	struct eval<Expr, proto::tag::function,
		typename boost::enable_if<proto::matches<Expr,AccumulateInitGrammar> >::type 
	>
	{
	    typedef typename proto::result_of::child_c<Expr,0>::type child0_type;
		typedef typename proto::result_of::child_c<Expr,1>::type child1_type;
		typedef typename proto::result_of::child_c<Expr,2>::type child2_type;
		typedef typename proto::result_of::child_c<Expr,3>::type child3_type;
		typedef typename proto::result_of::child_c<Expr,4>::type child4_type;

		typedef typename boost::result_of<LabelGrammar(child1_type)>::type particles_type_ref;
        typedef typename std::remove_reference<particles_type_ref>::type particles_type;
		typedef typename proto::result_of::eval<child3_type const, TwoParticleCtx<ParticlesType,particles_type> const>::type result_type_const_ref;
		typedef typename std::remove_const<typename std::remove_reference<result_type_const_ref>::type>::type  result_type;
        typedef typename proto::result_of::value<child0_type>::type functor_terminal_type;
        typedef typename functor_terminal_type::functor_type functor_type;


		result_type operator ()(Expr &expr, ParticleCtx const &ctx) const
		{
			particles_type_ref particlesb = LabelGrammar()(proto::child_c<1>(expr));
			result_type sum = proto::eval(proto::child_c<4>(expr),ctx);
            functor_type accumulate = proto::value(proto::child_c<0>(expr)).functor;
			for (auto i: particlesb.get_neighbours(get<position>(ctx.particle_))) {
				TwoParticleCtx<ParticlesType,particles_type> ctx2(std::get<1>(i),ctx.particle_,std::get<0>(i));
				if (proto::eval(proto::child_c<2>(expr),ctx2)) {
                    sum = accumulate(sum,proto::eval(proto::child_c<3>(expr),ctx2));
				}
            }
		    return sum;
	    }
	};

    template<typename Expr>
	struct eval<Expr, proto::tag::function,
	    typename boost::enable_if<proto::matches<Expr,AccumulateGrammar> >::type 
	    >
	{
		typedef typename proto::result_of::child_c<Expr,0>::type child0_type;
		typedef typename proto::result_of::child_c<Expr,1>::type child1_type;
		typedef typename proto::result_of::child_c<Expr,2>::type child2_type;
		typedef typename proto::result_of::child_c<Expr,3>::type child3_type;

		typedef typename boost::result_of<LabelGrammar(child1_type)>::type particles_type_ref;
        typedef typename std::remove_reference<particles_type_ref>::type particles_type;
		typedef typename proto::result_of::eval<child3_type const, TwoParticleCtx<ParticlesType,particles_type> const>::type result_type_const_ref;
		typedef typename std::remove_const<typename std::remove_reference<result_type_const_ref>::type>::type  result_type;
        typedef typename proto::result_of::value<child0_type>::type functor_terminal_type;
        typedef typename functor_terminal_type::functor_type functor_type;


		result_type operator ()(Expr &expr, ParticleCtx const &ctx) const
		{
	        particles_type_ref particlesb = LabelGrammar()(proto::child_c<1>(expr));
			result_type sum = 0;
            functor_type accumulate = proto::value(proto::child_c<0>(expr)).functor;
			for (auto i: particlesb.get_neighbours(get<position>(ctx.particle_))) {
				TwoParticleCtx<ParticlesType,particles_type> ctx2(std::get<1>(i),ctx.particle_,std::get<0>(i));
				if (proto::eval(proto::child_c<2>(expr),ctx2)) {
                    sum = accumulate(sum,proto::eval(proto::child_c<3>(expr),ctx2));
				}
            }
		    return sum;
		}
	};

	const typename ParticlesType::value_type& particle_;
};






// Tell proto how to generate expressions in the DataVectorDomain
struct DataVectorDomain
		: proto::domain<proto::generator<DataVectorExpr>, DataVectorGrammar >
		{};

// Declare that phoenix_domain is a sub-domain of spirit_domain
 struct LabelDomain 
        : proto::domain<proto::generator<LabelExpr>, LabelGrammar, DataVectorDomain>
        {};

 struct GeometryDomain 
        : proto::domain<proto::generator<GeometryExpr>, GeometryGrammar>
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
			typename proto::result_of::eval<Expr const, ParticleCtx<ParticleType> const>::type
			eval( const typename ParticleType::value_type& particle) const
			{
				ParticleCtx<ParticleType> const ctx(particle);
				return proto::eval(*this, ctx);
			}

			template<typename ParticleType1, typename ParticleType2>
			typename proto::result_of::eval<Expr const, TwoParticleCtx<ParticleType1,ParticleType2> const>::type
			eval( const Vect3d& dx, const typename ParticleType1::value_type& particle1,  const typename ParticleType2::value_type& particle2) const
			{
				TwoParticleCtx<ParticleType1,ParticleType2> const ctx(dx, particle1, particle2);
				return proto::eval(*this, ctx);
			}
};

template<typename Expr>
struct GeometryExpr: proto::extends<Expr, GeometryExpr<Expr>, GeometryDomain>
{
	explicit GeometryExpr(Expr const &expr)
		: proto::extends<Expr, GeometryExpr<Expr>, GeometryDomain>(expr)
		{}
};



template<typename Expr>
struct LabelExpr: proto::extends<Expr, LabelExpr<Expr>, LabelDomain>
{
	explicit LabelExpr(Expr const &expr)
		: proto::extends<Expr, LabelExpr<Expr>, LabelDomain>(expr)
		{}


    BOOST_PROTO_EXTENDS_USING_ASSIGN(LabelExpr)
};


template<unsigned int I>
struct Label 
    : LabelExpr<typename proto::terminal<label_<mpl::int_<I> > >::type> {

	typedef typename proto::terminal<label_<mpl::int_<I> > >::type expr_type;
    typedef label_<mpl::int_<I> > data_type;

	explicit Label()
		: LabelExpr<expr_type>( expr_type::make(data_type()))
	  	{}


    BOOST_PROTO_EXTENDS_USING_ASSIGN(Label)
};




struct Dx
    : proto::terminal<dx_>::type {};

struct Normal
    : proto::terminal<normal_>::type {};

template <typename T>
struct GeometrySymbolic
	: DataVectorExpr<typename proto::terminal<geometry_<T> >::type> {

	typedef typename proto::terminal<geometry_<T> >::type expr_type;
    typedef geometry_<T> data_type;

	explicit GeometrySymbolic()
	: DataVectorExpr<expr_type>( expr_type::make(data_type()) )
	  {}

};

template <typename T>
struct VectorSymbolic
	: DataVectorExpr<typename proto::terminal<vector_<T> >::type> {

	typedef typename proto::terminal<vector_<T> >::type expr_type;
    typedef vector_<T> data_type;

	explicit VectorSymbolic()
	: DataVectorExpr<expr_type>( expr_type::make(data_type()) )
	  {}

};

template <typename T>
struct Accumulate
	: DataVectorExpr<typename proto::terminal<accumulate_<T> >::type> {

	typedef typename proto::terminal<accumulate_<T> >::type expr_type;
    typedef accumulate_<T> data_type;

    explicit Accumulate()
	: DataVectorExpr<expr_type>( expr_type::make(data_type()) )
	  {}

	explicit Accumulate(const T& arg)
	: DataVectorExpr<expr_type>( expr_type::make(data_type(arg)) )
	  {}
};


template <typename T>
struct GeometriesSymbolic
	: DataVectorExpr<typename proto::terminal<geometries_<T> >::type> {

	typedef typename proto::terminal<geometries_<T> >::type expr_type;
    typedef geometries_<T> data_type;

	explicit GeometriesSymbolic()
	: DataVectorExpr<expr_type>( expr_type::make(data_type()) )
	  {}

};


template<typename T, typename ParticlesType>
struct DataVectorSymbolic
	: DataVectorExpr<typename proto::terminal<DataVector<T,ParticlesType> >::type> {

	typedef typename proto::terminal<DataVector<T,ParticlesType> >::type expr_type;
	typedef typename ParticlesType::value_type particle_type;
	typedef typename T::value_type value_type;

	explicit DataVectorSymbolic(ParticlesType &p)
	: DataVectorExpr<expr_type>( expr_type::make(DataVector<T,ParticlesType>(p)) )
	  {}


	template< typename Expr >
	DataVectorSymbolic &operator =(Expr const & expr) {
        BOOST_MPL_ASSERT_NOT(( boost::is_same<T,id > ));
		return this->assign(proto::as_expr<DataVectorDomain>(expr));
	}

	template< typename Expr >
	DataVectorSymbolic &operator +=(Expr const & expr) {
        BOOST_MPL_ASSERT_NOT(( boost::is_same<T,id > ));
		return this->increment(proto::as_expr<DataVectorDomain>(expr));
	}


private:

    void post() {
		ParticlesType &particles = proto::value(*this).get_particles();

        for (int i=0; i<particles.size(); i++) {
	        set<T>(particles[i],buffer[i]);	
	    }

        if (boost::is_same<T,position>::value) {
            particles.update_positions();
        }

        if (boost::is_same<T,alive>::value) {
            particles.delete_particles();
        }
    }


	template< typename Expr >
	DataVectorSymbolic &assign(Expr const & expr)
	{
		ParticlesType &particles = proto::value(*this).get_particles();

		buffer.resize(particles.size());

		//TODO: if vector to assign to does not exist in depth > 0, then don't need buffer
		for (int i=0; i<particles.size(); i++) {
			buffer[i] =  expr.template eval<ParticlesType>(particles[i]);	
		}

		post();
        
        return *this;
	}

	template< typename Expr >
	DataVectorSymbolic &increment(Expr const & expr)
	{
		ParticlesType &particles = proto::value(*this).get_particles();

		buffer.resize(particles.size());

		//TODO: if vector to assign to does not exist in depth > 0, then don't need buffer
		for (int i=0; i<particles.size(); i++) {
			buffer[i] = get<T>(particles[i]) + expr.template eval<ParticlesType>(particles[i]);	
		}

        post();	

		return *this;
	}

	std::vector<value_type> buffer;
};

template<typename T, typename ParticlesType>
DataVectorSymbolic<T,ParticlesType> get_vector(ParticlesType &p) {
	return DataVectorSymbolic<T,ParticlesType>(p);
}


struct no_position_terminal:
    proto::or_<
        proto::and_<
            proto::terminal<_>,
            proto::not_<terminal<DataVector<position,_> >
        >
      , proto::nary_expr< proto::_, proto::vararg< no_datavector_terminal > >
    >

struct with_position_terminal:
    proto::or_<
        proto::when<
            proto::terminal<DataVector<position,_>,
            , proto::_value
        >
        , proto::when< 
            proto::not_<proto::or_<
                proto::terminal<_>
                , proto::nary_expr<_, proto::vararg< no_position_terminal > > 
            > >
            , proto::_state
        >
    >


struct create_particle {
    create_particle(const Vect3d& p):p(p) {};
    template<typename Expr>
    void operator()(Expr const & offset_expr) const {
        T::value_type new_particle(p);
        set<position>(new_particle,offset_expr.template eval<ParticlesType>(new_particle));
        with_position_terminal(offset_expr).get_particles().push_back(p);
    }
    const Vect3d& p;
}

struct const_expr:
    proto::or_<
        proto::and_<
            proto::terminal<_>
            , proto::not_<proto::terminal< DataVector<_,_> > >
        >
        , proto::nary_expr<_, proto::vararg<const_expr> >

struct get_particles_data: proto::callable {
    template<typename V, typename PT>
    PT &operator()(DataVector<V,PT> & arg) const {
        return arg.get_particles();
    }
};

struct univariate_expr:
    proto::or_<
        proto::when< 
            proto::terminal< DataVector<_,_> >
            , get_particles_data(proto::_value)
        >
        proto::when< 
            proto::unary_expr<_, univariate_expr>
            , univariate_expr(proto::_left)
        >
        proto::when< 
            proto::binary_expr<_, const_expr, univariate_expr>
            , univariate_expr(proto::_right)
        >
        proto::when< 
            proto::binary_expr<_, univariate_expr, const_expr>
            , univariate_expr(proto::_left)
        >
        proto::when< 
            proto::and_<
                proto::binary_expr<_, univariate_expr, univariate_expr>
                , boost::is_same<univariate_expr(proto::_left),univariate_expr(proto::_right)>
            >
            , univariate_expr(proto::_left)
        >
    >

struct bivariate_expr:
    proto::or_<
        proto::when< 
            proto::unary_expr<_, bivariate_expr>
            , bivariate_expr(proto::_left)
        >
        proto::when< 
            proto::binary_expr<_, const_expr, bivariate_expr>
            , bivariate_expr(proto::_right)
        >
        proto::when< 
            proto::binary_expr<_, bivariate_expr, const_expr>
            , bivariate_expr(proto::_left)
        >
        proto::when< 
            proto::and_<
                proto::binary_expr<_, univariate_expr, univariate_expr>
                , proto::not_<boost::is_same<univariate_expr(proto::_left),univariate_expr(proto::_right)> >
            >
            , std::make_pair(univariate_expr(proto::_left),univariate_expr(proto::_right))
        >
    >


template<typename P, typename R, class Enable = void>
struct SinkSourceSymbolic {};

template<typename P, typename R>

struct SinkSourceSymbolic<P, R, typename enable_if<proto::matches<R, bivariate_expr> >::type >
	: DataVectorExpr<typename proto::terminal<std::pair<P,R> >::type> {};

	typedef typename proto::terminal<std::pair<P,R> >::type expr_type;

	explicit SinkSourceSymbolic(P& arg1, R& arg2)
	: DataVectorExpr<expr_type>( expr_type::make(std::pair<P,R>(arg1,arg2)) )
	  {}

	template< typename Expr >
	SinkSourceSymbolic &operator =(Expr const & expr) {
		return this->assign(proto::as_expr<DataVectorDomain>(expr));
	}


private:

	template< typename Expr >
	SourceSymbolic &assign(Expr const & expr)
	{
		const R &reactants = proto::value(*this).second;
        P products = proto::value(*this).first;
        
        typedef typename proto::result_of<bivariate_expr(reactants)>::type::first_type reactant1_particles_type;
        typedef typename proto::result_of<bivariate_expr(reactants)>::type::second_type reactant2_particles_type;
        
        for (auto i: bivariate_expr(reactants).first) {
            for (auto j: bivariate_expr(reactants).second.get_neighbours(get<position>(i))) {
                if (reactants.template eval<reactant1_particles_type,reactant2_particles_type>(i,j)) {
                    const Vect3d base_position = expr.template eval<reactant1_particles_type,reactant2_particles_type>(i,j);
                    boost::fusion::for_each(products,base_position);
                }
            }
        }

        return *this;
	}

};

template<typename P, typename R>
struct SinkSourceSymbolic<P, R, typename enable_if<proto::matches<R, univariate_expr> >::type >
	: DataVectorExpr<typename proto::terminal<std::pair<P,R> >::type> {};

	typedef typename proto::terminal<std::pair<P,R> >::type expr_type;


	explicit SinkSourceSymbolic(P& arg1, R& arg2)
	: DataVectorExpr<expr_type>( expr_type::make(std::pair<P,reactant_type>(arg1,arg2)) )
	  {}

	template< typename Expr >
	SinkSourceSymbolic &operator =(Expr const & expr) {
		return this->assign(proto::as_expr<DataVectorDomain>(expr));
	}

private:

	template< typename Expr >
	SourceSymbolic &assign(Expr const & expr)
	{
		const R &reactant = proto::value(*this).second;
        P products = proto::value(*this).first;
        
        typedef typename proto::result_of<univariate_expr(reactant)>::type reactant_particles_type;
        
        for (auto & i: univariate_expr(reactant)) {
            if (reactant.template eval<reactant_particles_type>(i)) {
                const Vect3d base_position = expr.template eval<reactant_particles_type>(i);
                boost::fusion::for_each(products,base_position);
            }
        }

        return *this;
	}

};

template<typename P>
struct SinkSourceSymbolic<P,unsigned int>
	: DataVectorExpr<typename proto::terminal<std::pair<P,unsigned int> >::type> {};

    typedef unsigned int reactant_type
	typedef typename proto::terminal<std::pair<P,reactant_type> >::type expr_type;


	explicit SinkSourceSymbolic(P& arg1, reactant_type& arg2)
	: DataVectorExpr<expr_type>( expr_type::make(std::pair<P,reactant_type>(arg1,arg2)) )
	  {}

	template< typename Expr >
	SinkSourceSymbolic &operator =(Expr const & expr) {
		return this->assign(proto::as_expr<DataVectorDomain>(expr));
	}


private:

	template< typename Expr >
	SinkSourceSymbolic &assign(Expr const & expr)
	{
		const unsigned int n = proto::value(*this).second;
        P products = proto::value(*this).first;

        for(unsigned int i = 0; i < n; i++) {
            const Vect3d base_position = expr.template eval();
            boost::fusion::for_each(products,base_position);
        }
        
        return *this;
	}

};

template<typename ...Expr>
struct sink_ {
    sink_(Expr... expr):expr(expr) {};
    std::tuple<Expr...> data;
}

template<typename ...Expr>
sink_ sink(Expr... expr) {
    return sink_(expr);
}

template<typename Expr>
struct source_{
    sink_(Expr expr):expr(expr) {};
    Expr data;
}

template<typename Expr>
source_ source(Expr expr) {
    return source_(expr);
}

template<typename P, typename R>
SinkSourceSymbolic<P,R> operator+(sink_<P> products, source_<R> reactants) {
    return SinkSourceSymbolic<P,R>(products,reactants);
}


}



#endif /* SYMBOLIC_H_ */
