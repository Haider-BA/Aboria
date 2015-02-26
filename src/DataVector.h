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

template<int I,typename DataType>
class DataVector {
	DataVector(Particles<DataType> &particles):
		particles(particles)
	{};
	typename ParticlesDataElem<DataType,I>::type&
	operator []( std::size_t i ) {
		return particles[i].template get_data_elem<I>();
	}
	Particles<DataType> &get_particles() {
		return particles;
	}
private:
	Particles<DataType> &particles;
};

}
#endif /* DATAVECTOR_H_ */
