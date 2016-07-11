#include "BlobGrid.h"

BlobGrid::BlobGrid() {
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
		blob.amplitude = 1.0 * dis(gen) / UINT_MAX; //0 to 1
		blob.period = (0.5 + 1 * dis(gen) / UINT_MAX)*60*4; //4 seconds max

		/*blob.x = 10;
		blob.y = 10;
		blob.z = 10;
		blob.s = 5;

		blob.start = 0; //0 to 2PI
		blob.amplitude = 1; //0 to 1
		blob.period = 60*2; //4 seconds max*/

		blobs.push_back(blob);
	}
	/*blobs[0].start = 1;
	blobs[0].s = 2;*/

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
}

double BlobGrid::get(double x, double y, double z, double time) {
	double val = 0;
	uint bs = blobsHere[x][y][z].size();
	for (uint ib = 0; ib<bs; ib++) {
		uint i = blobsHere[x][y][z][ib];
		double dx = blobs[i].x-x;
		double dy = blobs[i].y-y;
		double dz = blobs[i].z-z;
		double dist = sqrt(dx*dx + dy*dy + dz*dz);
		double distVal = blobs[i].s - dist;

		double angle = blobs[i].start + 2*PI*time/blobs[i].period;
		angle += distVal*sin(angle+PI/2); //shifting blob angle by distance to smooth out
		double valNow = blobs[i].amplitude*sin(angle);

		valNow = valNow*max(0.0, distVal/blobs[i].s);
		val += valNow;
	}
	if (!val)
		val = -1;
	return val;
}

void BlobGrid::get(double time) {
	gridVal.clear();

	for (uint x = 0; x<N; x++) {
		vector<vector<double>> yVec;
		for (uint y = 0; y<N; y++) {
			vector<double> zVec;
			for (uint z = 0; z<N; z++) {
				zVec.push_back(get(x, y, z, time));
			}
			yVec.push_back(zVec);
		}
		gridVal.push_back(yVec);
	}
}

Mesh BlobGrid::march() {
	Mesh mesh;
	unordered_set<XYZId> vertHashes;
	for (uint x = 0; x<N-1; x++) {
		for (uint y = 0; y<N-1; y++) {
			for (uint z = 0; z<N-1; z++) {
				GRIDCELL grid;

				grid.p[0].x = x;   grid.p[0].y = y;   grid.p[0].z = z;
				grid.p[1].x = x+1; grid.p[1].y = y;   grid.p[1].z = z;
				grid.p[2].x = x+1; grid.p[2].y = y+1; grid.p[2].z = z;
				grid.p[3].x = x;   grid.p[3].y = y+1; grid.p[3].z = z;
				grid.p[4].x = x;   grid.p[4].y = y;   grid.p[4].z = z+1;
				grid.p[5].x = x+1; grid.p[5].y = y;   grid.p[5].z = z+1;
				grid.p[6].x = x+1; grid.p[6].y = y+1; grid.p[6].z = z+1;
				grid.p[7].x = x;   grid.p[7].y = y+1; grid.p[7].z = z+1;

				grid.val[0] = gridVal[x][y][z];
				grid.val[1] = gridVal[x+1][y][z];
				grid.val[2] = gridVal[x+1][y+1][z];
				grid.val[3] = gridVal[x][y+1][z];
				grid.val[4] = gridVal[x][y][z+1];
				grid.val[5] = gridVal[x+1][y][z+1];
				grid.val[6] = gridVal[x+1][y+1][z+1];
				grid.val[7] = gridVal[x][y+1][z+1];

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
					

				/*	for (uint fi = 0; fi<3; fi++) {
						XYZId xyzId;
						xyzId.xyz = triangles[i].p[fi];
						xyzId.id = vertNow;

						unordered_set<XYZId>::const_iterator it;
						it = vertHashes.find(xyzId);
						if (it == vertHashes.end()) {
							mesh.vertices.push_back(triangles[i].p[fi]);
							vertNow++;
							fc[fi] = vertNow;
							xyzId.id = vertNow;
							vertHashes.insert(xyzId);
						}
						else {
							cout << triangles[i].p[fi].x << " " << mesh.vertices[(*it).id].x << " " << ((*it) == xyzId) << endl;
							fc[fi] = (*it).id;
						}
					}*/

					mesh.vertices.push_back(triangles[i].p[0]);
					mesh.vertices.push_back(triangles[i].p[1]);
					mesh.vertices.push_back(triangles[i].p[2]);

					/*Face f;
					f.v1 = fc[0];
					f.v2 = fc[1];
					f.v3 = fc[2];
					if (f.v1 == f.v2)
						continue;
					if (f.v2 == f.v3)
						continue;
					if (f.v1 == f.v3)
						continue;
					mesh.faces.push_back(f);*/

					Face f;
					f.v1 = mesh.vertices.size()-3;
					f.v2 = mesh.vertices.size()-2;
					f.v3 = mesh.vertices.size()-1;
					mesh.faces.push_back(f);
					
					/*Vec3<float> v[3];

					for (uint vi = 0; vi<3; vi++) {
						v[vi] = Vec3<float>(triangles[i].p[vi].x, 
							triangles[i].p[vi].y, triangles[i].p[vi].z);
						points.push_back(v[vi]);
					}

					Vec3<int> face;
					face = Vec3<int>(points.size() - 3,
						points.size() - 2, points.size() - 1);
					trianglesDec.push_back(face);*/
				}
			}
		}
	}
	/*
	// decimate mesh
	MeshDecimator myMDecimator;
	//myMDecimator.SetCallBack(&CallBack);
	myMDecimator.Initialize(points.size(), trianglesDec.size(), &points[0], &trianglesDec[0]);
	myMDecimator.Decimate(100, 50, 1);

	// allocate memory for decimated mesh
	vector< Vec3<Float> > decimatedPoints;
	vector< Vec3<int> > decimatedtriangles;
	decimatedPoints.resize(myMDecimator.GetNVertices());
	decimatedtriangles.resize(myMDecimator.GetNTriangles());

	// retreive decimated mesh
	myMDecimator.GetMeshData(&decimatedPoints[0], &decimatedtriangles[0]);

	for (uint i = 0; i<decimatedPoints.size(); i++) {
		XYZ vert;
		vert.x = decimatedPoints[i][0];
		vert.y = decimatedPoints[i][1];
		vert.z = decimatedPoints[i][2];
		mesh.vertices.push_back(vert);
	}

	for (uint i = 0; i<decimatedtriangles.size(); i++) {
		Face f;
		f.v1 = decimatedtriangles[i][0];
		f.v2 = decimatedtriangles[i][1];
		f.v3 = decimatedtriangles[i][2];
		mesh.faces.push_back(f);
	}*/

	/*map<uint, uint> remap;
	vector<XYZ> verts;
	vector<uint> hash;
	double th = 0.3;

	for (uint i = 0; i<mesh.vertices.size(); i++) {
		uint val = round((1.0/th)*mesh.vertices[i].x)*13723 + 987456451;
		val = val*round((1.0/th)*mesh.vertices[i].y)*35803 + 53746841654561;
		val = val*round((1.0/th)*mesh.vertices[i].z)*74891 + 5674561574654;
		hash.push_back(val);
	}

	// initialize original index locations
	vector<size_t> idx(hash.size());
	for (size_t i = 0; i != idx.size(); ++i) idx[i] = i;

	// sort indexes based on comparing values in v
	sort(idx.begin(), idx.end(),
		[&hash] (size_t i1, size_t i2) {return hash[i1] < hash[i2]; });
	sort(hash.begin(), hash.end());

	uint newId = 0;
	uint lastHash = hash[0];
	verts.push_back(mesh.vertices[idx[0]]);
	for (uint i = 0; i<hash.size(); i++) {
		if (hash[i]!=lastHash) {
			newId++;
			verts.push_back(mesh.vertices[idx[i]]);
		}
		remap[idx[i]] = newId;
		lastHash = hash[i];
	}*/

	/*vector<bool> checked;
	checked.insert(checked.begin(), mesh.vertices.size(), false);
	double th = 0.3;
	uint newId = 0;

	for (uint i = 0; i<mesh.vertices.size(); i++) {
		if (checked[i])
			continue;

		XYZ p1 = mesh.vertices[i];
		for (uint i2 = 0; i2<mesh.vertices.size(); i2++) {
			if (i == i2)
				continue;

			XYZ p2 = mesh.vertices[i2];
			double dx = p1.x - p2.x;
			double dy = p1.y - p2.y;
			double dz = p1.z - p2.z;

			double dist = sqrt(dx*dx + dy*dy + dz*dz);
			if (dist<th) {
				checked[i2] = true;
				remap[i2] = newId;
			}
		}
		checked[i] = true;
		remap[i] = newId;
		verts.push_back(mesh.vertices[i]);
		newId++;
	}*/

	/*vector<Face> faces;
	for (uint i = 0; i<mesh.faces.size(); i++) {
		Face face;
		face.v1 = remap[mesh.faces[i].v1];
		face.v2 = remap[mesh.faces[i].v2];
		face.v3 = remap[mesh.faces[i].v3];

		if (face.v1 == face.v2)
			continue;
		if (face.v2 == face.v3)
			continue;
		if (face.v1 == face.v3)
			continue;

		faces.push_back(face);
	}

	Mesh mesh2;
	mesh2.vertices = verts;
	mesh2.faces = faces;*/

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