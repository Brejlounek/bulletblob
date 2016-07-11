#include "OglMesh.h"
#include "BlobGrid.h"

class MeshFullscreenQuad : public OglMesh {
public:
	void init( uint flags );
};

class MeshPlane : public OglMesh {
public:
	void init( uint flags );
};

class MeshCube : public OglMesh {
public:
	void init( uint flags );
};

class MeshyMesh : public OglMesh {
public:
	void init(Mesh mesh, uint flags);
	void rewrite(Mesh mesh, uint flags);
};