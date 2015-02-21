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

template<typename Expr>
struct particle_expression;

// Tell proto how to generate expressions in the particle_domain
struct particle_domain
  : proto::domain<proto::generator<particle_expression> >
{};

// Will be used to define the placeholders
template<int I> struct placeholder {};

// Define a particle context, for evaluating arithmetic expressions
struct particle_context
  : proto::callable_context< particle_context const >
{
    // The values bound to the placeholders
    double d[2];
    ParticleType& p;

    // The result of evaluating arithmetic expressions
    typedef double result_type;

    explicit particle_context(ParticleType &p): p(p) {}

    // Handle the evaluation of the placeholder terminals
    template<int I>
    ParticleType::get_data_elem_type<I> operator ()(proto::tag::terminal, placeholder<I>) const
    {
        return p.get_data_elem<I>();
    }
};

// Wrap all particle expressions in this type, which defines
// operator () to evaluate the expression.
template<typename Expr>
struct particle_expression
  : proto::extends<Expr, particle_expression<Expr>, particle_domain>
{
    explicit particle_expression(Expr const &expr = Expr())
      : particle_expression::proto_extends(expr)
    {}

    BOOST_PROTO_EXTENDS_USING_ASSIGN(particle_expression<Expr>)

    // Override operator () to evaluate the expression
    double operator ()() const
    {
        particle_context const ctx;
        return proto::eval(*this, ctx);
    }

    double operator ()(double d1) const
    {
        particle_context const ctx(d1);
        return proto::eval(*this, ctx);
    }

    double operator ()(double d1, double d2) const
    {
        particle_context const ctx(d1, d2);
        return proto::eval(*this, ctx);
    }
};

// Define some placeholders (notice they're wrapped in particle_expression<>)
particle_expression<proto::terminal< placeholder< 1 > >::type> const _1;
particle_expression<proto::terminal< placeholder< 2 > >::type> const _2;




#include <boost/proto/proto.hpp>
namespace proto = boost::proto;

namespace Aboria {

template<int I,typename DataType>
class Symbolic {
	Symbolic(Particles<DataType> *particles):
		particles(particles)
	{};
private:
	Particles<DataType> *particles;
};

template<typename T>
struct is_terminal
: mpl::false_
  {};

// OK, "matrix" is a custom terminal type
template<int I,typename DataType>
struct is_terminal<Symbolic<I,DataType> >
: mpl::true_
  {};

// Define all the operator overloads to construct Proto
// expression templates, treating "matrix" and "vector"
// objects as if they were Proto terminals.
BOOST_PROTO_DEFINE_OPERATORS(is_terminal, proto::default_domain)

}

#endif /* SYMBOLIC_H_ */
