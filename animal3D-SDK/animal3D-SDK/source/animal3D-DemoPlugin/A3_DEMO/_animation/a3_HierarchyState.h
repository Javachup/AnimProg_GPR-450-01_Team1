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
	
	a3_HierarchyState.h
	Hierarchy transformation state.

	Joey Romanowski:
		Created and implemented a3heirarchyPoseLerp(...)
		Created and implemented a3heirarchyPoseSmoothStep(...)
		Created and implemented a3heirarchyPoseNearest(...)
*/

#ifndef __ANIMAL3D_HIERARCHYSTATE_H
#define __ANIMAL3D_HIERARCHYSTATE_H


// A3 hierarchy
#include "a3_Hierarchy.h"

// A3 spatial pose
#include "a3_SpatialPose.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct MOCAPSEGMENT				MOCAPSEGMENT;
typedef struct MOCAPHEADER				MOCAPHEADER;
typedef struct NODE						NODE;
typedef struct a3_HierarchyPose			a3_HierarchyPose;
typedef struct a3_HierarchyPoseGroup	a3_HierarchyPoseGroup;
typedef struct a3_HierarchyState		a3_HierarchyState;
#endif	// __cplusplus
	

//-----------------------------------------------------------------------------

// single pose for a collection of nodes
// makes algorithms easier to keep this as a separate data type
struct a3_HierarchyPose
{
	a3_SpatialPose* spatialPose;
};


// pose group
struct a3_HierarchyPoseGroup
{
	// pointer to hierarchy
	const a3_Hierarchy* hierarchy;

	a3_SpatialPose* pose;
	a3_HierarchyPose* hpose;

	// Ignoreing channels for this lab
	a3_SpatialPoseChannel* channel;
	// Also ingnoreing the Euler order for this lab
	a3_SpatialPoseEulerOrder order;
	// number of hierarchical poses
	a3ui32 hposeCount;
	// spacialPoseCount = poseCount * hierarchyNodeCount
};


// hierarchy state structure
struct a3_HierarchyState
{
	// pointer to hierarcy
	const a3_Hierarchy* hierarchy;
	a3_HierarchyPose localSpace;
	a3_HierarchyPose objectSpace;

};
	

//-----------------------------------------------------------------------------

// initialize pose set given an initialized hierarchy and key pose count
a3i32 a3hierarchyPoseGroupCreate(a3_HierarchyPoseGroup *poseGroup_out, const a3_Hierarchy *hierarchy, const a3ui32 poseCount);

// release pose set
a3i32 a3hierarchyPoseGroupRelease(a3_HierarchyPoseGroup *poseGroup);

// get offset to hierarchy pose in contiguous set
a3i32 a3hierarchyPoseGroupGetPoseOffsetIndex(const a3_HierarchyPoseGroup *poseGroup, const a3ui32 poseIndex);

// get offset to single node pose in contiguous set
a3i32 a3hierarchyPoseGroupGetNodePoseOffsetIndex(const a3_HierarchyPoseGroup *poseGroup, const a3ui32 poseIndex, const a3ui32 nodeIndex);


//-----------------------------------------------------------------------------

// reset full hierarchy pose
a3i32 a3hierarchyPoseReset(const a3_HierarchyPose* pose_inout, const a3ui32 nodeCount);

// concats the translation, rotation, and scale for each spatial pose in the hierarchy pose
a3i32 a3hierarchyPoseConcat(a3_HierarchyPose* keyPose_inout, const a3_HierarchyPose* basePose_in, const a3ui32 nodeCount);

// convert full hierarchy pose to hierarchy transforms
a3i32 a3hierarchyPoseConvert(const a3_HierarchyPose* pose_inout, const a3ui32 nodeCount, const a3_SpatialPoseChannel channel, const a3_SpatialPoseEulerOrder order);

// lerp between keyPose0 and keyPose1 by param [0,1] amount and save to samplePos_out
a3i32 a3hierarchyPoseLerp(a3_HierarchyPose* samplePose_out, const a3_HierarchyPose* keyPose0_in, const a3_HierarchyPose* keyPose1_in, const a3real param, const a3ui32 nodeCount);

// smooth step between keyPose0 and keyPose1 by param [0,1] amount and save to samplePos_out
a3i32 a3hierarchyPoseSmoothStep(a3_HierarchyPose* samplePose_out, const a3_HierarchyPose* keyPose0_in, const a3_HierarchyPose* keyPose1_in, const a3real param, const a3ui32 nodeCount);

// round to the nearest pose based on param [0, 1]
a3i32 a3hierarchyPoseNearest(a3_HierarchyPose* samplePose_out, const a3_HierarchyPose* keyPose0_in, const a3_HierarchyPose* keyPose1_in, const a3real param, const a3ui32 nodeCount);

// copy full hierarchy pose
a3i32 a3hierarchyPoseCopy(const a3_HierarchyPose* pose_out, const a3_HierarchyPose* pose_in, const a3ui32 nodeCount);


//-----------------------------------------------------------------------------

// initialize hierarchy state given an initialized hierarchy
a3i32 a3hierarchyStateCreate(a3_HierarchyState *state_out, const a3_Hierarchy *hierarchy);

// release hierarchy state
a3i32 a3hierarchyStateRelease(a3_HierarchyState *state);

// update inverse object-space matrices
a3i32 a3hierarchyStateUpdateObjectInverse(const a3_HierarchyState *state);


//-----------------------------------------------------------------------------

// load HTR file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadHTR(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath);

// load BVH file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadBVH(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath);


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_HierarchyState.inl"


#endif	// !__ANIMAL3D_HIERARCHYSTATE_H