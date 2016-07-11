#include "OglBuff.h"
#include <glm/glm.hpp>
#include <math.h>

OglBuffVertex::OglBuffVertex( ) {
	buftype = GL_ARRAY_BUFFER;
	updatable = false;
}
OglBuffVertex::~OglBuffVertex( )
{
	//glDeleteBuffers(1, &buffer);
}
void OglBuffVertex::addv( vector<glm::vec3> data )
{
	for ( unsigned int i = 0; i<data.size( ); i++ )
		add3( data[i] );
}
void OglBuffVertex::add3( glm::vec3 pos )
{
	vertexdata.push_back( pos.x );
	vertexdata.push_back( pos.y );
	vertexdata.push_back( pos.z );
}
void OglBuffVertex::updateAll( vector<glm::vec3> posdata )
{
	vertexdata.clear( );
	for ( unsigned int i = 0; i<posdata.size( ); i++ )
	{
		vertexdata.push_back( posdata[i].x );
		vertexdata.push_back( posdata[i].y );
		vertexdata.push_back( posdata[i].z );
	}
}

////////////////////////////////

OglBuffIndex::OglBuffIndex( ) {
	buftype = GL_ELEMENT_ARRAY_BUFFER;
	updatable = true;
	maxvert = 0;
}
void OglBuffIndex::enable( )
{
	glBindBuffer( buftype, buffer );
}
void OglBuffIndex::render( )
{
	glDrawElements( GL_TRIANGLES, cnt / len * 3, GL_UNSIGNED_INT, (void*) 0 );
}

#define max(a,b)            (((a) > (b)) ? (a) : (b))
void OglBuffIndex::add( uint d )
{
	uint prev = maxvert;
	vertexdata.push_back( d+prev );
	maxvert = max( maxvert, d+1 );
}
void OglBuffIndex::add( uint *d, int size )
{
	uint prev = maxvert;
	for ( int i = 0; i < size; i++ )
	{
		vertexdata.push_back( d[i] + prev );
		maxvert = max( maxvert, d[i] + prev );
	}
	maxvert++;
}
void OglBuffIndex::add( vector<uint> d )
{
	add( &d[0], d.size( ) );
}
void OglBuffIndex::minus(int cnt)
{
	/*for ( int i = 0; i<cnt; i++ )
	if ( vertexdata.size( )>0 )
	vertexdata.erase( vertexdata.begin( ) + vertexdata.size( ) - 1 );*/
	if (!cnt) {
		vertexdata.clear();
		return;
	}
	vertexdata.erase(vertexdata.end() - cnt, vertexdata.end());
	maxvert -= cnt;
}

//////////////////////////////////////

void OglBuffLine::render( )
{
	if ( list )
		glDrawArrays( GL_LINES, 0, cnt / len );
	else
		glDrawArrays( GL_LINE_STRIP, 0, cnt / len );
}