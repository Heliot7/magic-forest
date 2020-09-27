#ifndef __KINECT_H__
#define __KINECT_H__

#include <vector>
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include "headers/render/Geometry.h"
#include "headers/render/ShaderManager.h"
#include "headers/logic/Element.h"

enum TrackingStatusType { NIL = 0, TRACKING, CALIBRATING, POSING };
enum LegoPartsType {HEAD, BODY, LEFT_ARM, LEFT_LEG, LEFT_UPPER_LEG, RIGHT_ARM, RIGHT_LEG, RIGHT_UPPER_LEG};
enum KTDirection {LEFT = 1, RIGHT};

class Kinect
{
private:
	XnPoint3D joint_positions[25]; // 24 joints defined in enum XnSkeletonJoint, starting with 1
	XnConfidence joint_confidence[25];
	TrackingStatusType tracking_status;
	XnPoint3D pointTransformation(XnPoint3D pos, Vector scale, Vector offset);
	XnPoint3D jointTransformation(XnSkeletonJointPosition joint, Vector scale, Vector offset);
	void getJointPosition(XnUserID player, XnSkeletonJoint eJoint, XnPoint3D &pt);
	XnConfidence getJointPositionWithProb(XnUserID player, XnSkeletonJoint eJoint, XnPoint3D &pt);
	void draw_limb(XnUserID player, XnSkeletonJoint eJoint1, XnSkeletonJoint eJoint2, vector<Element *> legoParts);
	void renderLegoPart(float x1, float y1, float z1, float x2,float y2, float z2, vector<Element *> legoParts, LegoPartsType part);
	float dist(XnPoint3D p1, XnPoint3D p2);
	float distXZ(XnPoint3D p1, XnPoint3D p2);
	Vector scale;
	Vector offset;
public:
	Kinect()
	{ 
		this->tracking_status = NIL;
		this->isDrawingFireball = false;
		this->isDrawingAOE = false;
		this->scale = Vector(300/0.8, 200/0.8, 400/0.8);
		this->offset = Vector(0, 2.5, -7);
		XnPoint3D p;
		p.X = 0;
		p.Y = 0;
		p.Z = 0;
		for (int i=0; i<25; i++)
		{
			this->joint_positions[i] = p;
		}
	}
	~Kinect() {}
	int init();
	XnPoint3D left_hand_position;
	XnPoint3D right_hand_position;
	Vector left_shoot_dir;
	Vector right_shoot_dir;
	float left_shoot_dist;
	float right_shoot_dist;
	bool isDrawingFireball;
	bool isDrawingAOE;
	bool isLeftHandInAir();
	bool isRightHandInAir();
	bool isBothHandInAir();
	int isPushDetected();
	bool isShootDetected();
	void draw_avatar(vector<Element *> legoParts, bool isDraw);
	void updateTrackingStatus();
	TrackingStatusType getTrackingStatus() { return this->tracking_status; }
	XnPoint3D getJoint(XnSkeletonJoint joint);
};

#endif
