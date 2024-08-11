#pragma once
#include "WO.h"
#include "Mat4.h"
#include "PxPhysicsAPI.h"
#include "extensions/PxDefaultErrorCallback.h"
#include "extensions/PxDefaultAllocator.h"

using namespace Aftr;
using namespace physx;

class WOPhysX : public virtual WO {
public:
	void onCreatePhysX(WO* wo, PxScene* scene, PxPhysics* p, PxShape* shape);
	static WOPhysX* New() {
		WOPhysX* obj = new WOPhysX();
		return obj;
	}
	void updatePose();
	void setPose(Mat4 m);
	void reset();

	PxRigidDynamic* body = nullptr;
	WO* worldObj;

protected:
	WOPhysX() : Aftr::IFace(this)
	{
	}
};
