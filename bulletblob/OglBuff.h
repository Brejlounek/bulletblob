#ifndef OGLBUFFNOTCORE_6FD5H4564FH564DFG654H65F4TH56T456RT4
#define OGLBUFFNOTCORE_6FD5H4564FH564DFG654H65F4TH56T456RT4

#include "OglBuff_core.h"
#include "OglBuff_core.cpp"

#define uint unsigned int

class OglBuffVertex : public OglBuff<GLfloat> {
public:
	OglBuffVertex( );
	~OglBuffVertex( );

	void add3( glm::vec3 pos );
	void addv( vector<glm::vec3> data );
	void updateAll( vector<glm::vec3> posdata );

private:
};

class OglBuffIndex : public OglBuff<unsigned int> {
public:
	OglBuffIndex( );
	void enable( );
	void render( );

	void add( uint d );
	void add( uint *d, int size );
	void add( vector<uint> d );
	void minus(int cnt);
private:
	uint maxvert;
};

class OglBuffLine : public OglBuffVertex {
public:
	OglBuffLine( ) {
		buftype = GL_ARRAY_BUFFER;
		usage = GL_DYNAMIC_DRAW;
		updatable = true;
		list = false;
	}
	void render( );
	bool list;
};

#endif