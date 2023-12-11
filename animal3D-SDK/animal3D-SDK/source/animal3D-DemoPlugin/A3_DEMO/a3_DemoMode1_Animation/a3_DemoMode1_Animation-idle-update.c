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
	a3ui32 i, j;
	a3_DemoModelMatrixStack matrixStack[animationMaxCount_sceneObject];

	// active camera
	a3_DemoProjector const* activeCamera = demoMode->projector + demoMode->activeCamera;
	a3_DemoSceneObject const* activeCameraObject = activeCamera->sceneObject;

	// temp scale mat
	a3mat4 scaleMat = a3mat4_identity;

	a3demo_update_objects(demoState, dt,
		demoMode->object_scene, animationMaxCount_sceneObject, 0, 0);
	a3demo_update_objects(demoState, dt,
		demoMode->object_scene_ctrl, animationMaxCount_sceneObject, 0, 0);
	a3demo_update_objects(demoState, dt,
		demoMode->object_camera, animationMaxCount_cameraObject, 1, 0);

	a3demo_updateProjectorViewProjectionMat(demoMode->proj_camera_main);

	// apply scales to objects
	for (i = 0; i < animationMaxCount_sceneObject; ++i)
	{
		a3demo_applyScale_internal(demoMode->object_scene + i, scaleMat.m);
		a3demo_applyScale_internal(demoMode->object_scene_ctrl + i, scaleMat.m);
	}

	// update skybox
	a3demo_update_bindSkybox(demoMode->obj_camera_main, demoMode->obj_skybox);

	j = 0;
	demoMode->sceneGraphState->localSpace->pose[j++].transform = a3mat4_identity;
	demoMode->sceneGraphState->localSpace->pose[j++].transform = demoMode->object_camera->modelMat;
	demoMode->sceneGraphState->localSpace->pose[j++].transform = a3mat4_identity;
	demoMode->sceneGraphState->localSpace->pose[j++].transform = demoMode->obj_skeleton_ctrl->modelMat;
	// start of scene objects
	demoMode->sceneGraphState->localSpace->pose[j + 0].transform = demoMode->obj_skybox->modelMat;
	demoMode->sceneGraphState->localSpace->pose[j + 1].transform = demoMode->obj_skeleton->modelMat;
	a3kinematicsSolveForward(demoMode->sceneGraphState);
	a3hierarchyStateUpdateObjectInverse(demoMode->sceneGraphState);

	// update matrix stack data
	for (i = 0; i < animationMaxCount_sceneObject; ++i)
	{
		a3demo_updateModelMatrixStack(matrixStack + i,
			activeCamera->projectionMat.m,
			demoMode->sceneGraphState->objectSpace->pose[demoMode->obj_camera_main->sceneGraphIndex].transform.m,
			demoMode->sceneGraphState->objectSpaceInv->pose[demoMode->obj_camera_main->sceneGraphIndex].transform.m,
			demoMode->sceneGraphState->objectSpace->pose[demoMode->obj_skeleton->sceneGraphIndex].transform.m,
			a3mat4_identity.m);
	}

	if (demoState->updateAnimation)
	{
		demoMode->obj_skeleton->euler.x = demoMode->positionNode.angles.x;
		demoMode->obj_skeleton->euler.y = demoMode->positionNode.angles.y;
		demoMode->obj_skeleton->euler.z = demoMode->positionNode.angles.z;
		demoMode->obj_skeleton->position.x = demoMode->positionNode.translation.x;
		demoMode->obj_skeleton->position.y = demoMode->positionNode.translation.y;
	}

	// Update clipCtrl
	if (demoState->updateAnimation)
	{
		for (a3index i = 0; i < animationMaxCount_clipCtrl; i++)
		{
			a3clipControllerUpdate(demoMode->clipCtrls + i, dt);
		}

		a3real2 dir = { demoMode->velocityNode.translation.x, demoMode->velocityNode.translation.y };
		a3real speed = a3real2Length(dir);

		demoMode->wiggleOffset += dt * speed * 0.01; // Multiply by speed here
	}

	// skeletal
	a3_HierarchyState* baseHS = demoMode->hs_base;
	a3_HierarchyState* outputHS = demoMode->hs_output;

	a3_ClipController* clipCtrl1 = demoMode->clipCtrl1;

	// Set up base pose to be displayed on the screen
	a3_HierarchyState* hs = demoMode->hs_output;

	a3hierarchyPoseCopy(hs->localSpace, // goal to calculate
	baseHS->localSpace, // holds base pose
	demoMode->hierarchy_skel->numNodes);

	// MOVE THE JOINTS HERE
	//hs->localSpace->pose[1].translation.y = 10;
	//a3hierarchyPoseSnakePosition(hs->localSpace,
	//.25f, 150, demoState->timer_display->totalTime, demoMode->boneLength, animationMaxCount_snakeLength);
	a3hierarchyPoseSnakePosition(hs->localSpace,
		.25f, 150, demoMode->wiggleOffset, demoMode->boneLength, animationMaxCount_snakeLength);

	a3hierarchyPoseConvert(hs->localSpace,
		demoMode->hierarchy_skel->numNodes,
		demoMode->hierarchyPoseGroup_skel->channel,
		demoMode->hierarchyPoseGroup_skel->order);

	a3kinematicsSolveForward(hs);
	a3hierarchyStateUpdateObjectInverse(hs);
	a3hierarchyStateUpdateObjectBindToCurrent(hs, baseHS);

	// prepare and upload graphics data
	{
		a3ui32 const numBones = demoMode->hierarchy_skel->numNodes;

		a3addressdiff const skeletonIndex = demoMode->obj_skeleton - demoMode->object_scene;
		a3ui32 const mvp_size = numBones * sizeof(a3mat4);
		a3ui32 const t_skin_size = sizeof(demoMode->t_skin);
		a3ui32 const dq_skin_size = sizeof(demoMode->dq_skin);
		a3mat4 mvp_obj;
		a3mat4* mvp_joint, * mvp_bone, * t_skin;
		a3dualquat* dq_skin;
		//a3index i, currSkel;
		a3i32 p;

		const a3_HierarchyState* currentHS = demoMode->hs_output;
		
		{
			mvp_obj = matrixStack[skeletonIndex].modelViewProjectionMat;

			// update joint and bone transforms
			for (i = 0; i < numBones; ++i)
			{
				mvp_joint = demoMode->mvp_joint + i;
				mvp_bone = demoMode->mvp_bone + i;
				t_skin = demoMode->t_skin + i;
				dq_skin = demoMode->dq_skin + i;

				// joint transform
				a3real4x4SetScale(scaleMat.m, a3real_quarter);
				a3real4x4Concat(currentHS->objectSpace->pose[i].transform.m, scaleMat.m);
				a3real4x4Product(mvp_joint->m, mvp_obj.m, scaleMat.m);

				// bone transform
				p = demoMode->hierarchy_skel->nodes[i].parentIndex;
				if (p >= 0)
				{
					// position is parent joint's position
					scaleMat.v3 = currentHS->objectSpace->pose[p].transform.v3;

					// direction basis is from parent to current
					a3real3Diff(scaleMat.v2.v,
						currentHS->objectSpace->pose[i].transform.v3.v, scaleMat.v3.v);

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
				*t_skin = currentHS->objectSpaceBindToCurrent->pose[i].transform;

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
}


//-----------------------------------------------------------------------------
