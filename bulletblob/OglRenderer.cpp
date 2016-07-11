#include <random>
#include <chrono>
#include <dirent.h>
#include <string>

#include "OglRenderer.h"
#include "Meshes.h"

uint seed = chrono::system_clock::now( ).time_since_epoch( ).count( );
default_random_engine gen( seed );
uniform_real_distribution<double> dist( 0.0, 1.0 );

void OglRenderer::init(uint wi,uint he)
{
	texVelocity.intFormat = GL_RG;
	texVelocity.format = GL_RG;
	//texVelocity.type = GL_FLOAT;

	texColor.intFormat = GL_RGBA;
	texColor.format = GL_RGBA;

	vector<OglTexTarget*> textars;
	textars.push_back( &texColor );
	textars.push_back( &texNormal );
	textars.push_back( &texVelocity );
	GBuffer.init( textars, DEPTHBUF_TEXTURE, wi, he );

	textars.clear( );
	DirLBuffer.init( textars, DEPTHBUF_TEXTURE | DEPTHBUF_LINEAR | DEPTHBUF_SHADOW, 1024, 1024 );

	texNum = 2;
	cnt = 0;

	for ( int i = 0; i < texNum; i++ )
	{
		textars.clear();
		texAA[i].intFormat = GL_RGBA;
		texAA[i].format = GL_RGBA;
		texAA[i].magFilter = GL_LINEAR;
		texAA[i].minFilter = GL_LINEAR;
		texAA[i].clampBorder = true;
		textars.push_back( &texAA[i] );
		AABuffer[i].init( textars, 0, wi, he );
	}
	AAp = 0;

	textars.clear( );
	texAATemp.intFormat = GL_RGBA;
	texAATemp.format = GL_RGBA;
	texAATemp.magFilter = GL_LINEAR;
	texAATemp.minFilter = GL_LINEAR;
	textars.push_back( &texAATemp );
	AATemp.init( textars, 0, wi, he );

	textars.clear( );
	texAO.intFormat = GL_RG32F;
	texAO.format = GL_RG;
	//texAO.type = GL_FLOAT;
	texAO.magFilter = GL_LINEAR;
	texAO.minFilter = GL_LINEAR;
	texAO.clampBorder = true;
	textars.push_back( &texAO );
	AOBuffer.init( textars, 0, wi/2-1, he/2-1 );

	createRandomTexture();
	floorTex.magFilter = GL_LINEAR;
	floorTex.minFilter = GL_LINEAR;
	floorTex.init("floor.jpg",true);

	DIR *d;
	struct dirent *dir;
	d = opendir( "shaders" );
	int i = 0;
	if ( d )
	{
		while ( (dir = readdir( d )) != NULL )
		{
			if ( i > 1 )
			{
				string name = dir->d_name;
				string subname = name.substr( name.length() - 4, 4 );
				if ( subname == ".vsh" )
				{
					cout << "vertex: " << name << endl;
					string compileName = "shaders\\" + name;
					OglVertexShader vsh; vsh.compile( compileName.c_str() );
					vsh.notDelete();
					shVertex[name] = vsh;
				}
				else if ( subname == ".fsh" )
				{
					cout << "fragment: " << name << endl;
					string compileName = "shaders\\" + name;
					OglFragmentShader fsh; fsh.compile( compileName.c_str( ) );
					fsh.notDelete();
					shFragment[name] = fsh;
				}
				else
					cout << "error reading shader " << name << endl;
			}
			i++;
		}

		closedir( d );
	}

	buildShader( "GBuff", "GBuff.vsh", "GBuff.fsh" );
	buildShader( "GBuffFloor", "GBuff.vsh", "GBuffFloor.fsh" );
	buildShader( "GBuffAA", "GBuffAA.vsh", "GBuffAA.fsh" );
	buildShader( "AmbQ", "Quad.vsh", "AmbQ.fsh" );
	buildShader( "AmbDirQ", "Quad.vsh", "AmbDirQ.fsh" );
	buildShader( "AmbDirShadowQ", "QuadInv.vsh", "AmbDirShadowQ.fsh" );
	buildShader( "Quad", "Quad.vsh", "Quad.fsh" );
	buildShader( "QuadFXAA", "Quad.vsh", "QuadFXAA.fsh");
	buildShader( "QuadAA", "Quad.vsh", "QuadAA.fsh" );
	buildShader( "Blur", "Quad.vsh", "Blur.fsh" );
	buildShader( "ShadowMap", "ShadowMap.vsh", "ShadowMap.fsh" );
	buildShader( "EVSMGauss", "Quad.vsh", "EVSMGauss.fsh" );
	buildShader( "EVSMGaussV", "Quad.vsh", "EVSMGaussV.fsh" );
	buildShader( "AO", "QuadInv.vsh", "AO.fsh" );

	activeShader = 0;

	MeshFullscreenQuad quad;
	quad.init( BUF_POS | BUF_UV );
	bufQuad.init( BUF_POS | BUF_UV );

	quad.toBuffs( bufQuad );
	bufQuad.postInit( );

	lastWiggle = glm::vec3( 0, 0, 0 );
	wp = 0;
	rx[0] = 0.16; ry[0] = 0.35;
	rx[1] = 0.82; ry[1] = 0.20;
	rx[2] = 0.40; ry[2] = 0.88;
	rx[3] = 0.84; ry[3] = 0.69;

	enableBuffer( &AABuffer[0], true );
	enableBuffer( &AABuffer[1], true );
}

void OglRenderer::resize(uint wi, uint he) {
	texVelocity.intFormat = GL_RG;
	texVelocity.format = GL_RG;
	//texVelocity.type = GL_FLOAT;

	texColor.intFormat = GL_RGBA;
	texColor.format = GL_RGBA;

	vector<OglTexTarget*> textars;
	textars.push_back(&texColor);
	textars.push_back(&texNormal);
	textars.push_back(&texVelocity);
	GBuffer.reset();
	GBuffer.init(textars, DEPTHBUF_TEXTURE, wi, he);

	texNum = 2;
	cnt = 0;

	for (int i = 0; i < texNum; i++)
	{
		textars.clear();
		texAA[i].intFormat = GL_RGBA;
		texAA[i].format = GL_RGBA;
		texAA[i].magFilter = GL_LINEAR;
		texAA[i].minFilter = GL_LINEAR;
		texAA[i].clampBorder = true;
		textars.push_back(&texAA[i]);
		AABuffer[i].reset();
		AABuffer[i].init(textars, 0, wi, he);
	}
	AAp = 0;

	textars.clear();
	texAATemp.intFormat = GL_RGBA;
	texAATemp.format = GL_RGBA;
	texAATemp.magFilter = GL_LINEAR;
	texAATemp.minFilter = GL_LINEAR;
	textars.push_back(&texAATemp);
	AATemp.reset();
	AATemp.init(textars, 0, wi, he);

	textars.clear();
	texAO.intFormat = GL_RG32F;
	texAO.format = GL_RG;
	//texAO.type = GL_FLOAT;
	texAO.magFilter = GL_LINEAR;
	texAO.minFilter = GL_LINEAR;
	texAO.clampBorder = true;
	textars.push_back(&texAO);
	AOBuffer.reset();
	AOBuffer.init(textars, 0, wi/2-1, he/2-1);

	glClearColor(0, 0, 0, 0);
}

void OglRenderer::createRandomTexture( )
{
	vector<uchar> data;
	for ( uint i = 0; i < 64 * 64; i++ )
	{
		/*uchar r = dist( gen )*256;
		GLfloat angle = dist( gen ) * 2 * PI;
		uchar g = cos( angle )*128+128;
		uchar b = sin( angle )*128+128;*/
		uchar r = dist( gen ) * 256;
		uchar g = dist( gen ) * 256;
		uchar b = dist( gen ) * 256;

		data.push_back( r );
		data.push_back( g );
		data.push_back( b );
	}

	randomTex.intFormat = GL_RGB;
	randomTex.format = GL_RGB;
	randomTex.clampBorder = false;
	randomTex.magFilter = GL_LINEAR;
	randomTex.minFilter = GL_LINEAR;
	randomTex.init( &data[0], 64, 64 );
}

void OglRenderer::buildShader( string strProg, string strVertex, string strFragment )
{
	OglShaderProg prog;
	prog.link( shVertex[strVertex], shFragment[strFragment] );
	shProg[strProg] = prog;
}

void OglRenderer::setCamera( glm::mat4 camMat, bool wiggle )
{
	activeShader->importValue( "MVP", camMat );
}
void OglRenderer::setCameraPrev( glm::mat4 camMat, bool wiggle )
{
	activeShader->importValue( "MVPprev", camMat );
}
void OglRenderer::setCameraNoTransl( glm::mat4 camMat, bool wiggle )
{
	activeShader->importValue( "MVPnoTransl", camMat );
}

void OglRenderer::enableBuffer( OglFrameBuffer *buffer, bool clear )
{
	if ( buffer )
		buffer->enable();
	else
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	if (clear)
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}






void OglRenderer::GBufferP( OglFrameBuffer *buffer, uint wi, uint he )
{
	enableBuffer(buffer);
	glViewport(0, 0, wi, he);

	activeShader = &shProg["GBuff"];
	activeShader->enable( );
}
void OglRenderer::GBufferFloor(OglFrameBuffer *buffer, uint wi, uint he)
{
	/*enableBuffer(buffer);
	glViewport(0, 0, wi, he);*/

	activeShader = &shProg["GBuffFloor"];
	activeShader->enable();
	activeShader->importValue("floorTex", &floorTex);
	activeShader->importValue("col", floorCol);
}
void OglRenderer::GBufferAA( OglFrameBuffer *buffer, uint wi, uint he )
{
	enableBuffer( buffer );

	glm::vec3 wiggle = glm::vec3( (rx[wp]*0 + dist( gen )*1 - 0.5) * 1.5f / wi, (ry[wp]*0 + dist( gen )*1 - 0.5) * 1.5f / he, 0 );
	activeShader = &shProg["GBuffAA"];
	activeShader->enable( );
	activeShader->importValue( "wiggle", wiggle );
	activeShader->importValue( "lastWiggle", lastWiggle );
	lastWiggle = wiggle;

	wp++;
	wp %= 4;
}







void OglRenderer::Ambient( OglFrameBuffer *buffer, glm::vec3 ambient, uint wi, uint he )
{
	enableBuffer( buffer );
	activeShader = &shProg["AmbQ"];

	activeShader->enable( );
	activeShader->importValue( "colorTexture", &texColor );
	activeShader->importValue( "ambient", ambient );
	bufQuad.render();
}
void OglRenderer::AmbDir( OglFrameBuffer *buffer, glm::vec3 ambient, LightData lDirData, CamData camData, uint wi, uint he )
{
	enableBuffer( &AOBuffer );
	glViewport( 0, 0, AOBuffer.wi, AOBuffer.he );
	static float iRand = 1;

	activeShader = &shProg["AO"];
	activeShader->enable( );
	activeShader->importValue( "normalTexture", &texNormal );
	activeShader->importValue( "depthTexture", &GBuffer.depthTexture );
	activeShader->importValue( "randomTexture", &randomTex );

	activeShader->importValue( "pixel", glm::vec2( 1.0f / wi, 1.0f / he ) );
	activeShader->importValue( "MVP", camData.matrix );
	activeShader->importValue( "iRand", iRand );

	activeShader->importValue( "camPos", camData.pos );
	activeShader->importValue( "zClip", camData.zClip );
	activeShader->importValue( "zClipRatio", camData.zClipRatio );
	activeShader->importValue( "invMat", camData.invMatrix );

	bufQuad.render( );





	enableBuffer( buffer );
	glViewport( 0, 0, wi, he );

	if ( lDirData.shadow )
		activeShader = &shProg["AmbDirShadowQ"];
	else
		activeShader = &shProg["AmbDirQ"];

	activeShader->enable();
	activeShader->importValue( "colorTexture", &texColor );
	activeShader->importValue( "normalTexture", &texNormal );
	activeShader->importValue( "depthTexture", &GBuffer.depthTexture );
	activeShader->importValue( "aoTexture", &texAO );

	activeShader->importValue( "ambient", ambient );
	activeShader->importValue( "dirL", lDirData.pos );
	activeShader->importValue( "dirColor", lDirData.color );
	activeShader->importValue( "pixel", glm::vec2( 1.0f / wi, 1.0f / he) );
	activeShader->importValue( "iRand", iRand );
	activeShader->importValue( "MVP", camData.matrix );

	if ( lDirData.shadow )
	{
		activeShader->importValue( "camPos", camData.pos );
		activeShader->importValue( "zClip", camData.zClip );
		activeShader->importValue( "zClipRatio", camData.zClipRatio );
		activeShader->importValue( "invMat", camData.invMatrix );

		activeShader->importValue( "lDirMVP", lDirData.MVP );
		activeShader->importValue( "lDirDepthTexture", /*&texDirLRange[1]*/ &DirLBuffer.depthTexture );
		activeShader->importValue( "lDirPixel", glm::vec2( 1.0f / 1024, 1.0f / 1024 ) );
		activeShader->importValue( "randomTexture", &randomTex );
	}

	iRand+=1;
	if ( iRand > INT_MAX-1 ) iRand = 1;
	bufQuad.render( );
}











void OglRenderer::Quad( OglFrameBuffer *buffer, uint wi, uint he )
{
	enableBuffer( buffer );
	activeShader = &shProg["Quad"];

	activeShader->enable( );
	activeShader->importValue( "tex", /*&texAA[AAp]*/ &DirLBuffer.depthTexture );
	bufQuad.render( );

	AAp++;
	AAp %= texNum;
}
void OglRenderer::QuadFXAA(OglFrameBuffer *buffer, uint wi, uint he)
{
	enableBuffer(buffer);
	activeShader = &shProg["QuadFXAA"];

	activeShader->enable();
	activeShader->importValue("tex", &texAA[0]);
	activeShader->importValue("pixel", glm::vec2(1.0f / wi, 1.0f / he));
	bufQuad.render();
}
void OglRenderer::QuadAA( OglFrameBuffer *buffer, uint wi, uint he )
{
	static float iRand = 0;
	enableBuffer( buffer );
	activeShader = &shProg["QuadAA"];

	activeShader->enable( );
	activeShader->importValue( "tex", &texAATemp );
	activeShader->importValue( "prev0", &texAA[(AAp - 1) % texNum] );
	activeShader->importValue( "velTexture", &texVelocity );
	activeShader->importValue( "iRand", iRand );
	activeShader->importValue( "wiggle", lastWiggle );

	activeShader->importValue( "pixel", glm::vec3( 1.0f / wi, 1.0f / he, 0 ) );
	bufQuad.render( );

	iRand++;
	if ( iRand == 2 ) iRand = 0;
	//AAp++;
	//AAp %= texNum;
}











void OglRenderer::LightDirDepth( OglFrameBuffer *buffer, LightData lDirData, vector<Bufferism*> bufferisms, uint wi, uint he )
{
	enableBuffer( buffer );
	activeShader = &shProg["ShadowMap"];

	glViewport( 0, 0, DirLBuffer.wi, DirLBuffer.he );
	activeShader->enable( );
	activeShader->importValue( "MVP", lDirData.MVP );

	for ( int i = 0; i<bufferisms.size( ); i++ )
		bufferisms[0]->render( );

	//glViewport( 0, 0, wi, he );
}

void OglRenderer::EVSMGauss( OglFrameBuffer *buffer, uint wi, uint he )
{
	enableBuffer( buffer );
	activeShader = &shProg["EVSMGauss"];
	glViewport( 0, 0, 128, 128);

	activeShader->enable( );
	activeShader->importValue( "tex", &DirLBuffer.depthTexture );

	activeShader->importValue( "pixel", glm::vec2( 1.0f / 128, 1.0f / 128 ) );
	bufQuad.render( );

	//glViewport( 0, 0, wi, he );
}
void OglRenderer::EVSMGaussV( OglFrameBuffer *buffer, uint wi, uint he )
{
	enableBuffer( buffer );
	activeShader = &shProg["EVSMGaussV"];
	glViewport( 0, 0, 128, 128 );

	activeShader->enable( );
	activeShader->importValue( "tex", &texDirLRange[0] );

	activeShader->importValue( "pixel", glm::vec2( 1.0f / 128, 1.0f / 128 ) );
	bufQuad.render( );

	//glViewport( 0, 0, wi, he );
}