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
	
	a3_HierarchyStateBlend.h
	Hierarchy blend operations.
*/

#ifndef __ANIMAL3D_HIERARCHYSTATEBLEND_H
#define __ANIMAL3D_HIERARCHYSTATEBLEND_H


#include "a3_HierarchyState.h"
#include "a3_Kinematics.h"


#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus

#endif	// __cplusplus

//-----------------------------------------------------------------------------

typedef a3_Hierarchy    a3_BlendTree;
typedef a3_HierarchyPose    a3_BlendData;
typedef a3real            a3_BlendParam;
typedef a3ui32            a3_BlendNumNodes;

// bilinear
enum {
	a3blend_data_max = 4,
	a3blend_param_max = 3,
};

typedef struct a3_BlendNode
{
	a3_BlendData result; //p_out
	a3_BlendData const* data[a3blend_data_max];    // array of pose pointers
	a3_BlendParam const* param[a3blend_param_max];
	a3_BlendNumNodes numNodes;

	//a3_HierarchyPose* pose_out, a3ui32 numNodes, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3real const u
} a3_BlendNode;

// can be called to perform a blend operation
typedef a3boolean(*a3_BlendOp)(a3_BlendNode* node);

inline a3boolean a3_BlendOpLerp(a3_BlendNode* const node_lerp);

//-----------------------------------------------------------------------------

// pointer-based reset/identity operation for single spatial pose
a3_SpatialPose* a3spatialPoseOpIdentity(a3_SpatialPose* pose_out);

// pointer-based LERP operation for single spatial pose
inline a3_SpatialPose* a3spatialPoseOpLERP(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3real const u);
inline a3_SpatialPose* a3spatialPoseOPConstruct(a3_SpatialPose* pose_out, a3vec4 angles, a3vec4 scale, a3vec4 translation);
inline a3_SpatialPose* a3spatialPoseOPInvert(a3_SpatialPose* pose_out, a3_SpatialPose const* pose_in);
inline a3_SpatialPose* a3spatialPoseOPCubic(a3_SpatialPose* pose_out, a3_SpatialPose const* pose_b, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3_SpatialPose const* pose_a, a3real const t);
inline a3_SpatialPose* a3spatialPoseOpNearest(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3real const u);
inline a3_SpatialPose* a3spatialPoseOPSmoothStep(a3_SpatialPose* pose_out, a3_SpatialPose const pose0, a3_SpatialPose const pose1, a3real const u);
inline a3_SpatialPose* a3spatialPoseOPDescale(a3_SpatialPose* pose_out);

//Helper Functions
inline a3_SpatialPose* a3_cubicMultiplicative(a3_SpatialPose* pose_out, a3_SpatialPose const* pose_b, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3_SpatialPose const* pose_a, a3real const t);
//-----------------------------------------------------------------------------

// data-based reset/identity
a3_SpatialPose a3spatialPoseDOpIdentity();

// data-based LERP
a3_SpatialPose a3spatialPoseDOpLERP(a3_SpatialPose const pose0, a3_SpatialPose const pose1, a3real const u);


//-----------------------------------------------------------------------------

inline a3boolean a3_BlendOpConcat(a3_BlendNode* const node_concat);
inline a3boolean a3_BlendOpLerp(a3_BlendNode* const node_lerp);
inline a3boolean a3_BlendOpScale(a3_BlendNode* const node_scale);

/*____________________Hierarchy POSE FUNCTIONS - POINTER BASED____________________*/
//-----------------------------------------------------------------------------
// pointer-based reset/identity operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpIdentity(a3_HierarchyPose* pose_out, a3ui32 numNodes);
inline a3_HierarchyPose* a3hierarchyPoseOpConstruct(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3vec4 angles, a3vec4 scale, a3vec4 translation);
inline a3_HierarchyPose* a3hierarchyPoseOpCopy(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3_HierarchyPose const* pose_in);
inline a3_HierarchyPose* a3hierarchyPoseOpInvert(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3_HierarchyPose const* pose_in);
inline a3_HierarchyPose* a3hierarchyPoseOpConcat(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1);
inline a3_HierarchyPose* a3hierarchyPoseOpNearest(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3real const u);

// pointer-based LERP operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpLERP(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3real const u);
inline a3_HierarchyPose* a3hierarchyPoseOpCubic(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3_HierarchyPose const* pose_b, a3_HierarchyPose const* pose0,
	a3_HierarchyPose const* pose1, a3_HierarchyPose const* pose_a, a3real const t);
inline a3_HierarchyPose* a3hierarchyPoseOpSplit(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3_HierarchyPose const* poseL, a3_HierarchyPose const* poseR);
inline a3_HierarchyPose* a3hierarchyPoseOpScale(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3_HierarchyPose* pose_in, a3real const u);
inline a3_HierarchyPose* a3hierarchyPoseOpTriangular(a3_HierarchyPose* pose_out, a3ui32 numNodes,
	a3_HierarchyPose* pose0, a3_HierarchyPose* pose1, a3_HierarchyPose* pose2, a3real const u1, a3real const u2);
inline a3_HierarchyPose* a3hierarchyPoseOpBiNearest(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3_HierarchyPose const* pose0_0,
	a3_HierarchyPose const* pose0_1, a3_HierarchyPose const* pose1_0, a3_HierarchyPose const* pose1_1, a3real const u0, a3real const u1, a3real const u);
inline a3_HierarchyPose* a3hierarchyPoseOpBiLinear(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3_HierarchyPose const* pose0_0, a3_HierarchyPose const* pose0_1,
	a3_HierarchyPose const* pose1_0, a3_HierarchyPose const* pose1_1, a3real const u0, a3real const u1, a3real const u);
inline a3_HierarchyPose* a3hierarchyPoseOpBiCubic(a3_HierarchyPose* pose_out, a3ui32 numNodes,
	a3_HierarchyPose const* poseN1_n1, a3_HierarchyPose const* poseN1_0, a3_HierarchyPose const* poseN1_1, a3_HierarchyPose const* poseN1_2,
	a3_HierarchyPose const* pose0_n1, a3_HierarchyPose const* pose0_0, a3_HierarchyPose const* pose0_1, a3_HierarchyPose const* pose0_2,
	a3_HierarchyPose const* pose1_n1, a3_HierarchyPose const* pose1_0, a3_HierarchyPose const* pose1_1, a3_HierarchyPose const* pose1_2,
	a3_HierarchyPose const* pose2_n1, a3_HierarchyPose const* pose2_0, a3_HierarchyPose const* pose2_1, a3_HierarchyPose const* pose2_2,
	a3real const uN1, a3real const u0, a3real const u1, a3real const u2, a3real const u);

//Project3 Add On
inline a3_HierarchyPose* a3hierarchyPoseOpSmoothStep(a3_HierarchyPose* pose_out);
inline a3_HierarchyPose* a3hierarchyPoseOpDescale(a3_HierarchyPose* pose_out);
inline a3_HierarchyPose* a3hierarchyPoseOpInverseKinematics(a3_HierarchyPose* pose_out);

/*_______________SPATIAL POSE FUNCTIONS - POINTER BASED____________________*/
//-----------------------------------------------------------------------------
// pointer-based reset/identity operation for single spatial pose
a3_SpatialPose* a3spatialPoseOpIdentity(a3_SpatialPose* pose_out);

// pointer-based LERP operation for single spatial pose
inline a3_SpatialPose* a3spatialPoseOpLERP(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3real const u);
inline a3_SpatialPose* a3spatialPoseOPConstruct(a3_SpatialPose* pose_out, a3vec4 angles, a3vec4 scale, a3vec4 translation);
inline a3_SpatialPose* a3spatialPoseOPInvert(a3_SpatialPose* pose_out, a3_SpatialPose const* pose_in);
inline a3_SpatialPose* a3spatialPoseOPCubic(a3_SpatialPose* pose_out, a3_SpatialPose const* pose_b, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3_SpatialPose const* pose_a, a3real const t);
inline a3_SpatialPose* a3spatialPoseOpNearest(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3real const u);
inline a3_SpatialPose* a3spatialPoseOPSmoothStep(a3_SpatialPose* pose_out, a3_SpatialPose const pose0, a3_SpatialPose const pose1, a3real const u);
inline a3_SpatialPose* a3spatialPoseOPDescale(a3_SpatialPose* pose_out);

//Helper Functions
inline a3_SpatialPose* a3_cubicMultiplicative(a3_SpatialPose* pose_out, a3_SpatialPose const* pose_b, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3_SpatialPose const* pose_a, a3real const t);

/*__________DERIVATIVE BLEND OPERATIONS - POINTER BASED__________*/
//-----------------------------------------------------------------------------

// calculates the difference between the two control poses
inline a3_SpatialPose* a3spatialPoseOpSplit(a3_SpatialPose* pose_out, a3_SpatialPose const* poseL, a3_SpatialPose const* poseR);
// calculates the "scaled" pose, which is some blend between the ID pose and ctrl pose
inline a3_SpatialPose* a3spatialPoseOpScale(a3_SpatialPose* pose_out, a3_SpatialPose* pose_in, a3real const u);
// triangular interpolations for poses
inline a3_SpatialPose* a3spatialPoseOpTriangular(a3_SpatialPose* pose_out, a3_SpatialPose* pose0, a3_SpatialPose* pose1, a3_SpatialPose* pose2, a3real const u1, a3real const u2);
// binearest interpolation function for poses
inline a3_SpatialPose* a3spatialPoseOpBiNearest(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0_0, a3_SpatialPose const* pose0_1,
	a3_SpatialPose const* pose1_0, a3_SpatialPose const* pose1_1, a3real const u0, a3real const u1, a3real const u);
// bilinear interpolation function for poses
inline a3_SpatialPose* a3spatialPoseOpBiLinear(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0_0, a3_SpatialPose const* pose0_1,
	a3_SpatialPose const* pose1_0, a3_SpatialPose const* pose1_1, a3real const u0, a3real const u1, a3real const u);
// bicubic interpolation function for poses
inline a3_SpatialPose* a3spatialPoseOpBiCubic(a3_SpatialPose* pose_out,
	a3_SpatialPose const* poseN1_n1, a3_SpatialPose const* poseN1_0, a3_SpatialPose const* poseN1_1, a3_SpatialPose const* poseN1_2,
	a3_SpatialPose const* pose0_n1, a3_SpatialPose const* pose0_0, a3_SpatialPose const* pose0_1, a3_SpatialPose const* pose0_2,
	a3_SpatialPose const* pose1_n1, a3_SpatialPose const* pose1_0, a3_SpatialPose const* pose1_1, a3_SpatialPose const* pose1_2,
	a3_SpatialPose const* pose2_n1, a3_SpatialPose const* pose2_0, a3_SpatialPose const* pose2_1, a3_SpatialPose const* pose2_2,
	a3real const uN1, a3real const u0, a3real const u1, a3real const u2, a3real const u);
//-----------------------------------------------------------------------------
//// data-based reset/identity
//a3_SpatialPose a3spatialPoseDOpIdentity();
//
//// data-based LERP
//a3_SpatialPose a3spatialPoseDOpLERP(a3_SpatialPose const pose0, a3_SpatialPose const pose1, a3real const u);

/*_______________FUNCTIONS TO NODE____________________*/
//-----------------------------------------------------------------------------
//Wrap up convert in a node
a3mat4* a3matrixOpFK(a3mat4* object_out, a3mat4 const* local_in,
	a3_HierarchyNode const* hierarchyNodes, a3ui32 const numNodes);

a3mat4* a3matrixOpIK(a3mat4* local_out, a3mat4 const* object_in,
	a3_HierarchyNode const* hierarchyNodes, a3ui32 const numNodes);



#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_HierarchyStateBlend.inl"


#endif	// !__ANIMAL3D_HIERARCHYSTATEBLEND_H