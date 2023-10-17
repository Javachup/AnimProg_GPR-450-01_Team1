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


//-----------------------------------------------------------------------------

// pointer-based reset/identity operation for single spatial pose
inline a3_SpatialPose* a3spatialPoseOpIdentity(a3_SpatialPose* pose_out)
{
	pose_out->transform = a3mat4_identity;
	pose_out->translation = a3vec4_zero;
	pose_out->angles = a3vec4_zero;
	pose_out->scale = a3vec4_one;

	// done
	return pose_out;
}

//Controls (3): vectors representing rotation angles, scale and translation.
inline a3_SpatialPose* a3spatialPoseOPConstruct(a3_SpatialPose* pose_out, a3_SpatialPose const* pose_in)
{
	//vectors representing rotation angles, scale and translation.


	return pose_out;

}

//inline a3_SpatialPose* a3spatialPoseOPCopy(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1)
//{
//
//	return pose_out;
//}

//inline a3_SpatialPose* a3spatialPoseOPConcat(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1)
//{
//	return pose_out;
//}

//Controls (4): pre-initial, initial, terminal and post-terminal poses.
inline a3_SpatialPose* a3spatialPoseOPCubic(a3_SpatialPose* pose_out, a3_SpatialPose const* pre, a3_SpatialPose const* init, a3_SpatialPose const* term, a3_SpatialPose const* post, a3real const u)
{

	


	return pose_out;
}

// pointer-based LERP operation for single spatial pose
//inline a3_SpatialPose* a3spatialPoseOpLERP(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3real const u)
//{
//
//	// done
//	return pose_out;
//}

//Controls (1): spatial pose.
inline a3_SpatialPose* a3spatialPoseOPInvert(a3_SpatialPose* pose_out, a3_SpatialPose const* pose_in, a3real const u)
{
	if (pose_out && pose_in)
	{
		pose_out->angles.x;
	}
	/*if (spatialPose_out && spatialPose_lhs && spatialPose_rhs)
	{
		spatialPose_out->angles.x = a3trigValid_sind(spatialPose_lhs->angles.x + spatialPose_rhs->angles.x);
		spatialPose_out->angles.y = a3trigValid_sind(spatialPose_lhs->angles.y + spatialPose_rhs->angles.y);
		spatialPose_out->angles.z = a3trigValid_sind(spatialPose_lhs->angles.z + spatialPose_rhs->angles.z);

		spatialPose_out->scale.x = spatialPose_lhs->scale.x * spatialPose_rhs->scale.x;
		spatialPose_out->scale.y = spatialPose_lhs->scale.y * spatialPose_rhs->scale.y;
		spatialPose_out->scale.z = spatialPose_lhs->scale.z * spatialPose_rhs->scale.z;

		spatialPose_out->translation.x = spatialPose_lhs->translation.x + spatialPose_rhs->translation.x;
		spatialPose_out->translation.y = spatialPose_lhs->translation.y + spatialPose_rhs->translation.y;
		spatialPose_out->translation.z = spatialPose_lhs->translation.z + spatialPose_rhs->translation.z;

		return 1;
	}*/

	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOPNearest(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3real const u)
{
	//Controls (2): initial and terminal spatial poses.
	
	return pose_out;
}

//-----------------------------------------------------------------------------

// data-based reset/identity
inline a3_SpatialPose a3spatialPoseDOpIdentity()
{
	a3_SpatialPose const result = { a3mat4_identity /*, ...*/ };
	return result;
}

// data-based LERP
inline a3_SpatialPose a3spatialPoseDOpLERP(a3_SpatialPose const pose0, a3_SpatialPose const pose1, a3real const u)
{
	a3_SpatialPose result = { 0 };
	// ...

	// done
	return result;
}


//-----------------------------------------------------------------------------

// pointer-based reset/identity operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpIdentity(a3_HierarchyPose* pose_out)
{

	// done
	return pose_out;
}

// pointer-based LERP operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpLERP(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3real const u)
{

	// done
	return pose_out;
}


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_HIERARCHYSTATEBLEND_INL
#endif	// __ANIMAL3D_HIERARCHYSTATEBLEND_H