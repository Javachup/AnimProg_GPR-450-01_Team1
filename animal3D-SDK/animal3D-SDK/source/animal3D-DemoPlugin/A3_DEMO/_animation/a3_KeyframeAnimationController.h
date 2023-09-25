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
	
	a3_KeyframeAnimationController.h
	Keyframe animation clip controller. Basically a frame index manager. Very 
	limited in what one can do with this; could potentially be so much more.

	Joey Romanowski: 
		Added Terminus Action Functions
*/

#ifndef __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H
#define __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H


#include "a3_KeyframeAnimation.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_ClipController			a3_ClipController;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

// Function pointer for a function that takes a clipCtrl and left over time
// Both for forward terminus and reverse terminus
typedef a3ret(*ForwardTerminusFunc)(a3_ClipController* clipCtrl, a3f64 leftOverTime);
typedef a3ret(*ReverseTerminusFunc)(a3_ClipController* clipCtrl, a3f64 leftOverTime);

//-----------------------------------------------------------------------------
// 
// clip controller
// metaphor: playhead
struct a3_ClipController
{
	a3byte name[a3keyframeAnimation_nameLenMax];

	const a3_ClipPool* clipPool;
	a3ui32 clipIndex;
	a3f64 clipTime;
	a3f64 clipParameter;

	a3ui32 keyIndex;
	a3f64 keyTime;
	a3f64 keyParameter;

	// Terminus Functions CAN set:
	//		* clipTime
	//		* keyIndex
	//		* clipIndex
	//		* keyTime
	//		* playbackDirection
	//		* clipPool (TODO: untested)
	// Terminus Functions CAN NOT set:
	//		* name
	//		* clipParameter
	//		* keyParameter
	ForwardTerminusFunc forwardTerminus;
	ReverseTerminusFunc reverseTerminus;

	a3i16 playbackDirection; // 1 = forward, 0 = paused, -1 = reverse

	
};

// Macros for ease of use 
#define getCurrentClip(clipCtrl) (clipCtrl->clipPool->clip + clipCtrl->clipIndex)
#define getCurrentKeyframe(clipCtrl) (getCurrentClip(clipCtrl)->keyframes + clipCtrl->keyIndex)

// initialize clip controller
a3i32 a3clipControllerInit(a3_ClipController* clipCtrl_out, const a3byte ctrlName[a3keyframeAnimation_nameLenMax], const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool);

// update clip controller
a3i32 a3clipControllerUpdate(a3_ClipController* clipCtrl, const a3f64 dt);

// set clip to play
a3i32 a3clipControllerSetClip(a3_ClipController* clipCtrl, const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool);

// set current keyframe
a3ret a3clipControllerSetKeyframe(a3_ClipController* clipCtrl, a3ui32 keyIndex, a3boolean setToEndOfKey);

//-----------------------------------------------------------------------------

a3ret forwardLoop(a3_ClipController* clipCtrl, a3f64 leftOverTime);
a3ret reverseLoop(a3_ClipController* clipCtrl, a3f64 leftOverTime);

a3ret forwardStop(a3_ClipController* clipCtrl, a3f64 leftOverTime);
a3ret reverseStop(a3_ClipController* clipCtrl, a3f64 leftOverTime);

a3ret forwardPingPong(a3_ClipController* clipCtrl, a3f64 leftOverTime);
a3ret reversePingPong(a3_ClipController* clipCtrl, a3f64 leftOverTime);

//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_KeyframeAnimationController.inl"


#endif	// !__ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H