#include <random>
#include <chrono>
#include <dirent.h>
#include <string>

#include "OglRenderer.h"
#include "Meshes.h"

#define DEBUG

void OglRenderer::render( OglCam *cam, vector<Bufferism*> bufferisms, glm::vec3 ambient, OglLightDir *dirLight, bool AA, frameinfo frinf, double time, double lastTime )
{
	if ( AA )
		GBufferAA( &GBuffer, frinf.rwidth, frinf.rheight );
	else
		GBufferP( &GBuffer, frinf.rwidth, frinf.rheight );

	glm::mat4 camPrev = cam->prevMatrix;
	glm::mat4 camMatrix = cam->update( frinf );
	glm::mat4 camMatrixNoTransl = cam->noTranslMatrix;
	setCameraPrev( camPrev );
	setCamera( camMatrix );
	setCameraNoTransl( camMatrixNoTransl );

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//for(int i=0;i<bufferisms.size();i++)
	//	bufferisms[i]->render();
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	bufferisms[0]->render();

	GBufferFloor(&GBuffer, frinf.rwidth, frinf.rheight);
	setCameraPrev(camPrev);
	setCamera(camMatrix);
	setCameraNoTransl(camMatrixNoTransl);
	bufferisms[1]->render();

#ifdef DEBUG
	if ( finish )
		glFinish( );
	time = glfwGetTime( );
	(*(frinf.times))[1] += time - lastTime;
	lastTime = time;
#endif

	if ( dirLight->shadow )
	{
		LightDirDepth( &DirLBuffer, dirLight->getData(), bufferisms, frinf.rwidth, frinf.rheight );
		/*EVSMGauss( &DirLRangeBuffer[0], frinf.rwidth, frinf.rheight );
		EVSMGaussV( &DirLRangeBuffer[1], frinf.rwidth, frinf.rheight );*/
		/*Quad( 0, frinf.rwidth, frinf.rheight );
		glfwSwapBuffers( frinf.window );
		return;*/
	}

	OglFrameBuffer *renderTo = AA ? &AATemp : &AABuffer[0];
	if ( dirLight )
		AmbDir( renderTo, ambient, dirLight->getData(), cam->getData(), frinf.rwidth, frinf.rheight );
	else
		Ambient( renderTo, ambient, frinf.rwidth, frinf.rheight );

#ifdef DEBUG
	if ( finish )
		glFinish( );
	time = glfwGetTime( );
	(*(frinf.times))[2] += time - lastTime;
	lastTime = time;
#endif
	/*Quad( 0, frinf.rwidth, frinf.rheight );
	glfwSwapBuffers( frinf.window );
	return;*/

	if ( AA )
	{
		QuadAA( &AABuffer[AAp], frinf.rwidth, frinf.rheight );
		Quad( 0, frinf.rwidth, frinf.rheight );
	}
	else {
		QuadFXAA(0, frinf.rwidth, frinf.rheight);
	}

	//Quad( 0, 256,256 );

#ifdef DEBUG
	if ( finish )
		glFinish( );
	time = glfwGetTime( );
	(*(frinf.times))[3] += time - lastTime;
	lastTime = time;
#endif

	glfwSwapBuffers( frinf.window );
}