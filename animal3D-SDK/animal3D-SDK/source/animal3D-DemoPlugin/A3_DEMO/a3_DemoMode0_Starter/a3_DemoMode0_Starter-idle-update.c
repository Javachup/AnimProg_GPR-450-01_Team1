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
	
	a3_DemoMode0_Starter_idle-update.c
	Demo mode implementations: starter scene.

	********************************************
	*** UPDATE FOR STARTER SCENE MODE        ***
	********************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoMode0_Starter.h"

//typedef struct a3_DemoState a3_DemoState;
#include "../a3_DemoState.h"

#include "../_a3_demo_utilities/a3_DemoMacros.h"

#include <stdio.h>


//-----------------------------------------------------------------------------
// UPDATE

void a3demo_update_objects(a3_DemoState* demoState, a3f64 const dt,
	a3_DemoSceneObject* sceneObjectBase, a3ui32 count, a3boolean useZYX, a3boolean applyScale);
void a3demo_update_defaultAnimation(a3_DemoState* demoState, a3f64 const dt,
	a3_DemoSceneObject* sceneObjectBase, a3ui32 count, a3ui32 axis);
void a3demo_update_bindSkybox(a3_DemoSceneObject* obj_camera, a3_DemoSceneObject* obj_skybox);
void a3demo_update_pointLight(a3_DemoSceneObject* obj_camera, a3_DemoPointLight* pointLightBase, a3ui32 count);

void a3demo_applyScale_internal(a3_DemoSceneObject* sceneObject, a3real4x4p s);

void a3starter_update(a3_DemoState* demoState, a3_DemoMode0_Starter* demoMode, a3f64 const dt)
{
	a3ui32 i;
	a3_DemoModelMatrixStack matrixStack[starterMaxCount_sceneObject];

	// active camera
	a3_DemoProjector const* activeCamera = demoMode->projector + demoMode->activeCamera;
	a3_DemoSceneObject const* activeCameraObject = activeCamera->sceneObject;

	// temp scale mat
	a3mat4 scaleMat = a3mat4_identity;

	a3demo_update_objects(demoState, dt,
		demoMode->object_scene, starterMaxCount_sceneObject, 0, 0);
	a3demo_update_objects(demoState, dt,
		demoMode->object_camera, starterMaxCount_cameraObject, 1, 0);

	a3demo_updateProjectorViewProjectionMat(demoMode->proj_camera_main);

	a3demo_update_defaultAnimation(demoState, dt, demoMode->obj_box, 6, 2);

	// apply scales to starter objects
	for (i = 0; i < starterMaxCount_sceneObject; ++i)
		a3demo_applyScale_internal(demoMode->object_scene + i, scaleMat.m);

	// update skybox
	a3demo_update_bindSkybox(demoMode->obj_camera_main, demoMode->obj_skybox);

	// update matrix stack data
	for (i = 0; i < starterMaxCount_sceneObject; ++i)
	{
		a3demo_updateModelMatrixStack(matrixStack + i,
			activeCamera->projectionMat.m, activeCameraObject->modelMat.m, activeCameraObject->modelMatInv.m,
			demoMode->object_scene[i].modelMat.m, a3mat4_identity.m);
	}

	a3f64 dtMultiplier = demoMode->isNormalTime ? 1 : 0.5;

	a3clipControllerUpdate(&demoMode->clipCtrlZero, dt * dtMultiplier);
	a3clipControllerUpdate(&demoMode->clipCtrlOne, dt * dtMultiplier);
	a3clipControllerUpdate(&demoMode->clipCtrlTwo, dt * dtMultiplier);
}

// function to handle the input bullet point of step #4
void a3demo_input_keyPress(a3_DemoState const* demoState, a3_DemoMode0_Starter* demoMode, a3f64 const dt)
{
	// determines which clip controller we are currently on
	// for when the user wants to change which clip to control
	a3_ClipController temp;
	switch (demoMode->clipCtrlIndex)
	{
	case 0: temp = demoMode->clipCtrlZero; break;
	case 1: temp = demoMode->clipCtrlOne; break;
	case 2: temp = demoMode->clipCtrlTwo; break;
	}

	// select clip controller to edit using the number row
	if (a3keyboardIsPressed(demoState->keyboard, a3key_8) == 1)
	{
		demoMode->clipCtrlIndex = 0;
		printf("key number 8 was pressed");
		a3clipControllerUpdate(&demoMode->clipCtrlZero, 60);
	}
	if (a3keyboardIsPressed(demoState->keyboard, a3key_9) == 1)
	{
		demoMode->clipCtrlIndex = 1;
		a3clipControllerUpdate(&demoMode->clipCtrlOne, 60);
	}
	if (a3keyboardIsPressed(demoState->keyboard, a3key_0) == 1)
	{
		demoMode->clipCtrlIndex = 2;
		a3clipControllerUpdate(&demoMode->clipCtrlTwo, 60);
	}

	// home key plays/pauses the controller playback
	if (a3keyboardIsPressed(demoState->keyboard, a3key_home) == 1)
	{
		if (demoMode->isPlay)
		{
			demoMode->isPlay = a3false;
			temp.playbackDirection = 0;
		}
		else
		{
			demoMode->isPlay = a3true;
			temp.playbackDirection = 1;
		}
	}
	// end key sets to first/last frame in current clip
	if (a3keyboardIsPressed(demoState->keyboard, a3key_end) == 1)
	{
		if (demoMode->isFirstFrame)
		{
			demoMode->isFirstFrame = a3false;
			temp.keyIndex = temp.clipPool->clip->lastKeyIndex;
		}
		else
		{
			demoMode->isFirstFrame = a3true;
			temp.keyIndex = temp.clipPool->clip->firstKeyIndex;
		}
	}

	//  changes clip to control using the number row
	if (a3keyboardIsPressed(demoState->keyboard, a3key_1) == 1)
	{
		temp.clipIndex = 0;
	}
	if (a3keyboardIsPressed(demoState->keyboard, a3key_2) == 2)
	{
		temp.clipIndex = 1;
	}
	if (a3keyboardIsPressed(demoState->keyboard, a3key_3) == 3)
	{
		temp.clipIndex = 2;
	}
	if (a3keyboardIsPressed(demoState->keyboard, a3key_4) == 4)
	{
		temp.clipIndex = 3;
	}
	if (a3keyboardIsPressed(demoState->keyboard, a3key_5) == 5)
	{
		temp.clipIndex = 4;
	}

	// page up flips playback direction
	if (a3keyboardIsPressed(demoState->keyboard, a3key_pageup) == 1)
	{
		if (demoMode->isForwardDir)
		{
			demoMode->isForwardDir = a3false;
			temp.playbackDirection = -1;
		}
		else
		{
			demoMode->isForwardDir = a3true;
			temp.playbackDirection = 1;
		}
	}
	// page down switches to slow motion
	if (a3keyboardIsPressed(demoState->keyboard, a3key_pagedown) == 1)
	{
		if (demoMode->isNormalTime)
		{
			demoMode->isNormalTime = a3false;
		}
		else
		{
			demoMode->isNormalTime = a3true;
		}
	}
}


//-----------------------------------------------------------------------------
