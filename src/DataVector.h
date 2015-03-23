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

template<typename ParticlesType>
class DataVectorBase {
public:
	DataVectorBase():
		particles(particles)
	{};
	void set_particles(Particles *p) {
		particles = p;
	}
	ParticlesType *get_particles() {
		return particles;
	}
	std::size_t size() {
		return particles->size();
	}
protected:
	ParticlesType *particles;
};

template<int I, typename ParticlesType>
class DataVector: public DataVectorBase<ParticlesType> {
public:
	DataVector(ParticlesType &particles):
		DataVectorBase<ParticlesType>(particles)
	{};
	typename Elem<I, ParticlesType>::type&
	operator []( std::size_t i ) {
		return this->particles[i].template get_elem<I>();
	}
};

template<typename ParticlesType>
class DataVector<POSITION,ParticlesType>: public DataVectorBase<ParticlesType> {
public:
	DataVector(ParticlesType &particles):
		DataVectorBase<ParticlesType>(particles)
	{};
	typename Elem<POSITION, ParticlesType>::type&
	operator []( std::size_t i ) {
		return this->particles[i].get_position();
	}
};

template<typename ParticlesType>
class DataVector<ID,ParticlesType>: public DataVectorBase<ParticlesType> {
public:
	DataVector(ParticlesType &particles):
		DataVectorBase<ParticlesType>(particles)
	{};
	typename Elem<ID, ParticlesType>::type&
	operator []( std::size_t i ) {
		return this->particles[i].get_id();
	}
};

template<typename ParticlesType>
class DataVector<ALIVE,ParticlesType>: public DataVectorBase<ParticlesType> {
public:
	DataVector(ParticlesType &particles):
		DataVectorBase<ParticlesType>(particles)
	{};
	typename Elem<ALIVE, ParticlesType>::type&
	operator []( std::size_t i ) {
		return this->particles[i].get_alive();
	}
};




}
#endif /* DATAVECTOR_H_ */
