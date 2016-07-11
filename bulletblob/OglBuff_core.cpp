#include "OglBuff_core.h"

template<class SType>
OglBuff<SType>::OglBuff( )
{
	usage = GL_STATIC_DRAW;
	updatable = true;
}
template<class SType>
void OglBuff<SType>::addType( GLenum type, int len )
{
	types.push_back( MyType( type, len ) );
}
template<class SType>
void OglBuff<SType>::bind( )
{
	int allocsz = vertexdata.capacity( )*typesz;

	glGenBuffers( 1, &buffer );
	glBindBuffer( buftype, buffer );
	if ( !updatable )
		glBufferData( buftype, sz, data, usage );
	else
	{
		glBufferData( buftype, allocsz, 0, usage );
		glBufferData( buftype, allocsz, data, usage );
	}
}
template<class SType>
void OglBuff<SType>::enable( )
{
	for ( unsigned int i = 0; i<types.size( ); i++ )
	{
		glEnableVertexAttribArray( i );
	}

	glBindBuffer( GL_ARRAY_BUFFER, buffer );

	int nowlen = 0;
	for ( unsigned int i = 0; i<types.size( ); i++ )
	{
		glVertexAttribPointer( i, types[i].len, types[i].type, GL_FALSE, typesz*glen, (void*) (typesz*nowlen) );
		nowlen += types[i].len;
	}
}
template<class SType>
void OglBuff<SType>::disable( )
{
	for ( unsigned int i = 0; i<types.size( ); i++ )
		glDisableVertexAttribArray( i );
}
template<class SType>
void OglBuff<SType>::setUpdatable()
{
	updatable = true;
}
template<class SType>
void OglBuff<SType>::render( )
{
	glDrawArrays( GL_TRIANGLES, 0, cnt / len );
}
template<class SType>
OglBuff<SType>::~OglBuff( )
{
	int u = 0;
	//glDeleteBuffers(1, &buffer);
}

///////////////////////////////////////////////

template<class SType>
void OglBuff<SType>::fill( SType* d, int size )
{
	glen = 0;
	for ( unsigned int i = 0; i<types.size( ); i++ )
		glen += types[i].len;

	for ( unsigned int i = 0; i<(unsigned int) size; i++ )
		vertexdata.push_back( d[i] );

	data = &vertexdata[0];
	len = 3;
	cnt = size;
	sz = size*sizeof(GLfloat);
	typesz = sizeof(GLfloat);
}
template<class SType>
void OglBuff<SType>::add( SType d )
{
	vertexdata.push_back( d );
}
template<class SType>
void OglBuff<SType>::add( SType *d, int size )
{
	for(int i=0;i<size;i++)
		vertexdata.push_back( d[i] );
}
template<class SType>
void OglBuff<SType>::add( vector<SType> d )
{
	add( &d[0], d.size() );
}
template<class SType>
void OglBuff<SType>::minus( int cnt )
{
	/*for ( int i = 0; i<cnt; i++ )
	if ( vertexdata.size( )>0 )
		vertexdata.erase( vertexdata.begin( ) + vertexdata.size( ) - 1 );*/
	if (!cnt) {
		vertexdata.clear();
		return;
	}
	vertexdata.erase(vertexdata.end() - cnt, vertexdata.end());
}
template<class SType>
void OglBuff<SType>::wipe()
{
	vertexdata.clear();
}
template<class SType>
void OglBuff<SType>::finalize( )
{
	glen = 0;
	for ( unsigned int i = 0; i<types.size( ); i++ )
		glen += types[i].len;

	data = &vertexdata[0];
	len = 3;
	cnt = vertexdata.size( );
	sz = vertexdata.size()*sizeof(SType);
	typesz = sizeof(SType);
}
template<class SType>
void OglBuff<SType>::updateAll( vector<SType> newdata )
{
	vertexdata = newdata;
}
template<class SType>
void OglBuff<SType>::reset( )
{
	vertexdata.clear( );
}

//////////////////////////////////////

template<class SType>
int OglBuff<SType>::allocsize( )
{
	int alloccnt = 100;
	while ( alloccnt<cnt )
		alloccnt *= 4;
	int typesize = sz / cnt;
	int allocsz = typesize*alloccnt;
	return allocsz;
}

template<class SType>
void OglBuff<SType>::update( )
{
	int allocsz = vertexdata.capacity( )*typesz;

	glBindBuffer( buftype, buffer );
	//glBufferData(buftype, allocsz,0, usage); //hack from net
	glBufferData( buftype, allocsz, data, usage );
}