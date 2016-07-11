#include "Scene.h"
#include "frameinfo.h"
#include <glm/glm.hpp>
#include <glfw3.h>
#include <random>
#include <chrono>
using namespace glm;

#define DEBUG

void Scene::init( frameinfo frinf )
{
	//simulation.load("best905.16.txt");
	buf.init( BUF_POS | BUF_COLOR | BUF_NORMAL );
	floorBuf.init(BUF_POS | BUF_COLOR | BUF_NORMAL);

	uint seed = chrono::system_clock::now( ).time_since_epoch( ).count( );
	//seed = 3246574;
	default_random_engine gen( seed );
	uniform_real_distribution<double> dist( 0.0, 1.0 );

	mat4x4 matrix;
	Mesh mesh = simulation.step(matrix);
	blob.init(mesh, BUF_POS | BUF_COLOR | BUF_NORMAL);
	meshes.push_back(blob);

	/*for ( int i = 0; i < 300; i++ )
	{
		GLfloat wi = 14;
		//GLfloat wi = 1;
		GLfloat degr = dist( gen ) * 360;
		glm::mat4 transl = glm::translate( glm::vec3( 0, 1, 0 ) );
		double basesc = dist( gen )*dist( gen )*dist( gen )*0.3;
		glm::mat4 scale = glm::scale( glm::vec3( basesc + dist( gen )*0.3, basesc + dist( gen )*1, basesc + dist( gen )*0.3 ) );
		//glm::mat4 scale = glm::scale( glm::vec3( 0.003, basesc + dist( gen ) * 1, 0.003 ) );
		glm::mat4 rotate = glm::rotate( degr, glm::vec3( 0, 1, 0 ) );
		glm::mat4 transl2 = glm::translate( glm::vec3( dist( gen ) * wi - wi/2, 0, dist( gen ) * wi - wi/2 ) );

		MeshCube cube;
		cube.init( BUF_POS | BUF_COLOR | BUF_NORMAL );
		cube.color( glm::vec3( 1.0*dist( gen ), 1.0*dist( gen ), 1.0*dist( gen )) );
		//cube.color( glm::vec3( 1,1,1 ) );
		cube.transform(transl2*rotate*scale*transl);
		cube.rotateNormals( rotate );
		meshes.push_back( cube );
	}*/

	for ( uint i = 0; i < meshes.size(); i++ )
		meshes[i].toBuffs( buf );
	buf.postInit();

	MeshPlane plane;
	plane.init(BUF_POS | BUF_COLOR | BUF_NORMAL);
	plane.color(glm::vec3(1, 1, 1));
	plane.transform(glm::scale(glm::mat4(1), glm::vec3(1000, 1, 1000)));
	plane.transform(glm::translate(glm::vec3(0, 1, 0)));
	plane.toBuffs(floorBuf);
	floorBuf.postInit();

	renderer.floorCol = simulation.getColor();
	renderer.init( frinf.rwidth, frinf.rheight );
	lDir.color = glm::vec3( 0.7,0.7,0.7 );
	lDir.shadow = true;
}

int cnt = 0;

void Scene::render(OglOIS &ois,frameinfo frinf)
{
	cnt++;
	if (!(cnt%10)) {
		glm::vec3 pos = simulation.getPos();
		cout << cnt << " " << simulation.getDist() << endl;
	}

	double lastTime = glfwGetTime();
	double time=0;
#ifdef DEBUG
	if ( finish )
		glFinish( );
#endif

	///////////////////////////////////////////////////////
	///////////////// input
	MouseInfo mi = ois.getmouse( );
	ois.keyb->capture( );
	if ( ois.keyb->isKeyDown( KC_SPACE ) /*|| cnt>2*/ )
		return;

	cam.doinput( mi, ois.keyb, frinf.delta );
	cam.follow( simulation.getPos() );
	lDir.input( mi, frinf, simulation.getPos() );

#ifdef DEBUG
	if (finish)
		glFinish();
	time = glfwGetTime( );
	(*(frinf.times))[0] += time - lastTime;
	lastTime = time;
#endif

	///////////////////////////////////////////////////////
	///////////////// render
	vector<Bufferism*> bufv( 2 );
	bufv[0] = { &buf };
	bufv[1] = { &floorBuf };
	glm::vec3 ambient = glm::vec3( 0.3, 0.3, 0.3 );

	renderer.render( &cam, bufv, ambient, &lDir, false, frinf, time, lastTime );

	GLenum err= glGetError();
	int u = 0;

	mat4x4 matrix;
	Mesh mesh = simulation.step(matrix);
	blob.rewrite(mesh, BUF_POS | BUF_COLOR | BUF_NORMAL);
	blob.transform(matrix);
	blob.toBuffsRewrite(buf);
	buf.newUpdate();
}

void Scene::resize(uint wi, uint he) {
	renderer.resize(wi, he);
}