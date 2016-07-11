#ifndef INDIVIDUAL_KJHT7R565EDCJHFZTZ
#define INDIVIDUAL_KJHT7R565EDCJHFZTZ

#include <vector>
#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <random>
#include <float.h>
#include <chrono>
#include <map>
#include <sstream>
using namespace std;

#define uint unsigned int

#define POPSIZE 10
#define TOURNSIZE 2
#define THREADS 4

#define pIndividual shared_ptr<Individual>

class Individual;
struct Pair {
	pIndividual ind1;
	pIndividual ind2;
};

class Individual {
public:
	virtual void randomInit() = 0;
	virtual double getFitness() = 0;
	virtual double getRealFitness() = 0;
	virtual void adjustFitness(double nc) = 0;
	virtual double getDiff(pIndividual ind) = 0;
	virtual bool better(pIndividual ind, bool best = true) = 0;
	virtual pIndividual mutate() = 0;
	virtual Pair cross(pIndividual ind) = 0;
	virtual void print(ostream& out) = 0;
	virtual void read(istream& in) = 0;
protected:
};

#endif