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

	a3_DemoMode0_Starter_idle-input.c
	Demo mode implementations: starter scene.

	********************************************
	*** INPUT FOR STARTER SCENE MODE         ***
	********************************************
	 
	Ananda Shumock-Bailey
	Implemented the basic testing interface
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoMode0_Starter.h"

//typedef struct a3_DemoState a3_DemoState;
#include "../a3_DemoState.h"

#include "../_a3_demo_utilities/a3_DemoMacros.h"
#include <stdio.h>


//-----------------------------------------------------------------------------
// CALLBACKS

// main demo mode callback
void a3starter_input_keyCharPress(a3_DemoState const* demoState, a3_DemoMode0_Starter* demoMode, a3i32 const asciiKey, a3i32 const state)
{
	if (demoState->textMode == demoState_clipCtrl)
	{
		a3_ClipController* currCtrl = &demoMode->clipCtrls[demoMode->clipCtrlIndex];
		a3ui32 currClipIndex = currCtrl->clipIndex;

		switch (asciiKey)
		{
			// toggle clip controller
			a3demoCtrlCasesLoop(demoMode->clipCtrlIndex, starterMaxCount_clipCtrl, '2', '1');

			// toggle clip
			a3demoCtrlCasesLoop(currClipIndex, 5, ']', '[');

			// Toggles from 0 (paused) to 1 (forwards)
			// Can not unpause into the reverse direction
			a3demoCtrlCaseToggle(currCtrl->playbackDirection, ' ');

			// Reverse
		case ',':
			currCtrl->playbackDirection = -1;
			break;

			// Forwards
		case '.':
			currCtrl->playbackDirection = 1;
			break;

		case 'o':
			a3clipControllerSetKeyframe(currCtrl, getCurrentClip(currCtrl)->firstKeyIndex, false);
			break;

		case 'p':
			a3clipControllerSetKeyframe(currCtrl, getCurrentClip(currCtrl)->lastKeyIndex, true);
			break;

			// toggle slow motion
			a3demoCtrlCaseToggle(demoMode->isNormalTime, 's');
		}

		// Update new values
		if (currClipIndex != currCtrl->clipIndex)
		{
			a3clipControllerSetClip(currCtrl, currCtrl->clipPool, currClipIndex);
		}
	}
	else
	{
		switch (asciiKey)
		{
			// toggle render program
			a3demoCtrlCasesLoop(demoMode->render, starter_render_max, 'k', 'j');

			// toggle display program
			a3demoCtrlCasesLoop(demoMode->display, starter_display_max, 'K', 'J');

			// toggle active camera
			a3demoCtrlCasesLoop(demoMode->activeCamera, starter_camera_max, 'v', 'c');

			// toggle pipeline mode
			a3demoCtrlCasesLoop(demoMode->pipeline, starter_pipeline_max, ']', '[');

			// toggle target
			a3demoCtrlCasesLoop(demoMode->targetIndex[demoMode->pass], demoMode->targetCount[demoMode->pass], '}', '{');

			// toggle pass to display
			a3demoCtrlCasesLoop(demoMode->pass, starter_pass_max, ')', '(');
		}
	}
}

void a3starter_input_keyCharHold(a3_DemoState const* demoState, a3_DemoMode0_Starter* demoMode, a3i32 const asciiKey, a3i32 const state)
{
//	switch (asciiKey)
//	{
//
//	}
}


//-----------------------------------------------------------------------------

void a3demo_input_controlProjector(
	a3_DemoState* demoState, a3_DemoProjector* projector,
	a3f64 const dt, a3real ctrlMoveSpeed, a3real ctrlRotateSpeed, a3real ctrlZoomSpeed);

void a3starter_input(a3_DemoState* demoState, a3_DemoMode0_Starter* demoMode, a3f64 const dt)
{
	static a3integer frameCount = 0;
	frameCount++;
	a3_DemoProjector* projector = demoMode->projector + demoMode->activeCamera;

	// right click to ray pick
	if (a3mouseGetState(demoState->mouse, a3mouse_right) == a3input_down)
	{
		// get window coordinates
		a3i32 const x = a3mouseGetX(demoState->mouse) + demoState->frameBorder;
		a3i32 const y = a3mouseGetY(demoState->mouse) + demoState->frameBorder;

		// transform to NDC
		a3vec4 coord = a3vec4_one;
		coord.x = +((a3real)x * demoState->frameWidthInv * a3real_two - a3real_one);
		coord.y = -((a3real)y * demoState->frameHeightInv * a3real_two - a3real_one);
		coord.z = -a3real_one;

		// transform to view space
		a3real4Real4x4Mul(projector->projectionMatInv.m, coord.v);
		a3real4DivS(coord.v, coord.w);

		// transform to world space
		a3real4Real4x4Mul(projector->sceneObject->modelMat.m, coord.v);
	}
	
	// move camera
	a3demo_input_controlProjector(demoState, projector,
		dt, projector->ctrlMoveSpeed, projector->ctrlRotateSpeed, projector->ctrlZoomSpeed);

	/* CODE FOR CLASS */
	// determines which clip controller we are currently on
	a3_ClipController* temp = NULL;
	switch (demoMode->clipCtrlIndex)
	{
	case 0: temp = &demoMode->clipCtrlZero; break;
	case 1: temp = &demoMode->clipCtrlOne; break;
	case 2: temp = &demoMode->clipCtrlTwo; break;
	default: printf("clipCtrlIndex out of bounds!"); return;
	}

	if (demoMode->isPlay)
		temp->playbackDirection = demoMode->isForwardDir ? 1 : -1;
	else
		temp->playbackDirection = 0;

	// the keys O and P set to first/last frame in current clip
	if (a3keyboardIsPressed(demoState->keyboard, a3key_O) == 1)
	{
		a3clipControllerSetKeyframe(temp, getCurrentClip(temp)->firstKeyIndex, a3false);
	}
	if (a3keyboardIsPressed(demoState->keyboard, a3key_P) == 1)
	{
		a3clipControllerSetKeyframe(temp, getCurrentClip(temp)->lastKeyIndex, a3true);
	}

	if (temp->clipIndex != demoMode->clipIndex)
	{
		a3clipControllerSetClip(temp, temp->clipPool, demoMode->clipIndex);
	}
}


//-----------------------------------------------------------------------------
