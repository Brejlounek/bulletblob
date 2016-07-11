#ifndef ISOSURFACE_I654I7FV65E8O7TU075D6I7E5JE57D
#define ISOSURFACE_I654I7FV65E8O7TU075D6I7E5JE57D

#include <math.h>

struct XYZ {
	double x, y, z;
	XYZ operator-(const XYZ &m)
	{
		XYZ mm;
		mm.x = x - m.x;
		mm.y = y - m.y;
		mm.z = z - m.z;
		return mm;
	}
};

typedef struct {
	XYZ p[3];
} TRIANGLE;

typedef struct {
	XYZ p[8];
	double val[8];
} GRIDCELL;

class IsoSurface {
public:
	int Polygonise(GRIDCELL grid, double isolevel, TRIANGLE *triangles);
private:
	XYZ VertexInterp(double isolevel, XYZ p1, XYZ p2, double valp1, double valp2);
};

#endif