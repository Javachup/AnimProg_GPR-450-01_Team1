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
