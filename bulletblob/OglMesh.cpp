#include "OglMesh.h"
#include <map>
#include <vector>
using namespace std;

void OglMesh::toBuffs( Bufferism &buf )
{
	buf.add( verts );
	buf.add( inds );
}
void OglMesh::toBuffsRewrite(Bufferism &buf)
{
	buf.minusV(vS);
	buf.minusI(iS);

	buf.add(verts);
	buf.add(inds);

	vS = verts.size();
	iS = inds.size();
}

void OglMesh::color( glm::vec3 color )
{
	for ( uint i = 0; i < verts.size(); i += stepSize )
	{
		verts[i + 3] = color.x;
		verts[i + 4] = color.y;
		verts[i + 5] = color.z;
	}
}

void OglMesh::transform( glm::mat4 matrix )
{
	for ( uint i = 0; i < verts.size(); i += stepSize )
	{
		glm::vec4 pos = glm::vec4( verts[i], verts[i + 1], verts[i + 2], 1 );
		pos = matrix * pos;
		verts[i] = pos.x;
		verts[i + 1] = pos.y;
		verts[i + 2] = pos.z;
	}
	
	glm::mat4 rotMatrix = matrix;
	rotMatrix[3][0] = 0;
	rotMatrix[3][1] = 0;
	rotMatrix[3][2] = 0;
	rotateNormals(rotMatrix);
}

void OglMesh::rotateNormals( glm::mat4 matrix )
{
	uint startid = 3;
	if ( fl & BUF_COLOR )
		startid += 3;

	for ( uint i = 0; i < verts.size( ); i += stepSize )
	{
		glm::vec4 pos = glm::vec4( verts[i + startid], verts[i + 1 + startid], verts[i + 2 + startid], 1 );
		pos = matrix * pos;
		verts[i + startid] = pos.x;
		verts[i + 1 + startid] = pos.y;
		verts[i + 2 + startid] = pos.z;
	}
}

void OglMesh::calcNormals()
{

	//REDO 
	uint startid = 3;
	if ( fl & BUF_COLOR )
		startid+=3;

	vector<glm::vec3> normal;
	for ( uint i = 0; i < verts.size(); i += stepSize )
		normal.push_back( glm::vec3( 0, 0, 0 ) );

	if ( inds.size() > 0 )
	for ( uint i = 0; i < inds.size(); i+=3 )
	{
		vector<glm::vec3> triverts;
		for ( uint q = 0; q < 3; q++ )
		{
			glm::vec3 pos;
			pos.x = verts[inds[i + q] * stepSize + 0];
			pos.y = verts[inds[i + q] * stepSize + 1];
			pos.z = verts[inds[i + q] * stepSize + 2];
			triverts.push_back( pos );
		}

		glm::vec3 cross = glm::cross( triverts[0] - triverts[1], triverts[1] - triverts[2] );
		cross = glm::normalize( cross );
		normal[inds[i + 0]] += cross;
		normal[inds[i + 1]] += cross;
		normal[inds[i + 2]] += cross;
	}

	for ( uint i = 0; i < normal.size(); i++ )
	{
		if ( normal[i] == glm::vec3( 0, 0, 0 ) )
			continue;
		normal[i] = glm::normalize( normal[i] );
	}
	for ( uint i = 0; i < normal.size(); i++ )
	{
		verts[i*stepSize + startid + 0] = normal[i].x;
		verts[i*stepSize + startid + 1] = normal[i].y;
		verts[i*stepSize + startid + 2] = normal[i].z;
	}
}




void Bufferism::init( uint flags )
{
	if ( flags & BUF_POS )
		vbuf.addType( GL_FLOAT, 3 );
	if ( flags & BUF_COLOR )
		vbuf.addType( GL_FLOAT, 3 );
	if ( flags & BUF_NORMAL )
		vbuf.addType( GL_FLOAT, 3 );
	if ( flags & BUF_UV )
		vbuf.addType( GL_FLOAT, 2 );

	ibuf.addType( 0, 1 );
}

void Bufferism::postInit( )
{
	vbuf.finalize( );
	vbuf.bind( );
	ibuf.finalize( );
	ibuf.bind( );
}
void Bufferism::newUpdate() {
	vbuf.finalize();
	vbuf.update();
	ibuf.finalize();
	ibuf.update();
}

void Bufferism::render()
{
	vbuf.enable( );
	ibuf.enable( );
	ibuf.render( );
	vbuf.disable( );
	ibuf.disable();
}




void Bufferism::add( GLfloat v )
{
	vbuf.add( v );
}
void Bufferism::add( GLfloat* v, uint size )
{
	vbuf.add( v, size );
}
void Bufferism::add( vector<GLfloat> v )
{
	vbuf.add( v );
}

void Bufferism::add( uint v )
{
	ibuf.add( v );
}
void Bufferism::add( uint* v, uint size )
{
	ibuf.add( v, size );
}
void Bufferism::add( vector<uint> v )
{
	ibuf.add( v );
}

void Bufferism::minusV(int cnt) {
	vbuf.minus(cnt);
}
void Bufferism::minusI(int cnt) {
	ibuf.minus(cnt);
}