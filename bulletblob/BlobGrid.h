#ifndef PI_J5ZI7FVI6RWEI76TP7F97RF76R76R76R
#define PI_J5ZI7FVI6RWEI76TP7F97RF76R76R76R

const double PI = 3.14159265359;

#endif

#ifndef BLOBGRID_J5ZI7FVI6RWEI76TP7F97RF76R76R76R
#define BLOBGRID_J5ZI7FVI6RWEI76TP7F97RF76R76R76R

//#include "Simulation.h"
#include "IsoSurface.h"
#include "PerlinNoise.h"
//#include "mdMeshDecimator.h"
#include "Basics.h"

const uint blobN = 25;
const double maxS = 5;
const double minS = 1.5;
const uint blockSize = 5;
const uint steps = 1;

//extern const double PI;

//using namespace MeshDecimation;

struct Face {
	uint v1, v2, v3;
};

struct Mesh {
	vector<XYZ> vertices, colors;
	vector<Face> faces;
};

struct Blob {
	double x,y,z,s;
	double start, period;
};

struct XYZId {
	XYZ xyz;
	uint id;

	bool operator==(const XYZId &other) const
	{
		double th = 0.3;
		double dx = xyz.x - other.xyz.x;
		double dy = xyz.y - other.xyz.y;
		double dz = xyz.z - other.xyz.z;
		double dist = sqrt(dx*dx + dy*dy + dz*dz);

		return (dist<th);
	}
};

namespace std {

	template <>
	struct hash<XYZId>
	{
		std::size_t operator()(const XYZId& k) const
		{
			using std::size_t;
			using std::hash;
			using std::string;

			double th = 0.3;
			size_t val = round((1.0/th)*k.xyz.x)*13723 + 987456451;
			val = val*round((1.0/th)*k.xyz.y)*35803 + 53746841654561;
			val = val*round((1.0/th)*k.xyz.z)*74891 + 5674561574654;

			return k.xyz.x + k.xyz.y + k.xyz.z;
		}
	};

}






class BlobGrid {
public:
	BlobGrid();
	BlobGrid(vector<Blob> blobs);
	void init(vector<Blob> blobs);
	double get(double x, double y, double z, double time);
	void get(double time);
	Mesh march();
	void marchThread(uint x);
	glm::vec3 getCol();
private:
	glm::vec3 hsv2rgb(glm::vec3 in);
	void generateBlobs();

	vector<Blob> blobs;
	vector<vector<vector<vector<uchar>>>> blobsHere;
	vector<vector<vector<double>>> gridVal;
	vector<vector<vector<double>>> activity;

	Mesh miniMesh[N-1];
	glm::vec3 col1, col2, colFloor;
};

#endif