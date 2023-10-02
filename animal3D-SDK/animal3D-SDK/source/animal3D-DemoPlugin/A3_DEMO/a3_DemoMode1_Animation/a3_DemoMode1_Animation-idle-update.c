/*
	Copyright 2011-2020 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	a3_DemoMode1_Animation-idle-update.c
	Demo mode implementations: animation scene.

	********************************************
	*** UPDATE FOR ANIMATION SCENE MODE      ***
	********************************************
	 
	Ananda Shumock-Bailey
	Worked on hierarchical pose-to-pose animation
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoMode1_Animation.h"

//typedef struct a3_DemoState a3_DemoState;
#include "../a3_DemoState.h"

#include "../_a3_demo_utilities/a3_DemoMacros.h"


//-----------------------------------------------------------------------------
// UTILS

inline a3real4r a3demo_mat2quat_safe(a3real4 q, a3real4x4 const m)
{
	// ****TO-DO: 
	//	-> convert rotation part of matrix to quaternion
	//	-> NOTE: this is for testing dual quaternion skinning only; 
	//		quaternion data would normally be computed with poses

	return q;
}


//-----------------------------------------------------------------------------
// UPDATE

void a3demo_update_objects(a3_DemoState* demoState, a3f64 const dt,
	a3_DemoSceneObject* sceneObjectBase, a3ui32 count, a3boolean useZYX, a3boolean applyScale);
void a3demo_update_defaultAnimation(a3_DemoState* demoState, a3f64 const dt,
	a3_DemoSceneObject* sceneObjectBase, a3ui32 count, a3ui32 axis);
void a3demo_update_bindSkybox(a3_DemoSceneObject* obj_camera, a3_DemoSceneObject* obj_skybox);
void a3demo_update_pointLight(a3_DemoSceneObject* obj_camera, a3_DemoPointLight* pointLightBase, a3ui32 count);

void a3demo_applyScale_internal(a3_DemoSceneObject* sceneObject, a3real4x4p s);

void a3animation_update(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode, a3f64 const dt)
{
	a3ui32 i;
	a3_DemoModelMatrixStack matrixStack[animationMaxCount_sceneObject];

	a3_HierarchyState* activeHS = demoMode->hierarchyState_skel + 1, * baseHS = demoMode->hierarchyState_skel;

	// manually set up a skelton
	const a3ui32 jointCount = 32;
	a3ui32 jointIndex = 0;
	a3i32 jointParentIndex = -1;
	a3i32 rootJointIndex, upperSpineJointIndex, clavicleJointIndex, pelvisJointIndex;	
	
	// create a hierarchy with 32 to nodes to represent a humanoid skeleton
	a3_Hierarchy* hierarchy = demoMode->hierarchy_skel;
	a3hierarchyCreate(hierarchy, jointCount, 0);

	// set up joint relationships
	jointParentIndex = rootJointIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:root");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:spine_lower");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:spine_mid");
	jointParentIndex = upperSpineJointIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:spine_upper");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:neck");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:head");
	jointParentIndex = upperSpineJointIndex;
	jointParentIndex = clavicleJointIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:clavicle");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shoulderblade_r");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shoulder_r");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:elbow_r");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:forearm_r");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:wrist_r");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:hand_r");
	jointParentIndex = clavicleJointIndex;
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shoulderblade_l");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shoulder_l");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:elbow_l");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:forearm_l");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:wrist_l");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:hand_l");
	jointParentIndex = rootJointIndex;
	jointParentIndex = pelvisJointIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:pelvis");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:hip_r");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:knee_r");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shin_r");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:ankle_r");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:foot_r");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:toe_r");
	jointParentIndex = pelvisJointIndex;
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:hip_l");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:knee_l");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shin_l");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:ankle_l");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:foot_l");
	jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:toe_l");

	// initialize a hierarchical pose pool
	a3_HierarchyPoseGroup* hierarchyPoseGroup = demoMode->hierarchyPoseGroup_skel;
	a3hierarchyPoseGroupCreate(hierarchyPoseGroup, hierarchy, 4);
	hierarchyPoseGroup->hierarchy = 0;


	// define "bind pose" or "base pose" or the initial transformation 
	//	description for each joint (not a literal transform)
	a3ui32 p = 0;
	a3ui32 j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
	//a3_SpatialPoseChannel currentChannel = hierarchyPoseGroup->channel[j];
	a3_SpatialPose* spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetTranslation(spatialPose, 0.0f, 0.0f, +3.6f);
	//currentChannel = a3poseChannel_orient_xyz | a3poseChannel_scale_xyz | a3poseChannel_translate_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_lower");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, -90.0f, -5.0f);
	a3spatialPoseSetTranslation(spatialPose, 0.0f, -0.1f, +0.1f);
	//currentChannel = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_mid");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +10.0f);
	a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
	//currentChannel = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_upper");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -5.0f);
	a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
	//currentChannel = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:neck");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);
	//currentChannel = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:head");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:clavicle");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, +90.0f, 0.0f);
	a3spatialPoseSetTranslation(spatialPose, -0.1f, +0.1f, 0.0f);

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulderblade_r");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetTranslation(spatialPose, +0.5f, -0.1f, -0.5f);
	//currentChannel = a3poseChannel_translate_yz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulder_r");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, +30.0f, -10.0f);
	a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, +0.5f);
	//currentChannel = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:elbow_r");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +20.0f);
	a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
	//currentChannel = a3poseChannel_orient_z;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:forearm_r");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
	//currentChannel = a3poseChannel_orient_x;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:wrist_r");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -10.0f);
	a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
	//currentChannel = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:hand_r");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulderblade_l");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetTranslation(spatialPose, -0.5f, -0.1f, -0.5f);
	//currentChannel = a3poseChannel_translate_yz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulder_l");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, -30.0f, +10.0f);
	a3spatialPoseSetTranslation(spatialPose, -0.5f, 0.0f, +0.5f);
	//currentChannel = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:elbow_l");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -20.0f);
	a3spatialPoseSetTranslation(spatialPose, -2.0f, 0.0f, 0.0f);
	//currentChannel = a3poseChannel_orient_z;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:forearm_l");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
	//currentChannel = a3poseChannel_orient_x;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:wrist_l");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +10.0f);
	a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
	//currentChannel = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:hand_l");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetTranslation(spatialPose, -0.5f, 0.0f, 0.0f);

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:pelvis");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetTranslation(spatialPose, 0.0f, 0.0f, -0.1f);

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:hip_r");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, +90.0f, +10.0f);
	a3spatialPoseSetTranslation(spatialPose, +1.0f, -0.1f, +0.5f);
	//currentChannel = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:knee_r");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -20.0f);
	a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
	//currentChannel = a3poseChannel_orient_z;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shin_r");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
	//currentChannel = a3poseChannel_orient_x;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:ankle_r");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +90.0f);
	a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
	//currentChannel = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:foot_r");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +10.0f);
	a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);
	//currentChannel = a3poseChannel_orient_z;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:toe_r");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetTranslation(spatialPose, +0.2f, 0.0f, 0.0f);

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:hip_l");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, -90.0f, -10.0f);
	a3spatialPoseSetTranslation(spatialPose, -1.0f, -0.1f, +0.5f);
	//currentChannel = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:knee_l");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +20.0f);
	a3spatialPoseSetTranslation(spatialPose, -2.0f, 0.0f, 0.0f);
	//currentChannel = a3poseChannel_orient_z;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shin_l");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
	//currentChannel = a3poseChannel_orient_x;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:ankle_l");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -90.0f);
	a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
	//currentChannel = a3poseChannel_orient_xyz;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:foot_l");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -10.0f);
	a3spatialPoseSetTranslation(spatialPose, -0.5f, 0.0f, 0.0f);
	//currentChannel = a3poseChannel_orient_z;

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:toe_l");
	//currentChannel = hierarchyPoseGroup->channel[j];
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetTranslation(spatialPose, -0.2f, 0.0f, 0.0f);


	// each remaining pose represents a "delta" from the base
	// initialize the changes where applicable
	// (note: the channels describe which pose components can change)
	p = 2;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetRotation(spatialPose, +45.0f, +60.0f, +90.0f);	// rotate whole figure about all axes


	p = 3;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetScale(spatialPose, 2.0f, 2.0f, 2.0f);	// uniformly scale whole figure up to 200%


	p = 4;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
	spatialPose = hierarchyPoseGroup->hpose[p].spatialPose + j;
	a3spatialPoseSetTranslation(spatialPose, +3.0f, +4.0f, +5.0f);	// shift whole figure by some vector


	// active camera
	a3_DemoProjector const* activeCamera = demoMode->projector + demoMode->activeCamera;
	a3_DemoSceneObject const* activeCameraObject = activeCamera->sceneObject;

	// temp scale mat
	a3mat4 scaleMat = a3mat4_identity;

	a3demo_update_objects(demoState, dt,
		demoMode->object_scene, animationMaxCount_sceneObject, 0, 0);
	a3demo_update_objects(demoState, dt,
		demoMode->object_camera, animationMaxCount_cameraObject, 1, 0);

	a3demo_updateProjectorViewProjectionMat(demoMode->proj_camera_main);

	// apply scales to objects
	for (i = 0; i < animationMaxCount_sceneObject; ++i)
		a3demo_applyScale_internal(demoMode->object_scene + i, scaleMat.m);

	// update skybox
	a3demo_update_bindSkybox(demoMode->obj_camera_main, demoMode->obj_skybox);

	// update matrix stack data
	for (i = 0; i < animationMaxCount_sceneObject; ++i)
	{
		a3demo_updateModelMatrixStack(matrixStack + i,
			activeCamera->projectionMat.m, activeCameraObject->modelMat.m, activeCameraObject->modelMatInv.m,
			demoMode->object_scene[i].modelMat.m, a3mat4_identity.m);
	}


	// skeletal
	if (demoState->updateAnimation)
	{
		i = (a3ui32)(demoState->timer_display->totalTime);
		demoMode->hierarchyKeyPose_display[0] = (i + 0) % (demoMode->hierarchyPoseGroup_skel->hposeCount - 1);
		demoMode->hierarchyKeyPose_display[1] = (i + 1) % (demoMode->hierarchyPoseGroup_skel->hposeCount - 1);
		demoMode->hierarchyKeyPose_param = (a3real)(demoState->timer_display->totalTime - (a3f64)i);
	}

	//a3hierarchyPoseCopy(activeHS->objectSpace,
	//	demoMode->hierarchyPoseGroup_skel->hpose + demoMode->hierarchyKeyPose_display[0] + 1,
	//	demoMode->hierarchy_skel->numNodes);
	//a3hierarchyPoseLerp(activeHS->objectSpace,	// use as temp storage
	//	demoMode->hierarchyPoseGroup_skel->hpose + demoMode->hierarchyKeyPose_display[0] + 1,
	//	demoMode->hierarchyPoseGroup_skel->hpose + demoMode->hierarchyKeyPose_display[1] + 1,
	//	demoMode->hierarchyKeyPose_param,
	//	demoMode->hierarchy_skel->numNodes);
	//a3hierarchyPoseConcat(activeHS->localSpace,	// goal to calculate
	//	baseHS->localSpace, // holds base pose
	//	activeHS->objectSpace, // temp storage
	//	demoMode->hierarchy_skel->numNodes);
	a3hierarchyPoseConvert(&activeHS->localSpace,
		demoMode->hierarchy_skel->numNodes,
		*demoMode->hierarchyPoseGroup_skel->channel,
		demoMode->hierarchyPoseGroup_skel->order);
	a3kinematicsSolveForward(activeHS);
	a3hierarchyStateUpdateObjectInverse(activeHS);
	//a3hierarchyStateUpdateObjectBindToCurrent(activeHS, baseHS);


	// prepare and upload graphics data
	{
		a3addressdiff const skeletonIndex = demoMode->obj_skeleton - demoMode->object_scene;
		a3ui32 const mvp_size = demoMode->hierarchy_skel->numNodes * sizeof(a3mat4);
		a3ui32 const t_skin_size = sizeof(demoMode->t_skin);
		a3ui32 const dq_skin_size = sizeof(demoMode->dq_skin);
		//a3mat4 const mvp_obj = matrixStack[skeletonIndex].modelViewProjectionMat;
		a3mat4* mvp_joint, * mvp_bone, * t_skin;
		a3dualquat* dq_skin;
		a3index i;
		//a3i32 p;
		
		// update joint and bone transforms
		for (i = 0; i < demoMode->hierarchy_skel->numNodes; ++i)
		{
			mvp_joint = demoMode->mvp_joint + i;
			mvp_bone = demoMode->mvp_bone + i;
			t_skin = demoMode->t_skin + i;
			dq_skin = demoMode->dq_skin + i;
		/*
			// joint transform
			a3real4x4SetScale(scaleMat.m, a3real_quarter);
			a3real4x4Concat(activeHS->objectSpace->pose[i].transform.m, scaleMat.m);
			a3real4x4Product(mvp_joint->m, mvp_obj.m, scaleMat.m);
			
			// bone transform
			p = demoMode->hierarchy_skel->nodes[i].parentIndex;
			if (p >= 0)
			{
				// position is parent joint's position
				scaleMat.v3 = activeHS->objectSpace->pose[p].transform.v3;

				// direction basis is from parent to current
				a3real3Diff(scaleMat.v2.v,
					activeHS->objectSpace->pose[i].transform.v3.v, scaleMat.v3.v);

				// right basis is cross of some upward vector and direction
				// select 'z' for up if either of the other dimensions is set
				a3real3MulS(a3real3CrossUnit(scaleMat.v0.v,
					a3real2LengthSquared(scaleMat.v2.v) > a3real_zero
					? a3vec3_z.v : a3vec3_y.v, scaleMat.v2.v), a3real_quarter);
			
				// up basis is cross of direction and right
				a3real3MulS(a3real3CrossUnit(scaleMat.v1.v,
					scaleMat.v2.v, scaleMat.v0.v), a3real_quarter);
			}
			else
			{
				// if we are a root joint, make bone invisible
				a3real4x4SetScale(scaleMat.m, a3real_zero);
			}
			a3real4x4Product(mvp_bone->m, mvp_obj.m, scaleMat.m);

			// get base to current object-space
			*t_skin = activeHS->objectSpaceBindToCurrent->pose[i].transform;
		*/
			// calculate DQ
			{
				a3real4 d = { a3real_zero };
				a3demo_mat2quat_safe(dq_skin->r.q, t_skin->m);
				a3real3ProductS(d, t_skin->v3.v, a3real_half);
				a3quatProduct(dq_skin->d.q, d, dq_skin->r.q);
			}
		}
		
		// upload
		a3bufferRefill(demoState->ubo_transformMVP, 0, mvp_size, demoMode->mvp_joint);
		a3bufferRefill(demoState->ubo_transformMVPB, 0, mvp_size, demoMode->mvp_bone);
		a3bufferRefill(demoState->ubo_transformBlend, 0, t_skin_size, demoMode->t_skin);
		a3bufferRefillOffset(demoState->ubo_transformBlend, 0, t_skin_size, dq_skin_size, demoMode->dq_skin);
	}
}


//-----------------------------------------------------------------------------
