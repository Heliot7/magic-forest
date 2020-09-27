#include <cstdio>
#include <iostream>

#include "headers/kinect/Kinect.h"
#include "headers/kinect/Cylinder.h"
#include "headers/render/Scene.h"

using namespace std;

#define SAMPLE_XML_PATH "data/openni_config.xml"
#define CHECK_RC(nRetVal, what)										\
	if (nRetVal != XN_STATUS_OK)									\
	{																\
		printf("%s failed: %s\n", what, xnGetStatusString(nRetVal));\
		return nRetVal;												\
	}


xn::Context g_Context;
xn::DepthGenerator g_DepthGenerator;
xn::UserGenerator g_UserGenerator;
xn::GestureGenerator g_GestureGenerator;


XnBool g_bNeedPose = false;
XnChar g_strPose[20] = "";
XnBool g_bDrawBackground = true;
XnBool g_bDrawPixels = true;
XnBool g_bDrawSkeleton = true;
XnBool g_bPrintID = true;
XnBool g_bPrintState = true;
XnBool g_bPause = false;
XnBool g_bRecord = false;
XnBool g_bQuit = false;

XnBool g_Push = false;
XnPoint3D pushPos;

///////////////////////////////// Callbacks ////////////////////////////////
// Callback: New user was detected
void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	printf("New User %d\n", nId);
	// New user found
	if (g_bNeedPose)
	{
		g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
	}
	else
	{
		g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
	}
}

// Callback: An existing user was lost
void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	printf("Lost user %d\n", nId);
}

// Callback: Detected a pose
void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie)
{
	printf("Pose %s detected for user %d\n", strPose, nId);
	g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
	g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
}

// Callback: Started calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie)
{
	printf("Calibration started for user %d\n", nId);
}

// Callback: Finished calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationEnd(xn::SkeletonCapability& capability, XnUserID nId, XnBool bSuccess, void* pCookie)
{
	if (bSuccess)
	{
		// Calibration succeeded
		printf("Calibration complete, start tracking user %d\n", nId);
		g_UserGenerator.GetSkeletonCap().StartTracking(nId);
	}
	else
	{
		// Calibration failed
		printf("Calibration failed for user %d\n", nId);
		if (g_bNeedPose)
		{
			g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
		}
		else
		{
			g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
		}
	}
}

// callback function for gesture recognized
void XN_CALLBACK_TYPE GRecognized( xn::GestureGenerator &generator,
                                   const XnChar *strGesture,
                                   const XnPoint3D *pIDPosition,
                                   const XnPoint3D *pEndPosition,
                                   void *pCookie )
{
	g_Push = true;
	pushPos = *(pEndPosition);
	//cout << strGesture << " recognized" << endl;
}

// callback function for gesture progress
void XN_CALLBACK_TYPE GProgress( xn::GestureGenerator &generator,
                                 const XnChar *strGesture,
                                 const XnPoint3D *pPosition,
                                 XnFloat fProgress,
                                 void *pCookie )
{
	g_Push = true;
	pushPos = *(pPosition);
	//cout << strGesture << " progress" << endl;
}
//////////////////////////////////////////////////////////////////////////////



int Kinect::init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	xn::EnumerationErrors errors;
	nRetVal = g_Context.InitFromXmlFile(SAMPLE_XML_PATH, &errors);
	if (nRetVal == XN_STATUS_NO_NODE_PRESENT)
	{
		XnChar strError[1024];
		errors.ToString(strError, 1024);
		printf("%s\n", strError);
		return nRetVal;
	}
	else if (nRetVal != XN_STATUS_OK)
	{
		printf("Open failed: %s\n", xnGetStatusString(nRetVal));
		return nRetVal;
	}
	

	nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
	CHECK_RC(nRetVal, "Find depth generator");
	nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_USER, g_UserGenerator);
	if (nRetVal != XN_STATUS_OK)
	{
		nRetVal = g_UserGenerator.Create(g_Context);
		CHECK_RC(nRetVal, "Find user generator");
	}

	XnCallbackHandle hUserCallbacks, hCalibrationCallbacks, hPoseCallbacks;
	if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
	{
		printf("Supplied user generator doesn't support skeleton\n");
		return -1;
	}
	g_UserGenerator.RegisterUserCallbacks(User_NewUser, User_LostUser, NULL, hUserCallbacks);
	g_UserGenerator.GetSkeletonCap().RegisterCalibrationCallbacks(UserCalibration_CalibrationStart, UserCalibration_CalibrationEnd, NULL, hCalibrationCallbacks);

	if (g_UserGenerator.GetSkeletonCap().NeedPoseForCalibration())
	{
		g_bNeedPose = TRUE;
		if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
		{
			printf("Pose required, but not supported\n");
			return -1;
		}
		g_UserGenerator.GetPoseDetectionCap().RegisterToPoseCallbacks(UserPose_PoseDetected, NULL, NULL, hPoseCallbacks);
		g_UserGenerator.GetSkeletonCap().GetCalibrationPose(g_strPose);
	}

	g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

	// Initialize gestures
	nRetVal = g_GestureGenerator.Create( g_Context );
	g_GestureGenerator.AddGesture( "Click", NULL );
	XnCallbackHandle hHandle;
	g_GestureGenerator.RegisterGestureCallbacks( GRecognized, GProgress, NULL, hHandle );

	//g_UserGenerator.GetSkeletonCap().SetSmoothing(0.4);


	nRetVal = g_Context.StartGeneratingAll();
	CHECK_RC(nRetVal, "StartGenerating");

	return nRetVal;
}

bool Kinect::isShootDetected()
{
	XnPoint3D rhand = this->joint_positions[XN_SKEL_RIGHT_HAND];
	XnPoint3D rshoulder = this->joint_positions[XN_SKEL_RIGHT_SHOULDER];

	XnPoint3D lhand = this->joint_positions[XN_SKEL_LEFT_HAND];
	XnPoint3D lshoulder = this->joint_positions[XN_SKEL_LEFT_SHOULDER];
	
	float threshold = 1.0;
	//float ldist = fabs(lhand.Z - lshoulder.Z);
	//float rdist = fabs(rhand.Z - rshoulder.Z);
	float ldist = distXZ(lhand, lshoulder);
	float rdist = distXZ(rhand, rshoulder);

	if (ldist > threshold && rdist > threshold)
		return true;
	return false;
}

int Kinect::isPushDetected()
{
	if (g_Push)
	{
		XnPoint3D lhand = this->joint_positions[XN_SKEL_LEFT_HAND];
		XnPoint3D rhand = this->joint_positions[XN_SKEL_RIGHT_HAND];
		
		float ldist = this->dist(lhand, pushPos);
		float rdist = this->dist(rhand, pushPos);
		if (ldist > rdist)
			return LEFT;
		else
			return RIGHT;
	}

	return false;
}

bool Kinect::isLeftHandInAir()
{
	XnPoint3D left_hand_pos = this->joint_positions[XN_SKEL_LEFT_HAND];
	XnPoint3D torso_pos = this->joint_positions[XN_SKEL_TORSO];
	if (left_hand_pos.Y > torso_pos.Y)
	{
		return true;
	}
	return false;
}

bool Kinect::isRightHandInAir()
{
	XnPoint3D right_hand_pos = this->joint_positions[XN_SKEL_RIGHT_HAND];
	XnPoint3D torso_pos = this->joint_positions[XN_SKEL_TORSO];
	if (right_hand_pos.Y > torso_pos.Y)
	{
		return true;
	}
	return false;
}

bool Kinect::isBothHandInAir()
{
	float offset = 1.0;
	XnPoint3D left_hand_pos = this->joint_positions[XN_SKEL_LEFT_HAND];
	XnPoint3D right_hand_pos = this->joint_positions[XN_SKEL_RIGHT_HAND];
	XnPoint3D head_pos = this->joint_positions[XN_SKEL_HEAD];
	if (left_hand_pos.Y > head_pos.Y + offset && right_hand_pos.Y > head_pos.Y + offset)
	{
		return true;
	}
	return false;
}

float Kinect::dist(XnPoint3D p1, XnPoint3D p2)
{
	float xdist = p1.X - p2.X;
	float ydist = p1.Y - p2.Y;
	float zdist = p1.Z - p2.Z;
	return sqrt(xdist*xdist + ydist*ydist + zdist*zdist);
}

float Kinect::distXZ(XnPoint3D p1, XnPoint3D p2)
{
	float xdist = p1.X - p2.X;
	float zdist = p1.Z - p2.Z;
	return sqrt(xdist*xdist + zdist*zdist);
}

XnPoint3D Kinect::pointTransformation(XnPoint3D pos, Vector scale, Vector offset)
{
	XnPoint3D pt;
	pt.X = pos.X/scale.x+offset.x;
	pt.Y = pos.Y/scale.y+offset.y;
	pt.Z = pos.Z/scale.z+offset.z;
	return pt;
}

XnPoint3D Kinect::jointTransformation(XnSkeletonJointPosition joint, Vector scale, Vector offset)
{
	XnPoint3D pt;
	pt = joint.position;

	pt.X = pt.X/scale.x+offset.x;
	pt.Y = pt.Y/scale.y+offset.y;
	pt.Z = pt.Z/scale.z+offset.z;

	return pt;
}

XnConfidence Kinect::getJointPositionWithProb(XnUserID player, XnSkeletonJoint eJoint, XnPoint3D &pt)
{
	XnSkeletonJointPosition joint;
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJoint, joint);
	pt = this->jointTransformation(joint, this->scale, this->offset);
	return joint.fConfidence;
}

void Kinect::getJointPosition(XnUserID player, XnSkeletonJoint eJoint, XnPoint3D &pt)
{
	XnSkeletonJointPosition joint;
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJoint, joint);
	pt = this->jointTransformation(joint, this->scale, this->offset);	
}


void Kinect::renderLegoPart(float x1, float y1, float z1, float x2,float y2, float z2, vector<Element *> legoParts, LegoPartsType part)
{
  float vx = x2-x1;
  float vy = y2-y1;
  float vz = z2-z1;

  //handle the degenerate case of z1 == z2 with an approximation
  if(vz == 0)
      vz = .0001;

  float v = sqrt( vx*vx + vy*vy + vz*vz );
  float ax = 57.2957795*acos( vz/v );
  if ( vz < 0.0 )
      ax = -ax;
  float rx = -vy*vz;
  float ry = vx*vz;

  glPushMatrix();
	glTranslatef( x1,y1,z1 );
	glRotatef(ax, rx, ry, 0.0);
	Scene::drawModel(legoParts[part]->getModel(), false);
  glPopMatrix();

  Point centre = Point( (x1+x2)/2, (y1+y2)/2, (z1+z2)/2);
  legoParts[part]->setPos(centre);
}

void Kinect::draw_limb(XnUserID player, XnSkeletonJoint eJoint1, XnSkeletonJoint eJoint2, vector<Element *> legoParts)
{

	if (!g_UserGenerator.GetSkeletonCap().IsTracking(player))
	{
		printf("not tracked!\n");
		return;
	}


	XnPoint3D pt[2];
	XnConfidence conf1, conf2;
	conf1 = joint_confidence[eJoint1];
	conf2 = joint_confidence[eJoint2];

	pt[0] = joint_positions[eJoint1];
	pt[1] = joint_positions[eJoint2];

	//if (conf1 < 0.2 || conf2 < 0.2)
	//{
	//	return;
	//}

/*
	if (eJoint2 == XN_SKEL_LEFT_HAND) {
		this -> left_hand_position = pt[1];
		this -> left_shoot_dist = fabs(pt[0].Z - pt[1].Z);
		this -> left_shoot_dir = Vector(pt[1].X - pt[0].X, pt[1].Y - pt[0].Y, pt[1].Z - pt[0].Z);
		this -> left_shoot_dir.normalize();
	}

	if (eJoint2 == XN_SKEL_RIGHT_FOOT) {
		this -> right_hand_position = pt[1];
		this -> right_shoot_dist = fabs(pt[0].Z - pt[1].Z);
		this -> right_shoot_dir = Vector(pt[1].X - pt[0].X, pt[1].Y - pt[0].Y, pt[1].Z - pt[0].Z);
		this -> right_shoot_dir.normalize();
	}
*/

	//float radius = 0.3;
	//int subdivisions = 10;
	//GLUquadricObj *quadric = gluNewQuadric();
	//gluQuadricNormals(quadric, GLU_SMOOTH);
	//renderCylinder(pt[0].X, pt[0].Y, pt[0].Z, pt[1].X, pt[1].Y, pt[1].Z, radius, subdivisions, quadric);
	//gluDeleteQuadric(quadric);


	// Limbs
	if (eJoint1 == XN_SKEL_LEFT_ELBOW && eJoint2 == XN_SKEL_LEFT_HAND) {
		renderLegoPart(pt[0].X, pt[0].Y, pt[0].Z, pt[1].X, pt[1].Y, pt[1].Z, legoParts, LEFT_ARM);
	}
	if (eJoint1 == XN_SKEL_LEFT_KNEE && eJoint2 == XN_SKEL_LEFT_FOOT) {
		renderLegoPart(pt[0].X, pt[0].Y, pt[0].Z, pt[1].X, 0.0, pt[1].Z, legoParts, LEFT_LEG);
	}
	if (eJoint1 == XN_SKEL_LEFT_HIP && eJoint2 == XN_SKEL_LEFT_KNEE) {
		renderLegoPart(pt[0].X, pt[0].Y, pt[0].Z, pt[1].X, pt[1].Y, pt[1].Z, legoParts, LEFT_UPPER_LEG);
	}
	if (eJoint1 == XN_SKEL_RIGHT_ELBOW && eJoint2 == XN_SKEL_RIGHT_HAND) {
		renderLegoPart(pt[0].X, pt[0].Y, pt[0].Z, pt[1].X, pt[1].Y, pt[1].Z, legoParts, RIGHT_ARM);
	}
	if (eJoint1 == XN_SKEL_RIGHT_KNEE && eJoint2 == XN_SKEL_RIGHT_FOOT) {
		renderLegoPart(pt[0].X, pt[0].Y, pt[0].Z, pt[1].X, 0.0, pt[1].Z, legoParts, RIGHT_LEG);
	}
	if (eJoint1 == XN_SKEL_RIGHT_HIP && eJoint2 == XN_SKEL_RIGHT_KNEE) {
		renderLegoPart(pt[0].X, pt[0].Y, pt[0].Z, pt[1].X, pt[1].Y, pt[1].Z, legoParts, RIGHT_UPPER_LEG);
	}

	// Head and Body
	if (eJoint1 == XN_SKEL_HEAD && eJoint2 == XN_SKEL_NECK) {
		renderLegoPart(pt[0].X, pt[0].Y, pt[0].Z, pt[1].X, pt[1].Y, pt[1].Z, legoParts, HEAD);
	}
	if (eJoint1 == XN_SKEL_NECK) {
		XnPoint3D hip[2];
		hip[0] = this->joint_positions[XN_SKEL_LEFT_HIP];
		hip[1] = this->joint_positions[XN_SKEL_RIGHT_HIP];
		renderLegoPart(pt[0].X, pt[0].Y, pt[0].Z, (hip[0].X+hip[1].X)/2, (hip[0].Y+hip[1].Y)/2, (hip[0].Z+hip[1].Z)/2, legoParts, BODY);
	}

}


void Kinect::draw_avatar(vector<Element *> legoParts, bool isDraw)
{
	xn::SceneMetaData sceneMD;
	xn::DepthMetaData depthMD;
	g_DepthGenerator.GetMetaData(depthMD);

	g_Push = false;
	
	if (!g_bPause)
	{
		// Read next available data
		g_Context.WaitAndUpdateAll();
	}

	// Process the data
	g_DepthGenerator.GetMetaData(depthMD);
	g_UserGenerator.GetUserPixels(0, sceneMD);


	// 10. get users
	XnUserID aUsers[15];
	XnUInt16 nUsers = 15;
	g_UserGenerator.GetUsers(aUsers, nUsers);

	// 12. check each user
	for( int i = 0; i < nUsers; ++i )
	{
		// 13. if is tracking skeleton
		if (g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i]))
		{
			if (tracking_status != TRACKING) {
				tracking_status = TRACKING;
				cout << aUsers[i] << " - Tracking..." << endl;
			}

			if (isDraw)
			{
				// Get skeleton joint positions
				joint_confidence[XN_SKEL_TORSO] = getJointPositionWithProb(aUsers[i], XN_SKEL_TORSO, joint_positions[XN_SKEL_TORSO]);
				joint_confidence[XN_SKEL_HEAD] = getJointPositionWithProb(aUsers[i], XN_SKEL_HEAD, joint_positions[XN_SKEL_HEAD]);
				joint_confidence[XN_SKEL_NECK] = getJointPositionWithProb(aUsers[i], XN_SKEL_NECK, joint_positions[XN_SKEL_NECK]);
				joint_confidence[XN_SKEL_LEFT_SHOULDER] = getJointPositionWithProb(aUsers[i], XN_SKEL_LEFT_SHOULDER, joint_positions[XN_SKEL_LEFT_SHOULDER]);
				joint_confidence[XN_SKEL_LEFT_ELBOW] = getJointPositionWithProb(aUsers[i], XN_SKEL_LEFT_ELBOW, joint_positions[XN_SKEL_LEFT_ELBOW]);
				joint_confidence[XN_SKEL_LEFT_HAND] = getJointPositionWithProb(aUsers[i], XN_SKEL_LEFT_HAND, joint_positions[XN_SKEL_LEFT_HAND]);
				joint_confidence[XN_SKEL_LEFT_HIP] = getJointPositionWithProb(aUsers[i], XN_SKEL_LEFT_HIP, joint_positions[XN_SKEL_LEFT_HIP]);
				joint_confidence[XN_SKEL_LEFT_KNEE] = getJointPositionWithProb(aUsers[i], XN_SKEL_LEFT_KNEE, joint_positions[XN_SKEL_LEFT_KNEE]);
				joint_confidence[XN_SKEL_LEFT_FOOT] = getJointPositionWithProb(aUsers[i], XN_SKEL_LEFT_FOOT, joint_positions[XN_SKEL_LEFT_FOOT]);
				joint_confidence[XN_SKEL_RIGHT_SHOULDER] = getJointPositionWithProb(aUsers[i], XN_SKEL_RIGHT_SHOULDER, joint_positions[XN_SKEL_RIGHT_SHOULDER]);
				joint_confidence[XN_SKEL_RIGHT_ELBOW] = getJointPositionWithProb(aUsers[i], XN_SKEL_RIGHT_ELBOW, joint_positions[XN_SKEL_RIGHT_ELBOW]);
				joint_confidence[XN_SKEL_RIGHT_HAND] = getJointPositionWithProb(aUsers[i], XN_SKEL_RIGHT_HAND, joint_positions[XN_SKEL_RIGHT_HAND]);
				joint_confidence[XN_SKEL_RIGHT_HIP] = getJointPositionWithProb(aUsers[i], XN_SKEL_RIGHT_HIP, joint_positions[XN_SKEL_RIGHT_HIP]);
				joint_confidence[XN_SKEL_RIGHT_KNEE] = getJointPositionWithProb(aUsers[i], XN_SKEL_RIGHT_KNEE, joint_positions[XN_SKEL_RIGHT_KNEE]);
				joint_confidence[XN_SKEL_RIGHT_FOOT] = getJointPositionWithProb(aUsers[i], XN_SKEL_RIGHT_FOOT, joint_positions[XN_SKEL_RIGHT_FOOT]);


				// Increase movement in x direction
				Vector screenOffset = Vector(joint_positions[XN_SKEL_TORSO].X, 0, 0);
				Vector screenScale = Vector(1, 1, 1);
				for (int j = 0; j < 25; j++) 
				{	
					joint_positions[j] = pointTransformation(joint_positions[j], screenScale, screenOffset);
				}

				
				// Draw kinect avatar
				draw_limb(aUsers[i], XN_SKEL_HEAD, XN_SKEL_NECK, legoParts);

				draw_limb(aUsers[i], XN_SKEL_NECK, XN_SKEL_LEFT_SHOULDER, legoParts);
				draw_limb(aUsers[i], XN_SKEL_LEFT_SHOULDER, XN_SKEL_LEFT_ELBOW, legoParts);
				draw_limb(aUsers[i], XN_SKEL_LEFT_ELBOW, XN_SKEL_LEFT_HAND, legoParts);

				draw_limb(aUsers[i], XN_SKEL_NECK, XN_SKEL_RIGHT_SHOULDER, legoParts);
				draw_limb(aUsers[i], XN_SKEL_RIGHT_SHOULDER, XN_SKEL_RIGHT_ELBOW, legoParts);
				draw_limb(aUsers[i], XN_SKEL_RIGHT_ELBOW, XN_SKEL_RIGHT_HAND, legoParts);

				draw_limb(aUsers[i], XN_SKEL_LEFT_SHOULDER, XN_SKEL_TORSO, legoParts);
				draw_limb(aUsers[i], XN_SKEL_RIGHT_SHOULDER, XN_SKEL_TORSO, legoParts);

				draw_limb(aUsers[i], XN_SKEL_TORSO, XN_SKEL_LEFT_HIP, legoParts);
				draw_limb(aUsers[i], XN_SKEL_LEFT_HIP, XN_SKEL_LEFT_KNEE, legoParts);
				draw_limb(aUsers[i], XN_SKEL_LEFT_KNEE, XN_SKEL_LEFT_FOOT, legoParts);

				draw_limb(aUsers[i], XN_SKEL_TORSO, XN_SKEL_RIGHT_HIP, legoParts);
				draw_limb(aUsers[i], XN_SKEL_RIGHT_HIP, XN_SKEL_RIGHT_KNEE, legoParts);
				draw_limb(aUsers[i], XN_SKEL_RIGHT_KNEE, XN_SKEL_RIGHT_FOOT, legoParts);

				draw_limb(aUsers[i], XN_SKEL_LEFT_HIP, XN_SKEL_RIGHT_HIP, legoParts);
			}
		}
		else if (g_UserGenerator.GetSkeletonCap().IsCalibrating(aUsers[i]))
		{
			if (tracking_status != CALIBRATING) {
				tracking_status = CALIBRATING;
				cout << aUsers[i] << " - Calibrating..." << endl;
			}
		}
		else
		{
			if (tracking_status != POSING) {
				tracking_status = POSING;
				//cout << aUsers[i] << " - Looking fo pose..." << endl;
			}
		}
	}
}

XnPoint3D Kinect::getJoint(XnSkeletonJoint j)
{
	return joint_positions[j];
}
