#include "Switch.h"

#ifdef M3D

#include <GL/glew.h>
#include <glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
using namespace std;

#include "frameinfo.h"
#include "Scene.h"
#include "OglOIS.h"

#define DEBUG

static void error_callback( int error, const char* description )
{
	fputs( description, stderr );
}

static void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
		glfwSetWindowShouldClose( window, GL_TRUE );
}

#define FULLSCREEN 0
#define VSYNC 1
#define WI 1280
#define HE 720
OglOIS ois;

void printTime( double delta )
{
	cout << fixed << setprecision( 2 ) << delta * 1000 << " ms, "
		<< setprecision( 0 ) << 1 / delta << " fps" << endl;
}

#include "ppm.h"

int main( void )
{
	ppm file;
	file.read("lookup.pbm");

	glm::vec4 a(1, 1, 1, 1);
	glm::mat4 m(1);
	a = m*a;

	GLFWwindow* window;
	glfwSetErrorCallback( error_callback );

	if ( !glfwInit( ) )
		exit( EXIT_FAILURE );

	//glfwWindowHint( GLFW_SAMPLES, 8 );
	/*glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );*/
	glfwWindowHint( GLFW_DECORATED, !FULLSCREEN );

	window = glfwCreateWindow( WI, HE, "Huhuhyhu", FULLSCREEN ? glfwGetPrimaryMonitor() : 0, 0 );
	if ( !window )
	{
		glfwTerminate( );
		exit( EXIT_FAILURE );
	}

	glfwMakeContextCurrent( window );
	glfwSetKeyCallback( window, key_callback );
	glfwSwapInterval( VSYNC );

	cout << "OpenGL version " << glGetString( GL_VERSION ) << endl;

	glewExperimental = GL_TRUE;
	if ( glewInit( ) != GLEW_OK )
	{
		glfwTerminate( );
		exit( EXIT_FAILURE );
	}

	ois.init();

	Scene scene;
	frameinfo frinf;
	frinf.rwidth = WI;
	frinf.rheight = HE;
	scene.init( frinf );

	glEnable( GL_CULL_FACE );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LESS );

	HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
	COORD coord;
	coord.X = 0;
	coord.Y = 80;

#ifdef DEBUG
	vector<double> times;
	int timesN = 4;
	for(int i=0;i<timesN;i++)
		times.push_back( 0 );
#endif

	double lastFrame = glfwGetTime( ), tosecond = lastFrame;
	double deltaAvg = 0; int deltaCnt = 0;

	frameinfo frinf0;
	glfwGetFramebufferSize(window, &frinf0.rwidth, &frinf0.rheight);
	int lastWi = frinf0.rwidth;
	int lastHe = frinf0.rheight;
	while ( !glfwWindowShouldClose( window ) )
	{
		frameinfo frinf;
		frinf.window = window;

#ifdef DEBUG
		frinf.times = &times;
#endif
		double currentFrame = glfwGetTime( );
		frinf.delta = (currentFrame - lastFrame);
		lastFrame = currentFrame;

		deltaAvg += frinf.delta;
		deltaCnt++;

		glfwGetFramebufferSize( window, &frinf.rwidth, &frinf.rheight );
		frinf.ratio = frinf.rwidth / (float) frinf.rheight;
		if (frinf.rwidth!=lastWi || frinf.rheight!=lastHe) {
			scene.resize(frinf.rwidth, frinf.rheight);
			lastWi = frinf.rwidth;
			lastHe = frinf.rheight;
		}
		glClear( GL_COLOR_BUFFER_BIT );

		scene.render(ois,frinf);
		glfwPollEvents( );

		if ( currentFrame - tosecond > 1 && 0)
		{
			tosecond = currentFrame;
			double deltaNow = deltaAvg / deltaCnt;
			SetConsoleCursorPosition( hConsole, coord );

#ifdef DEBUG
			for ( int i = 0; i<timesN; i++ )
				times[i] /= deltaCnt;

			cout << "input: ";
			printTime( times[0] );
			cout << "GBuffer: ";
			printTime( times[1] );
			cout << "Light: ";
			printTime( times[2] );
			cout << "Post: ";
			printTime( times[3] );
#endif
			cout << "frame: ";
			printTime( deltaNow );

#ifdef DEBUG
			deltaAvg = 0; deltaCnt = 0;
			for ( int i = 0; i<timesN; i++ )
				times[i] = 0;
#endif
		}
	}

	glfwDestroyWindow( window );
	glfwTerminate( );
	exit( EXIT_SUCCESS );
}

#endif