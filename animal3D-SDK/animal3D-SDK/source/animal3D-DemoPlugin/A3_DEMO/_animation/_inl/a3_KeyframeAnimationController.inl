#include "a3_KeyframeAnimationController.h"
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
inline a3i32 a3clipControllerUpdate(a3_ClipController* clipCtrl, const a3f64 dt)
{
	// NOTE: abs(clipCtrl->playbackDirection) > 1 could lead to unintended behaviors
	//		 Time alteration (slow mo/fast forward) should be applied to dt outside of this function

	// --== Error Checking ==--
	if (!clipCtrl) // maybe have a clipCtrl->init
		return -1;

	// Make sure we have a clip pool that has at least 1 clip 
	if (!clipCtrl->clipPool || 
		!clipCtrl->clipPool->clip || 
		clipCtrl->clipPool->count < 0)
		return -1;

	// Make sure we have a key pool that has at least 1 key frame 
	if (!clipCtrl->clipPool->clip->keyframePool ||
		!clipCtrl->clipPool->clip->keyframePool->keyframe ||
		clipCtrl->clipPool->clip->keyframePool->count < 0)
		return -1;

	// --== Pre-resolution ==--
	// Integrate time for current clip and keyframe
	// Case: Forward, Paused, and Reverse
	clipCtrl->clipTime += dt * clipCtrl->playbackDirection;
	clipCtrl->keyTime += dt * clipCtrl->playbackDirection;

	// --== Resolution ==--
	//a3boolean resolved = clipCtrl->playbackDirection == 0; // If stopped then it is resolved
	//while (!resolved)
	{
		// If clip is moving forwards
		if (clipCtrl->playbackDirection > 0)
		{
			// Case: Forward Skip
			while (clipCtrl->keyTime >= getCurrentKeyframe(clipCtrl)->duration)
			{
				clipCtrl->keyTime -= getCurrentKeyframe(clipCtrl)->duration;
				clipCtrl->keyIndex++;

				// Case: Forward Terminus
				while (clipCtrl->clipTime >= getCurrentClip(clipCtrl)->duration)
				{
					// Loop back to start
					clipCtrl->keyIndex = getCurrentClip(clipCtrl)->firstKeyIndex;
					clipCtrl->clipTime -= getCurrentClip(clipCtrl)->duration; // Don't lose the extra time 
				}
			}
		}

		// If clip is moving backwards
		if (clipCtrl->playbackDirection < 0)
		{
			// Case: Reverse Skip
			while (clipCtrl->keyTime < 0)
			{
				// Case: Reverse Terminus
				while (clipCtrl->clipTime < 0)
				{
					// Loop back to end
					clipCtrl->keyIndex = getCurrentClip(clipCtrl)->lastKeyIndex + 1; // this gets subtracted to be back in range later 
					clipCtrl->clipTime += getCurrentClip(clipCtrl)->duration;
				}

				clipCtrl->keyIndex--; // this needs to happen first
				clipCtrl->keyTime += getCurrentKeyframe(clipCtrl)->duration;
			}
		}
	}

	// --== Post-resolution ==--
	clipCtrl->clipParameter = clipCtrl->clipTime * getCurrentClip(clipCtrl)->invDuration;
	clipCtrl->keyParameter = clipCtrl->keyTime * getCurrentKeyframe(clipCtrl)->invDuration;

	return 0;
}

// set clip to play
inline a3i32 a3clipControllerSetClip(a3_ClipController* clipCtrl, const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool)
{
	clipCtrl->clipPool = clipPool;
	clipCtrl->clipIndex = clipIndex_pool;

	// Set to the first keyframe of the clip (resets all time values) 
	return a3clipControllerSetKeyframe(clipCtrl, getCurrentClip(clipCtrl)->firstKeyIndex, a3false);
}

inline a3ret a3clipControllerSetKeyframe(a3_ClipController* clipCtrl, a3ui32 keyIndex, a3boolean setToEndOfKey)
{
	// Make sure the keyframe index is in bounds 
	if (keyIndex < getCurrentClip(clipCtrl)->firstKeyIndex &&
		keyIndex > getCurrentClip(clipCtrl)->lastKeyIndex)
		return -1;

	clipCtrl->keyIndex = keyIndex;

	// Reset all of the times to the the start/end 
	if (setToEndOfKey)
	{
		clipCtrl->keyTime = getCurrentKeyframe(clipCtrl)->duration;
		clipCtrl->keyParameter = 1;
		clipCtrl->clipTime = getCurrentClip(clipCtrl)->duration;
		clipCtrl->clipParameter = 1;
	}
	else
	{
		clipCtrl->keyTime = 0;
		clipCtrl->keyParameter = 0;
		clipCtrl->clipTime = 0;
		clipCtrl->clipParameter = 0;
	}

	return 0;
}


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL
#endif	// __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H