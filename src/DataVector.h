/*
 * DataVector.h
 *
 *  Created on: 23 Feb 2015
 *      Author: robinsonm
 */

#ifndef DATAVECTOR_H_
#define DATAVECTOR_H_

#include "Particle.h"


namespace Aboria {

template<int I,typename DataType>
class DataVector {
	DataVector(Particles<DataType> &particles):
		particles(particles)
	{};
	typename Particles<DataType>::data_elem<I>::type
	operator []( std::size_t i ) const {
		return particles[i].get_data_elem<I>();
	}
	Particles<DataType> &get_particles() {
		return particles;
	}
private:
	Particles<DataType> &particles;
};

}
#endif /* DATAVECTOR_H_ */
