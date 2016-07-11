#ifndef TEXTURE_DF654GH65D4FH456H4456FG4JH56G4F
#define TEXTURE_DF654GH65D4FH456H4456FG4JH56G4F

#include <GL/glew.h>
#include <glfw3.h>
#include <IL/il.h>
#include <iostream>
using namespace std;

#define uint unsigned int
#define uchar unsigned char

class OglTex {
public:
	OglTex();
	void init(bool mipmaps = false);
	void init( GLvoid* data, uint wi, uint he );
	void init( string file, bool mipmaps = false );
	GLuint getId() { return id; }

	GLenum format, type, intFormat, minFilter, magFilter, compMode;
	bool clampBorder; float color[4];
protected:
	GLuint id;
};

class OglTexTarget : public OglTex {
public:
	void init(uint wi, uint he);
};

#endif