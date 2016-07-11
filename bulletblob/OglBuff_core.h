#ifndef OGLBUFF_6FD5H4564FH564DFG654H65F4TH56T456RT4
#define OGLBUFF_6FD5H4564FH564DFG654H65F4TH56T456RT4

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <GL/glew.h>

#include <gl/gl.h>
#include <gl/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

#define TYPE_P 0
#define TYPE_PC 1

class MyType {
public:
	MyType( GLenum type, int len ) :type( type ), len( len ) {}
	GLenum type;
	int len;
};

template<class SType>
class OglBuff {
public:
	OglBuff( );
	~OglBuff( );
	void bind( );
	void addType( GLenum type, int len );
	void enable( );
	void disable( );
	void render( );
	int allocsize( );

	void fill( SType *d, int size );
	void add( SType d );
	void add( SType *d, int size );
	void add( vector<SType> d );
	void minus( int cnt );
	void wipe();

	void finalize( );
	void update( );
	void reset( );
	void updateAll( vector<SType> newdata );
	void setUpdatable();

	vector<SType> vertexdata;
protected:
	GLuint buffer;
	void* data;
	vector<MyType> types;
	int len, sz, cnt, glen, typesz;
	GLenum buftype, usage;
	bool updatable;
};

#endif