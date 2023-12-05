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

	a3_DemoMode1_Animation-load.c
	Demo mode implementations: animation scene.

	********************************************
	*** LOADING FOR ANIMATION SCENE MODE     ***
	********************************************

	Joey Romanowski:
		Load skeleton positions
		Load Keyframe and Clip data
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoMode1_Animation.h"

#include "../a3_DemoState.h"

#include <stdio.h>


//-----------------------------------------------------------------------------

// utility to load animation
void a3animation_init_animation(a3_DemoState const* demoState, a3_DemoMode1_Animation* demoMode)
{
	// general counters
	a3ui32 j, p;

	// object pointers
	a3_Hierarchy* hierarchy = 0;
	a3_HierarchyState* hierarchyState = 0;
	a3_HierarchyPoseGroup* hierarchyPoseGroup = 0;
	a3_SpatialPose* spatialPose = 0;

	a3_FileStream fileStream[1] = { 0 };
	const a3byte* const geometryStream = "./data/anim_data_gpro_base.dat";

	// stream animation assets
	if (demoState->streaming && a3fileStreamOpenRead(fileStream, geometryStream))
	{
		// load hierarchy assets
		hierarchy = demoMode->hierarchy_skel;
		a3hierarchyLoadBinary(hierarchy, fileStream);

		// done
		a3fileStreamClose(fileStream);
	}
	// not streaming or stream doesn't exist
	else if (!demoState->streaming || a3fileStreamOpenWrite(fileStream, geometryStream))
	{
		// manually set up a skeleton
		// first is the hierarchy: the general non-spatial relationship between bones
		const a3ui32 jointCount = 16;

		// indices of joints, their parents and branching joints
		a3ui32 jointIndex = 0;
		a3i32 jointParentIndex = -1;

		// initialize hierarchy
		hierarchy = demoMode->hierarchy_skel;
		a3hierarchyCreate(hierarchy, jointCount, 0);

		// set up joint relationships
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:root");

		// create the last 15 joints
		for (int i = 0; i < 15; ++i)
		{
			char jointName[20];
			sprintf(jointName, "skel:vert%d", i + 1);
			jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, jointName);
		}

		// done
		a3fileStreamClose(fileStream);
	}

	demoMode->obj_skeleton->position.y = +a3real_four;
	demoMode->obj_skeleton->euler.z = +a3real_oneeighty;
	//demoMode->obj_skeleton->euler.x = -a3real_ninety;

	// next set up hierarchy poses
	hierarchy = demoMode->hierarchy_skel;
	hierarchyPoseGroup = demoMode->hierarchyPoseGroup_skel;
	hierarchyPoseGroup->hierarchy = 0;

	// allocate poses
	a3hierarchyPoseGroupCreate(hierarchyPoseGroup, hierarchy, 1, a3poseEulerOrder_xyz | a3poseEulerOrder_yzx | a3poseEulerOrder_zxy | a3poseEulerOrder_yxz | a3poseEulerOrder_xzy | a3poseEulerOrder_zyx);

	// define "bind pose" or "base pose" or the initial transformation
	//	description for each joint (not a literal transform)
	p = 0;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
	spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
	a3spatialPoseSetTranslation(spatialPose, 0.0f, 0.0f, 0.0f);
	//hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_xyz;

	// each joint is 2 units behind the previous one
	for (a3index i = 1; i < animationMaxCount_snakeLength; ++i)
	{
		char jointName[20];
		sprintf(jointName, "skel:vert%d", i + 1);
		spatialPose = hierarchyPoseGroup->hpose[p].pose + i;
		a3spatialPoseSetTranslation(spatialPose, 2.0f, 0.0f, 0.0f);
		//hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_xyz;
	}

	//// load from file
	//a3hierarchyPoseGroupLoadHTR(demoMode->hierarchyPoseGroup_skel, demoMode->hierarchy_skel,
	//	"../../../../resource/animdata/egnaro/egnaro_skel_anim.htr");
	//

	// set up hierarchy states
	for (a3index i = 0; i < animationMaxCount_hs; i++)
	{
		demoMode->hierarchyState_skel[i].hierarchy = 0;
		a3hierarchyStateCreate(demoMode->hierarchyState_skel + i, hierarchy);
	}

	// Copy base pose to the base hs
	a3hierarchyPoseCopy(demoMode->hs_base->localSpace, hierarchyPoseGroup->hpose, hierarchy->numNodes);
	a3hierarchyPoseConvert(demoMode->hs_base->localSpace, hierarchy->numNodes, hierarchyPoseGroup->channel, hierarchyPoseGroup->order);
	a3kinematicsSolveForward(demoMode->hs_base);
	a3hierarchyStateUpdateObjectInverse(demoMode->hs_base);
}


//-----------------------------------------------------------------------------

void a3animation_input(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode, a3f64 const dt);
void a3animation_update(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode, a3f64 const dt);
void a3animation_render(a3_DemoState const* demoState, a3_DemoMode1_Animation const* demoMode, a3f64 const dt);
void a3animation_input_keyCharPress(a3_DemoState const* demoState, a3_DemoMode1_Animation* demoMode, a3i32 const asciiKey, a3i32 const state);
void a3animation_input_keyCharHold(a3_DemoState const* demoState, a3_DemoMode1_Animation* demoMode, a3i32 const asciiKey, a3i32 const state);

void a3animation_loadValidate(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode)
{
	// initialize callbacks
	a3_DemoModeCallbacks* const callbacks = demoState->demoModeCallbacks + demoState_modeAnimation;
	callbacks->demoMode = demoMode;
	callbacks->handleInput =	(a3_DemoMode_EventCallback)		a3animation_input;
	callbacks->handleUpdate =	(a3_DemoMode_EventCallback)		a3animation_update;
	callbacks->handleRender =	(a3_DemoMode_EventCallbackConst)a3animation_render;
	callbacks->handleKeyPress = (a3_DemoMode_InputCallback)		a3animation_input_keyCharPress;
	callbacks->handleKeyHold =	(a3_DemoMode_InputCallback)		a3animation_input_keyCharHold;

	// initialize cameras dependent on viewport
	demoMode->proj_camera_main->aspect = demoState->frameAspect;
	a3demo_updateProjectorProjectionMat(demoMode->proj_camera_main);
	a3demo_setProjectorSceneObject(demoMode->proj_camera_main, demoMode->obj_camera_main);

	// initialize cameras not dependent on viewport

	// animation
	demoMode->hierarchyState_skel->hierarchy = demoMode->hierarchy_skel;
	demoMode->hierarchyPoseGroup_skel->hierarchy = demoMode->hierarchy_skel;
}


void a3animation_load(a3_DemoState const* demoState, a3_DemoMode1_Animation* demoMode)
{
	a3ui32 i;

	a3_DemoSceneObject* currentSceneObject;
	a3_DemoProjector* projector;


	// camera's starting orientation depends on "vertical" axis
	// we want the exact same view in either case
	const a3real sceneCameraAxisPos = 20.0f;
	const a3vec3 sceneCameraStartPos = {
		+0.0f,
		-70.0f,
		+20.0f,
	};
	const a3vec3 sceneCameraStartEuler = {
		+80.0f,
		+0.0f,
		+0.0f,
	};
	const a3f32 sceneObjectDistance = 8.0f;
	const a3f32 sceneObjectHeight = 2.0f;


	// all objects
	for (i = 0; i < animationMaxCount_sceneObject; ++i)
		a3demo_initSceneObject(demoMode->object_scene + i);
	for (i = 0; i < animationMaxCount_cameraObject; ++i)
		a3demo_initSceneObject(demoMode->object_camera + i);
	for (i = 0; i < animationMaxCount_projector; ++i)
		a3demo_initProjector(demoMode->projector + i);

	currentSceneObject = demoMode->obj_skybox;
	currentSceneObject->scaleMode = 1;
	currentSceneObject->scale.x = 256.0f;

	currentSceneObject = demoMode->obj_skeleton;
	currentSceneObject->scaleMode = 0;


	// set up cameras
	projector = demoMode->proj_camera_main;
	projector->perspective = a3true;
	projector->fovy = a3real_fortyfive;
	projector->znear = 1.0f;
	projector->zfar = 1024.0f;
	projector->ctrlMoveSpeed = 10.0f;
	projector->ctrlRotateSpeed = 5.0f;
	projector->ctrlZoomSpeed = 5.0f;
	projector->sceneObject->position = sceneCameraStartPos;
	projector->sceneObject->euler = sceneCameraStartEuler;


	// set flags
	demoMode->render = animation_renderPhong;
	demoMode->display = animation_displayTexture;
	demoMode->activeCamera = animation_cameraSceneViewer;

	demoMode->pipeline = animation_forward;
	demoMode->pass = animation_passComposite;

	demoMode->targetIndex[animation_passScene] = animation_scene_finalcolor;
	demoMode->targetIndex[animation_passComposite] = animation_scene_finalcolor;

	demoMode->targetCount[animation_passScene] = animation_target_scene_max;
	demoMode->targetCount[animation_passComposite] = 1;


	// setup
	a3animation_init_animation(demoState, demoMode);
}


//-----------------------------------------------------------------------------
