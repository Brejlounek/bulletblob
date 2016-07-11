#include "OglFrameBuffer.h"

void error( )
{
	cout << "error " << glGetError( ) << endl;
}

void OglFrameBuffer::init(vector<OglTexTarget*> tex, int depthFlags, uint wi_, uint he_)
{
	id = 0;
	wi = wi_; he = he_;
	glGenFramebuffers( 1, &id );
	glBindFramebuffer( GL_FRAMEBUFFER, id );

	if ( depthFlags & DEPTHBUF )
	{
		glGenRenderbuffers( 1, &depthId );
		glBindRenderbuffer( GL_RENDERBUFFER, depthId );
		glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, wi, he );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthId );
	}
	else if ( depthFlags & DEPTHBUF_TEXTURE )
	{
		depthTexture.intFormat = 0x8CAC;
		depthTexture.format = GL_DEPTH_COMPONENT;
		depthTexture.type = GL_FLOAT;
		if ( depthFlags & DEPTHBUF_LINEAR )
		{
			depthTexture.magFilter = GL_LINEAR;
			depthTexture.minFilter = GL_LINEAR;
		}
		if ( depthFlags & DEPTHBUF_SHADOW )
			depthTexture.compMode = GL_COMPARE_REF_TO_TEXTURE;
		depthTexture.init( wi, he );

		glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture.getId(), 0 );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
		float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color );
	}

	vector<GLenum> DrawBuffers;
	for ( int i = 0; i < tex.size(); i++ )
	{
		tex[i]->init( wi, he );
		glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, tex[i]->getId( ), 0 );
		DrawBuffers.push_back( GL_COLOR_ATTACHMENT0 + i );
	}

	if (DrawBuffers.size( ))
		glDrawBuffers( DrawBuffers.size(), &DrawBuffers[0] );

	if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
		cout << "framebuffer error" << endl;
}

void OglFrameBuffer::enable()
{
	glBindFramebuffer( GL_FRAMEBUFFER, id );
	glViewport( 0, 0, wi, he );
}

void OglFrameBuffer::reset() {
	glDeleteFramebuffers(1, &id);
}