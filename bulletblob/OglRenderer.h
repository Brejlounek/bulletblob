#ifndef RENDERER_KFJSHKJFHHKJFHKGHFKJHJHJ
#define RENDERER_KFJSHKJFHHKJFHKGHFKJHJHJ

#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>

#include "OglShader.h"
#include "OglLight.h"
#include "OglCam.h"
#include "OglTex.h"
#include "OglMesh.h"
#include "OglFrameBuffer.h"

static bool finish = true;

#define uint unsigned int
#define uchar unsigned char

class OglRenderer {
public:
	void init(uint wi,uint he);
	void createRandomTexture();
	void setCamera( glm::mat4 camMat, bool wiggle = true );
	void setCameraPrev( glm::mat4 camMat, bool wiggle = true );
	void setCameraNoTransl( glm::mat4 camMat, bool wiggle = true );
	void buildShader( string strProg, string strVertex, string strFragment );
	void enableBuffer( OglFrameBuffer *buffer, bool clear = true );
	void resize(uint wi, uint he);

	//rendering passes
	void GBufferP( OglFrameBuffer *buffer, uint wi, uint he );
	void GBufferFloor(OglFrameBuffer *buffer, uint wi, uint he);
	void GBufferAA( OglFrameBuffer *buffer,  uint wi, uint he );
	void Ambient( OglFrameBuffer *buffer, glm::vec3 ambient, uint wi, uint he );
	void AmbDir( OglFrameBuffer *buffer, glm::vec3 ambient, LightData lDirData, CamData camData, uint wi, uint he );
	void Quad( OglFrameBuffer *buffer, uint wi, uint he );
	void QuadAA( OglFrameBuffer *buffer, uint wi, uint he );
	void QuadFXAA(OglFrameBuffer *buffer, uint wi, uint he);

	void LightDirDepth( OglFrameBuffer *buffer, LightData lDirData, vector<Bufferism*> bufferisms, uint wi, uint he );
	void EVSMGauss( OglFrameBuffer *buffer, uint wi, uint he );
	void EVSMGaussV( OglFrameBuffer *buffer, uint wi, uint he );

	void Blur( OglFrameBuffer *buffer, OglTex *whatTex, uint wi, uint he );

	void render( OglCam *cam, vector<Bufferism*> bufferisms, glm::vec3 ambient, OglLightDir *dirLight, bool AA, frameinfo frinf, double time, double lastTime );

	glm::vec3 floorCol;
private:
	Bufferism bufQuad;
	OglFrameBuffer GBuffer, AABuffer[2], AATemp, DirLBuffer, DirLRangeBuffer[2], AOBuffer;

	OglShaderProg *activeShader;
	unordered_map<string, OglVertexShader> shVertex;
	unordered_map<string, OglFragmentShader> shFragment;
	unordered_map<string, OglShaderProg> shProg;

	OglTexTarget texColor, texNormal, texVelocity, texAATemp, texAO;
	OglTexTarget texAA[2]; uint texNum, AAp;
	OglTexTarget texDirLRange[2];
	OglTex randomTex, floorTex;

	glm::vec3 lastWiggle;
	uint wim, hem;
	uint wp;
	GLfloat rx[4], ry[4];
	uint cnt;
};

#endif