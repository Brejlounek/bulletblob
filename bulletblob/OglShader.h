#ifndef OGLSHADER_ED561HDF4G614764R64Z
#define OGLSHADER_ED561HDF4G614764R64Z

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

#include "OglTex.h"

#define uint unsigned int

struct UniformNameType {
	UniformNameType( string name, char type ) :name( name ), type( type ) {}
	string name;
	char type;
};
struct UniformTypeID {
	UniformTypeID( ) { type = 0; id = 0; }
	UniformTypeID( char type, GLuint id ) :type( type ), id( id ) {}
	char type;
	GLuint id;
};

class OglShader {
public:
	virtual void compile_( const char * file_path, GLenum type );
	OglShader( );
	virtual ~OglShader( );
	void notDelete( );
	GLuint ShaderID;
	vector<UniformNameType> uniformtypes;
private:
	bool del;
};
class OglVertexShader : public OglShader {
public:
	void compile( const char * file_path );
};
class OglFragmentShader : public OglShader {
public:
	void compile( const char * file_path );
};

class OglShaderProg {
public:
	OglShaderProg();
	void link( OglShader& vsh, OglShader& fsh );
	void enable( );
	void setColor( float r, float g, float b );
	void regTexture( OglTex *tex );

	void importValue( string name, float data );
	void importValue( string name, int data );
	void importValue( string name, glm::mat4 data );
	void importValue( string name, glm::vec3 data );
	void importValue( string name, glm::vec2 data );
	void importValue( string name, OglTex *data );

	GLuint ProgramID;
private:
	void processUniform( UniformNameType unf );
	map<string, UniformTypeID> uniforms;
	map<OglTex*, uint> textureMap;
	uint textures;
};

#endif