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

// complete motion capture skeleton
struct MOCAPSEGMENT
{
	char* name;				// name of motion capture file
	NODE* root;				// pointer to the root node
	MOCAPHEADER* header;	// pointer to a struct w/ global params
	NODE** nodeList;		// array of pointers to skeletal nodes
};

// holds global information about the animation
struct MOCAPHEADER
{
	a3ui32 numOfSegments;	// num of body segments
	a3ui64 numOfFrames;		// num of frames
	a3ui32 dataRate;		// num of fps
	a3ui32 euler[3][3];		// euler angle definition
	a3real callib;			// scale factor -> translating units to meteres
	a3boolean degrees;		// rotational measurements in degrees
	a3real scaleFactor;		// scale factor
	a3ui64 currentFrame;	// stores the current frame to render
	a3real floor;			// position of the floor along the y-axis
};

// contains the data of a specific bone in the animation
struct NODE
{
	char* name;	
	a3real length;			// length of segment along y-axis
	a3real offset[3];		// transitional offset w/ respect to parent
	a3real euler[3];		// rotation of base position
	a3real color[3];		// color used when displaying wire frame skeleton
	a3ui32 numOfChildren;	// number of child nodes
	NODE** children;		// array of pointers to child nodes
	NODE* parent;			// back pointer to parent node
	a3real** froset;		// array of offsets for each frame
	a3real** freuler;		// array of angles for each frame
	a3real* scale;			// array of scale factors for each frame
};

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