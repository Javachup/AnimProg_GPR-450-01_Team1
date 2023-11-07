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

	a3_DemoMode1_Animation-idle-input.c
	Demo mode implementations: animation scene.

	********************************************
	*** INPUT FOR ANIMATION SCENE MODE       ***
	********************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoMode1_Animation.h"

//typedef struct a3_DemoState a3_DemoState;
#include "../a3_DemoState.h"

#include "../_a3_demo_utilities/a3_DemoMacros.h"


//-----------------------------------------------------------------------------
// CALLBACKS

// main demo mode callback
void a3animation_input_keyCharPress(a3_DemoState const* demoState, a3_DemoMode1_Animation* demoMode, a3i32 const asciiKey, a3i32 const state)
{
	switch (asciiKey)
	{
		// toggle render program
		//a3demoCtrlCasesLoop(demoMode->render, animation_render_max, 'k', 'j');

		// toggle display program
		//a3demoCtrlCasesLoop(demoMode->display, animation_display_max, 'K', 'J');

		// toggle active camera
		//a3demoCtrlCasesLoop(demoMode->activeCamera, animation_camera_max, 'v', 'c');

		// toggle pipeline mode
		a3demoCtrlCasesLoop(demoMode->pipeline, animation_pipeline_max, ']', '[');

		// toggle target
		a3demoCtrlCasesLoop(demoMode->targetIndex[demoMode->pass], demoMode->targetCount[demoMode->pass], '}', '{');

		// toggle pass to display
		a3demoCtrlCasesLoop(demoMode->pass, animation_pass_max, ')', '(');

		// toggle control target
		a3demoCtrlCasesLoop(demoMode->ctrl_target, animation_ctrlmode_max, '\'', ';');

		// toggle position input mode
		a3demoCtrlCasesLoop(demoMode->ctrl_position, animation_inputmode_max, '=', '-');

		// toggle rotation input mode
		a3demoCtrlCasesLoop(demoMode->ctrl_rotation, animation_inputmode_max, '+', '_');
	}
}

void a3animation_input_keyCharHold(a3_DemoState const* demoState, a3_DemoMode1_Animation* demoMode, a3i32 const asciiKey, a3i32 const state)
{
//	switch (asciiKey)
//	{
//
//	}
}
//-----------------------------------------------------------------------------
//locomotion algorithms

//Euler Integration
a3real fIntegrateEuler(a3real x, a3real dx_dt, a3real dt)
{
	a3real x_tdt;
	x_tdt = x + (dx_dt * dt);
	return x_tdt;
	//return 0;
}

//Kinematic Integration
a3real fIntegrateKinematics(a3real x, a3real dx_dt, a3real dx_dt2, a3real dt)
{
	a3real x_tdt;
	x_tdt = x + dx_dt * (dt) + dx_dt2 * ((a3real)pow(dt, 2))/2;
	return x_tdt;
	//return 0;
}

//Interpolation-based Integration
a3real fIntegrateInterpolated(a3real x, a3real x_c, a3real u)
{
	a3real x_tdt;
	x_tdt = x + (x_c - x) * u;
	return x_tdt;
	//return 0;
}


//-----------------------------------------------------------------------------

void a3demo_input_controlProjector(
	a3_DemoState* demoState, a3_DemoProjector* projector,
	a3f64 const dt, a3real ctrlMoveSpeed, a3real ctrlRotateSpeed, a3real ctrlZoomSpeed);

void a3animation_input(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode, a3f64 const dt)
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
	
	a3vec2 inputPos = { a3real_zero, a3real_zero };
	a3vec2 inputRot = { a3real_zero, a3real_zero };
	a3real scalePos = a3real_two;
	a3real scaleRot = a3real_fortyfive;

	// choose control target
	switch (demoMode->ctrl_target)
	{
	case animation_ctrl_camera:
		// move camera
		a3demo_input_controlProjector(demoState, projector,
			dt, projector->ctrlMoveSpeed, projector->ctrlRotateSpeed, projector->ctrlZoomSpeed);
		break;
	case animation_ctrl_character:
		// capture axes
		if (a3XboxControlIsConnected(demoState->xcontrol))
		{
			// ****TO-DO:
			// get directly from joysticks
			a3f64 xinput[2];
			a3XboxControlGetLeftJoystick(demoState->xcontrol, xinput);
			inputPos.x = scalePos * (a3real)xinput[0];
			inputPos.y = scalePos * (a3real)xinput[1];

			a3XboxControlGetRightJoystick(demoState->xcontrol, xinput);
			inputRot.x = scaleRot * (a3real)xinput[0];
			inputRot.y = scaleRot * (a3real)xinput[1];
		}
		else
		{
			inputPos.x = scalePos * (a3real)a3keyboardIsHeld(demoState->keyboard, a3key_A) - (a3real)a3keyboardIsHeld(demoState->keyboard, a3key_D);
			inputPos.y = scalePos * (a3real)a3keyboardIsHeld(demoState->keyboard, a3key_S) - (a3real)a3keyboardIsHeld(demoState->keyboard, a3key_W);

			inputRot.x = scaleRot * ((a3real)a3keyboardIsHeld(demoState->keyboard, a3key_I) - (a3real)a3keyboardIsHeld(demoState->keyboard, a3key_K));
			inputRot.y = scaleRot * ((a3real)a3keyboardIsHeld(demoState->keyboard, a3key_I) - (a3real)a3keyboardIsHeld(demoState->keyboard, a3key_K));
		}

		switch (demoMode->ctrl_position)
		{
			case animation_input_direct:
			{
				// direct translation assignment
				demoMode->positionNode.translate.xy = inputPos;

				// direct rotation assignment
				demoMode->positionNode.rotate.xy = inputRot;
				break;
			}

			case animation_input_euler:
			{
				// control velocity translation
				demoMode->velocityNode.translate.x = inputPos.x;
				demoMode->velocityNode.translate.y = inputPos.y;

				demoMode->positionNode.translate.x += demoMode->velocityNode.translate.x * (a3real)demoState->dt_timer;
				demoMode->positionNode.translate.y += demoMode->velocityNode.translate.y * (a3real)demoState->dt_timer;

				// control velocity rotation
				demoMode->velocityNode.rotate.x = inputRot.x;
				demoMode->velocityNode.rotate.y = inputRot.y;

				demoMode->positionNode.rotate.x += demoMode->velocityNode.rotate.x * (a3real)demoState->dt_timer;
				demoMode->positionNode.rotate.y += demoMode->velocityNode.rotate.y * (a3real)demoState->dt_timer;
				break;
			}

			case animation_input_kinematic:
			{
				// control acceleration translation
				demoMode->velocityNode.translate.x += inputPos.x * (a3real)demoState->dt_timer;
				demoMode->velocityNode.translate.y += inputPos.y * (a3real)demoState->dt_timer;

				demoMode->positionNode.translate.x += demoMode->velocityNode.translate.x * (a3real)demoState->dt_timer;
				demoMode->positionNode.translate.y += demoMode->velocityNode.translate.y * (a3real)demoState->dt_timer;
						
				// control acceleration rotation
				demoMode->velocityNode.rotate.x += inputRot.x * (a3real)demoState->dt_timer;
				demoMode->velocityNode.rotate.y += inputRot.y * (a3real)demoState->dt_timer;

				demoMode->positionNode.rotate.x += demoMode->velocityNode.rotate.x * (a3real)demoState->dt_timer;
				demoMode->positionNode.rotate.y += demoMode->velocityNode.rotate.y * (a3real)demoState->dt_timer;
				break;
			}

			// TODO:
			case animation_input_interpolate1: //two lines
			{
				// fake velocity translation
				a3real dirX = (a3real)a3keyboardIsHeld(demoState->keyboard, a3key_A) -(a3real)a3keyboardIsHeld(demoState->keyboard, a3key_D);
				a3real dirY = (a3real)a3keyboardIsHeld(demoState->keyboard, a3key_S) - (a3real)a3keyboardIsHeld(demoState->keyboard, a3key_W);
				demoMode->positionNode.translate.x = fIntegrateInterpolated(demoMode->positionNode.translate.x, dirX, (a3real)demoState->dt_timer);
				demoMode->positionNode.translate.y = fIntegrateInterpolated(demoMode->positionNode.translate.y, dirY, (a3real)demoState->dt_timer);

				// fake velocity rotation
				a3real rotX = a3real_pi * ((a3real)a3keyboardIsHeld(demoState->keyboard, a3key_I) - (a3real)a3keyboardIsHeld(demoState->keyboard, a3key_K));
				a3real rotY = a3real_pi * ((a3real)a3keyboardIsHeld(demoState->keyboard, a3key_J) - (a3real)a3keyboardIsHeld(demoState->keyboard, a3key_L));
				demoMode->positionNode.rotate.x = (demoMode->positionNode.rotate.y, rotY, (a3real)demoState->dt_timer);
				demoMode->positionNode.rotate.y = fIntegrateInterpolated(demoMode->positionNode.rotate.x, rotX, (a3real)demoState->dt_timer);
				break;
			}

			// TODO:
			case animation_input_interpolate2: //three lines
			{
				// fake acceleration translation
				a3real dirX = (a3real)a3keyboardIsHeld(demoState->keyboard, a3key_A) - (a3real)a3keyboardIsHeld(demoState->keyboard, a3key_D);
				a3real dirY = (a3real)a3keyboardIsHeld(demoState->keyboard, a3key_S) - (a3real)a3keyboardIsHeld(demoState->keyboard, a3key_W);
				// target velocity in world space 
					// integrate current velocity -> euler
					// integrate velocity -> interpolation
				demoMode->positionNode.translate.x = fIntegrateEuler(demoMode->positionNode.translate.x, dirX, (a3real)demoState->dt_timer);
				demoMode->positionNode.translate.y = fIntegrateEuler(demoMode->positionNode.translate.y, dirY, (a3real)demoState->dt_timer);
				demoMode->positionNode.translate.x = fIntegrateInterpolated(demoMode->positionNode.translate.x, dirX, (a3real)demoState->dt_timer);
				demoMode->positionNode.translate.y = fIntegrateInterpolated(demoMode->positionNode.translate.y, dirY, (a3real)demoState->dt_timer);

				// fake acceleration rotation
				a3real rotX = a3real_pi * ((a3real)a3keyboardIsHeld(demoState->keyboard, a3key_I) - (a3real)a3keyboardIsHeld(demoState->keyboard, a3key_K));
				a3real rotY = a3real_pi * ((a3real)a3keyboardIsHeld(demoState->keyboard, a3key_J) - (a3real)a3keyboardIsHeld(demoState->keyboard, a3key_L));
				// target angular velocity
					// integrate current angular velocity -> euler
					// integrate angular velocity -> interpolation

				demoMode->positionNode.rotate.x = fIntegrateEuler(demoMode->positionNode.rotate.x, rotX, (a3real)demoState->dt_timer);
				demoMode->positionNode.rotate.y = fIntegrateEuler(demoMode->positionNode.rotate.y, rotY, (a3real)demoState->dt_timer);
				demoMode->positionNode.rotate.x = fIntegrateInterpolated(demoMode->positionNode.rotate.x, rotX, (a3real)demoState->dt_timer);
				demoMode->positionNode.rotate.y = fIntegrateInterpolated(demoMode->positionNode.rotate.y, rotY, (a3real)demoState->dt_timer);
				break;
			}

			// find the magnitude of (x, y) and send it to the branching transformation blend tree
			/*
			make a new member variable in demoMode to hold 'x'
			find the magnitude of the vector in each input
			the blend tree is a member var of demoMode
			*/
			demoMode->branchTransParam = a3sqrt((inputPos.x * inputPos.x) + (inputPos.y * inputPos.y));
			demoMode->branchTransParamInv = 1 - demoMode->branchTransParam;
		}
		break;
	}

	// allow the controller, if connected, to change control targets
	if (a3XboxControlIsConnected(demoState->xcontrol))
	{
		if (a3XboxControlIsPressed(demoState->xcontrol, a3xbox_DPAD_right))
			a3demoCtrlIncLoop(demoMode->ctrl_target, animation_ctrlmode_max);
		if (a3XboxControlIsPressed(demoState->xcontrol, a3xbox_DPAD_left))
			a3demoCtrlDecLoop(demoMode->ctrl_target, animation_ctrlmode_max);

		if (a3XboxControlIsPressed(demoState->xcontrol, a3xbox_B))
			a3demoCtrlIncLoop(demoMode->ctrl_position, animation_inputmode_max);
		if (a3XboxControlIsPressed(demoState->xcontrol, a3xbox_X))
			a3demoCtrlDecLoop(demoMode->ctrl_position, animation_inputmode_max);

		if (a3XboxControlIsPressed(demoState->xcontrol, a3xbox_Y))
			a3demoCtrlIncLoop(demoMode->ctrl_rotation, animation_inputmode_max);
		if (a3XboxControlIsPressed(demoState->xcontrol, a3xbox_A))
			a3demoCtrlDecLoop(demoMode->ctrl_rotation, animation_inputmode_max);
	}
}


//-----------------------------------------------------------------------------
