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


#ifndef PARTICLE_CONTAINER_H_
#define PARTICLE_CONTAINER_H_

#include <cxxtest/TestSuite.h>

#include "Aboria.h"

using namespace Aboria;


class ParticleContainerTest : public CxxTest::TestSuite {
public:
    template<template <typename,typename> class V, template <typename> class SearchMethod>
    void helper_add_particle1(void) {
    	typedef Particles<std::tuple<>,3,V,SearchMethod> Test_type;
    	Test_type test;
    	typename Test_type::value_type p;
    	test.push_back(p);
    	TS_ASSERT_EQUALS(test.size(),1);
    }

    template<template <typename,typename> class V, template <typename> class SearchMethod>
    void helper_add_particle2(void) {
        ABORIA_VARIABLE(scalar,double,"scalar")
        typedef std::tuple<scalar> variables_type;
    	typedef Particles<variables_type,3,V,SearchMethod> Test_type;
    	Test_type test;
    	typename Test_type::value_type p;
    	test.push_back(p);
    	TS_ASSERT_EQUALS(test.size(),1);
    }

    template<template <typename,typename> class V, template <typename> class SearchMethod>
    void helper_add_particle2_dimensions(void) {
        ABORIA_VARIABLE(scalar,double,"scalar")
        typedef std::tuple<scalar> variables_type;
    	typedef Particles<variables_type,6,V,SearchMethod> Test_type;
    	Test_type test;
    	typename Test_type::value_type p;
        typedef Vector<double,6> double6;
        typedef position_d<6> position;
        get<position>(p) = double6(2.0);
    	test.push_back(p);
    	TS_ASSERT_EQUALS(test.size(),1);
        TS_ASSERT(((double6)get<position>(test[0])==double6(2.0)).all());
    }

    template<template <typename,typename> class V, template <typename> class SearchMethod>
    void helper_add_delete_particle(void) {
        ABORIA_VARIABLE(scalar,double,"scalar")
        typedef std::tuple<scalar> variables_type;
    	typedef Particles<variables_type,3,V,SearchMethod> Test_type;
    	Test_type test;
    	typename Test_type::value_type p;
    	test.push_back(p);
    	test.pop_back();
    	TS_ASSERT_EQUALS(test.size(),0);
    	test.push_back(p);
    	test.push_back(p);
    	test.push_back(p);
    	TS_ASSERT_EQUALS(test.size(),3);
    	test.erase(test.begin());
    	TS_ASSERT_EQUALS(test.size(),2);
    	test.erase(test.begin(),test.end());
    	TS_ASSERT_EQUALS(test.size(),0);
    }

    void test_std_vector_bucket_search_serial(void) {
        helper_add_particle1<std::vector,bucket_search_serial>();
        helper_add_particle2<std::vector,bucket_search_serial>();
        helper_add_particle2_dimensions<std::vector,bucket_search_serial>();
        helper_add_delete_particle<std::vector,bucket_search_serial>();
    }

    void test_std_vector_bucket_search_parallel(void) {
        helper_add_particle1<std::vector,bucket_search_parallel>();
        helper_add_particle2<std::vector,bucket_search_parallel>();
        helper_add_particle2_dimensions<std::vector,bucket_search_parallel>();
        helper_add_delete_particle<std::vector,bucket_search_parallel>();
    }

    void test_thrust_vector(void) {
#ifdef HAVE_THRUST
        helper_add_particle1<thrust::device_vector>();
        helper_add_particle2<thrust::device_vector>();
        helper_add_particle2_dimensions<thrust::device_vector>();
        helper_add_delete_particle<thrust::device_vector>();
#endif
    }

};




#endif /* PARTICLE_CONTAINER_H_ */
