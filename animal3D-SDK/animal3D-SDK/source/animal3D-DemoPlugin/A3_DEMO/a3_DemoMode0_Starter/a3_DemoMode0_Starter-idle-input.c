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

		/* input controls for basic testing interface */
		// toggle clip controller
		a3demoCtrlCasesLoop(demoMode->clipCtrlIndex, 3, '2', '1');

		// toggle play and pause
		a3demoCtrlCaseToggle(demoMode->isPlay, '3');

		// toggle first and last frame
		a3demoCtrlCaseToggle(demoMode->isFirstFrame, '4');

		// toggle clip
		a3demoCtrlCasesLoop(demoMode->clipIndex, 5, '6', '5');

		// toggle playback direction
		a3demoCtrlCaseToggle(demoMode->isForwardDir, '7');

		// toggle slow motion
		a3demoCtrlCaseToggle(demoMode->isNormalTime, '8');
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
	a3_ClipController temp;
	switch (demoMode->clipCtrlIndex)
	{
	case 0: temp = demoMode->clipCtrlZero; break;
	case 1: temp = demoMode->clipCtrlOne; break;
	case 2: temp = demoMode->clipCtrlTwo; break;
	}

	// play or pause controller based on bool (0 pause, 1 play)
	temp.playbackDirection = demoMode->isPlay;

	// set to first/last frame in current clip based on bool
	if (demoMode->isFirstFrame)
	{
		temp.keyIndex = temp.clipPool->clip->firstKeyIndex;
	}
	else
	{
		temp.keyIndex = temp.clipPool->clip->lastKeyIndex;
	}

	// flips playback direction based on bool
	if (demoMode->isForwardDir)
	{
		temp.playbackDirection = 1;
	}
	else
	{
		temp.playbackDirection = -1;
	}

	if (frameCount == 20)
	{
		frameCount = 0;

		printf("ccindex: %d, ", demoMode->clipCtrlIndex);

		// toggle play and pause
		printf("isplay: %d, ", demoMode->isPlay);

		// toggle first and last frame
		printf("whatframe: %d, ", demoMode->isFirstFrame);

		// toggle clip
		printf("clipindex: %d, ", demoMode->clipIndex);

		// toggle playback direction
		printf("forward: %d, ", demoMode->isForwardDir);

		// toggle slow motion
		printf("time: %d\n", demoMode->isNormalTime);

		//// display menu for controls
		//printf("\n\n\n____________MENU CONTROLS____________\n");
		//printf("Press keys 1 and 2 to select clip controller.\n");
		//printf("	1 is previous and 2 is next.\n");
		//printf("Press key 3 to play/pause controller playback.\n");
		//printf("Press key 4 to set first/last frame in current clip.\n");
		//printf("Press keys 5 and 6 to change which clip is being controlled.\n");
		//printf("	5 is previous and 6 is next.\n");
		//printf("Press key 7 to flip playback direction.\n");
		//printf("Press key 8 to toggle slow-motion on and off.\n\n\n\n");

		//// display clip controllers and their information
		//if (demoMode->clipCtrlIndex == 0)
		//{
		//	printf(">>>%s: clipPool - %p, clipIndex - %d, clipTime - %f, clipParameter - %f"
		//		"\nkeyIndex - %d, keyTime - %f, keyParameter - %f, playbackDirection - %d\n\n",
		//		demoMode->clipCtrlZero.name, demoMode->clipCtrlZero.clipPool, demoMode->clipCtrlZero.clipIndex,
		//		demoMode->clipCtrlZero.clipTime, demoMode->clipCtrlZero.clipParameter, demoMode->clipCtrlZero.keyIndex,
		//		demoMode->clipCtrlZero.keyTime, demoMode->clipCtrlZero.keyParameter, demoMode->clipCtrlZero.playbackDirection);

		//	printf("%s: clipPool - %p, clipIndex - %d, clipTime - %f, clipParameter - %f"
		//		"\nkeyIndex - %d, keyTime - %f, keyParameter - %f, playbackDirection - %d\n\n",
		//		demoMode->clipCtrlOne.name, demoMode->clipCtrlOne.clipPool, demoMode->clipCtrlOne.clipIndex,
		//		demoMode->clipCtrlOne.clipTime, demoMode->clipCtrlOne.clipParameter, demoMode->clipCtrlOne.keyIndex,
		//		demoMode->clipCtrlOne.keyTime, demoMode->clipCtrlOne.keyParameter, demoMode->clipCtrlOne.playbackDirection);

		//	printf("%s: clipPool - %p, clipIndex - %d, clipTime - %f, clipParameter - %f"
		//		"\nkeyIndex - %d, keyTime - %f, keyParameter - %f, playbackDirection - %d\n\n",
		//		demoMode->clipCtrlTwo.name, demoMode->clipCtrlTwo.clipPool, demoMode->clipCtrlTwo.clipIndex,
		//		demoMode->clipCtrlTwo.clipTime, demoMode->clipCtrlTwo.clipParameter, demoMode->clipCtrlTwo.keyIndex,
		//		demoMode->clipCtrlTwo.keyTime, demoMode->clipCtrlTwo.keyParameter, demoMode->clipCtrlTwo.playbackDirection);
		//}
		//else if (demoMode->clipCtrlIndex == 1)
		//{
		//	printf("%s: clipPool - %p, clipIndex - %d, clipTime - %f, clipParameter - %f"
		//		"\nkeyIndex - %d, keyTime - %f, keyParameter - %f, playbackDirection - %d\n\n",
		//		demoMode->clipCtrlZero.name, demoMode->clipCtrlZero.clipPool, demoMode->clipCtrlZero.clipIndex,
		//		demoMode->clipCtrlZero.clipTime, demoMode->clipCtrlZero.clipParameter, demoMode->clipCtrlZero.keyIndex,
		//		demoMode->clipCtrlZero.keyTime, demoMode->clipCtrlZero.keyParameter, demoMode->clipCtrlZero.playbackDirection);

		//	printf(">>>%s: clipPool - %p, clipIndex - %d, clipTime - %f, clipParameter - %f"
		//		"\nkeyIndex - %d, keyTime - %f, keyParameter - %f, playbackDirection - %d\n\n",
		//		demoMode->clipCtrlOne.name, demoMode->clipCtrlOne.clipPool, demoMode->clipCtrlOne.clipIndex,
		//		demoMode->clipCtrlOne.clipTime, demoMode->clipCtrlOne.clipParameter, demoMode->clipCtrlOne.keyIndex,
		//		demoMode->clipCtrlOne.keyTime, demoMode->clipCtrlOne.keyParameter, demoMode->clipCtrlOne.playbackDirection);

		//	printf("%s: clipPool - %p, clipIndex - %d, clipTime - %f, clipParameter - %f"
		//		"\nkeyIndex - %d, keyTime - %f, keyParameter - %f, playbackDirection - %d\n\n",
		//		demoMode->clipCtrlTwo.name, demoMode->clipCtrlTwo.clipPool, demoMode->clipCtrlTwo.clipIndex,
		//		demoMode->clipCtrlTwo.clipTime, demoMode->clipCtrlTwo.clipParameter, demoMode->clipCtrlTwo.keyIndex,
		//		demoMode->clipCtrlTwo.keyTime, demoMode->clipCtrlTwo.keyParameter, demoMode->clipCtrlTwo.playbackDirection);
		//}
		//else if (demoMode->clipCtrlIndex == 2)
		//{
		//	printf("%s: clipPool - %p, clipIndex - %d, clipTime - %f, clipParameter - %f"
		//		"\nkeyIndex - %d, keyTime - %f, keyParameter - %f, playbackDirection - %d\n\n",
		//		demoMode->clipCtrlZero.name, demoMode->clipCtrlZero.clipPool, demoMode->clipCtrlZero.clipIndex,
		//		demoMode->clipCtrlZero.clipTime, demoMode->clipCtrlZero.clipParameter, demoMode->clipCtrlZero.keyIndex,
		//		demoMode->clipCtrlZero.keyTime, demoMode->clipCtrlZero.keyParameter, demoMode->clipCtrlZero.playbackDirection);

		//	printf("%s: clipPool - %p, clipIndex - %d, clipTime - %f, clipParameter - %f"
		//		"\nkeyIndex - %d, keyTime - %f, keyParameter - %f, playbackDirection - %d\n\n",
		//		demoMode->clipCtrlOne.name, demoMode->clipCtrlOne.clipPool, demoMode->clipCtrlOne.clipIndex,
		//		demoMode->clipCtrlOne.clipTime, demoMode->clipCtrlOne.clipParameter, demoMode->clipCtrlOne.keyIndex,
		//		demoMode->clipCtrlOne.keyTime, demoMode->clipCtrlOne.keyParameter, demoMode->clipCtrlOne.playbackDirection);

		//	printf(">>>%s: clipPool - %p, clipIndex - %d, clipTime - %f, clipParameter - %f"
		//		"\nkeyIndex - %d, keyTime - %f, keyParameter - %f, playbackDirection - %d\n\n",
		//		demoMode->clipCtrlTwo.name, demoMode->clipCtrlTwo.clipPool, demoMode->clipCtrlTwo.clipIndex,
		//		demoMode->clipCtrlTwo.clipTime, demoMode->clipCtrlTwo.clipParameter, demoMode->clipCtrlTwo.keyIndex,
		//		demoMode->clipCtrlTwo.keyTime, demoMode->clipCtrlTwo.keyParameter, demoMode->clipCtrlTwo.playbackDirection);
		//}
	}
}


//-----------------------------------------------------------------------------
