/*
 * Variable.h
 *
 *  Created on: 23 Feb 2015
 *      Author: robinsonm
 */

#ifndef VARIABLE_H_ 
#define VARIABLE_H_ 

namespace Aboria {

template<typename T, typename NAME>
class Variable {
    static const char *name = NAME::name;
    typedef T value_type;

    T & value() { return m_data; }

    void set(const T &arg) { m_data = arg; }

    Variable<T,NAME> & operator=(const T &arg) {
        m_data = arg;
        return *this;
    }

    template <typename OTHER_T, typename OTHER_NAME> 
    Variable<T,NAME> & operator=(const Variable<OTHER_T,OTHER_NAME> &arg) {
        BOOST_MPL_ASSERT_MSG(
            is_convertable<OTHER_T,T>::value
            , CANNOT_CONVERT_BETWEEN_VARIABLE_DATA_TYPES 
            , (OTHER_T,T)
            );
        m_data = arg.value();
        return *this;
    }

private:
    T m_data;

};

#define ABORIA_VARIABLE(NAME,DATA_TYPE,NAME_STRING)        \
        struct NAME_description {                          \
            static const char *name = NAME_STRING;         \
        }                                                  \
        typedef Variable<DATA_TYPE,NAME_description> NAME; \

ABORIA_VARIABLE(position,Vect3d,"position")
ABORIA_VARIABLE(alive,bool,"is alive")
ABORIA_VARIABLE(id,size_t,"id")

}
#endif /* VARIABLE_H_ */
