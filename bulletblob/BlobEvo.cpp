#include "BlobEvo.h"

void BlobEvo::init() {
	minstd_rand0 gen(chrono::system_clock::now().time_since_epoch().count());
	uniform_int_distribution<uint> dis(0, UINT_MAX);

	for (uint ti = 0; ti<THREADS; ti++) {
		for (uint i = 0; i < POPSIZE; i++) {
			BlobInd ind;
			ind.randomInit();

			pIndividual pInd = make_shared<BlobInd>(ind);
			Evolution::threadIndividuals[ti].push_back(pInd);
		}
		//cout << threadIndividuals[ti].size() << endl;
	}

	id = dis(gen);
	evoName = "eh";
}

/*void BlobEvo::init(string name) {
	sch.load(name);
	init(sch, name);
}*/

void BlobEvo::checkpoint(uint step) {
	BlobInd *best = (BlobInd*) individuals[getBest()].get();
	ostringstream bestName;
	bestName.setf(cout.setf(ios::fixed));
	bestName << setprecision(2);
	bestName << "best" << best->getFitness() << ".txt";
	string huehue = bestName.str();
	best->print(huehue);

	uint sz = individuals.size();
	ostringstream vname;
	vname << "pops/" << evoName << "_" << id%10000 << "_" << best->getFitness() << ".pop";

	ofstream out;
	out.open(vname.str(),ios::binary);
	out.write((char*)&sz,sizeof(uint));
	for (uint i = 0; i < individuals.size(); i++) {
		individuals[i]->print(out);
	}
	out.close();
}

void BlobEvo::read(string name) {
	ifstream in;
	in.open(name, ios::binary);
	if (in.fail())
		cout << "error opening file " << name << endl;

	uint sz;
	in.read((char*)&sz, sizeof(uint));

	//cout << info->cN << endl;
	for (uint ti = 0; ti < THREADS; ti++) {
		threadIndividuals[ti].clear();
	}
	for (uint i = 0; i < sz; i++) {
		//cout << i << endl;
		BlobInd ind;
		ind.read(in);

		uint ti = i%THREADS;
		pIndividual pInd = make_shared<BlobInd>(ind);
		Evolution::threadIndividuals[ti].push_back(pInd);
	}
	for (uint ti = 0; ti < THREADS; ti++) {
		uint tsz = Evolution::threadIndividuals[ti].size();
		if (!(tsz < POPSIZE))
			continue;
		uint sz2 = POPSIZE - tsz;
		for (uint i = 0; i < sz2; i++) {
			BlobInd ind;
			Evolution::threadIndividuals[ti].push_back(Evolution::threadIndividuals[ti][i%tsz]);
		}
	}
	in.close();
}