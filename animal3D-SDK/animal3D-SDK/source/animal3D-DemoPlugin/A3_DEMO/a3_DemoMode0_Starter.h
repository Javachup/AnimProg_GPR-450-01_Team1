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

	a3_DemoMode0_Starter.h
	Demo mode interface: starter scene.

	********************************************
	*** THIS IS ONE DEMO MODE'S HEADER FILE  ***
	********************************************
	
	Ananda Shumock-Bailey
	Added booleans for menu and variables for indices
*/

#ifndef __ANIMAL3D_DEMOMODE0_STARTER_H
#define __ANIMAL3D_DEMOMODE0_STARTER_H


//-----------------------------------------------------------------------------

#include "_a3_demo_utilities/a3_DemoSceneObject.h"
#include "_animation/a3_KeyframeAnimationController.h"
#include "animal3D-A3DG/a3graphics/A3_TEXTUREATLAS.H"

//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_DemoMode0_Starter							a3_DemoMode0_Starter;
typedef enum a3_DemoMode0_Starter_RenderProgramName			a3_DemoMode0_Starter_RenderProgramName;
typedef enum a3_DemoMode0_Starter_DisplayProgramName		a3_DemoMode0_Starter_DisplayProgramName;
typedef enum a3_DemoMode0_Starter_ActiveCameraName			a3_DemoMode0_Starter_ActiveCameraName;
typedef enum a3_DemoMode0_Starter_PipelineName				a3_DemoMode0_Starter_PipelineName;
typedef enum a3_DemoMode0_Starter_PassName					a3_DemoMode0_Starter_PassName;
typedef enum a3_DemoMode0_Starter_TargetName				a3_DemoMode0_Starter_TargetName;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

	// maximum unique objects
	enum a3_DemoMode0_Starter_ObjectMaxCount
	{
		starterMaxCount_sceneObject = 8,
		starterMaxCount_cameraObject = 1,
		starterMaxCount_projector = 1,
		starterMaxCount_clipCtrl = 2,
	};

	// scene object rendering program names
	enum a3_DemoMode0_Starter_RenderProgramName
	{
		starter_renderSolid,			// solid color
		starter_renderTexture,			// textured

		starter_render_max
	};

	// final display modes
	enum a3_DemoMode0_Starter_DisplayProgramName
	{
		starter_displayTexture,			// display simple texture

		starter_display_max
	};

	// active camera names
	enum a3_DemoMode0_Starter_ActiveCameraName
	{
		starter_cameraSceneViewer,		// scene viewing camera

		starter_camera_max
	};

	// pipeline names
	enum a3_DemoMode0_Starter_PipelineName
	{
		starter_forward,				// forward lighting pipeline

		starter_pipeline_max
	};

	// render passes
	enum a3_DemoMode0_Starter_PassName
	{
		starter_passScene,				// render scene objects
		starter_passComposite,			// composite layers

		starter_pass_max
	};

	// render target names
	enum a3_DemoMode0_Starter_TargetName
	{
		starter_scene_finalcolor = 0,	// final display color
		starter_scene_fragdepth,		// fragment depth
	
		starter_target_scene_max,
	};

	enum a3_DemoMode0_Starter_TerminusActions
	{
		starter_loop,
		starter_stop,
		starter_pingPong,

		starter_terminusActions_max
	};


//-----------------------------------------------------------------------------

	// demo mode for basic shading
	struct a3_DemoMode0_Starter
	{
		a3_DemoMode0_Starter_RenderProgramName render;
		a3_DemoMode0_Starter_DisplayProgramName display;
		a3_DemoMode0_Starter_ActiveCameraName activeCamera;

		a3_DemoMode0_Starter_PipelineName pipeline;
		a3_DemoMode0_Starter_PassName pass;
		a3_DemoMode0_Starter_TargetName targetIndex[starter_pass_max], targetCount[starter_pass_max];

		// objects
		union {
			a3_DemoSceneObject object_scene[starterMaxCount_sceneObject];
			struct {
				a3_DemoSceneObject
					obj_skybox[1];
				a3_DemoSceneObject
					obj_plane[1],
					obj_box[1],
					obj_sphere[1],
					obj_cylinder[1],
					obj_capsule[1],
					obj_torus[1];
				a3_DemoSceneObject
					obj_teapot[1];
			};
		};
		union {
			a3_DemoSceneObject object_camera[starterMaxCount_cameraObject];
			struct {
				a3_DemoSceneObject
					obj_camera_main[1];
			};
		};
		union {
			a3_DemoProjector projector[starterMaxCount_projector];
			struct {
				a3_DemoProjector
					proj_camera_main[1];
			};
		};

		// A collection of all the clip controllers
		// In this pattern so that we can update them all in a loop 
		// but also access them individually
		union
		{
			a3_ClipController clipCtrls[starterMaxCount_clipCtrl];
			struct
			{
				a3_ClipController
					clipCtrl_sprite[1],
					clipCtrl_lerp[1];
			};
		};

		// code added in class
		a3_ClipPool clipPool_sprite;
		a3_KeyframePool keyPool_sprite;

		// For lerping
		a3_ClipPool clipPool_lerp;
		a3_KeyframePool keyPool_lerp;

		// Booleans for controls
		a3boolean isNormalTime;

		// Numbers for clip controllers and clips
		a3integer clipCtrlIndex;

		// Texture atlas for sprites
		a3_TextureAtlas spriteTestAtlas;
        
		// Terminus Actions
		ForwardTerminusFunc forwardTerminusActions[starter_terminusActions_max];
		ReverseTerminusFunc reverseTerminusActions[starter_terminusActions_max];
		a3ui32 forwardAction, reverseAction;

		// DEBUG
		a3real lerp;
	};

//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// !__ANIMAL3D_DEMOMODE0_STARTER_H