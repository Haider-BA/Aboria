/*
 * DataVector.h
 *
 *  Created on: 23 Feb 2015
 *      Author: robinsonm
 */

#ifndef DATAVECTOR_H_
#define DATAVECTOR_H_

#include "Particles.h"

namespace Aboria {

template<int I, typename ParticlesType>
class DataVector {
	DataVector(ParticlesType &particles):
		particles(particles)
	{};
	typename Elem<I, ParticlesType>::type&
	operator []( std::size_t i ) {
		return get_elem<I, ParticlesType>(particles[i]);
	}
	ParticlesType &get_particles() {
		return particles;
	}
private:
	ParticlesType &particles;
};

template< int I, typename ParticlesType>
DataVector<I,ParticlesType> get_vector(ParticlesType &p) {
	return DataVector<I,ParticlesType>(p);
}

}
#endif /* DATAVECTOR_H_ */
