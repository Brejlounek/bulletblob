#define NOTMAIN

#ifndef NOTMAIN

#include "Basics.h"
#include "Simulation.h"
#include "BlobGrid.h"

int main(void) {
	/*BlobGrid bGrid;
	bGrid.get(0);
	Mesh mesh = bGrid.march();
	for (uint i = 0; i<mesh.vertices.size(); i++) {
		cout << mesh.vertices[i].x << " " << mesh.vertices[i].y << " " << mesh.vertices[i].z << endl;
	}
	return 0;*/

	Simulation simulation;
	for (uint i = 0; i<-1; i++)
		simulation.step();
	return 0;
}

/*int main(void)
{

	btBroadphaseInterface* broadphase = new btDbvtBroadphase();

	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -10, 0));


	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);

	btCollisionShape* fallShape = new btSphereShape(1);

	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
	btRigidBody::btRigidBodyConstructionInfo
		groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	dynamicsWorld->addRigidBody(groundRigidBody);


	btDefaultMotionState* fallMotionState =
		new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0.00000001, 50, 0)));
	btDefaultMotionState* fallMotionState2 =
		new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 58, 0.000000001)));
	btDefaultMotionState* fallMotionState3 =
		new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 62, 0.000000001)));
	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	fallShape->calculateLocalInertia(mass, fallInertia);

	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
	btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
	dynamicsWorld->addRigidBody(fallRigidBody);

	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI2(mass, fallMotionState2, fallShape, fallInertia);
	btRigidBody* fallRigidBody2 = new btRigidBody(fallRigidBodyCI2);
	dynamicsWorld->addRigidBody(fallRigidBody2);

	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI3(mass, fallMotionState3, fallShape, fallInertia);
	btRigidBody* fallRigidBody3 = new btRigidBody(fallRigidBodyCI3);
	dynamicsWorld->addRigidBody(fallRigidBody3);

	btTypedConstraint* p2p1 = new btPoint2PointConstraint(*fallRigidBody, *fallRigidBody2, btVector3(0, 4, 0), btVector3(0, -4, 0));
	btTypedConstraint* p2p2 = new btPoint2PointConstraint(*fallRigidBody2, *fallRigidBody3, btVector3(0, 4, 0), btVector3(0, -4, 0));
	btTypedConstraint* p2p3 = new btPoint2PointConstraint(*fallRigidBody3, *fallRigidBody, btVector3(0, 4, 0), btVector3(0, -4, 0));

	dynamicsWorld->addConstraint(p2p1);
	dynamicsWorld->addConstraint(p2p2);
	dynamicsWorld->addConstraint(p2p3);

	for (int i = 0; i < 1000; i++) {
		dynamicsWorld->stepSimulation(1 / 60.f, 10);

		btTransform trans, trans2, trans3;
		fallRigidBody->getMotionState()->getWorldTransform(trans);
		fallRigidBody2->getMotionState()->getWorldTransform(trans2);
		fallRigidBody3->getMotionState()->getWorldTransform(trans3);

		std::cout << trans.getOrigin().getY() << " " << trans2.getOrigin().getY() << " " << trans3.getOrigin().getY() << std::endl;
	}

	btTransform trans, trans2, trans3;
	fallRigidBody->getMotionState()->getWorldTransform(trans);
	fallRigidBody2->getMotionState()->getWorldTransform(trans2);
	fallRigidBody3->getMotionState()->getWorldTransform(trans3);
	std::cout << trans.getOrigin().getX() << " " << trans.getOrigin().getY() << " " << trans.getOrigin().getZ() << std::endl;
	std::cout << trans2.getOrigin().getX() << " " << trans2.getOrigin().getY() << " " << trans2.getOrigin().getZ() << std::endl;
	std::cout << trans3.getOrigin().getX() << " " << trans3.getOrigin().getY() << " " << trans3.getOrigin().getZ() << std::endl;

	dynamicsWorld->removeRigidBody(fallRigidBody);
	delete fallRigidBody->getMotionState();
	delete fallRigidBody;

	dynamicsWorld->removeRigidBody(groundRigidBody);
	delete groundRigidBody->getMotionState();
	delete groundRigidBody;


	delete fallShape;
	delete groundShape;

	delete dynamicsWorld;
	delete solver;
	delete collisionConfiguration;
	delete dispatcher;
	delete broadphase;

	return 0;
}*/

#endif