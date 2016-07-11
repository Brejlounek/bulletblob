#include "Evolution.h"

/*void Evolution::init(string name) {

	ifstream file;
	file.open("data\\" + name);
	string strstr = "data\\" + name;

	file >> info.fN >> info.cN;
	for (int i = 0; i<info.fN; i++) {
		int cost_, cap_; double x, y;
		file >> cost_ >> cap_ >> x >> y;
		info.fX.push_back(x);
		info.fY.push_back(y);
		info.cost.push_back(cost_);
		info.cap.push_back(cap_);
	}
	for (int i = 0; i<info.cN; i++) {
		int demand_; double x, y;
		file >> demand_ >> x >> y;
		info.demand.push_back(demand_);
		info.cX.push_back(x);
		info.cY.push_back(y);
	}
	file.close();

	for (uint i = 0; i < POPSIZE; i++) {
		Individual ind;
		ind.randomInit(&info);
		individuals.push_back(ind);
	}
}*/

uint Evolution::select(uint ti, bool best) {
	minstd_rand0 gen(chrono::system_clock::now().time_since_epoch().count());
	uniform_int_distribution<uint> dis(0, UINT_MAX);

	vector<uint> id;
	for (uint i = 0; i < TOURNSIZE; i++) {
		id.push_back(dis(gen) % threadIndividuals[ti].size());
	}
	uint bestId = id[0];
	for (uint i = 0; i < TOURNSIZE; i++) {
		if (threadIndividuals[ti][id[i]]->better(threadIndividuals[ti][bestId], best))
			bestId = id[i];
	}

	return bestId;
}

void Evolution::evolve(EvolveSettings setup){
	minstd_rand0 gen(chrono::system_clock::now().time_since_epoch().count());
	uniform_int_distribution<uint> dis(0, UINT_MAX);
	auto start = chrono::steady_clock::now();

	uint evals = setup.evals;
	if (setup.steady) {
		uint evN = 0;
		while (evN < evals) {
			Pair pair = modify();

			uint worst;
			worst = getWorst();
			individuals[worst] = pair.ind1;
			worst = getWorst();
			individuals[worst] = pair.ind2;
			if (setup.evolveLimit)
				evN += 2;

			if (setup.debug && !(evN % setup.debugStep)) {
				cout << "ev " << evN << ": best " << getBestFit() << ", worst " << getWorstFit() << "\n";
			}

			if (!(evN % setup.debugStep)) {
				checkpoint(evN);
			}

		}
	}
	else {
		uint evN = 0, gen = 0;
		while (evN < evals) {
			vector< pIndividual > newIndividuals;
			/*if (individuals.size()) {
				for (uint ti = 0; ti<THREADS; ti++)
					threadIndividuals[ti].push_back(individuals[getBest()]);
			}*/

			/*for (uint ti = 0; ti<THREADS; ti++) {
				evolveThread(ti, setup.threadGens);
			}*/
			vector<thread> threads;
			for (uint ti = 0; ti<THREADS; ti++) {
				//cout << threadIndividuals[ti].size() << endl;
				threads.push_back(thread(&Evolution::evolveThread, this, ti, setup.threadGens));
			}
			for (uint ti = 0; ti<THREADS; ti++)
				threads[ti].join();
			for (uint ti = 0; ti<THREADS; ti++) {
				newIndividuals.insert(newIndividuals.end(), 
					threadIndividuals[ti].begin(), threadIndividuals[ti].end());
				threadIndividuals[ti].clear();
			}
			random_shuffle(newIndividuals.begin(), newIndividuals.end());

			uint oneSize = newIndividuals.size()/THREADS;
			for (uint ti = 0; ti<THREADS; ti++) {
				if (ti == THREADS - 1)
					threadIndividuals[ti].insert(threadIndividuals[ti].end(),
					newIndividuals.begin() + ti*oneSize, newIndividuals.end());
				else
					threadIndividuals[ti].insert(threadIndividuals[ti].end(),
					newIndividuals.begin() + ti*oneSize, newIndividuals.begin() + (ti + 1)*oneSize);
			}

			/*uint inThread = 0, inInd = newIndividuals.size();
			for (uint ti = 0; ti < THREADS; ti++) {
				inThread += threadIndividuals[ti].size();
			}
			cout << "sanity check: " << inThread << " " << inInd << endl;*/

			individuals = newIndividuals;

			double avgNCGlobal = 0;
			for (uint ti = 0; ti<THREADS; ti++) {
				avgNCGlobal += 1.0 * avgNC[ti]/THREADS;
			}

			if (setup.debug && !(gen%setup.debugStep)) {
				auto end = chrono::steady_clock::now();
				std::chrono::duration<double> time = end - start;
				cout << setprecision(2);
				cout << "gen " << gen << ": best " << getBestFit() << ", real " << getBestReal() << ", worst " << getWorstFit() << ", avgNC: " << avgNCGlobal << ", time: " << time.count() << endl;
				if (setup.writeOut) {
					individuals[getBest()]->print(cout);
					individuals[getWorst()]->print(cout);
				}
			}
			mtx.lock();
			if (!(gen%setup.checkpointStep))
				checkpoint(gen);
			mtx.unlock();

			evN += 2*setup.threadGens;
			gen += setup.threadGens;
		}
	}

}

void Evolution::evolveThread(uint ti, uint genN) {
	uint gen = 0;
	//cout << "begin " << threadIndividuals[ti].size() << endl;
	while (gen < genN) {
		vector< pIndividual > newIndividuals;
		newIndividuals.push_back(threadIndividuals[ti][getBest(ti)]);

		for (uint i = 0; i<POPSIZE; i += 2) {
			Pair pair = modify(ti);
			pair.ind1.get()->getFitness();
			pair.ind2.get()->getFitness();

			newIndividuals.push_back(pair.ind1);
			newIndividuals.push_back(pair.ind2);

			gen += 2;
		}

		mtx.lock();
		threadIndividuals[ti] = newIndividuals;
		mtx.unlock();

		const double share = 0.1;
		avgNC[ti] = 0;
		for (uint i = 0; i<threadIndividuals[ti].size(); i++) {

			double nc = 0;
			for (uint i2 = 0; i2<threadIndividuals[ti].size(); i2++) {
				if (i != i2) {
					double diff = threadIndividuals[ti][i]->getDiff(threadIndividuals[ti][i2]);
					double sh = max(0.0,1.0 - 1.0*diff/share);
					nc += sh;
				}
			}

			threadIndividuals[ti][i]->adjustFitness(nc);
			avgNC[ti] += 1.0 * nc/max(1.0,1.0*threadIndividuals[ti].size());
		}

		gen++;
	}
	//cout << "end " << threadIndividuals[ti].size() << endl;
}

Pair Evolution::modify(uint ti) {
	minstd_rand0 gen(chrono::system_clock::now().time_since_epoch().count());
	uniform_int_distribution<uint> dis(0, UINT_MAX);

	uint i1 = select(ti);
	uint i2 = select(ti);
	while (i2 == i1) {
		i2 = (i1 + 77)%POPSIZE;
	}

	pIndividual ind1 = threadIndividuals[ti][i1];
	pIndividual ind2 = threadIndividuals[ti][i2];
	double r = 1.0 * dis(gen) / UINT_MAX;
	if (r < CROSS_RATE) {
		Pair pair = threadIndividuals[ti][i1]->cross(threadIndividuals[ti][i2]);
		ind1 = pair.ind1; ind2 = pair.ind2;
		r = 1.0 * dis(gen) / UINT_MAX;
		if (r < MUTATE_RATE) {
			ind1 = ind1->mutate();
			ind2 = ind2->mutate();
		}
	}
	else {
		ind1 = threadIndividuals[ti][i1]->mutate();
		ind2 = threadIndividuals[ti][i2]->mutate();
	}

	Pair pair;
	pair.ind1 = ind1;
	pair.ind2 = ind2;
	return pair;
}


uint Evolution::getWorst() {
	uint worst = 0;
	for (uint i = 0; i < individuals.size(); i++) {
		if (!individuals[i]->better(individuals[worst])) {
			worst = i;
		}
	}
	return worst;
}
uint Evolution::getBest() {
	uint best = 0;
	for (uint i = 0; i < individuals.size(); i++) {
		if (individuals[i]->better(individuals[best])) {
			best = i;
		}
	}
	return best;
}
uint Evolution::getBest(uint ti) {
	uint best = 0;
	for (uint i = 0; i < threadIndividuals[ti].size(); i++) {
		if (threadIndividuals[ti][i]->better(threadIndividuals[ti][best])) {
			best = i;
		}
	}
	return best;
}

double Evolution::getBestFit() {
	uint best = getBest();
	return individuals[best]->getFitness();
}
double Evolution::getBestReal() {
	uint best = getBest();
	return individuals[best]->getRealFitness();
}
double Evolution::getWorstFit() {
	uint worst = getWorst();
	return individuals[worst]->getFitness();
}