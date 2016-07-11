#ifndef OGLMESH_DFG654R6D54G654DF564GH
#define OGLMESH_DFG654R6D54G654DF564GH

#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
//#include <glm/gtx/decomposition.hpp>
using namespace std;

#include "OglBuff.h"

#include <glfw3.h>

#define uint unsigned int

#define BUF_POS (1<<0)
#define BUF_COLOR (1<<1)
#define BUF_NORMAL (1<<2)
#define BUF_UV (1<<3)

class Bufferism {
public:
	void init( uint flags );
	void postInit();
	void newUpdate();
	void render();

	void add( GLfloat v );
	void add( GLfloat* v, uint size );
	void add( vector<GLfloat> v);

	void add( uint v );
	void add( uint* v, uint size );
	void add( vector<uint> v );

	void minusV(int cnt);
	void minusI(int cnt);
private:
	OglBuffVertex vbuf;
	OglBuffIndex ibuf;
};

class OglMesh {
public:
	void toBuffs(Bufferism &buf);
	void toBuffsRewrite(Bufferism &buf);
	void calcNormals();
	void color(glm::vec3 color);
	void transform(glm::mat4 matrix);
	void rotateNormals( glm::mat4 matrix );
protected:
	vector<GLfloat> verts;
	vector<unsigned int> inds;
	uint stepSize,fl,vS,iS;
};

#endif