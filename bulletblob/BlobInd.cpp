#include "BlobInd.h"

void BlobInd::randomInit() {
	minstd_rand0 gen(chrono::system_clock::now().time_since_epoch().count());
	uniform_int_distribution<uint> dis(0, UINT_MAX);

	blobs.clear();
	for (uint i = 0; i<blobN; i++) {
		Blob blob;
		double r = 1.0*dis(gen)/UINT_MAX;
		blob.x = maxS+1 + 1.0*(N-2*(maxS+1)) * dis(gen)/UINT_MAX;
		blob.y = maxS+1 + 1.0*(N-2*(maxS+1)) * dis(gen)/UINT_MAX;
		blob.z = maxS+1 + 1.0*(N-2*(maxS+1)) * dis(gen)/UINT_MAX;
		blob.s = minS + r*(maxS-minS);

		blob.start = 2.0 * PI * dis(gen) / UINT_MAX; //0 to 2PI
		//blob.period = (0.2 + 1.0 * dis(gen) / UINT_MAX)*60*4; //2 seconds max
		blob.period = dis(gen)%5 + 1;

		blobs.push_back(blob);
	}

	fitnessOk = false;
}

pIndividual BlobInd::mutate() {
	minstd_rand0 gen(chrono::system_clock::now().time_since_epoch().count());
	uniform_int_distribution<uint> dis(0, UINT_MAX);
	normal_distribution<double> normDis5(0, 5);
	normal_distribution<double> normDis2(0, 2);
	normal_distribution<double> normDis1(0, 1);
	normal_distribution<double> normDisH(0, 0.4);

	BlobInd ind = *this;
	uint bi = dis(gen)%blobs.size();
	uint mi = dis(gen)%4;
	switch (mi) {
		case 0: //pos
			blobs[bi].x += normDis5(gen);
			blobs[bi].y += normDis5(gen);
			blobs[bi].z += normDis5(gen);

			blobs[bi].x = glm::max(maxS+1, blobs[bi].x);
			blobs[bi].x = glm::min(N-maxS-1, blobs[bi].x);
			blobs[bi].y = glm::max(maxS+1, blobs[bi].y);
			blobs[bi].y = glm::min(N-maxS-1, blobs[bi].y);
			blobs[bi].z = glm::max(maxS+1, blobs[bi].z);
			blobs[bi].z = glm::min(N-maxS-1, blobs[bi].z);
			break;
		case 1: //size
			blobs[bi].s += normDis2(gen);

			blobs[bi].s = glm::max(minS, blobs[bi].s);
			blobs[bi].s = glm::min(maxS, blobs[bi].s);
			break;
		case 2: //start
			blobs[bi].start += normDis1(gen);

			blobs[bi].start = glm::max(0.0, blobs[bi].start);
			blobs[bi].start = glm::min(2*PI, blobs[bi].start);
			break;
		case 3: //period
			if (dis(gen)%2)
				blobs[bi].period++;
			else
				blobs[bi].period--;

			blobs[bi].period = glm::max(1.0, blobs[bi].period);
			blobs[bi].period = glm::min(5.0, blobs[bi].period);
			blobs[bi].period *= 60*4;
			break;
	}

	ind.fitnessOk = false;
	return make_shared<BlobInd>(ind);
}

double BlobInd::getDiff(pIndividual ind) {
	BlobInd *ind2 = (BlobInd*) ind.get();

	double rangeXYZ = N - 2*(maxS+1);
	rangeXYZ = glm::sqrt(3.0*pow(rangeXYZ, 2));
	double rangeS = maxS - minS;
	double rangeStart = PI*2;
	double rangePer = 5;
	double diffSum = 0;
	for (uint bi = 0; bi<blobs.size(); bi++) {
		double diff = 0;

		glm::vec3 pos1(blobs[bi].x, blobs[bi].y, blobs[bi].z);
		glm::vec3 pos2(ind2->blobs[bi].x, ind2->blobs[bi].y, ind2->blobs[bi].z);
		glm::vec3 posDiff = pos1 - pos2;
		double posLen = glm::sqrt(posDiff.x*posDiff.x + posDiff.y*posDiff.y + posDiff.z*posDiff.z);
		diff += 1.0*posLen/rangeXYZ;

		diff += 1.0*abs(blobs[bi].s - ind2->blobs[bi].s)/rangeS;
		diff += 1.0*abs(blobs[bi].start - ind2->blobs[bi].start)/rangeStart;
		diff += 1.0*abs(blobs[bi].period - ind2->blobs[bi].period)/rangePer;

		diffSum += diff/4.0;
		//cout << diff << endl;
	}
	diffSum /= 1.0*blobs.size();
	return diffSum;
}

Pair BlobInd::cross(pIndividual ind) {
	minstd_rand0 gen(chrono::system_clock::now().time_since_epoch().count());
	uniform_int_distribution<uint> dis(0, UINT_MAX);

	BlobInd ind1 = *this;
	BlobInd ind2 = *(BlobInd*)ind.get();

	for (uint bi = 0; bi<blobs.size(); bi++) {
		double r = 1.0 * dis(gen) / UINT_MAX;
		if (r<0.5)
			swap(ind1.blobs[bi], ind2.blobs[bi]);
	}

	ind1.fitnessOk = false;
	ind2.fitnessOk = false;

	Pair pair;
	pair.ind1 = make_shared<BlobInd>(ind1);
	pair.ind2 = make_shared<BlobInd>(ind2);
	return pair;
}

bool BlobInd::better(pIndividual ind, bool best) {
	double f1 = getFitness();
	double f2 = ind->getFitness();

	if (f1 > f2)
		return best;
	else
		return !best;
}

double BlobInd::getFitness() {
	if (fitnessOk)
		return fitness;

	Simulation simulation(blobs);
	for (uint i = 0; i<2000; i++)
		simulation.step();
	fitness = simulation.getDist();

	realFitness = fitness;

	fitnessOk = true;
	return fitness;
}

double BlobInd::getRealFitness() {
	getFitness();
	return realFitness;
}

void BlobInd::adjustFitness(double nc) {
	getFitness();
	fitness /= std::max(1.0,nc);
}

void BlobInd::print(ostream& out) {
	/*for (uint i = 0; i < cus2fac.size(); i++) {
		out << cus2fac[i] << " ";
	}*/

	for (uint bi = 0; bi<blobs.size(); bi++) {
		out.write((const char*) &blobs[bi].x, sizeof(blobs[bi].x));
		out.write((const char*) &blobs[bi].y, sizeof(blobs[bi].y));
		out.write((const char*) &blobs[bi].z, sizeof(blobs[bi].z));
		out.write((const char*) &blobs[bi].s, sizeof(blobs[bi].s));
		out.write((const char*) &blobs[bi].start, sizeof(blobs[bi].start));
		out.write((const char*) &blobs[bi].period, sizeof(blobs[bi].period));
	}
}

void BlobInd::read(istream& in) {
	blobs.clear();
	for (uint bi = 0; bi<blobN; bi++) {
		Blob blob;
		in.read((char*) &blob.x, sizeof(blob.x));
		in.read((char*) &blob.y, sizeof(blob.y));
		in.read((char*) &blob.z, sizeof(blob.z));
		in.read((char*) &blob.s, sizeof(blob.s));
		in.read((char*) &blob.start, sizeof(blob.start));
		in.read((char*) &blob.period, sizeof(blob.period));

		blobs.push_back(blob);
	}
}

void BlobInd::print(string name) {
	ofstream file;
	file.precision(17);
	file.open(name);
	file << getFitness() << endl;
	//cout << "file " << file.bad() << " " << name << endl;

	for (uint bi = 0; bi<blobN; bi++) {
		file << blobs[bi].x << " ";
		file << blobs[bi].y << " ";
		file << blobs[bi].z << " ";
		file << blobs[bi].s << endl;

		file << blobs[bi].start << " ";
		file << blobs[bi].period << endl;
	}
	file.close();
}