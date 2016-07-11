#include <vector>
using namespace std;

#include "OglBuff.h"
#include "OglCam.h"
#include "OglShader.h"
#include "OglOIS.h"
#include "OglMesh.h"
#include "OglLight.h"
#include "OglRenderer.h"

#include "frameinfo.h"
#include "Meshes.h"
#include "Simulation.h"

class Scene {
public:
	void init( frameinfo frinf );
	void render(OglOIS &ois,frameinfo frinf);
	void resize(uint wi,uint he);
private:
	Bufferism buf, floorBuf;
	vector<OglMesh> meshes;
	glm::vec3 pos;

	Simulation simulation;
	MeshyMesh blob;

	OglRenderer renderer;
	OglLightDir lDir;
	//CamWASDRF cam;
	CamFollow cam;
};