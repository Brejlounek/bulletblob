#include "Meshes.h"

/////////////////////////////////////////////////
////////////////// cube

GLfloat cubeVerts[] = {
	-1, 1, 1,
	1, 1, 1,
	1, 1, -1,
	-1, 1, -1,
	-1, -1, 1,
	1, -1, 1,
	1, -1, -1,
	-1, -1, -1,
};

GLfloat cubeNormals[] = {
	1, 0, 0,
	-1, 0, 0,
	0, 1, 0,
	0, -1, 0,
	0, 0, 1,
	0, 0, -1,
};

GLfloat cubeUV[] = {
	0, 0,
	0, 1,
	1, 1,
	1, 0,
};

uint cubeInds[] = {
	//X
	1, 5, 6,
	1, 6, 2,
	3, 7, 4,
	3, 4, 0,
	//Y
	1, 2, 3,
	1, 3, 0,
	4, 7, 6,
	4, 6, 5,
	//Z
	0, 4, 5,
	0, 5, 1,
	2, 6, 7,
	2, 7, 3,
};

void MeshCube::init( uint flags )
{
	fl = flags;
	for ( int i = 0; i < 8*3; i++ )
	{
		uint si = i % 8;
		uint face = i / 8;
		uint normalface = i / 4;
		if ( flags & BUF_POS )
		{
			verts.push_back( cubeVerts[si * 3 + 0] );
			verts.push_back( cubeVerts[si * 3 + 1] );
			verts.push_back( cubeVerts[si * 3 + 2] );
		}
		if ( flags & BUF_COLOR )
		{
			verts.push_back( 1 );
			verts.push_back( 0 );
			verts.push_back( 1 );
		}
		if ( flags & BUF_NORMAL )
		{
			verts.push_back( cubeNormals[0] );
			verts.push_back( cubeNormals[0] );
			verts.push_back( cubeNormals[0] );
		}
		if ( flags & BUF_UV )
		{
			verts.push_back( 0 );
			verts.push_back( 0 );
		}
	}

	for ( int i = 0; i < 12; i++ )
	{
		uint face = i / 4;
		inds.push_back( cubeInds[i * 3 + 0] + face * 8 );
		inds.push_back( cubeInds[i * 3 + 1] + face * 8 );
		inds.push_back( cubeInds[i * 3 + 2] + face * 8 );
	}

	stepSize = 0;
	if ( flags & BUF_POS )
		stepSize += 3;
	if ( flags & BUF_COLOR )
		stepSize += 3;
	if ( flags & BUF_NORMAL )
		stepSize += 3;
	if ( flags & BUF_UV )
		stepSize += 2;

	if ( flags & BUF_NORMAL )
		calcNormals( );
}





/////////////////////////////////////////////////
////////////////// plane

GLfloat planeVerts[] = {
	-1, 0, -1,
	1, 0, -1,
	1, 0, 1,
	-1, 0, 1,

	-1, 0, -1,
	1, 0, -1,
	1, 0, 1,
	-1, 0, 1,
};

GLfloat planeUV[] = {
	0, 0,
	1, 0,
	1, 1,
	0, 1,

	0, 0,
	1, 0,
	1, 1,
	0, 1,
};

uint planeInds[] = {
	0, 2, 1,
	0, 3, 2,
	4, 5, 6,
	4, 6, 7,
};

void MeshPlane::init( uint flags )
{
	fl = flags;
	for ( int i = 0; i < 8; i++ )
	{
		if ( flags & BUF_POS )
		{
			verts.push_back( planeVerts[i * 3 + 0] );
			verts.push_back( planeVerts[i * 3 + 1] );
			verts.push_back( planeVerts[i * 3 + 2] );
		}
		if ( flags & BUF_COLOR )
		{
			verts.push_back( 1 );
			verts.push_back( 0 );
			verts.push_back( 1 );
		}
		if ( flags & BUF_NORMAL )
		{
			verts.push_back( 0 );
			verts.push_back( 0 );
			verts.push_back( 0 );
		}
		if ( flags & BUF_UV )
		{
			verts.push_back( planeUV[i * 2 + 0] );
			verts.push_back( planeUV[i * 2 + 1] );
		}
	}

	for ( int i = 0; i < 4; i++ )
	{
		inds.push_back( planeInds[i * 3 + 0] );
		inds.push_back( planeInds[i * 3 + 1] );
		inds.push_back( planeInds[i * 3 + 2] );
	}

	stepSize = 0;
	if ( flags & BUF_POS )
		stepSize += 3;
	if ( flags & BUF_COLOR )
		stepSize += 3;
	if ( flags & BUF_NORMAL )
		stepSize += 3;
	if ( flags & BUF_UV )
		stepSize += 2;
	if ( flags & BUF_NORMAL )
		calcNormals();
}






/////////////////////////////////////////////////
////////////////// fullscreen quad

GLfloat fsquadVerts[] = {
	-1, -1, 0,
	1, -1, 0,
	1, 1, 0,
	-1, 1, 0,
};

GLfloat fsquadUV[] = {
	0, 0,
	1, 0,
	1, 1,
	0, 1,
};

uint fsquadInds[] = {
	0, 1, 2,
	0, 2, 3,
};

void MeshFullscreenQuad::init( uint flags )
{
	fl = flags;
	if ( flags != (BUF_POS | BUF_UV) )
		cout << "bad fullscreen quad flags" << endl;
	for ( int i = 0; i < 4; i++ )
	{
		if ( flags & BUF_POS )
		{
			verts.push_back( fsquadVerts[i * 3 + 0] );
			verts.push_back( fsquadVerts[i * 3 + 1] );
			verts.push_back( fsquadVerts[i * 3 + 2] );
		}
		if ( flags & BUF_UV )
		{
			verts.push_back( fsquadUV[i * 2 + 0] );
			verts.push_back( fsquadUV[i * 2 + 1] );
		}
	}

	for ( int i = 0; i < 2; i++ )
	{
		inds.push_back( fsquadInds[i * 3 + 0] );
		inds.push_back( fsquadInds[i * 3 + 1] );
		inds.push_back( fsquadInds[i * 3 + 2] );
	}

	stepSize = 0;
	if ( flags & BUF_POS )
		stepSize += 3;
	if ( flags & BUF_COLOR )
		stepSize += 3;
	if ( flags & BUF_NORMAL )
		stepSize += 3;
	if ( flags & BUF_UV )
		stepSize += 2;
}




//////////////////////////////////////////////////////
////////////// meshy mesh

void MeshyMesh::init(Mesh mesh, uint flags) {
	fl = flags;
	for (int i = 0; i < mesh.vertices.size(); i++)
	{
		if (flags & BUF_POS)
		{
			verts.push_back(mesh.vertices[i].x);
			verts.push_back(mesh.vertices[i].y);
			verts.push_back(mesh.vertices[i].z);
		}
		if (flags & BUF_COLOR)
		{
			if (mesh.colors.size()) {
				verts.push_back(mesh.colors[i].x);
				verts.push_back(mesh.colors[i].y);
				verts.push_back(mesh.colors[i].z);
			}
			else {
				verts.push_back(1);
				verts.push_back(1);
				verts.push_back(1);
			}
		}
		if (flags & BUF_NORMAL)
		{
			verts.push_back(0);
			verts.push_back(0);
			verts.push_back(0);
		}
		if (flags & BUF_UV)
		{
			verts.push_back(0);
			verts.push_back(1);
		}
	}

	for (int i = 0; i < mesh.faces.size(); i++)
	{
		inds.push_back(mesh.faces[i].v1);
		inds.push_back(mesh.faces[i].v2);
		inds.push_back(mesh.faces[i].v3);
	}

	stepSize = 0;
	if (flags & BUF_POS)
		stepSize += 3;
	if (flags & BUF_COLOR)
		stepSize += 3;
	if (flags & BUF_NORMAL)
		stepSize += 3;
	if (flags & BUF_UV)
		stepSize += 2;
	if (flags & BUF_NORMAL)
		calcNormals();

	vS = verts.size();
	iS = inds.size();
}

void MeshyMesh::rewrite(Mesh mesh, uint flags) {
	verts.clear();
	inds.clear();

	uint vStemp = vS, iStemp = iS;
	init(mesh, flags);
	vS = vStemp;
	iS = iStemp;
}