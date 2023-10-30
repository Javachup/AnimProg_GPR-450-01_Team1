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
	
	a3_HierarchyStateBlend.inl
	Implementation of inline hierarchical blend operations.
*/


#ifdef __ANIMAL3D_HIERARCHYSTATEBLEND_H
#ifndef __ANIMAL3D_HIERARCHYSTATEBLEND_INL
#define __ANIMAL3D_HIERARCHYSTATEBLEND_INL
#include "math.h"

//BlendOps
//-----------------------------------------------------------------------------
//a3_BlendData* a3_BlendFuncLerp(a3_BlendData* const data_out, a3_BlendData const* const data0,
//	a3_BlendData const* const data1, a3_BlendParam const param)
//{
//	if (!data_out || !data0 || !data1)
//	{
//		return 0;
//	}
//
//	//a3spatialPoseOpLERP(data_out, data0, data1, param);
//
//	return data_out;
//}

inline a3boolean a3_BlendOpLerp(a3_BlendNode* const node_lerp)
{
	if (!node_lerp)
	{
		return false;
	}

	a3_BlendData* const data_out = &(node_lerp->result);
	a3_BlendData const* const data0 = node_lerp->data[0];
	a3_BlendData const* const data1 = node_lerp->data[1];
	a3_BlendParam const param = *(node_lerp->param[0]);

	a3_BlendData const* const result = a3spatialPoseOpLERP(data_out, data0, data1, param);
	return (result == data_out);
}

inline a3boolean a3_BlendOpConcat(a3_BlendNode* const node_concat)
{
	if (!node_concat)
	{
		return false;
	}

	a3_BlendData* const data_out = &(node_concat->result);
	a3_BlendData const* const data0 = node_concat->data[0];
	a3_BlendData const* const data1 = node_concat->data[1];
	a3_BlendNumNodes const numNodes = node_concat->numNodes;

	a3_BlendData const* const result = a3hierarchyPoseOpConcat(data_out, numNodes, data0, data1);

	return (result == data_out);
}

inline a3boolean a3_BlendOpLerp(a3_BlendNode* const node_lerp)
{
	if (!node_lerp)
	{
		return false;
	}

	a3_BlendData* const data_out = &(node_lerp->result); 
	a3_BlendData const* const data0 = node_lerp->data[0];
	a3_BlendData const* const data1 = node_lerp->data[1];
	a3_BlendParam const param = *(node_lerp->param[0]);
	a3_BlendNumNodes const numNodes = node_lerp->numNodes;

	a3_BlendData const* const result = a3hierarchyPoseOpLERP(data_out, numNodes, data0, data1, param);

	return (result == data_out);
}

inline a3boolean a3_BlendOpScale(a3_BlendNode* const node_scale)
{
	if (!node_scale)
	{
		return false;
	}

	a3_BlendData* const data_out = &(node_scale->result);
	a3_BlendData* const data_in = &(node_scale->result);
	a3_BlendData const* const data0 = node_scale->data[0];
	a3_BlendParam const param = *(node_scale->param[0]);
	a3_BlendNumNodes const numNodes = node_scale->numNodes;

	a3_BlendData const* const result = a3hierarchyPoseOpScale(data_out, numNodes, data_in,param);

	return (result == data_out);
}
//-----------------------------------------------------------------------------

// pointer-based reset/identity operation for single spatial pose
inline a3_SpatialPose* a3spatialPoseOpIdentity(a3_SpatialPose* pose_out)
{
	// set transform to identity matrix
	pose_out->transform = a3mat4_identity;

	// set translation, rotation, scale
	pose_out->translation.xyz = (a3vec3){ 0.0, 0.0, 0.0 };
	pose_out->angles.xyz = (a3vec3){ 0.0, 0.0, 0.0 };
	pose_out->scale.xyz = (a3vec3){ 1.0, 1.0, 1.0 };

	// done
	return pose_out;
}

//Controls (3): vectors representing rotation angles, scale and translation.
inline a3_SpatialPose* a3spatialPoseOPConstruct(a3_SpatialPose* pose_out, a3vec4 angles, a3vec4 scale, a3vec4 translation)
{
	//vectors representing rotation angles, scale and translation.
	pose_out->angles.x = angles.x;
	pose_out->angles.y = angles.y;
	pose_out->angles.z = angles.z;

	pose_out->translation.x = translation.x;
	pose_out->translation.y = translation.y;
	pose_out->translation.z = translation.z;

	pose_out->scale.x = scale.x;
	pose_out->scale.y = scale.y;
	pose_out->scale.z = scale.z;

	return pose_out;

}

//Cubic Multiplicative
inline a3_SpatialPose* a3_cubicMultiplicative(a3_SpatialPose* pose_out, a3_SpatialPose const* pose_b, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3_SpatialPose const* pose_a, a3real const t)
{
	a3real tp2, tp3, tpinv, tp2inv, tp3inv;

	tpinv = powf(t, -1);
	tp2 = powf(t, 2);
	tp2inv = powf(tp2, -1);
	tp3 = powf(t, 3);
	tp3inv = powf(tp3, -1);
	

	/*
	pose_out->scale.x = a3CatmullRom(pose_b->scale.x, pose0->scale.x, pose1->scale.x, pose_a->scale.x, t);
	pose_out->scale.y = a3CatmullRom(pose_b->scale.y, pose0->scale.y, pose1->scale.y, pose_a->scale.y, t);
	pose_out->scale.z = a3CatmullRom(pose_b->scale.z, pose0->scale.z, pose1->scale.z, pose_a->scale.z, t);
	*/
	return pose_out;
}

//Controls (4): pre-initial = pose_b, initial, terminal and post-terminal = pose_a.
inline a3_SpatialPose* a3spatialPoseOPCubic(a3_SpatialPose* pose_out, a3_SpatialPose const* pose_b, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3_SpatialPose const* pose_a, a3real const t)
{
	// FUNCTION IS ADDITIVE
	pose_out->angles.x = a3CatmullRom(pose_b->angles.x, pose0->angles.x, pose1->angles.x, pose_a->angles.x, t);
	pose_out->angles.y = a3CatmullRom(pose_b->angles.y, pose0->angles.y, pose1->angles.y, pose_a->angles.y, t);
	pose_out->angles.z = a3CatmullRom(pose_b->angles.z, pose0->angles.z, pose1->angles.z, pose_a->angles.z, t);

	pose_out->translation.x = a3CatmullRom(pose_b->translation.x, pose0->translation.x, pose1->translation.x, pose_a->translation.x, t);
	pose_out->translation.y = a3CatmullRom(pose_b->translation.y, pose0->translation.y, pose1->translation.y, pose_a->translation.y, t);
	pose_out->translation.z = a3CatmullRom(pose_b->translation.z, pose0->translation.z, pose1->translation.z, pose_a->translation.z, t);

	// MAKE MULTIPLICATIVE
	pose_out->scale.x = a3CatmullRom(pose_b->scale.x, pose0->scale.x, pose1->scale.x, pose_a->scale.x, t);
	pose_out->scale.y = a3CatmullRom(pose_b->scale.y, pose0->scale.y, pose1->scale.y, pose_a->scale.y, t);
	pose_out->scale.z = a3CatmullRom(pose_b->scale.z, pose0->scale.z, pose1->scale.z, pose_a->scale.z, t);

	return pose_out;
}

// pointer-based LERP operation for single spatial pose
inline a3_SpatialPose* a3spatialPoseOpLERP(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3real const u)
{
	a3real3Lerp(pose_out->angles.v, pose0->angles.v, pose1->angles.v, u);

	// LOGLERP FOR SCALE
	a3real3LerpScaleMult(pose_out, pose0, pose1, u);

	a3real3Lerp(pose_out->translation.v, pose0->translation.v, pose1->translation.v, u);

	return pose_out;
}

//Controls (1): spatial pose.
inline a3_SpatialPose* a3spatialPoseOPInvert(a3_SpatialPose* pose_out, a3_SpatialPose const* pose_in)
{
	pose_out->angles.x = -(pose_in->angles.x);
	pose_out->angles.y = -(pose_in->angles.y);
	pose_out->angles.z = -(pose_in->angles.z);

	pose_out->translation.x = -(pose_in->translation.x);
	pose_out->translation.y = -(pose_in->translation.y);
	pose_out->translation.z = -(pose_in->translation.z);

	pose_out->scale.x = 1 / pose_in->scale.x;
	pose_out->scale.y = 1 / pose_in->scale.y;
	pose_out->scale.z = 1 / pose_in->scale.z;

	return pose_out;
}

// pointer-based Nearest operation for a single spatial pose
inline a3_SpatialPose* a3spatialPoseOpNearest(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3real const u)
{
	if (u < 0.5f)
	{
		a3spatialPoseOPCopy(pose_out, pose0);
	}
	else
	{
		a3spatialPoseOPCopy(pose_out, pose1);
	}

	// done
	return pose_out;
}

//Project3 Add On
inline a3_SpatialPose* a3spatialPoseOPSmoothStep(a3_SpatialPose* pose_out, a3_SpatialPose const pose0, a3_SpatialPose const pose1, a3real const u)
{

	return pose_out;
}
inline a3_SpatialPose* a3spatialPoseOPDescale(a3_SpatialPose* pose_out)
{

	return pose_out;
}



//-----------------------------------------------------------------------------

// data-based reset/identity
//inline a3_SpatialPose a3spatialPoseDOpIdentity()
//{
//	a3_SpatialPose const result = { a3mat4_identity /*, ...*/ };
//	return result;
//}
//
//// data-based LERP
//inline a3_SpatialPose a3spatialPoseDOpLERP(a3_SpatialPose const pose0, a3_SpatialPose const pose1, a3real const u)
//{
//	a3_SpatialPose result = { 0 };
//	// ...
//
//	// done
//	return result;
//}


//-----------------------------------------------------------------------------

// pointer-based reset/identity operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpIdentity(a3_HierarchyPose* pose_out, a3ui32 numNodes)
{
	for (a3ui32 i = 0; i < numNodes; ++i)
	{
		a3spatialPoseOpIdentity(pose_out->pose + i);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpConstruct(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3vec4 angles, a3vec4 scale, a3vec4 translation)
{
	for (a3ui32 i = 0; i < numNodes; ++i)
	{
		a3spatialPoseOPConstruct(pose_out->pose + i, pose_out->pose[i].angles, pose_out->pose[i].scale, pose_out->pose[i].translation);
	}

	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpCopy(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3_HierarchyPose const* pose_in)
{
	for (a3ui32 i = 0; i < numNodes; ++i)
	{
		a3spatialPoseOPCopy(pose_out->pose + i, pose_in->pose + i);
	}

	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpInvert(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3_HierarchyPose const* pose_in)
{
	for (a3ui32 i = 0; i < numNodes; ++i)
	{
		a3spatialPoseOPInvert(pose_out->pose + i, pose_in->pose + i);
	}

	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpConcat(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1)
{
	for (a3ui32 i = 0; i < numNodes; ++i)
	{
		a3spatialPoseConcat(pose_out->pose + i, pose0->pose + i, pose1->pose + i);
	}

	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpNearest(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3real const u)
{
	for (a3ui32 i = 0; i < numNodes; ++i)
	{
		a3spatialPoseOpNearest(pose_out->pose + i, pose0->pose + i, pose1->pose + i, u);
	}

	return pose_out;
}

// pointer-based LERP operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpLERP(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3real const u)
{
	for (a3ui32 i = 0; i < numNodes; ++i)
	{
		a3spatialPoseOpLERP(pose_out->pose + i, pose0->pose + i, pose1->pose + i, u);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpCubic(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3_HierarchyPose const* pose_b, a3_HierarchyPose const* pose0,
	a3_HierarchyPose const* pose1, a3_HierarchyPose const* pose_a, a3real const t)
{
	for (a3ui32 i = 0; i < numNodes; ++i)
	{
		a3spatialPoseOPCubic(pose_out->pose + i, pose_b->pose + i, pose0->pose + i, pose1->pose + i, pose_a->pose + i, t);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpSplit(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3_HierarchyPose const* poseL, a3_HierarchyPose const* poseR)
{
	for (a3ui32 i = 0; i < numNodes; ++i)
	{
		a3spatialPoseOpSplit(pose_out->pose + i, poseL->pose + i, poseR->pose + i);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpScale(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3_HierarchyPose* pose_in, a3real const u)
{
	for (a3ui32 i = 0; i < numNodes; ++i)
	{
		a3spatialPoseOpScale(pose_out->pose + i, pose_in->pose + i, u);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpTriangular(a3_HierarchyPose* pose_out, a3ui32 numNodes,
	a3_HierarchyPose* pose0, a3_HierarchyPose* pose1, a3_HierarchyPose* pose2, a3real const u1, a3real const u2)
{
	for (a3ui32 i = 0; i < numNodes; ++i)
	{
		a3spatialPoseOpTriangular(pose_out->pose + i, pose0->pose + i, pose1->pose + i, pose2->pose + i, u1, u2);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpBiNearest(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3_HierarchyPose const* pose0_0,
	a3_HierarchyPose const* pose0_1, a3_HierarchyPose const* pose1_0, a3_HierarchyPose const* pose1_1, a3real const u0, a3real const u1, a3real const u)
{
	for (a3ui32 i = 0; i < numNodes; ++i)
	{
		a3spatialPoseOpBiNearest(pose_out->pose + i, pose0_0->pose + i, pose0_1->pose + i,
			pose1_0->pose + i, pose1_1->pose + i, u0, u1, u);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpBiLinear(a3_HierarchyPose* pose_out, a3ui32 numNodes, a3_HierarchyPose const* pose0_0, a3_HierarchyPose const* pose0_1,
	a3_HierarchyPose const* pose1_0, a3_HierarchyPose const* pose1_1, a3real const u0, a3real const u1, a3real const u)
{
	for (a3ui32 i = 0; i < numNodes; ++i)
	{
		a3spatialPoseOpBiLinear(pose_out->pose + i, pose0_0->pose + i, pose0_1->pose + i,
			pose1_0->pose + i, pose1_1->pose + i, u0, u1, u);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpBiCubic(a3_HierarchyPose* pose_out, a3ui32 numNodes,
	a3_HierarchyPose const* poseN1_n1, a3_HierarchyPose const* poseN1_0, a3_HierarchyPose const* poseN1_1, a3_HierarchyPose const* poseN1_2,
	a3_HierarchyPose const* pose0_n1, a3_HierarchyPose const* pose0_0, a3_HierarchyPose const* pose0_1, a3_HierarchyPose const* pose0_2,
	a3_HierarchyPose const* pose1_n1, a3_HierarchyPose const* pose1_0, a3_HierarchyPose const* pose1_1, a3_HierarchyPose const* pose1_2,
	a3_HierarchyPose const* pose2_n1, a3_HierarchyPose const* pose2_0, a3_HierarchyPose const* pose2_1, a3_HierarchyPose const* pose2_2,
	a3real const uN1, a3real const u0, a3real const u1, a3real const u2, a3real const u)
{
	for (a3ui32 i = 0; i < numNodes; ++i)
	{
		a3spatialPoseOpBiCubic(pose_out->pose + i,
			poseN1_n1->pose + i, poseN1_0->pose + i, poseN1_1->pose + i, poseN1_2->pose + i,
			pose0_n1->pose + i, pose0_0->pose + i, pose0_1->pose + i, pose0_2->pose + i,
			pose1_n1->pose + i, pose1_0->pose + i, pose1_1->pose + i, pose1_2->pose + i,
			pose2_n1->pose + i, pose2_0->pose + i, pose2_1->pose + i, pose2_2->pose + i,
			uN1, u0, u1, u2, u);
	}
	return pose_out;
}


//Project3 Add On 
inline a3_HierarchyPose* a3hierarchyPoseOpSmoothStep(a3_HierarchyPose* pose_out)
{
	return pose_out;
}
inline a3_HierarchyPose* a3hierarchyPoseOpDescale(a3_HierarchyPose* pose_out)
{
	return pose_out;
}
inline a3_HierarchyPose* a3hierarchyPoseOpInverseKinematics(a3_HierarchyPose* pose_out)
{
	return pose_out;
}


//-----------------------------------------------------------------------------

/*__________DERIVATIVE BLEND OPERATIONS - POINTER BASED__________*/

// calculates the difference between the two control poses
inline a3_SpatialPose* a3spatialPoseOpSplit(a3_SpatialPose* pose_out, a3_SpatialPose const* poseL, a3_SpatialPose const* poseR)
{
	// pose_out = poseL - poseR
	if (pose_out && poseL && poseR)
	{
		pose_out->angles.x = a3trigValid_sind(poseL->angles.x - poseR->angles.x);
		pose_out->angles.y = a3trigValid_sind(poseL->angles.y - poseR->angles.y);
		pose_out->angles.z = a3trigValid_sind(poseL->angles.z - poseR->angles.z);

		pose_out->scale.x = poseL->scale.x / poseR->scale.x;
		pose_out->scale.y = poseL->scale.y / poseR->scale.y;
		pose_out->scale.z = poseL->scale.z / poseR->scale.z;

		pose_out->translation.x = poseL->translation.x - poseR->translation.x;
		pose_out->translation.y = poseL->translation.y - poseR->translation.y;
		pose_out->translation.z = poseL->translation.z - poseR->translation.z;
	}
	return pose_out;
}

// calculates the "scaled" pose, which is some blend between the ID pose and ctrl pose
inline a3_SpatialPose* a3spatialPoseOpScale(a3_SpatialPose* pose_out, a3_SpatialPose* pose_in, a3real const u)
{	

	pose_out->angles.x = pose_in->angles.x * u;
	pose_out->angles.y = pose_in->angles.y * u;
	pose_out->angles.z = pose_in->angles.z * u;

	pose_out->scale.x = powf(pose_in->scale.x, u);
	pose_out->scale.y = powf(pose_in->scale.y, u);
	pose_out->scale.z = powf(pose_in->scale.z, u);

	pose_out->translation.x = pose_in->translation.x * u;
	pose_out->translation.y = pose_in->translation.y * u;
	pose_out->translation.z = pose_in->translation.z * u;

	return pose_out;
}

// triangular interpolations for poses
inline a3_SpatialPose* a3spatialPoseOpTriangular(a3_SpatialPose* pose_out, a3_SpatialPose* pose0, a3_SpatialPose* pose1, a3_SpatialPose* pose2, a3real const u1, a3real const u2)
{
	a3_SpatialPose scaled1, scaled2, scaled3;
	a3spatialPoseOpScale(&scaled1, pose0, (1 - u1 - u2));
	a3spatialPoseOpScale(&scaled2, pose1, u1);
	a3spatialPoseOpScale(&scaled3, pose2, u2);

	a3_SpatialPose temp;

	a3spatialPoseConcat(pose_out, 
		a3spatialPoseConcat(&temp, &scaled1, &scaled2),
		&scaled3);
	return pose_out;
}

// binearest interpolation function for poses
inline a3_SpatialPose* a3spatialPoseOpBiNearest(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0_0, a3_SpatialPose const* pose0_1, 
	a3_SpatialPose const* pose1_0, a3_SpatialPose const* pose1_1, a3real const u0, a3real const u1, a3real const u)
{
	// conditional based on nearest
	a3_SpatialPose nearest1, nearest2;
	a3spatialPoseOpNearest(&nearest1, pose0_0, pose0_1, u0);
	a3spatialPoseOpNearest(&nearest2, pose1_0, pose1_1, u1);

	a3spatialPoseOpNearest(pose_out, &nearest1, &nearest2, u);
	
	return pose_out;
}

// bilinear interpolation function for poses
inline a3_SpatialPose* a3spatialPoseOpBiLinear(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0_0, a3_SpatialPose const* pose0_1,
	a3_SpatialPose const* pose1_0, a3_SpatialPose const* pose1_1, a3real const u0, a3real const u1, a3real const u)
{
	a3_SpatialPose blend0, blend1;
	a3spatialPoseOpLERP(&blend0, pose0_0, pose0_1, u0);
	a3spatialPoseOpLERP(&blend1, pose1_0, pose1_1, u1);

	a3spatialPoseOpLERP(pose_out, &blend0, &blend1, u);
	return pose_out;
}

// bicubic interpolation function for poses
inline a3_SpatialPose* a3spatialPoseOpBiCubic(a3_SpatialPose* pose_out,
	a3_SpatialPose const* poseN1_n1, a3_SpatialPose const* poseN1_0, a3_SpatialPose const* poseN1_1, a3_SpatialPose const* poseN1_2,
	a3_SpatialPose const* pose0_n1, a3_SpatialPose const* pose0_0, a3_SpatialPose const* pose0_1, a3_SpatialPose const* pose0_2, 
	a3_SpatialPose const* pose1_n1, a3_SpatialPose const* pose1_0, a3_SpatialPose const* pose1_1, a3_SpatialPose const* pose1_2, 
	a3_SpatialPose const* pose2_n1, a3_SpatialPose const* pose2_0, a3_SpatialPose const* pose2_1, a3_SpatialPose const* pose2_2,
	a3real const uN1, a3real const u0, a3real const u1, a3real const u2, a3real const u)
{
	a3_SpatialPose cubic1, cubic2, cubic3, cubic4;
	a3spatialPoseOPCubic(&cubic1, poseN1_n1, poseN1_0, poseN1_1, poseN1_2, u);
	a3spatialPoseOPCubic(&cubic2, pose0_n1, pose0_0, pose0_1, pose0_2, u);
	a3spatialPoseOPCubic(&cubic3, pose1_n1, pose1_0, pose1_1, pose1_2, u);
	a3spatialPoseOPCubic(&cubic4, pose2_n1, pose2_0, pose2_1, pose2_2, u);

	a3spatialPoseOPCubic(pose_out, &cubic1, &cubic2, &cubic3, &cubic4, u);
	return pose_out;
}

//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_HIERARCHYSTATEBLEND_INL
#endif	// __ANIMAL3D_HIERARCHYSTATEBLEND_H