#include "Switch.h"

#ifdef MSIM

#include <iostream>
#include <iomanip>
#include "BlobEvo.h"
using namespace std;

int main() {
	/*Simulation simulation;
	for (uint i = 0; i<1000; i++) {
		simulation.step();
		if (!(i%100))
			cout << i << endl;
	}
	double dist = simulation.getDist();
	cout.setf(ios::fixed);
	cout << setprecision(2) << dist << endl;
	return 0;*/

	cout.setf(ios::fixed);
	BlobEvo blobEvo;
	EvolveSettings setup;
	setup.writeOut = false;
	blobEvo.init();
	blobEvo.evolve(setup);
}

#endif