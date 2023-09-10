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
	inline definitions for keyframe animation controller.

	Joey Romanowski: 
		Implemented a3clipControllerUpdate(...)
		Implemented a3clipControllerSetClip(...)
*/

#ifdef __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H
#ifndef __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL
#define __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL


//-----------------------------------------------------------------------------

// update clip controller
inline a3i32 a3clipControllerUpdate(a3_ClipController* clipCtrl, const a3real dt)
{
	// TODO: Get the current clips and their important values 

	// NOTE: abs(clipCtrl->playbackDirection) > 1 could lead to unintended behaviors

	// --== Pre-resolution ==--
	// Integrate time for current clip and keyframe
	// Case: Forward, Paused, and Reverse
	clipCtrl->clipTime += dt * clipCtrl->playbackDirection;
	clipCtrl->keyTime += dt * clipCtrl->playbackDirection;

	// --== Resolution ==--
	// If clip is moving forwards
	if (clipCtrl->playbackDirection > 0)
	{
		// Case: Forward Skip
		// if (keyTime >= key->duration)
		// {
		//		keyTime -= key->duration;
		//		keyIndex++;
		// }
		// 
		// Case: Forward Terminus
		// if (clipTime >= clip->duration)
		// {
		//		// Loop back to start
		//		keyIndex = clip->firstKeyframe;
		//		clipTime -= clip->duration; // Don't lose the extra time 
		// }
		//
	}

	// If clip is moving backwards
	if (clipCtrl->playbackDirection < 0)
	{
		// // Case: Reverse Skip
		// if (keyTime < 0)
		// {
		//		keyTime += key->duration;
		//		keyIndex--;
		// }
		// 
		// // Case: Reverse Terminus
		// if (clipTime < 0)
		// {
		//		// Loop back to end
		//		keyIndex = clip->lastKeyframe;
		//		clipTime += clip->duration;
		// }
		//
	}

	// --== Post-resolution ==--
	//clipCtrl->clipParameter = clipCtrl->clipTime / clip->durataion;
	//clipCtrl->keyParameter = clipCtrl->keyTime / key->durataion;

	return -1;
}

// set clip to play
inline a3i32 a3clipControllerSetClip(a3_ClipController* clipCtrl, const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool)
{
	clipCtrl->clipPool = clipPool;
	clipCtrl->clipIndex = clipIndex_pool;
	return 0;
}


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL
#endif	// __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H