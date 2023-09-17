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
	
	a3_KeyframeAnimation.c
	Implementation of keyframe animation interfaces.
*/

#include "../a3_KeyframeAnimation.h"

#include <stdlib.h>
#include <string.h>


// macros to help with names
#define A3_CLIP_DEFAULTNAME		("unnamed clip")
#define A3_CLIP_SEARCHNAME		((clipName && *clipName) ? clipName : A3_CLIP_DEFAULTNAME)


//-----------------------------------------------------------------------------

// allocate keyframe pool
a3i32 a3keyframePoolCreate(a3_KeyframePool* keyframePool_out, const a3ui32 count)
{
	keyframePool_out->count = count;

	keyframePool_out->keyframe = (a3_Keyframe*)malloc(sizeof(a3_Keyframe) * count);//allocating memory size

	if (!(keyframePool_out->keyframe))
	{
		return -1;
	}

	//default values for keyframe variables
	for (a3ui32 i = 0; i < count; i++)
	{
		(keyframePool_out->keyframe + i)->index = i;
		(keyframePool_out->keyframe + i)->duration = 1.0;
		(keyframePool_out->keyframe + i)->invDuration = 1.0;
		(keyframePool_out->keyframe + i)->data = 0;
	}
	return 0;
}

// release keyframe pool
a3i32 a3keyframePoolRelease(a3_KeyframePool* keyframePool)
{
	if (!keyframePool)
	{
		return -1;
	}

	free(keyframePool->keyframe); //C version of freeing memory

	return 0;
}

// initialize keyframe
a3i32 a3keyframeInit(a3_Keyframe* keyframe_out, const a3real duration, const a3ui32 value_x)
{
	keyframe_out->duration = duration;
	keyframe_out->invDuration = (1 / duration);
	keyframe_out->data = value_x;

	return 0;
}

// allocate clip pool
a3i32 a3clipPoolCreate(a3_ClipPool* clipPool_out, const a3ui32 count)
{
	clipPool_out->clip = (a3_Clip*)malloc(sizeof(a3_Clip) * count);//allocating memory size
	// Check if allocation worked
	if (clipPool_out->clip == NULL)
		return -1;

	clipPool_out->count = count; 

	//default values for all clip variables
	for (a3ui32 i = 0; i < clipPool_out->count; i++)
	{
		(clipPool_out->clip + i)->duration = 0.0;
		(clipPool_out->clip + i)->invDuration = 0.0;
		(clipPool_out->clip + i)->keyCount = 0;
		(clipPool_out->clip + i)->firstKeyIndex = 0;
		(clipPool_out->clip + i)->lastKeyIndex = 0;
		(clipPool_out->clip + i)->keyframes = NULL;
		(clipPool_out->clip + i)->index = i;
	}

	return 0;
}

// release clip pool
a3i32 a3clipPoolRelease(a3_ClipPool* clipPool)
{
	if (clipPool == NULL)
		return -1;

	free(clipPool->clip);

	return 0;
}

// initialize clip with first and last indices
a3i32 a3clipInit(a3_Clip* clip_out, const a3byte clipName[a3keyframeAnimation_nameLenMax], const a3_KeyframePool* keyframePool, const a3ui32 firstKeyframeIndex, const a3ui32 finalKeyframeIndex)
{
	// Error Checking
	// Null ptr
	if (keyframePool == NULL)
		return -1;

	// Uninitialized pool
	if (keyframePool->keyframe == NULL)
		return -1;

	// Out of bounds
	if (firstKeyframeIndex < 0 ||
		finalKeyframeIndex >= keyframePool->count)
		return -1;

	memcpy(clip_out->name, clipName, a3keyframeAnimation_nameLenMax); //https://www.geeksforgeeks.org/different-ways-to-copy-a-string-in-c-c/#
	clip_out->keyframes = keyframePool->keyframe;
	clip_out->firstKeyIndex = firstKeyframeIndex;
	clip_out->lastKeyIndex = finalKeyframeIndex;
	clip_out->keyCount = finalKeyframeIndex - firstKeyframeIndex + 1;

	return 0;
}

// get clip index from pool
a3i32 a3clipGetIndexInPool(const a3_ClipPool* clipPool, const a3byte clipName[a3keyframeAnimation_nameLenMax])
{
	for (a3ui32 i = 0; i < clipPool->count; i++)
	{
		a3_ClipPool temp = clipPool[i];

		if (temp.clip->name == clipName)
		{
			return temp.clip->index;
		}
	}

	return -1;
}
//-----------------------------------------------------------------------------
