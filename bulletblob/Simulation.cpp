#include "Simulation.h"

Simulation::Simulation(vector<Blob> blobs) : bGrid(blobs) {
	random_device rd;
	mt19937 gen(rd());
	normal_distribution<> d(0, 1);
	colWas = false;
	minY = -999;
	center = glm::vec3(0, 0, 0);

	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -10, 0));

	groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	//fallShape = new btSphereShape(1);

	fallShape = 0;
	getMesh();

	groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo
		groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	//groundRigidBodyCI.m_restitution = 0;
	groundRigidBodyCI.m_friction = 3;
	groundRigidBodyCI.m_restitution = 0.8;
	groundRigidBodyCI.m_rollingFriction = ROLFR;
	groundRigidBody = new btRigidBody(groundRigidBodyCI);
	dynamicsWorld->addRigidBody(groundRigidBody);

	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	fallShape->calculateLocalInertia(mass, fallInertia);
	fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(N*0.5, N*0.5, (double) (N*0.5))));

	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
	//fallRigidBodyCI.m_restitution = 0;
	fallRigidBodyCI.m_friction = 3;
	fallRigidBodyCI.m_restitution = 0.8;
	fallRigidBodyCI.m_rollingFriction = ROLFR;
	fallRigidBody = new btRigidBody(fallRigidBodyCI);
	fallRigidBody->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(fallRigidBody);
	time = 0;
}

Simulation::Simulation() {
	random_device rd;
	mt19937 gen(rd());
	normal_distribution<> d(0, 1);
	colWas = false;
	minY = -999;
	center = glm::vec3(0, 0, 0);

	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -10, 0));

	groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	//fallShape = new btSphereShape(1);

	fallShape = 0;
	getMesh();

	groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo
		groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	//groundRigidBodyCI.m_restitution = 0;
	groundRigidBodyCI.m_friction = 3;
	groundRigidBodyCI.m_restitution = 0.8;
	groundRigidBodyCI.m_rollingFriction = ROLFR;
	groundRigidBody = new btRigidBody(groundRigidBodyCI);
	dynamicsWorld->addRigidBody(groundRigidBody);

	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	fallShape->calculateLocalInertia(mass, fallInertia);
	fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), /*btVector3(N*0.5, N*0.5, (double)(N*0.5))*/btVector3(0,0,0)));

	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
	//fallRigidBodyCI.m_restitution = 0;
	fallRigidBodyCI.m_friction = 3;
	fallRigidBodyCI.m_restitution = 0.8;
	fallRigidBodyCI.m_rollingFriction = ROLFR;
	fallRigidBody = new btRigidBody(fallRigidBodyCI);
	fallRigidBody->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(fallRigidBody);
	time = 0;
}



Mesh Simulation::step(mat4x4 &matrix) {
	random_device rd;
	mt19937 gen(rd());
	normal_distribution<> d(0, 1);

	btTransform transLast, trans;
	fallRigidBody->getMotionState()->getWorldTransform(transLast);
	dynamicsWorld->stepSimulation(1.0 / FPS);
	fallRigidBody->getMotionState()->getWorldTransform(trans);

	//if (!((uint)time%10))
	//	cout << trans.getOrigin().getX() << " " << trans.getOrigin().getY() << " " << trans.getOrigin().getZ() << endl;

	dynamicsWorld->removeRigidBody(fallRigidBody);

	Mesh mesh = getMesh();
	fallRigidBody->setCollisionShape(fallShape);
	//fallRigidBody->applyImpulse(btVector3(0, 1, 0), btVector3(0, 0, 0));

	double rTime = time-warmup;
	if (FPS == 20)
		rTime = time-66;

	colWas = false;
	MyContactResultCallback callback(this);
	dynamicsWorld->contactPairTest(fallRigidBody, groundRigidBody, callback);

	double yDiff = trans.getOrigin().y() - transLast.getOrigin().y();
	if (colWas && yDiff>0 && rTime>0) {
		if (!colBefore) {
			//cout << "up! " << chrono::system_clock::now().time_since_epoch().count() << endl;
			glm::vec3 pos(minPos.x(), minPos.y(), minPos.z());
			pos = glm::normalize(pos);
			pos *= -yDiff*1000;
			//cout << pos.x << " " << pos.y << " " << pos.z << endl;

			fallRigidBody->applyForce(btVector3(0, yDiff*500, 0), minPos);
			colBefore = true;
		}
	}
	else
		colBefore = false;

	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	fallShape->calculateLocalInertia(mass, fallInertia);
	fallRigidBody->setMassProps(1, fallInertia);
	fallRigidBody->updateInertiaTensor();

	/*if (rTime>0)
		fallRigidBody->setRollingFriction(-0.5);*/
	dynamicsWorld->addRigidBody(fallRigidBody);
	if (rTime>0) {
		dynamicsWorld->removeRigidBody(groundRigidBody);
		//groundRigidBody->setRollingFriction(0.2);
		dynamicsWorld->addRigidBody(groundRigidBody);
	}
	if (rTime==0) {
		basePos = getPos();
	}

	time++;

	/*btVector3 posV = trans.getOrigin();//fallRigidBody->getCenterOfMassPosition();
	btQuaternion quat = trans.getRotation();//fallRigidBody->getOrientation();
	vec3 tVec(posV.x(), posV.y(), posV.z());

	glm::quat quaternion(quat.x(), quat.y(), quat.z(), quat.w());
	matrix = mat4_cast(quaternion);
	matrix = translate(matrix, tVec);*/
	trans.getOpenGLMatrix(value_ptr(matrix));
	lastMatrix = matrix;

	return mesh;

	/*btSoftBodyWorldInfo m_softBodyWorldInfo;
	btSoftBody softBody(&m_softBodyWorldInfo);

	softBody.*/
}

void Simulation::collision(vec3 colPos) {
	colWas = true;
	this->colPos.setValue(colPos.x,colPos.y,colPos.z);
}

Mesh Simulation::getMesh() {
	double rTime = time-warmup;
	if (FPS == 20)
		rTime = time-66;
	if (rTime<0) {
		rTime = 0;
		if (time > 0.1)
			return lastMesh;
	}

	bGrid.get(rTime);
	Mesh mesh = bGrid.march();

	vector<btScalar> vertices;
	minY = FLT_MAX;
	center = glm::vec3(0, 0, 0);
	for (uint i = 0; i<mesh.vertices.size(); i++) {
		vertices.push_back(mesh.vertices[i].x);
		vertices.push_back(mesh.vertices[i].y);
		vertices.push_back(mesh.vertices[i].z);
		//center += glm::vec3(mesh.vertices[i].x, mesh.vertices[i].y, mesh.vertices[i].z);

		if (mesh.vertices[i].y<minY) {
			minY = mesh.vertices[i].y;
			vec4 vert = vec4(mesh.vertices[i].x, mesh.vertices[i].y, mesh.vertices[i].z, 1);
			vert = lastMatrix*vert;
			vert.x /= vert.w;
			vert.y /= vert.w;
			vert.z /= vert.w;

			minPos.setValue(vert.x, vert.y, vert.z);
		}
	}
	//center *= 1.0/mesh.vertices.size();
	delete fallShape;
	fallShape = new btConvexHullShape(&vertices[0], mesh.vertices.size(), sizeof(btScalar) * 3);

	btShapeHull* hull = new btShapeHull(fallShape);
	btScalar margin = 0.001;// fallShape->getMargin();
	hull->buildHull(margin);

	vector<btScalar> vertDec;
	const btVector3 *vertHull = hull->getVertexPointer();
	for (uint i = 0; i<hull->numVertices(); i++) {
		vertDec.push_back(vertHull[i].x());
		vertDec.push_back(vertHull[i].y());
		vertDec.push_back(vertHull[i].z());
		center += glm::vec3(vertHull[i].x(), vertHull[i].y(), vertHull[i].z());
	}
	center *= 1.0/hull->numVertices();

	delete fallShape;
	delete hull;
	fallShape = new btConvexHullShape(&vertDec[0], vertDec.size()/3, sizeof(btScalar) * 3);

	lastMesh = mesh;

	/*glm::vec4 center4 = glm::vec4(center.x, center.y, center.z, 1);
	center4 = lastMatrix*center4;
	center = glm::vec3(center4.x, center4.y, center4.z)/center4.w;*/

	return mesh;
}

glm::vec3 Simulation::getPos() {
	/*btTransform trans;
	btMotionState *motionState = fallRigidBody->getMotionState();
	motionState->getWorldTransform(trans);
	btVector3 orPos = trans.getOrigin();
	center = glm::vec3(orPos.x(), orPos.y(), orPos.z());
	return center;*/

	/*glm::vec4 center4 = glm::vec4(center.x, center.y, center.z, 1);
	center4 = lastMatrix*center4;
	center = glm::vec3(center4.x, center4.y, center4.z)/center4.w;*/
	//return center;

	glm::vec4 origin(N*0.5, N*0.5, N*0.5, 1);
	origin = lastMatrix*origin;
	origin.x /= origin.w;
	origin.y /= origin.w;
	origin.z /= origin.w;

	return glm::vec3(origin.x,origin.y,origin.z);
}

double Simulation::getDist() {
	glm::vec3 pos = getPos() - basePos;
	return glm::sqrt(pos.x*pos.x + pos.y*pos.y);
}

glm::vec3 Simulation::getColor(){
	return bGrid.getCol();
}

void Simulation::load(string name) {
	ifstream file;
	file.open(name);

	double fitness;
	file >> fitness;
	vector<Blob> blobs;
	for (uint bi = 0; bi<blobN; bi++) {
		Blob blob;
		file >> blob.x >> blob.y >> blob.z >> blob.s;
		file >> blob.start >> blob.period;
		blobs.push_back(blob);
	}

	BlobGrid bGr(blobs);
	bGrid = bGr;
}





Simulation::~Simulation() {
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
}