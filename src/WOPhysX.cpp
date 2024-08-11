#include "WOPhysX.h"

void WOPhysX::onCreatePhysX(WO* wo, PxScene* scene, PxPhysics* p, PxShape* shape) {
	worldObj = wo;
	PxTransform t({ 0,0,0 });

	body = p->createRigidDynamic(t);
	body->attachShape(*shape);
	scene->addActor(*body);

	physx::PxMat44 m;
	Aftr::Mat4 m2;

	m2 = wo->getPose();
	for (int i = 0; i < 16; i++) m(i % 4, i / 4) = m2[i];
	body->setGlobalPose(PxTransform(m));

	body->userData = this;
	scene->addActor(*body);
}

void WOPhysX::updatePose() {
	physx::PxMat44 m;
	m = body->getGlobalPose();
	Aftr::Mat4 m2;
	for (int i = 0; i < 16; i++) m2[i] = m(i % 4, i / 4);
	worldObj->setPose(m2);
}

void WOPhysX::setPose(Mat4 m) {
	physx::PxMat44 m2;
	for (int i = 0; i < 16; i++) m2(i % 4, i / 4) = m[i];
	body->setKinematicTarget(physx::PxTransform(m2));
	worldObj->setPose(m);
}

void WOPhysX::reset() {
	worldObj->setPosition(0, 0, 50);
	PxTransform t(0, 0, 50);
	body->setGlobalPose(t);
	body->setLinearVelocity(PxVec3(0,0,0));
	body->setAngularVelocity(PxVec3(0, 0, 0));
}