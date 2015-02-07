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
