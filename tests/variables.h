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


#ifndef VARIABLES_H_ 
#define VARIABLES_H_ 

#include <cxxtest/TestSuite.h>

#define LOG_LEVEL 1
#include "Aboria.h"

using namespace Aboria;


class VariablesTest: public CxxTest::TestSuite {
public:
    void testScalar(void) {

        ABORIA_VARIABLE(a,double,"a")
        ABORIA_VARIABLE(b,float,"b")
        ABORIA_VARIABLE(c,int,"c")

        Particles<std::tuple<a,b,c>>::value_type p;
        get<a>(p) = 1.2;
        get<b>(p) = 1.3;
        get<c>(p) = 1;

        TS_ASSERT_DELTA(get<a>(p),1.2,std::numeric_limits<double>::epsilon());
        TS_ASSERT_DELTA(get<b>(p),1.3,std::numeric_limits<float>::epsilon());
        TS_ASSERT_EQUALS(get<c>(p),1);

        get<a>(p) = 2.2;
        get<b>(p) = 2.3;
        get<c>(p) = 2;

        TS_ASSERT_DELTA(get<a>(p),2.2,std::numeric_limits<double>::epsilon());
        TS_ASSERT_DELTA(get<b>(p),2.3,std::numeric_limits<float>::epsilon());
        TS_ASSERT_EQUALS(get<c>(p),2);

    }

    void testVector(void) {

        ABORIA_VARIABLE(a,double3,"a")
        ABORIA_VARIABLE(b,int3,"b")

        Particles<std::tuple<a,b>>::value_type p;

        get<a>(p) = double3(1.1,1.2,1.3);
        get<b>(p) = double3(1,2,3);

        std::cout << "get<a>(p) = " << get<a>(p) <<std::endl;
        TS_ASSERT((get<a>(p)==double3(1.1,1.2,1.3)).all());
        TS_ASSERT((get<b>(p)==int3(1,2,3)).all());

        get<a>(p) = double3(2.1,2.2,2.3);
        get<b>(p) = int3(2,3,4);

        TS_ASSERT((get<a>(p)==double3(2.1,2.2,2.3)).all());
        TS_ASSERT((get<b>(p)==int3(2,3,4)).all());
    }

};


#endif /* CONSTRUCTORS_H_ */
