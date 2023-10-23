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
		Implemented Terminus Action Functions
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
	if (!clipCtrl->clipPool->clip->keyframes ||
		!clipCtrl->clipPool->clip->keyCount < 0)
		return -1;

	// --== Pre-resolution ==--
	// Integrate time for current clip and keyframe
	// Case: Forward, Paused, and Reverse
	clipCtrl->clipTime += dt * clipCtrl->playbackDirection;
	clipCtrl->keyTime += dt * clipCtrl->playbackDirection;

	// --== Resolution ==--
	a3boolean resolved = clipCtrl->playbackDirection == 0; // If stopped then it is resolved
	while (!resolved)
	{
		// If clip is moving forwards
		if (clipCtrl->playbackDirection > 0)
		{
			// Case: Forward Skip
			if (clipCtrl->keyTime >= getCurrentKeyframe(clipCtrl)->duration)
			{
				// Case: Forward Terminus
				if (clipCtrl->clipTime >= getCurrentClip(clipCtrl)->duration)
				{
					a3f64 leftOverTime = clipCtrl->clipTime - getCurrentClip(clipCtrl)->duration;
					a3ret ret = clipCtrl->forwardTerminus(clipCtrl, leftOverTime);
					//if (ret < 0) return ret; // Make sure that call did not fail
					resolved = clipCtrl->playbackDirection == 0;
				}
				else
				{
					clipCtrl->keyTime -= getCurrentKeyframe(clipCtrl)->duration;
					clipCtrl->keyIndex++;
				}
			}
			else
			{
				resolved = a3true;
			}
		}

		// If clip is moving backwards
		else // (clipCtrl->playbackDirection < 0)
		{
			// Case: Reverse Skip
			if (clipCtrl->keyTime < 0)
			{
				// Case: Reverse Terminus
				// This need to set the proper keyIndex otherwise we will get errors later
				if (clipCtrl->clipTime < 0)
				{
					a3f64 leftOverTime = -clipCtrl->clipTime;
					a3ret ret = clipCtrl->reverseTerminus(clipCtrl, leftOverTime);
					//if (ret < 0) return ret; // Make sure that call did not fail
					resolved = clipCtrl->playbackDirection == 0;
				}
				else
				{
					clipCtrl->keyIndex--;
					clipCtrl->keyTime += getCurrentKeyframe(clipCtrl)->duration;
				}
			}
			else
			{
				resolved = a3true;
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

inline a3ret forwardLoop(a3_ClipController* clipCtrl, a3f64 leftOverTime)
{
	// Loop back to start
	clipCtrl->keyIndex = getCurrentClip(clipCtrl)->firstKeyIndex;
	clipCtrl->clipTime = leftOverTime; // Don't lose the extra time
	clipCtrl->keyTime = leftOverTime;

	return 0;
}

inline a3ret reverseLoop(a3_ClipController* clipCtrl, a3f64 leftOverTime)
{
	// Loop back to end
	clipCtrl->keyIndex = getCurrentClip(clipCtrl)->lastKeyIndex;
	clipCtrl->clipTime = getCurrentClip(clipCtrl)->duration - leftOverTime;
	clipCtrl->keyTime = getCurrentKeyframe(clipCtrl)->duration - leftOverTime;

	return 0;
}

inline a3ret forwardStop(a3_ClipController* clipCtrl, a3f64 leftOverTime)
{
	clipCtrl->clipTime = getCurrentClip(clipCtrl)->duration;
	clipCtrl->keyTime = getCurrentKeyframe(clipCtrl)->duration;
	clipCtrl->playbackDirection = 0; // Pause

	return 0;
}

inline a3ret reverseStop(a3_ClipController* clipCtrl, a3f64 leftOverTime)
{
	clipCtrl->clipTime = 0;
	clipCtrl->keyTime = 0;
	clipCtrl->playbackDirection = 0; // Pause

	return 0;
}

inline a3ret forwardPingPong(a3_ClipController* clipCtrl, a3f64 leftOverTime)
{
	clipCtrl->clipTime = getCurrentClip(clipCtrl)->duration - leftOverTime;
	clipCtrl->keyTime = getCurrentKeyframe(clipCtrl)->duration - leftOverTime;
	clipCtrl->playbackDirection = -1;

	return 0;
}

inline a3ret reversePingPong(a3_ClipController* clipCtrl, a3f64 leftOverTime)
{
	clipCtrl->clipTime = leftOverTime;
	clipCtrl->keyTime = leftOverTime;
	clipCtrl->playbackDirection = 1;

	return 0;
}

//-----------------------------------------------------------------------------

inline a3ret returnLerp(a3_ClipController* clipCtrl, a3_HierarchyPoseGroup* hposeGroup, const a3_HierarchyPose* pose_out, a3ui32 numNodes)
{
	a3hierarchyPoseLerp(pose_out, hposeGroup->hpose + getCurrentKeyframe(clipCtrl)->data,
		hposeGroup->hpose + getNextKeyframe(clipCtrl)->data, (a3real)(clipCtrl)->keyParameter, numNodes);

	return 0;
}

inline a3_HierarchyPose* a3clipLerp(a3_ClipController* clipCtrl1, a3_ClipController* clipCtrl2, a3_HierarchyPose* pose_out)
{
	// lerp between current node and next one for clipCtrl1

	// lerp between current node and next one for clipCtrl2

	// lerp between the two outputs and return

	return pose_out;
}

inline a3_HierarchyPose* a3clipAdd(a3_ClipController* clipCtrl1, a3_ClipController* clipCtrl2, a3_HierarchyPose* pose_out)
{
	// add current node and next one for clipCtrl1

	// add current node and next one for clipCtrl2

	// add the two outputs and return

	return pose_out;
}

inline a3_HierarchyPose* a3clipScale(a3_ClipController* clipCtrl, a3ui32 scale, a3_HierarchyPose* pose_out)
{
	pose_out->pose->scale.x = scale * clipCtrl->clipPool->clip->keyframes->data;

	return pose_out;
}

//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL
#endif	// __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H