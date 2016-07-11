#ifndef FACCOCOEVO_KLHI6465DFUZ85DTZH
#define FACCOCOEVO_KLHI6465DFUZ85DTZH

#include "Evolution.h"
#include "BlobInd.h"

#define pPeriInd shared_ptr<PeriInd>

class BlobEvo : public Evolution {
public:
	void init();
	void init(string name);
	void checkpoint(uint step);
	void read(string name);
private:
	uint id;
	string evoName;
};

#endif