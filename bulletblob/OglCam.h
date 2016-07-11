#ifndef PI_J5ZI7FVI6RWEI76TP7F97RF76R76R76R
#define PI_J5ZI7FVI6RWEI76TP7F97RF76R76R76R

const double PI = 3.14159265359;

#endif

#ifndef OGLCAM_3FG5H564DF654H564DF564HG54EFH565DF456H4
#define OGLCAM_3FG5H564DF654H564DF564HG54EFH565DF456H4

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>
#include <GL/glew.h>

#include <gl/gl.h>
#include <gl/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "OglOIS.h"
#include "frameinfo.h"

using namespace std;
using namespace OIS;

#define DEGTORAD 0.01745329252f
#define uint unsigned int

struct CamID {
	CamID( GLint id_pos ) :id_pos( id_pos ) {}
	GLint id_pos;
};

struct CamData {
	glm::mat4 invMatrix;
	glm::mat4 noTranslMatrix;
	glm::mat4 matrix;
	glm::vec3 pos;
	glm::vec2 zClip;
	GLfloat zClipRatio;
};

class OglCam {
public:
	OglCam( );
	virtual glm::mat4 update(frameinfo frinf);

	float x, y, z;
	float xt, yt, zt;
	float rotx, roty;
	float fov, nearz, farz, roll;
	float wi, he;
	glm::mat4 matrix, invMatrix, prevMatrix, noTranslMatrix;

	static void lookat( float &xo, float &yo, float &zo, float rotxi, float rotyi );
	virtual void lookat();
	virtual glm::vec3 getGeneric( );
	virtual void init( const char* name );
	virtual CamData getData( );
protected:
	const float sens = 200;
	const float sensZ = 100;
	const float speed = 30;
	vector<CamID> IDs;
	string namestr;

	glm::vec3 look, right, up_preroll, up;
};

class OglCamOrtho : public OglCam {
public:
	virtual glm::mat4 update(frameinfo frinf, glm::vec3 pos = glm::vec3(0,0,0));
};

class CamWASDRF : public OglCam {
public:
	CamWASDRF( );
	void doinput( MouseInfo &mi, Keyboard *keyb, double delta);
protected:
	void move( KeyCode code, bool shift );
	bool lpressed;
	double delta;
};

class CamFollow : public OglCam {
public:
	CamFollow();
	void follow(glm::vec3 target);
	void doinput(MouseInfo &mi, Keyboard *keyb, double delta);
protected:
	bool lpressed;
	double delta, dist;
};

#endif