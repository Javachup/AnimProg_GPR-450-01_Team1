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

	a3_DemoMode1_Animation.h
	Demo mode interface: animation scene.

	********************************************
	*** THIS IS ONE DEMO MODE'S HEADER FILE  ***
	********************************************
*/

#ifndef __ANIMAL3D_DEMOMODE1_ANIMATION_H
#define __ANIMAL3D_DEMOMODE1_ANIMATION_H


//-----------------------------------------------------------------------------

#include "_a3_demo_utilities/a3_DemoSceneObject.h"

#include "_animation/a3_HierarchyStateBlend.h"

#include "_animation/a3_KeyframeAnimationController.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_DemoMode1_Animation						a3_DemoMode1_Animation;
typedef struct a3_DemoMode1_Animation_DisplayInfo			a3_DemoMode1_Animation_DisplayInfo;
typedef enum a3_DemoMode1_Animation_RenderProgramName		a3_DemoMode1_Animation_RenderProgramName;
typedef enum a3_DemoMode1_Animation_DisplayProgramName		a3_DemoMode1_Animation_DisplayProgramName;
typedef enum a3_DemoMode1_Animation_ActiveCameraName		a3_DemoMode1_Animation_ActiveCameraName;
typedef enum a3_DemoMode1_Animation_PipelineName			a3_DemoMode1_Animation_PipelineName;
typedef enum a3_DemoMode1_Animation_PassName				a3_DemoMode1_Animation_PassName;
typedef enum a3_DemoMode1_Animation_TargetName				a3_DemoMode1_Animation_TargetName;
typedef enum a3_DemoMode1_Animation_Operation				a3_DemoMode1_Animation_Operation;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

	// maximum unique objects
	enum a3_DemoMode1_Animation_ObjectMaxCount
	{
		animationMaxCount_sceneObject = 8,
		animationMaxCount_cameraObject = 1,
		animationMaxCount_projector = 1,
		animationMaxCount_skeleton = 3,
		animationMaxCount_hs = 6,
		animationMaxCount_clipCtrl = 4,
		animationMaxCount_paramters = 8,
	};

	// scene object rendering program names
	enum a3_DemoMode1_Animation_RenderProgramName
	{
		animation_renderSolid,			// solid color
		animation_renderTexture,			// textured
		animation_renderLambert,			// Lambert shading model
		animation_renderPhong,			// Phong shading model

		animation_render_max
	};

	// final display modes
	enum a3_DemoMode1_Animation_DisplayProgramName
	{
		animation_displayTexture,			// display simple texture

		animation_display_max
	};

	// active camera names
	enum a3_DemoMode1_Animation_ActiveCameraName
	{
		animation_cameraSceneViewer,		// scene viewing camera

		animation_camera_max
	};

	// pipeline names
	enum a3_DemoMode1_Animation_PipelineName
	{
		animation_forward,				// forward lighting pipeline

		animation_pipeline_max
	};

	// render passes
	enum a3_DemoMode1_Animation_PassName
	{
		animation_passScene,				// render scene objects
		animation_passComposite,			// composite layers

		animation_pass_max
	};

	// render target names
	enum a3_DemoMode1_Animation_TargetName
	{
		animation_scene_finalcolor = 0,	// final display color
		animation_scene_fragdepth,		// fragment depth
	
		animation_target_scene_max,
	};

	enum a3_DemoMode1_Animation_Operation
	{
		animation_op_invert,
		animation_op_concat,
		animation_op_nearest,
		animation_op_lerp,
		animation_op_cubic,
		animation_op_split,
		animation_op_scale,
		animation_op_triangular,
		animation_op_binearest,
		animation_op_bilinear,
		animation_op_bicubic,

		animation_op_max = 10 // ignore bicubic
	};

	struct a3_DemoMode1_Animation_DisplayInfo
	{
		a3ui32 numSkelToDraw;
		a3ui32 numParameters;
		a3real parameters[animationMaxCount_paramters];
	};


//-----------------------------------------------------------------------------

	// demo mode for basic shading
	struct a3_DemoMode1_Animation
	{
		a3_DemoMode1_Animation_RenderProgramName render;
		a3_DemoMode1_Animation_DisplayProgramName display;
		a3_DemoMode1_Animation_ActiveCameraName activeCamera;

		a3_DemoMode1_Animation_PipelineName pipeline;
		a3_DemoMode1_Animation_PassName pass;
		a3_DemoMode1_Animation_TargetName targetIndex[animation_pass_max], targetCount[animation_pass_max];

		// skeletal animation
		a3_Hierarchy hierarchy_skel[1];
		a3_HierarchyPoseGroup hierarchyPoseGroup_skel[1];

		// 4 hierarchy states for base, output, and 2 controls
		union
		{
			a3_HierarchyState hierarchyState_skel[animationMaxCount_hs];
			struct
			{
				a3_HierarchyState
					hs_base[1],
					hs_output[1],
					hs_control_short1[1],
					hs_control_short2[1],
					hs_control_hug[1],
					hs_control_4[1];
			};
		};

		a3_ClipPool clips[1];
		a3_KeyframePool keys[1];

		union
		{
			a3_ClipController clipCtrls[animationMaxCount_clipCtrl];
			struct
			{
				a3_ClipController
					short1[1],
					short2[1],
					hug[1],
					clipCtrl4[1];
			};
		};

		// For blend tree
		a3_BlendNodePool nodePool[1];
		a3_BlendTree blendTree[1];

		a3real hugScaleAmount;
		a3real shortsLerpAmount;

		a3mat4 mvp_joint[128], mvp_bone[128], t_skin[128];
		a3dualquat dq_skin[128];

		// for display
		a3_DemoMode1_Animation_Operation currentOp;
		a3boolean shouldDisplayOp; // Either display operation info or clip ctrls info
		a3_DemoMode1_Animation_DisplayInfo displayInfo;
		a3real displayParam;

		// objects
		union {
			a3_DemoSceneObject object_scene[animationMaxCount_sceneObject];
			struct {
				a3_DemoSceneObject
					obj_skybox[1];
				a3_DemoSceneObject
					obj_skeleton[animationMaxCount_skeleton];
			};
		};
		union {
			a3_DemoSceneObject object_camera[animationMaxCount_cameraObject];
			struct {
				a3_DemoSceneObject
					obj_camera_main[1];
			};
		};
		union {
			a3_DemoProjector projector[animationMaxCount_projector];
			struct {
				a3_DemoProjector
					proj_camera_main[1];
			};
		};
	};


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// !__ANIMAL3D_DEMOMODE1_ANIMATION_H