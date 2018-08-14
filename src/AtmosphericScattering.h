#pragma once

#ifndef __ATMOSPHERIC_SCATTERING_H_
#define __ATMOSPHERIC_SCATTERING_H_

#include <glm/glm.hpp>

class Atmosphere {
public:
	//The center of the planet
	glm::vec3 center;
	//The distance from the center of the planet to the surface of the planet
	float surfaceRadius;
	//The distance from the center of the planet to the edge of the atmosphere, where the atmosphere's density is 0
	float atmosphereRadius;

	// Returns the density of the atmosphere at the given position. 
	// Returns 0 if the position is past the atmosphere radius
	// Returns -1 if the position is within the surface radius
	float density(glm::vec3 position);
};

class AtmosphericScattering {
protected:
	Atmosphere atmosphere;



public:
	AtmosphericScattering(const Atmosphere & atmosphere) : atmosphere(atmosphere){}

	void drawAtmosphere(const glm::mat4 &P, const glm::mat4 &V, float depthBuffer, const glm::vec3 & camPos);
};





#endif