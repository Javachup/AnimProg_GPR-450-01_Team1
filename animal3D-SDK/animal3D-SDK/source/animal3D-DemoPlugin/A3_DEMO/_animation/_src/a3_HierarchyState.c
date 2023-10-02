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
	
	a3_HierarchyState.c
	Implementation of transform hierarchy state.
*/

#include "../a3_HierarchyState.h"

#include <stdlib.h>
#include <string.h>


//-----------------------------------------------------------------------------

// initialize pose set given an initialized hierarchy and key pose count
a3i32 a3hierarchyPoseGroupCreate(a3_HierarchyPoseGroup *poseGroup_out, const a3_Hierarchy *hierarchy, const a3ui32 poseCount)
{
	// validate params and initialization states
	//	(output is not yet initialized, hierarchy is initialized)
	if (poseGroup_out && hierarchy && !poseGroup_out->hierarchy && hierarchy->nodes)
	{
		// determine memory requirements
		// The raw data is the spactial poses that is wrapped in a hierarchy pose
		const size_t size = sizeof(a3_HierarchyPose) * poseCount +
			sizeof(a3_SpatialPose) * poseCount * hierarchy->numNodes;
		// allocate everything (one malloc)
		void* mem = malloc(size);

		// set pointers
		poseGroup_out->hierarchy = hierarchy;
		// Staring address for heirarchical poses
		a3_HierarchyPose* hposeBase = (a3_HierarchyPose*)mem;
		// Starting address for spatial poses
		a3_SpatialPose* poseBase = (a3_SpatialPose*)(hposeBase + poseCount);
		// Link the 2 together: each hpose get numNodes number of spatial poses
		poseGroup_out->hpose = hposeBase;

		// For each hpose, set the spatialPose pointer
		for (a3ui32 i = 0; i < poseCount; i++)
		{
			// Cast to non-const for the initialization
			// Start at poseBase and add the size of every hierarchy pose before it (i + numNodes) to end up at that hpose's spatial pose memeory
			//(a3_HierarchyPose*)(poseGroup_out->hpose[i]).spatialPose = poseBase + i * hierarchy->numNodes;
			(a3_SpatialPose*)((poseGroup_out->hpose[i]).spatialPose) = poseBase + i * hierarchy->numNodes;

		}

		// reset all data
		// For every spatialPose, reset it
		// number of spatial poses = numPoses * numNodes
		for (a3ui32 i = 0; i < poseCount * hierarchy->numNodes; i++)
		{
			a3spatialPoseReset(poseBase + i);
		}

		// done
		return 1;
	}
	return -1;
}

// release pose set
a3i32 a3hierarchyPoseGroupRelease(a3_HierarchyPoseGroup *poseGroup)
{
	// validate param exists and is initialized
	if (poseGroup && poseGroup->hierarchy)
	{
		// release everything (one free)
		//free(???);

		// reset pointers
		poseGroup->hierarchy = 0;

		// done
		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// initialize hierarchy state given an initialized hierarchy
a3i32 a3hierarchyStateCreate(a3_HierarchyState *state_out, const a3_Hierarchy *hierarchy)
{
	// validate params and initialization states
	//	(output is not yet initialized, hierarchy is initialized)
	if (state_out && hierarchy && !state_out->hierarchy && hierarchy->nodes)
	{
		// determine memory requirements

		// allocate everything (one malloc)
		//??? = (...)malloc(sz);

		// set pointers
		state_out->hierarchy = hierarchy;

		// reset all data

		// done
		return 1;
	}
	return -1;
}

// release hierarchy state
a3i32 a3hierarchyStateRelease(a3_HierarchyState *state)
{
	// validate param exists and is initialized
	if (state && state->hierarchy)
	{
		// release everything (one free)
		//free(???);

		// reset pointers
		state->hierarchy = 0;

		// done
		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// load HTR file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadHTR(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath)
{
	if (poseGroup_out && !poseGroup_out->hposeCount && hierarchy_out && !hierarchy_out->numNodes && resourceFilePath && *resourceFilePath)
	{

	}
	return -1;
}

// load BVH file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadBVH(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath)
{
	if (poseGroup_out && !poseGroup_out->hposeCount && hierarchy_out && !hierarchy_out->numNodes && resourceFilePath && *resourceFilePath)
	{

	}
	return -1;
}


//-----------------------------------------------------------------------------
