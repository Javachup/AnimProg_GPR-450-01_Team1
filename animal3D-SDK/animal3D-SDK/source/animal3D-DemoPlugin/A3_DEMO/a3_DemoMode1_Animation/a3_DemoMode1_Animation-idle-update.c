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
	a3real4SetReal4(q, a3vec4_w.v);

	return q;
}

inline a3real4x2r a3demo_mat2dquat_safe(a3real4x2 Q, a3real4x4 const m)
{
	a3demo_mat2quat_safe(Q[0], m);
	a3real4SetReal4(Q[1], a3vec4_zero.v);

	return Q;
}


//-----------------------------------------------------------------------------
// UPDATE

void a3demo_update_objects(a3f64 const dt, a3_DemoSceneObject* sceneObjectBase,
	a3ui32 count, a3boolean useZYX, a3boolean applyScale);
void a3demo_update_defaultAnimation(a3_DemoState* demoState, a3f64 const dt,
	a3_DemoSceneObject* sceneObjectBase, a3ui32 count, a3ui32 axis);
void a3demo_update_bindSkybox(a3_DemoSceneObject* obj_camera, a3_DemoSceneObject* obj_skybox);
void a3demo_update_pointLight(a3_DemoSceneObject* obj_camera, a3_DemoPointLight* pointLightBase, a3ui32 count);

void a3demo_applyScale_internal(a3_DemoSceneObject* sceneObject, a3real4x4p s);

void a3animation_update_graphics(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode,
	a3_DemoModelMatrixStack const* matrixStack, a3_HierarchyState const* activeHS)
{
	// active camera
	a3_DemoProjector const* activeCamera = demoMode->projector + demoMode->activeCamera;
	a3_DemoSceneObject const* activeCameraObject = activeCamera->sceneObject;

	// temp scale mat
	a3mat4 scaleMat = a3mat4_identity;
	a3addressdiff const skeletonIndex = demoMode->obj_skeleton - demoMode->object_scene;
	a3ui32 const mvp_size = demoMode->hierarchy_skel->numNodes * sizeof(a3mat4);
	a3ui32 const t_skin_size = sizeof(demoMode->t_skin);
	a3ui32 const dq_skin_size = sizeof(demoMode->dq_skin);
	a3mat4 const mvp_obj = matrixStack[skeletonIndex].modelViewProjectionMat;
	a3mat4* mvp_joint, * mvp_bone, * t_skin;
	a3dualquat* dq_skin;
	a3index i;
	a3i32 p;

	// update joint and bone transforms
	for (i = 0; i < demoMode->hierarchy_skel->numNodes; ++i)
	{
		mvp_joint = demoMode->mvp_joint + i;
		mvp_bone = demoMode->mvp_bone + i;
		t_skin = demoMode->t_skin + i;
		dq_skin = demoMode->dq_skin + i;

		// joint transform
		a3real4x4SetScale(scaleMat.m, a3real_sixth);
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
				? a3vec3_z.v : a3vec3_y.v, scaleMat.v2.v), a3real_sixth);

			// up basis is cross of direction and right
			a3real3MulS(a3real3CrossUnit(scaleMat.v1.v,
				scaleMat.v2.v, scaleMat.v0.v), a3real_sixth);
		}
		else
		{
			// if we are a root joint, make bone invisible
			a3real4x4SetScale(scaleMat.m, a3real_zero);
		}
		a3real4x4Product(mvp_bone->m, mvp_obj.m, scaleMat.m);

		// get base to current object-space
		*t_skin = activeHS->objectSpaceBindToCurrent->pose[i].transform;

		// calculate DQ
		a3demo_mat2dquat_safe(dq_skin->Q, t_skin->m);
	}

	// upload
	a3bufferRefill(demoState->ubo_transformMVP, 0, mvp_size, demoMode->mvp_joint);
	a3bufferRefill(demoState->ubo_transformMVPB, 0, mvp_size, demoMode->mvp_bone);
	a3bufferRefill(demoState->ubo_transformBlend, 0, t_skin_size, demoMode->t_skin);
	a3bufferRefillOffset(demoState->ubo_transformBlend, 0, t_skin_size, dq_skin_size, demoMode->dq_skin);
}

void a3animation_update_fk(a3_HierarchyState* activeHS,
	a3_HierarchyState const* baseHS, a3_HierarchyPoseGroup const* poseGroup)
{
	if (activeHS->hierarchy == baseHS->hierarchy &&
		activeHS->hierarchy == poseGroup->hierarchy)
	{
		// FK pipeline
		a3hierarchyPoseConcat(activeHS->localSpace,	// local: goal to calculate
			activeHS->animPose, // holds current sample pose
			baseHS->localSpace, // holds base pose (animPose is all identity poses)
			activeHS->hierarchy->numNodes);
		a3hierarchyPoseConvert(activeHS->localSpace,
			activeHS->hierarchy->numNodes,
			poseGroup->channel,
			poseGroup->order);
		a3kinematicsSolveForward(activeHS);
	}
}

void a3animation_update_ik(a3_HierarchyState* activeHS,
	a3_HierarchyState const* baseHS, a3_HierarchyPoseGroup const* poseGroup)
{
	if (activeHS->hierarchy == baseHS->hierarchy &&
		activeHS->hierarchy == poseGroup->hierarchy)
	{
		// IK pipeline
		a3kinematicsSolveInverse(activeHS); // Solve the local space matrices
		a3hierarchyPoseRestore(activeHS->localSpace, // Restore the local space to have individual components
			activeHS->hierarchy->numNodes,
			poseGroup->channel,
			poseGroup->order);
		a3hierarchyPoseDeconcat(activeHS->animPose, // Remove the base pose from the sample pos to get the delta pos
			activeHS->localSpace,
			baseHS->localSpace,
			activeHS->hierarchy->numNodes);
	}
}

void a3animation_update_applyEffectors(a3_DemoMode1_Animation* demoMode,
	a3_HierarchyState* activeHS, a3_HierarchyState const* baseHS, a3_HierarchyPoseGroup const* poseGroup)
{
	if (activeHS->hierarchy == baseHS->hierarchy &&
		activeHS->hierarchy == poseGroup->hierarchy)
	{
		a3_DemoSceneObject* sceneObject = demoMode->obj_skeleton;
		a3ui32 j = sceneObject->sceneGraphIndex;

		// need to properly transform joints to their parent frame and vice-versa
		a3mat4 const controlToSkeleton = demoMode->sceneGraphState->localSpaceInv->pose[j].transform;
		a3vec4 controlLocator_headEffector, controlLocator_bodyEffector[15];
		a3mat4 jointTransform_head = a3mat4_identity, jointTransform_body[15] = a3mat4_identity;
		a3ui32 j_head, j_body[15];

		// Head movement
		{
			// head effector
			sceneObject = demoMode->obj_skeleton_headEffector_ctrl;
			a3real4Real4x4Product(controlLocator_headEffector.v, controlToSkeleton.m,
				demoMode->sceneGraphState->localSpace->pose[sceneObject->sceneGraphIndex].transform.v3.v);
			j = j_head = a3hierarchyGetNodeIndex(activeHS->hierarchy, "skel:head");
			jointTransform_head = activeHS->objectSpace->pose[j].transform;

			// body bases
			for (a3index i = 1; i < animationMaxCount_snakeLength; ++i)
			{
				int obj_index = i - 1;
				char jointName[20];
				sprintf(jointName, "skel:vert%d", i);
				j = j_body[obj_index] = a3hierarchyGetNodeIndex(activeHS->hierarchy, jointName);
				jointTransform_body[obj_index] = activeHS->objectSpace->pose[j].transform;
				controlLocator_bodyEffector[obj_index] = jointTransform_body[obj_index].v3;
			}

			// ****TO-DO: 
			// solve positions and orientations for joints
			// in this example, +X points away from child, +Y is normal
			// 1) check if solution exists
			//	-> get vector between base and end effector; if it extends max length, straighten limb
			//	-> position of end effector's target is at the minimum possible distance along this vector

			// For now, I will assume that the positions and orientations are in a spatial pose
			a3_DemoSceneObject
				* headEffector,
				* bodyEffector[15];

			a3_SpatialPose
				* headPos,
				* bodyPos[15];

			// current position
			headPos = &activeHS->objectSpace->pose[j_head];		// head pos
			for (a3index i = 1; i < animationMaxCount_snakeLength; ++i)
			{
				int obj_index = i - 1;
				char jointName[20];
				bodyPos[obj_index] = &activeHS->objectSpace->pose[j_body[obj_index]];
			}

			headEffector = demoMode->obj_skeleton_headEffector_ctrl;
			for (a3index i = 1; i < animationMaxCount_snakeLength; ++i)
			{
				int obj_index = i - 1;
				char jointName[20];
				bodyEffector[obj_index] = &demoMode->obj_skeleton_bodyEffector_ctrl[obj_index];
			}

			//we want the spatial pose to minipulate it
			// TODO: Fill these w matrices then pull out the position and orientation out
			a3real3SetReal3(baseToEnd.v, a3real3Sub(wristEffector->position.v, shoulderPos->translate.v));
			a3real3SetReal3(baseToConstraint.v, a3real3Sub(wristConstraint->position.v, shoulderPos->translate.v));

			upperLength = a3real3Length(a3real3Sub(elbowPos->translate.v, shoulderPos->translate.v));
			lowerLength = a3real3Length(a3real3Sub(wristPos->translate.v, elbowPos->translate.v));

			if (upperLength + lowerLength >= a3real3Length(baseToEnd.v))
			{
				// Set the elbow to be straight towards end effector
				a3real3SetReal3(elbowPos->translate.v, baseToEnd.v);
				a3real3Normalize(elbowPos->translate.v);
				a3real3MulS(elbowPos->translate.v, upperLength);
				a3real3Add(elbowPos->translate.v, shoulderPos->translate.v);

				// Set the wrist to be straight towards end effector
				a3real3SetReal3(wristPos->translate.v, baseToEnd.v);
				a3real3Normalize(wristPos->translate.v);
				a3real3MulS(wristPos->translate.v, lowerLength);
				a3real3Add(wristPos->translate.v, elbowPos->translate.v);
			}
			else // there is a solution
			{
				//// Wrist is just where the effector is poopy poo poo poopy poo... poo
				a3real3SetReal3(wristPos->scale.v, wristEffector->scale.v);
				a3real3SetReal3(wristPos->rotate.v, wristEffector->euler.v);
				a3real3SetReal3(wristPos->translate.v, wristEffector->position.v);

				// Calculate position of the elbow
				a3real baseLength = a3real3Length(baseToConstraint.v);
				a3real s = 0.5f * (baseLength + upperLength + lowerLength);
				a3real area = a3sqrt(s * (s - baseLength) * (s - upperLength) * (s - lowerLength));
				a3real height = 2 * area / baseLength;

				a3real displacement = a3sqrt(upperLength * upperLength - height * height);

				a3vec3 d, n, h;
				a3real3SetReal3(d.v, baseToEnd.v);
				a3real3Normalize(d.v);

				a3real3Cross(n.v, baseToEnd.v, baseToConstraint.v);
				a3real3Normalize(n.v);

				a3real3Cross(h.v, n.v, d.v);

				a3real3MulS(d.v, displacement);
				a3real3MulS(h.v, height);
				a3real3SetReal3(elbowPos->translate.v, a3real3Add(d.v, h.v));

				//math is done isolated
				//need to put in matrix
				// TODO: Orientation
				// That might need to go above before we mess with d, h, and n so that we can use them
			}


			// ****TO-DO: 
			// reassign resolved transforms to OBJECT-SPACE matrices
			// work from root to leaf too get correct transformations

		}
	}
}

void a3animation_update_sceneGraph(a3_DemoMode1_Animation* demoMode, a3f64 const dt)
{
	a3ui32 i;
	a3mat4 scaleMat = a3mat4_identity;

	a3demo_update_objects(dt, demoMode->object_scene, animationMaxCount_sceneObject, 0, 0);
	a3demo_update_objects(dt, demoMode->obj_camera_main, 1, 1, 0);

	a3demo_updateProjectorViewProjectionMat(demoMode->proj_camera_main);

	// apply scales to objects
	for (i = 0; i < animationMaxCount_sceneObject; ++i)
	{
		a3demo_applyScale_internal(demoMode->object_scene + i, scaleMat.m);
	}

	// update skybox
	a3demo_update_bindSkybox(demoMode->obj_camera_main, demoMode->obj_skybox);

	for (i = 0; i < animationMaxCount_sceneObject; ++i)
		demoMode->sceneGraphState->localSpace->pose[i].transform = demoMode->object_scene[i].modelMat;
	a3kinematicsSolveForward(demoMode->sceneGraphState);
	a3hierarchyStateUpdateLocalInverse(demoMode->sceneGraphState);
	a3hierarchyStateUpdateObjectInverse(demoMode->sceneGraphState);
}

void a3animation_update(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode, a3f64 const dt)
{
	a3ui32 i;
	a3_DemoModelMatrixStack matrixStack[animationMaxCount_sceneObject];

	// active camera
	a3_DemoProjector const* activeCamera = demoMode->projector + demoMode->activeCamera;
	a3_DemoSceneObject const* activeCameraObject = activeCamera->sceneObject;

	// skeletal
	if (demoState->updateAnimation)
		a3animation_update_animation(demoMode, dt, 1);

	// update scene graph local transforms
	a3animation_update_sceneGraph(demoMode, dt);

	// update matrix stack data using scene graph
	for (i = 0; i < animationMaxCount_sceneObject; ++i)
	{
		a3demo_updateModelMatrixStack(matrixStack + i,
			activeCamera->projectionMat.m,
			demoMode->sceneGraphState->objectSpace->pose[demoMode->obj_camera_main->sceneGraphIndex].transform.m,
			demoMode->sceneGraphState->objectSpaceInv->pose[demoMode->obj_camera_main->sceneGraphIndex].transform.m,
			demoMode->sceneGraphState->objectSpace->pose[demoMode->object_scene[i].sceneGraphIndex].transform.m,
			a3mat4_identity.m);
	}
}


//-----------------------------------------------------------------------------