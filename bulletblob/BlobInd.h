#ifndef PERIIND_KJTZR7RUIHJRE5Z4UKFDUTE87I
#define PERIIND_KJTZR7RUIHJRE5Z4UKFDUTE87IT

#include "Individual.h"
#include "Simulation.h"

#define MUTATE_RATIO 4
#define CROSS_RATIO 1

class BlobInd : public Individual {
public:
	void randomInit();
	double getFitness();
	double getRealFitness();
	bool better(pIndividual ind, bool best = true);
	pIndividual mutate();
	Pair cross(pIndividual ind);
	void adjustFitness(double nc);
	double getDiff(pIndividual ind);
	void print(ostream& out);
	void print(string file);
	void read(istream& in);
private:
	double fitness, realFitness;
	bool fitnessOk;

	vector<Blob> blobs;
	uint len;
};

#endif