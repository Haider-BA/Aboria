/*
 * variables.h
 * 
 * Copyright 2014 Martin Robinson
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
 *  Created on: 27 Nov 2014
 *      Author: robinsonm
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
        struct a_name {
            static const char *name = "a";
        }
        struct a_name {
            static const char *name = "b";
        }
        struct a_name {
            static const char *name = "c";
        }
        Variable<double,a_name> a(1);
        Variable<double,b_name> b(2.5);
        Variable<double,c_name> c = a + b;

        TS_ASSERT_EQUALS(c.value(),3.6);
        TS_ASSERT_EQUALS(a*b.value(),2.5);
        TS_ASSERT_EQUALS(a/b.value(),0.4);

        c.set(3.6);

        TS_ASSERT_EQUALS(c.value(),3.6);
    }

    void testVector(void) {
        struct a_name {
            static const char *name = "a";
        }
        struct a_name {
            static const char *name = "b";
        }
        struct a_name {
            static const char *name = "c";
        }
        Variable<Vect3d,a_name> a(Vect3d(1,1,1));
        Variable<Vect3d,b_name> b(Vect3d(2,2,2));
        Variable<Vect3d,c_name> c = a + b;

        TS_ASSERT_EQUALS(c.value(),Vect3d(3,3,3));
        TS_ASSERT_EQUALS(a*b.value(),Vect3d(2,2,2));
        TS_ASSERT_EQUALS(a/b.value(),Vect3d(0.5,0.5,0.5);

        c.set(Vect3d(3.6,3.6,3.6));

        TS_ASSERT_EQUALS(c.value(),Vect3d(3.6,3.6,3.6));
    }
};


#endif /* CONSTRUCTORS_H_ */
