/*
 * symbolic.h
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


#include <cxxtest/TestSuite.h>

#define LOG_LEVEL 1
#include "Aboria.h"

using namespace Aboria;


class SymbolicTest : public CxxTest::TestSuite {
public:
    void test_create_double_vector(void) {
    	typedef Particles<std::tuple<double> > ParticlesType;
    	ParticlesType particles;
    	auto theDouble = particles.get_vector<0>();
    }

    void test_create_default_vectors(void) {
    	typedef Particles<std::tuple<double> > ParticlesType;
    	ParticlesType particles;
    	auto position = particles.get_sym<POSITION>();
    	auto id = particles.get_vector<ID>();
    	auto alive = particles.get_vector<ALIVE>();
    }

    void test_transform(void) {
    	typedef Particles<std::tuple<double> > ParticlesType;
    	ParticlesType particles;
    	auto theDouble = particles.get_vector<0>();
    	auto position = particles.get_vector<POSITION>();
    	auto id = particles.get_vector<ID>();
    	auto alive = particles.get_vector<ALIVE>();

    	execute(theDouble = 0);

    	ParticlesType::value_type p;
    	particles.push_back(p);
    	particles.push_back(p);

    	execute(theDouble = 0);

    	TS_ASSERT_EQUALS(particles[0].get_data_elem<0>(),0);
    	TS_ASSERT_EQUALS(particles[1].get_data_elem<0>(),0);

    	execute(theDouble = 1);

    	TS_ASSERT_EQUALS(particles[0].get_data_elem<0>(),1);
    	TS_ASSERT_EQUALS(particles[1].get_data_elem<0>(),1);

    	execute(position = Vect3d(1,2,3));

    	TS_ASSERT_EQUALS(particles[0].get_position()[0],1);
    	TS_ASSERT_EQUALS(particles[0].get_position()[1],2);
    	TS_ASSERT_EQUALS(particles[0].get_position()[2],3);
    }

    void test_transform_multiple(void) {
    	typedef Particles<std::tuple<double> > ParticlesType;
    	ParticlesType particles;
    	auto theDouble = particles.get_sym<0>();
    	auto position = particles.get_sym<POSITION>();
    	auto id = particles.get_sym<ID>();
    	auto alive = particles.get_sym<ALIVE>();

    	ParticlesType::value_type p;
    	particles.push_back(p);
    	particles.push_back(p);

    	execute([
    	         theDouble = 2,
    	         position = Vect3d(1,2,3)
    	        ]);

    	TS_ASSERT_EQUALS(particles[0].get_data_elem<0>(),2);
    	TS_ASSERT_EQUALS(particles[1].get_data_elem<0>(),2);

    	TS_ASSERT_EQUALS(particles[0].get_position()[0],1);
    	TS_ASSERT_EQUALS(particles[0].get_position()[1],2);
    	TS_ASSERT_EQUALS(particles[0].get_position()[2],3);

    	execute([
    	         theDouble = 1,
    	         position = Vect3d(theDouble,theDouble*3,theDouble*theDouble)
    	         ]);

    	TS_ASSERT_EQUALS(particles[0].get_data_elem<0>(),1);
    	TS_ASSERT_EQUALS(particles[1].get_data_elem<0>(),1);

    	TS_ASSERT_EQUALS(particles[0].get_position()[0],2);
    	TS_ASSERT_EQUALS(particles[0].get_position()[1],6);
    	TS_ASSERT_EQUALS(particles[0].get_position()[2],4);

    }


};

#endif /* SYMBOLIC_H_ */
