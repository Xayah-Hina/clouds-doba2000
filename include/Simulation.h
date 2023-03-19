#pragma once
#include <stdio.h>
#include "define.h"
#include <vector>

class Simulation
{
public:
	Simulation(int xSize, int ySize);
	virtual ~Simulation(void);

	void seed(int num);
	void tick();
	void render();
	void renderEllipsoids();

private:
	//Variables:
	int xSize;
	int ySize;

	float minP;
	float maxP;

	// Using 1D arrays with unsigned longs for storage of the three simulation properties of each cell
	// where each bit of the unsigned long is the bool of the z index, so currently limted to 32 deep
	unsigned long *hum; // Vapor/humidity
	unsigned long *cld; // Cloud
	unsigned long *act; // Phase transition factors

	float **Phum; // Probability of humidity
	float **Pext; // Probability of extinction
	float **Pact; // Probability of pahse transition

	std::vector<Ellipsoid> Ellipsoids;

	//Methods:
	static Triplet functionOffset[11];

	void setupProbabilities();

	bool inBounds(Triplet point);
	void smoothCld();
	
	void humTransition(unsigned long *oldhum, unsigned long *oldact, Triplet point);
	void cldTransition(unsigned long *oldcld, unsigned long *oldact, Triplet point);
	void actTransition(unsigned long *oldhum, unsigned long *oldact, Triplet point);
	bool functionTransition(unsigned long *oldact, Triplet point);

	void humExtinction(unsigned long *oldhum, float **Phum, Triplet point);
	void cldExtinction(unsigned long *oldcld, float **Pext, Triplet point);
	void actExtinction(unsigned long *oldact, float **Pact, Triplet point);

	void setXYZ(unsigned long *a, Triplet point, bool bitVal);
	bool atXYZ(unsigned long *a, Triplet point);
	bool getBit(unsigned long n, int position);
	float random();
};

