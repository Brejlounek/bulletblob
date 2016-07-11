#ifndef SIMULATION_I646U5DCIZRE6R8O7FO7TSD57DUETCIDZ5R
#define SIMULATION_I646U5DCIZRE6R8O7FO7TSD57DUETCIDZ5R

#include "Basics.h"
#include "BlobGrid.h"
//#include <glm\gtx\matrix_decompose.hpp>

using namespace glm;

const uint warmup = 100;
const double FPS = 60.0;
const double ROLFR = 1;

class Simulation {
public:
	Simulation();
	Simulation(vector<Blob> blobs);
	~Simulation();
	Mesh step(mat4x4 &matrix = mat4x4(1));
	glm::vec3 getPos();
	double getDist();
	glm::vec3 getColor();
	void collision(vec3 colPos);
	void load(string name);
private:
	Mesh getMesh();

	double time;
	BlobGrid bGrid;
	Mesh lastMesh; mat4x4 lastMatrix;
	bool colWas, colBefore = false; btVector3 colPos;
	double minY; btVector3 minPos;
	glm::vec3 center, basePos = glm::vec3(0);

	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	btConvexHullShape* fallShape;
	btDefaultMotionState* fallMotionState;
	btRigidBody* fallRigidBody;

	btCollisionShape* groundShape;
	btRigidBody* groundRigidBody;
	btDefaultMotionState* groundMotionState;
};






struct MyContactResultCallback : public btCollisionWorld::ContactResultCallback
{
	MyContactResultCallback(Simulation* ptr) : context(ptr) {}

	btScalar addSingleResult(btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0Wrap,
		int partId0,
		int index0,
		const btCollisionObjectWrapper* colObj1Wrap,
		int partId1,
		int index1)
	{
		vec3 colPos;
		colPos.x = cp.m_localPointA.x();
		colPos.y = cp.m_localPointA.y();
		colPos.z = cp.m_localPointA.z();
		btVector3 vec = cp.m_normalWorldOnB;
		//cout << cp.m_localPointA.x() << endl;
		context->collision(colPos);
		return 0;
	}

	Simulation* context;
};

/*MyContactResultCallback callback(ptr_to_some_object);
world.contactPairTest(bodyA, bodyB, callback);*/





#endif