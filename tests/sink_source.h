/*
 * sink_source.h
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

#ifndef SINKSOURCETEST_H_
#define SINKSOURCETEST_H_


#include <cxxtest/TestSuite.h>

#define LOG_LEVEL 1
#include "Aboria.h"

using namespace Aboria;


class SinkSourceTest : public CxxTest::TestSuite {
public:
    void test_create_n_particles(void) {
        ABORIA_VARIABLE(scalar,double,"scalar")
    	typedef Particles<scalar> ParticlesType;
    	ParticlesType particles;

    	auto s = get_vector<scalar>(particles);
    	auto p = get_vector<position>(particles);
		
        Normal N;
		VectorSymbolic<double> vector;		

        sink(p) + source(2) = Vect3d(0.3,0.2,0.1); 

       	TS_ASSERT_EQUALS(particles.size(),2);
       	TS_ASSERT_EQUALS(get<position>(particles[0])[0],0.3);
       	TS_ASSERT_EQUALS(get<position>(particles[0])[1],0.2);
       	TS_ASSERT_EQUALS(get<position>(particles[0])[2],0.1);
       	TS_ASSERT_EQUALS(get<position>(particles[1])[0],0.3);
       	TS_ASSERT_EQUALS(get<position>(particles[1])[1],0.2);
       	TS_ASSERT_EQUALS(get<position>(particles[1])[2],0.1);

        sink(p) + source(2) = Vect3d(1.3,1.2,1.1); 

        TS_ASSERT_EQUALS(particles.size(),4);
       	TS_ASSERT_EQUALS(get<position>(particles[2])[0],1.3);
       	TS_ASSERT_EQUALS(get<position>(particles[2])[1],1.2);
       	TS_ASSERT_EQUALS(get<position>(particles[2])[2],1.1);
       	TS_ASSERT_EQUALS(get<position>(particles[3])[0],1.3);
       	TS_ASSERT_EQUALS(get<position>(particles[3])[1],1.2);
       	TS_ASSERT_EQUALS(get<position>(particles[3])[2],1.1);

        sink(p-Vect3d(0.3,0.2,0.1)) + source(1) = Vect3d(1.3,1.2,1.1); 

        TS_ASSERT_EQUALS(particles.size(),5);
       	TS_ASSERT_EQUALS(get<position>(particles[4])[0],1.0);
       	TS_ASSERT_EQUALS(get<position>(particles[4])[1],1.0);
       	TS_ASSERT_EQUALS(get<position>(particles[4])[2],1.0);

    	ParticlesType particles2;
    	auto s2 = get_vector<scalar>(particles2);
    	auto p2 = get_vector<position>(particles2);

        sink(p-Vect3d(0.3,0.2,0.1),p2) + source(1) = Vect3d(1.3,1.2,1.1); 

        TS_ASSERT_EQUALS(particles.size(),6);
        TS_ASSERT_EQUALS(particles2.size(),1);
       	TS_ASSERT_EQUALS(get<position>(particles[5])[0],1.0);
       	TS_ASSERT_EQUALS(get<position>(particles[5])[1],1.0);
       	TS_ASSERT_EQUALS(get<position>(particles[5])[2],1.0);
       	TS_ASSERT_EQUALS(get<position>(particles2[0])[0],1.3);
       	TS_ASSERT_EQUALS(get<position>(particles2[0])[1],1.2);
       	TS_ASSERT_EQUALS(get<position>(particles2[0])[2],1.1);

    }
    void test_create_univariate_particles(void) {
        ABORIA_VARIABLE(scalar,double,"scalar")
    	typedef Particles<scalar> ParticlesType;
    	ParticlesType particles;

    	auto s = get_vector<scalar>(particles);
    	auto p = get_vector<position>(particles);
		
        Normal N;
		VectorSymbolic<double> vector;		

        sink(p) + source(2) = Vect3d(0.3,0.2,0.1); 

       	TS_ASSERT_EQUALS(particles.size(),2);
       	TS_ASSERT_EQUALS(get<position>(particles[0])[0],0.3);
       	TS_ASSERT_EQUALS(get<position>(particles[0])[1],0.2);
       	TS_ASSERT_EQUALS(get<position>(particles[0])[2],0.1);
       	TS_ASSERT_EQUALS(get<position>(particles[1])[0],0.3);
       	TS_ASSERT_EQUALS(get<position>(particles[1])[1],0.2);
       	TS_ASSERT_EQUALS(get<position>(particles[1])[2],0.1);

        sink(p) + source(s==s) = p+1;

        TS_ASSERT_EQUALS(particles.size(),4);
       	TS_ASSERT_EQUALS(get<position>(particles[2])[0],1.3);
       	TS_ASSERT_EQUALS(get<position>(particles[2])[1],1.2);
       	TS_ASSERT_EQUALS(get<position>(particles[2])[2],1.1);
       	TS_ASSERT_EQUALS(get<position>(particles[3])[0],1.3);
       	TS_ASSERT_EQUALS(get<position>(particles[3])[1],1.2);
       	TS_ASSERT_EQUALS(get<position>(particles[3])[2],1.1);

        sink(p) + source(p[0]<0.0) = p;

        TS_ASSERT_EQUALS(particles.size(),4);

        sink(p+2) + source(p[0]>1.0) = p;

        TS_ASSERT_EQUALS(particles.size(),6);
       	TS_ASSERT_EQUALS(get<position>(particles[4])[0],3.3);
       	TS_ASSERT_EQUALS(get<position>(particles[4])[1],3.2);
       	TS_ASSERT_EQUALS(get<position>(particles[4])[2],3.1);
       	TS_ASSERT_EQUALS(get<position>(particles[5])[0],3.3);
       	TS_ASSERT_EQUALS(get<position>(particles[5])[1],3.2);
       	TS_ASSERT_EQUALS(get<position>(particles[5])[2],3.1);
    }
};

#endif /* SINKSOURCETEST_H_ */
