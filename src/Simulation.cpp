#include "Simulation.h"
#include <stdio.h>
#include <cstdlib>
#include "define.h"
#include <GL/glut.h>
#include <time.h>
#include <cmath>
#include <vector>


Simulation::Simulation(int x, int y)
{
	xSize = x;
	ySize = y;

	minP = 0.0001;
	maxP = 0.1;

	hum = new unsigned long[xSize * ySize];
	cld = new unsigned long[xSize * ySize];
	act = new unsigned long[xSize * ySize];

	Phum = new float*[xSize * ySize];
	Pext = new float*[xSize * ySize];
	Pact = new float*[xSize * ySize];

	for(int i = 0; i < xSize * ySize; i++){
		Phum[i] = new float[sizeof(unsigned long)*8];
		Pext[i] = new float[sizeof(unsigned long)*8];
		Pact[i] = new float[sizeof(unsigned long)*8];
	}


	for(int i = 0; i < xSize * ySize; i++){
		hum[i] = 0;
		cld[i] = 0;
		act[i] = 0;
	}

	
	srand (time (NULL));
	


	seed(10);
	
	setupProbabilities();


	for(int i = 0; i < 1; i++){
		for(int j = 0; j < 1; j++){
			for(int k = 0; k< 32; k++){
				Triplet point = {i, j, k};
				printf("(%d, %d, %d) \tPext: %f\n", i, j, k, Pext[point.x*point.y][point.z]);
				printf("\t\tPhum: %f\n", Phum[point.x*point.y][point.z]);
				printf("\t\tPact: %f\n\n", Pact[point.x*point.y][point.z]);
			}
		}
	}
}

Simulation::~Simulation(void)
{
	if(hum!=NULL){
		delete[] hum;
	}
	if(cld!=NULL){
		delete[] cld;
	}
	if(act!=NULL){
		delete[] act;
	}


	if(Phum!=NULL){
		delete[] act;
	}
	if(Pext!=NULL){
		delete[] act;
	}
	if(Pact!=NULL){
		delete[] act;
	}
}

// Sets n random bits in the cld to 1
void Simulation::seed(int num){
	for(int i = 0; i < num; i++){
		Triplet randomPoint = {rand()%xSize, rand()%ySize, rand()%(sizeof(unsigned long)*8)};
		setXYZ(hum, randomPoint, 1);
		//printf("Set randomPoint (%d, %d, %d) to 1", randomPoint.x, randomPoint.y, randomPoint.z);
		//printf(" is: %d\n",atXYZ(hum, randomPoint));
	}

	for(int i = 0; i < num; i++){
		Triplet randomPoint = {rand()%xSize, rand()%ySize, rand()%(sizeof(unsigned long)*8)};
		//Triplet randomPoint = {i, i, i};
		setXYZ(act, randomPoint, 1);
		//printf("Set randomPoint (%d, %d, %d) to 1", randomPoint.x, randomPoint.y, randomPoint.z);
		//printf(" is: %d\n",atXYZ(act, randomPoint));
	}

	for(int i = 0; i<2; i++){
		Ellipsoid e = {static_cast <float> (rand()%xSize), static_cast <float> (rand()%ySize), static_cast <float> (rand()%(sizeof(unsigned long)*8)), static_cast <float> (rand()%5+2), static_cast <float> (rand()%5+2), static_cast <float> (rand()%5+2)};
		//Ellipsoid e = {1, 1, 16, 8, 8, 8};
		Ellipsoids.push_back(e);
		//printf("(%f, %f, %f), (%f, %f, %f)\n", e.x, e.y, e.z, e.a, e.b, e.c);
		//printf("(%f, %f, %f)\n", Ellipsoids.back().x, Ellipsoids.back().y, Ellipsoids.back().z);
		//printf("%d", Ellipsoids.size());
	}
}


// Smooths the binary distribution of the cld array in preperation of rendering
void Simulation:: smoothCld(){
	//NOTE: Don't change cld array as will change the next tick cycle
}



void Simulation::render(){
	glColor3f(1.0f,1.0f,1.0f);

	smoothCld();
	glPushMatrix();
	for(int i = 0; i < xSize; i++){
		glPushMatrix();
		glTranslatef(i,0,0); 
		for(int j = 0; j < ySize; j++){
			glPushMatrix();
			glTranslatef(0,j,0);
			for(int k = 0; k < sizeof(unsigned long)*8; k++){
				glPushMatrix();
				glTranslatef(0,0,k);
				Triplet point = {i, j, k};
				
				if(atXYZ(cld, point)){	
					glutSolidSphere(0.5, 10, 10); 
					//printf("Sphere drawn at (%d,%d,%d)\n", i, j, k);
				}
				glPopMatrix();
			}
			glPopMatrix();
		}
		glPopMatrix();
	}
	glPopMatrix();
}

void Simulation::renderEllipsoids(){
	//Ellipsoids for testing purposes
	for(int l=0; l < Ellipsoids.size(); l++){
		Ellipsoid e = Ellipsoids.at(l);
		glColor3f(1.0f,0.0f,0.0f);

		glPushMatrix();
			glTranslatef(e.x,e.y, e.z);
			//glScalef(e.a, e.b, e.c);
			glutWireSphere(1, 10, 10); 
		glPopMatrix();
	}
}


// ticks the simulation on one tick/timeframe
void Simulation::tick(){
	//printf("tick\n");
	
	unsigned long *oldhum = hum;
	unsigned long *oldcld = cld;
	unsigned long *oldact = act;
	

	//	Transition Rules
	//		hum = 1 means the vapor amount is enough to form a cloud
	//		act = 1 means the phase transition from vapor to water is ready to occur
	//		cld = 1 means that the cell has a cloud particle

	//	Cloud extinction
	//		Phum is larger toward the center of the ellipsoid	0.1 at center
	//		Pact is larger toward the center of the ellipsoid	0.1 at center
	//		Pext is larger toward the edge of the ellipsoid		0.001 at center
	for(int i = 0; i < xSize; i++){
		for(int j = 0; j < ySize; j++){
			for(int k = 0; k < sizeof(unsigned long)*8; k++){ //TODO change out for whole 32bits at once
				Triplet point = {i, j, k};

				//Transition rules
				humTransition(oldhum, oldact, point);
				cldTransition(oldcld, oldact, point);
				actTransition(oldhum, oldact, point);


				//Cloud extinction rules
				humExtinction(oldhum, Phum, point);
				cldExtinction(oldcld, Pext, point);
				actExtinction(oldact, Pact, point);
			}
		}
	}


	//printf("End of tick()\n");
}

// Calculates and updates hum according to the Vapor/humidity transition equation:
//		hum(i, j, k, t+1) =  hum(i, j, k, t) & !act(i, j, k, t)
void Simulation::humTransition(unsigned long *oldhum, unsigned long *oldact, Triplet point){
	bool a = atXYZ(oldhum, point);	//where a = hum(i, j, k, t)
	bool b = atXYZ(oldact, point);	//where b = act(i, j, k, t)
	if(a && !b){
		setXYZ(hum, point, 1);
	} else {
		setXYZ(hum, point, 0);
	}
}

// Calculates and updates cld according to the cloud transition equation:
//		cld(i, j, k, t+1) =  cld(i, j, k, t)	||  act(i, j, k, t)
void Simulation::cldTransition(unsigned long *oldcld, unsigned long *oldact, Triplet point){
	bool a = atXYZ(oldcld, point);	//where a = cld(i, j, k, t)
	bool b = atXYZ(oldact, point);	//where b = act(i, j, k, t)

	if(a || b){
		setXYZ(cld, point, 1);
	} else {
		setXYZ(cld, point, 0);
	}
}

// Calculates and updates cld according to the cloud transition equation:
//		act(i, j, k, t+1) = -act(i, j, k, t)	&  hum(i, j, k, t)	& f(i, j, k)
void Simulation::actTransition(unsigned long *oldhum, unsigned long *oldact, Triplet point){
	//printf("Start of actTransition\n");
	bool a = atXYZ(oldact, point);				//where a = act(i, j, k, t)
	bool b = atXYZ(oldhum, point);				//where b = hum(i, j, k, t)
	bool c = functionTransition(oldact, point);	//where c = f(i, j, k)
	//("actTransition\n\tBefore: a = %d, b= %d, c = %d\n", a, b, c);


	if(!a && b && c){
		//printf("Set to 1");
		setXYZ(act, point, 1);
		//printf(" %d\n", atXYZ(act, point));
	} else {
		setXYZ(act, point, 0);
	}
}


Triplet Simulation::functionOffset[11] = {	{1,0,0},  {0,1,0},  {0,0,1}, {-1,0,0}, {0,-1,0}, {0,0,-1},
										{-2,0,0}, {0,-2,0}, {0,0,-2},
										{2,0,0},  {0,2,0} };
// Calculates the binary function used in the act transition equation according to:
//		f(i, j, k) = act(i+1, j, k, t)	||	act(i, j+1, k, t)	
//			 || act(i, j, k+1, t)	||	act(i-1, j, k, t)	||	act(i, j-1, k, t)
//			 || act(i, j, k-1, t)	||	act(i-2, j, k, t)	||	act(i+2, j, k, t)
//			 || act(i, j-2, k, t)	||	act(i, j+2, k, t)	||	act(i, j, k-2, t)
bool Simulation::functionTransition(unsigned long *oldact, Triplet point){
	for(int i = 0; i < 11; i++){
		// functionOffset has all of the different offsets for each of the neighbouring cells
		Triplet newPoint = {(point.x+functionOffset[i].x), (point.y+functionOffset[i].y), (point.z+functionOffset[i].z)};
		
		if(inBounds(newPoint)){
			//printf("newPoint = (%d, %d, %d) = inBounds:%d, atXYZ:%d\n",newPoint.x, newPoint.y, newPoint.z, inBounds(newPoint), atXYZ(act, newPoint));
			if(atXYZ(act, newPoint)){
				// printf("newPoint = (%d, %d, %d) = inBounds:%d, atXYZ:%d\n",newPoint.x, newPoint.y, newPoint.z, inBounds(newPoint), atXYZ(act, newPoint));
				return 1;
			}
		}
	}	
	return 0;
}

//Calculates and accounts for hum extinction
//		hum(i, j, k, t+1) = hum(i, j, k, t) | is (rnd < Phum(i, j, k, t))
void Simulation::humExtinction(unsigned long *oldhum, float **Phum, Triplet point){
	bool a = atXYZ(oldhum, point);			// Where a =  hum(i, j, k, t)
	float randomNum = random();
	bool b = randomNum < Phum[point.x*point.y][point.z];	// Where b = rnd < Phum(i, j, k, t)

	if(a || b){
		//printf("(%d, %d, %d) humExt occured: %f %d %d\n", point.x, point.y, point.z, Phum[point.x*point.y][point.z], a, b);
		//printf("%f < %f\n", randomNum, Phum[point.x*point.y][point.z]);
		setXYZ(hum, point, 1);
	} 
}

// If cld is 1, rnd probability of extinction
//		 cld(i, j, k, t+1) = cld(i, j, k, t) & is (rnd > Pext(i, j, k, t))
void Simulation::cldExtinction(unsigned long *oldcld, float **Pext, Triplet point){
	bool a = atXYZ(oldcld, point);
	float randomNum = random();
	bool b = randomNum < Pext[point.x*point.y][point.z];

	if(a && b){
		//printf("(%d, %d, %d) cld occured: %f %d %d\n", point.x, point.y, point.z, Pext[point.x*point.y][point.z], a, b);
		//printf("%f < %f\n", randomNum, Pext[point.x*point.y][point.z]);
		setXYZ(cld, point, 0);
	}
}


//		act(i, j, k, t+1) = act(i, j, k, t) | is (rnd < Pact(i, j, k, t))
void Simulation::actExtinction(unsigned long *oldact, float **Pact, Triplet point){
	bool a = atXYZ(oldact, point);
	bool b = random() < Pact[point.x*point.y][point.z];

	if(a || b){
		//printf("(%d, %d, %d) act occured %f %d %d\n", point.x, point.y, point.z, Pact[point.x*point.y][point.z], a, b);
		setXYZ(act, point, 1);
	}
}

//Calculates the probability look up tables for Phum, Pext and Pact. Changed based upon the ellipsoids
// controlling the clouds and the point's position within the ellipsoid.
void Simulation::setupProbabilities(){
	//printf("in setupProbs\n");
	for(int i = 0; i < xSize; i++){
		for(int j = 0; j < ySize; j++){
			for(int k = 0; k < sizeof(unsigned long)*8; k++){
				//Default values if not in ellipse, otherwise value overwritten
				Phum[i*j][k] = minP;
				Pext[i*j][k] = maxP;
				Pact[i*j][k] = minP;
				for(int l=0; l < Ellipsoids.size(); l++){
					Ellipsoid e = Ellipsoids.at(l);
					//Is it in the ellipsoids bounding rectangle?
					if(i > (e.x-e.a) && i < (e.x+e.a) &&	
						   j > (e.y-e.b) && j < (e.y+e.b) &&
						   k > (e.z-e.c) && k < (e.z+e.c))	{
							  // printf("inside of ellipsoid\n");
						//Is it inside of the ellipsoid and how close to the center is it? 1 =  on the edge, 0 = centre
						//(x/a)2 + (y/b)2 + (z/c)2 = 1 general equation for ellipsoid
						float result = powf((i - e.x)/e.a,2) + powf((j - e.y)/e.b,2) + powf((k -e.z)/e.c,2);
						if(result <= 1){
							//Inside the ellipsoid
							// probability = result * maxP + minP; for extinction swap max and min for hum and act
							//((x - a) / (b-a) * (b' -a') +a'
							Pext[i*j][k] = result * (maxP - minP) + minP;
							Phum[i*j][k] = result * (minP - maxP) + maxP;
							Pact[i*j][k] = result * (minP - maxP) + maxP;

							break; // Found an ellipsoid the cell is in, no need to check others - could be more accurate here but not found to have an impact
						}
					}
				}
			}
		}
	}	
}

// Is the point in bounds?
bool Simulation::inBounds(Triplet point){
	//printf("Checking inBounds of (%d, %d, %d)", point.x, point.y, point.z);
	if(point.x >= 0 && point.x < xSize && point.y >=0 && point.y < ySize && point.z >=0 && point.z <=sizeof(unsigned long)*8){
		//printf(", in bounds.\n");
		return 1;
	} else {
		//printf(", out of bounds.\n");
		return 0;
	}
}

// Sets the bit in array a at location (x,y,z) to boolVal
void Simulation:: setXYZ(unsigned long *a, Triplet point, bool bitVal){
	//printf("Setting (%d, %d, %d) to %d\n", point.x, point.y, point.z, bitVal);
	unsigned long groupOfBits = a[point.y*ySize + point.x];
	if (bitVal==1) {
		//printf("Group of bits before setting: %d\n", groupOfBits);
		groupOfBits |= 1 << point.z; //set the bit
		//printf("Group of bitsafer setting: %d\n", groupOfBits);
	} else {
		groupOfBits &= ~(1 << point.z);	 //clear the bit
	}
	a[point.y*ySize + point.x] = groupOfBits;
}

// Returns the bit for the array a for the given (x,y,z) coordinates
bool Simulation:: atXYZ(unsigned long *a, Triplet point){
	return getBit(a[point.y*ySize + point.x], point.z);
}

// Returns bit from unsigned long n at position
bool Simulation::getBit(unsigned long n, int position){
	return (n >> position) & 0x1;
}

//Returns a random float between 0 and 1
float Simulation::random(){
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}