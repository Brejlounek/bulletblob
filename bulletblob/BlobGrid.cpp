#include "BlobGrid.h"

BlobGrid::BlobGrid() {
	minstd_rand0 gen(chrono::system_clock::now().time_since_epoch().count());
	uniform_int_distribution<uint> dis(0, UINT_MAX);

	generateBlobs();
	init(blobs);
}

BlobGrid::BlobGrid(vector<Blob> blobs) {
	init(blobs);
}

void BlobGrid::init(vector<Blob> blobs) {
	this->blobs = blobs;

	minstd_rand0 gen(chrono::system_clock::now().time_since_epoch().count());
	uniform_int_distribution<uint> dis(0, UINT_MAX);

	blobsHere.clear();
	for (uint x = 0; x<N; x++) {
		vector<vector<vector<uchar>>> yBlobs;
		for (uint y = 0; y<N; y++) {
			vector<vector<uchar>> zBlobs;
			for (uint z = 0; z<N; z++) {
				vector<uchar> nowBlobs;
				for (uchar i = 0; i<blobN; i++) {
					double dx = blobs[i].x-x;
					double dy = blobs[i].y-y;
					double dz = blobs[i].z-z;
					double dist = sqrt(dx*dx + dy*dy + dz*dz);

					if (dist < blobs[i].s)
						nowBlobs.push_back(i);
				}
				zBlobs.push_back(nowBlobs);
			}
			yBlobs.push_back(zBlobs);
		}
		blobsHere.push_back(yBlobs);
	}

	gridVal.clear();

	for (uint x = 0; x<N; x++) {
		vector<vector<double>> yVec;
		for (uint y = 0; y<N; y++) {
			vector<double> zVec;
			for (uint z = 0; z<N; z++) {
				zVec.push_back(0);
			}
			yVec.push_back(zVec);
		}
		gridVal.push_back(yVec);
	}
	activity = gridVal;

	double hue = 359.0 * dis(gen)/UINT_MAX;
	double hueShift = hue+180, hueShift2 = hue+90;
	if (hueShift>360)
		hueShift -= 360;
	if (hueShift2>360)
		hueShift2 -= 360;
	col1 = glm::vec3(hue, 1, 1);
	col2 = glm::vec3(hueShift, 1, 1);
	colFloor = glm::vec3(hueShift2, 0.4, 0.2);
	col1 = hsv2rgb(col1);
	col2 = hsv2rgb(col2);
	colFloor = hsv2rgb(colFloor);

	//col1 = glm::vec3(1, 1, 1);
	//col2 = glm::vec3(1, 1, 1);

	PerlinNoise pn(chrono::system_clock::now().time_since_epoch().count());
	for (uint x = 0; x<N; x++) {
		for (uint y = 0; y<N; y++) {
			for (uint z = 0; z<N; z++) {
				activity[x][y][z] = pn.noise(0.5 + x/7.0, 0.5 + y/7.0, 0.5 + z/7.0);
			}
		}
	}
}

void BlobGrid::generateBlobs() {
	minstd_rand0 gen(chrono::system_clock::now().time_since_epoch().count());
	uniform_int_distribution<uint> dis(0, UINT_MAX);

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
}

double BlobGrid::get(double x, double y, double z, double time) {
	double val = 0, act = 0;
	uint bs = blobsHere[x][y][z].size();
	for (uint ib = 0; ib<bs; ib++) {
		uint i = blobsHere[x][y][z][ib];
		double dx = blobs[i].x-x;
		double dy = blobs[i].y-y;
		double dz = blobs[i].z-z;
		double dist = sqrt(dx*dx + dy*dy + dz*dz);
		double distVal = blobs[i].s - dist;

		double realPer = 60.0*4/blobs[i].period;
		double angle = blobs[i].start + 2*PI*time/realPer;
		angle += distVal*sin(angle+PI/2); //shifting blob angle by distance to smooth out
		double valNow = /*blobs[i].amplitude*/sin(angle);

		valNow = valNow*max(0.0, distVal/blobs[i].s);
		val += valNow;
	}
	if (!val)
		val = -1;

	return val;
}

void BlobGrid::get(double time) {
	for (uint x = 0; x<N; x++) {
		for (uint y = 0; y<N; y++) {
			for (uint z = 0; z<N; z++) {
				gridVal[x][y][z] = get(x, y, z, time);
			}
		}
	}
}

void BlobGrid::marchThread(uint x) {

}

Mesh BlobGrid::march() {
	minstd_rand0 gen(chrono::system_clock::now().time_since_epoch().count());
	uniform_int_distribution<uint> dis(0, UINT_MAX);

	Mesh mesh;
	unordered_set<XYZId> vertHashes;

	for (uint x = 0; x<N-steps; x += steps) {
		for (uint y = 0; y<N-steps; y += steps) {
			for (uint z = 0; z<N-steps; z += steps) {
				GRIDCELL grid;

				grid.p[0].x = x;       grid.p[0].y = y;       grid.p[0].z = z;
				grid.p[1].x = x+steps; grid.p[1].y = y;       grid.p[1].z = z;
				grid.p[2].x = x+steps; grid.p[2].y = y+steps; grid.p[2].z = z;
				grid.p[3].x = x;       grid.p[3].y = y+steps; grid.p[3].z = z;
				grid.p[4].x = x;       grid.p[4].y = y;       grid.p[4].z = z+steps;
				grid.p[5].x = x+steps; grid.p[5].y = y;       grid.p[5].z = z+steps;
				grid.p[6].x = x+steps; grid.p[6].y = y+steps; grid.p[6].z = z+steps;
				grid.p[7].x = x;       grid.p[7].y = y+steps; grid.p[7].z = z+steps;

				grid.val[0] = gridVal[x][y][z];
				grid.val[1] = gridVal[x+steps][y][z];
				grid.val[2] = gridVal[x+steps][y+steps][z];
				grid.val[3] = gridVal[x][y+steps][z];
				grid.val[4] = gridVal[x][y][z+steps];
				grid.val[5] = gridVal[x+steps][y][z+steps];
				grid.val[6] = gridVal[x+steps][y+steps][z+steps];
				grid.val[7] = gridVal[x][y+steps][z+steps];

				if (grid.val[0]<0 &&
					grid.val[1]<0 &&
					grid.val[2]<0 &&
					grid.val[3]<0 &&
					grid.val[4]<0 &&
					grid.val[5]<0 &&
					grid.val[6]<0 &&
					grid.val[7]<0)
					continue;

				if (grid.val[0]>0 &&
					grid.val[1]>0 &&
					grid.val[2]>0 &&
					grid.val[3]>0 &&
					grid.val[4]>0 &&
					grid.val[5]>0 &&
					grid.val[6]>0 &&
					grid.val[7]>0)
					continue;


				TRIANGLE triangles[5];
				IsoSurface isoSurface;
				int triN = isoSurface.Polygonise(grid, 0, triangles);

				uint vertNow = -1;
				for (uint i = 0; i<triN; i++) {
					uint fc[3];

					mesh.vertices.push_back(triangles[i].p[0]);
					mesh.vertices.push_back(triangles[i].p[1]);
					mesh.vertices.push_back(triangles[i].p[2]);

					Face f;
					f.v1 = mesh.vertices.size()-3;
					f.v2 = mesh.vertices.size()-2;
					f.v3 = mesh.vertices.size()-1;
					mesh.faces.push_back(f);

					double ratio = activity[x][y][z];
					glm::vec3 colTemp1 = col1; colTemp1 *= ratio;
					glm::vec3 colTemp2 = col2; colTemp2 *= (1-ratio);
					glm::vec3 colRes = colTemp1 + colTemp2;
					XYZ colXYZ;
					colXYZ.x = colRes.x;
					colXYZ.y = colRes.y;
					colXYZ.z = colRes.z;

					mesh.colors.push_back(colXYZ);
					mesh.colors.push_back(colXYZ);
					mesh.colors.push_back(colXYZ);
				}
			}
		}
	}

	if (!mesh.vertices.size() || !mesh.faces.size()) {
		mesh.vertices.clear();

		XYZ v; v.x = 0; v.y = 0; v.z = 0;
		mesh.vertices.push_back(v);
		       v.x = 1; v.y = 0; v.z = 0;
		mesh.vertices.push_back(v);
		       v.x = 1; v.y = 1; v.z = 0;
		mesh.vertices.push_back(v);

		Face face;
		face.v1 = 0; face.v2 = 1; face.v3 = 2;
		mesh.faces.push_back(face);
	}

	return mesh;
}






glm::vec3 BlobGrid::hsv2rgb(glm::vec3 in) {
	double      hh, p, q, t, ff;
	long        i;
	glm::vec3   out;

	if (in.y <= 0.0) {       // < is bogus, just shuts up warnings
		out.r = in.z;
		out.g = in.z;
		out.b = in.z;
		return out;
	}
	hh = in.x;
	if (hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long) hh;
	ff = hh - i;
	p = in.z * (1.0 - in.y);
	q = in.z * (1.0 - (in.y * ff));
	t = in.z * (1.0 - (in.y * (1.0 - ff)));

	switch (i) {
		case 0:
			out.r = in.z;
			out.g = t;
			out.b = p;
			break;
		case 1:
			out.r = q;
			out.g = in.z;
			out.b = p;
			break;
		case 2:
			out.r = p;
			out.g = in.z;
			out.b = t;
			break;

		case 3:
			out.r = p;
			out.g = q;
			out.b = in.z;
			break;
		case 4:
			out.r = t;
			out.g = p;
			out.b = in.z;
			break;
		case 5:
		default:
			out.r = in.z;
			out.g = p;
			out.b = q;
			break;
	}
	return out;
}

glm::vec3 BlobGrid::getCol() {
	return colFloor;
}