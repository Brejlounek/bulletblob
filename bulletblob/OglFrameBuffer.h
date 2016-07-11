#include <GL/glew.h>
#include <glfw3.h>
#include <vector>
#include <iostream>
using namespace std;

#include "OglTex.h"

#define uint unsigned int

#define DEPTHBUF (1<<0)
#define DEPTHBUF_TEXTURE (1<<1)
#define DEPTHBUF_LINEAR (1<<2)
#define DEPTHBUF_MULTISAMPLED (1<<3)
#define DEPTHBUF_SHADOW (1<<4)

class OglFrameBuffer {
public:
	void init( vector<OglTexTarget*>, int depthFlags, uint wi, uint he );
	void enable();
	void reset();
	OglTexTarget depthTexture;
	uint wi, he;

private:
	GLuint id, depthId;
};