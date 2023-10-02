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
	
	a3_HierarchyState.inl
	Implementation of inline transform hierarchy state operations.
*/


#ifdef __ANIMAL3D_HIERARCHYSTATE_H
#ifndef __ANIMAL3D_HIERARCHYSTATE_INL
#define __ANIMAL3D_HIERARCHYSTATE_INL


//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------

// get offset to hierarchy pose in contiguous set
inline a3i32 a3hierarchyPoseGroupGetPoseOffsetIndex(const a3_HierarchyPoseGroup *poseGroup, const a3ui32 poseIndex)
{
	if (poseGroup && poseGroup->hierarchy)
		return (poseIndex * poseGroup->hierarchy->numNodes);
	return -1;
}

// get offset to single node pose in contiguous set
inline a3i32 a3hierarchyPoseGroupGetNodePoseOffsetIndex(const a3_HierarchyPoseGroup *poseGroup, const a3ui32 poseIndex, const a3ui32 nodeIndex)
{
	if (poseGroup && poseGroup->hierarchy)
		return (poseIndex * poseGroup->hierarchy->numNodes + nodeIndex);
	return -1;
}

//-----------------------------------------------------------------------------

// reset full hierarchy pose
inline a3i32 a3hierarchyPoseReset(const a3_HierarchyPose* pose_inout, const a3ui32 nodeCount)
{
	if (pose_inout && nodeCount)
	{

	}
	return -1;
}

//// convert full hierarchy pose to hierarchy transforms
//inline a3i32 a3hierarchyPoseConvert(const a3_HierarchyPose* pose_inout, const a3ui32 nodeCount, const a3_SpatialPoseChannel channel, const a3_SpatialPoseEulerOrder order)
//{
//	if (pose_inout && nodeCount)
//	{
//		for (a3ui32 i = 0; i < nodeCount; ++i)
//		{
//			a3_SpatialPose* currentPose = pose_inout->spatialPose;
//
//			a3real4x4 rotationX = {
//				{1.0f, 0.0f, 0.0f, 0.0f},
//				{0.0f, a3acosd(currentPose[i].rotation[0]), -a3asind(currentPose[i].rotation[0]), 0.0f},
//				{0.0f, a3asind(currentPose[i].rotation[0]), a3acosd(currentPose[i].rotation[0]), 0.0f},
//				{0.0f, 0.0f, 0.0f, 1.0f}
//			};
//
//			a3real4x4 rotationY = {
//				{a3acosd(currentPose[i].rotation[1]), 0.0f, a3asind(currentPose[i].rotation[1]), 0.0f},
//				{0.0f, 1.0f, 0.0f, 0.0f},
//				{-a3asind(currentPose[i].rotation[1]), 0.0f, a3acosd(currentPose[i].rotation[1]), 0.0f},
//				{0.0f, 0.0f, 0.0f, 1.0f}
//			};
//
//			a3real4x4 rotationZ = {
//				{a3acosd(currentPose[i].rotation[2]), -a3asind(currentPose[i].rotation[2]), 0.0f, 0.0f},
//				{a3asind(currentPose[i].rotation[2]), a3acosd(currentPose[i].rotation[2]), 0.0f, 0.0f},
//				{0.0f, 0.0f, 1.0f, 0.0f},
//				{0.0f, 0.0f, 0.0f, 1.0f}
//			};
//
//			a3real4x4 rotationMat = {
//				{1.0f, 0.0f, 0.0f, 0.0f},
//				{0.0f, 1.0f, 0.0f, 0.0f},
//				{0.0f, 0.0f, 1.0f, 0.0f},
//				{0.0f, 0.0f, 0.0f, 1.0f},
//			};
//
//			a3real4x4Product(rotationMat, rotationMat, rotationZ);
//			a3real4x4Product(rotationMat, rotationMat, rotationY);
//			a3real4x4Product(rotationMat, rotationMat, rotationX);
//
//			a3real4x4 scaleMat = {
//				{currentPose[i].scale[0], 0.0f, 0.0f, 0.0f},
//				{0.0f, currentPose[i].scale[1], 0.0f, 0.0f},
//				{0.0f, 0.0f, currentPose[i].scale[2], 0.0f},
//				{0.0f, 0.0f, 0.0f, 1.0f}
//			};
//
//			a3real4x4 translationMat = {
//				{1.0f, 0.0f, 0.0f, currentPose[i].translation[0]},
//				{0.0f, 1.0f, 0.0f, currentPose[i].translation[1]},
//				{0.0f, 0.0f, 1.0f, currentPose[i].translation[2]},
//				{0.0f, 0.0f, 0.0f, 1.0f}
//			};
//
//			a3real4x4 resultMat = {
//				{1.0f, 0.0f, 0.0f, 0.0f},
//				{0.0f, 1.0f, 0.0f, 0.0f},
//				{0.0f, 0.0f, 1.0f, 0.0f},
//				{0.0f, 0.0f, 0.0f, 1.0f},
//			};
//
//			a3real4x4Sum(resultMat, resultMat, translationMat);
//			a3real4x4Sum(resultMat, resultMat, rotationMat);
//			a3real4x4Product(resultMat, resultMat, scaleMat);
//
//			a3real4x4SetReal4x4(currentPose[i].transform.m, resultMat);
//			
//		}
//		return 0;
//	}
//	return -1;
//}

// convert full hierarchy pose to hierarchy transforms
inline a3i32 a3hierarchyPoseConvert(const a3_HierarchyPose* pose_inout, const a3ui32 nodeCount, const a3_SpatialPoseEulerOrder order)
{
	if (pose_inout && nodeCount)
	{
		for (a3ui32 i = 0; i < nodeCount; ++i)
		{
			//a3_SpatialPose currentPose = pose_inout->spatialPose[i];

			a3real4x4 rotationX = {
				{1.0f, 0.0f, 0.0f, 0.0f},
				{0.0f, a3acosd(pose_inout->spatialPose[i].rotation[0]), -a3asind(pose_inout->spatialPose[i].rotation[0]), 0.0f},
				{0.0f, a3asind(pose_inout->spatialPose[i].rotation[0]), a3acosd(pose_inout->spatialPose[i].rotation[0]), 0.0f},
				{0.0f, 0.0f, 0.0f, 1.0f}
			};

			a3real4x4 rotationY = {
				{a3acosd(pose_inout->spatialPose[i].rotation[1]), 0.0f, a3asind(pose_inout->spatialPose[i].rotation[1]), 0.0f},
				{0.0f, 1.0f, 0.0f, 0.0f},
				{-a3asind(pose_inout->spatialPose[i].rotation[1]), 0.0f, a3acosd(pose_inout->spatialPose[i].rotation[1]), 0.0f},
				{0.0f, 0.0f, 0.0f, 1.0f}
			};

			a3real4x4 rotationZ = {
				{a3acosd(pose_inout->spatialPose[i].rotation[2]), -a3asind(pose_inout->spatialPose[i].rotation[2]), 0.0f, 0.0f},
				{a3asind(pose_inout->spatialPose[i].rotation[2]), a3acosd(pose_inout->spatialPose[i].rotation[2]), 0.0f, 0.0f},
				{0.0f, 0.0f, 1.0f, 0.0f},
				{0.0f, 0.0f, 0.0f, 1.0f}
			};

			a3real4x4 rotationMat = {
				{1.0f, 0.0f, 0.0f, 0.0f},
				{0.0f, 1.0f, 0.0f, 0.0f},
				{0.0f, 0.0f, 1.0f, 0.0f},
				{0.0f, 0.0f, 0.0f, 1.0f},
			};

			a3real4x4Product(rotationMat, rotationMat, rotationZ);
			a3real4x4Product(rotationMat, rotationMat, rotationY);
			a3real4x4Product(rotationMat, rotationMat, rotationX);

			a3real4x4 scaleMat = {
				{pose_inout->spatialPose[i].scale[0], 0.0f, 0.0f, 0.0f},
				{0.0f, pose_inout->spatialPose[i].scale[1], 0.0f, 0.0f},
				{0.0f, 0.0f, pose_inout->spatialPose[i].scale[2], 0.0f},
				{0.0f, 0.0f, 0.0f, 1.0f}
			};

			a3real4x4 translationMat = {
				{1.0f, 0.0f, 0.0f, pose_inout->spatialPose[i].translation[0]},
				{0.0f, 1.0f, 0.0f, pose_inout->spatialPose[i].translation[1]},
				{0.0f, 0.0f, 1.0f, pose_inout->spatialPose[i].translation[2]},
				{0.0f, 0.0f, 0.0f, 1.0f}
			};

			a3real4x4 resultMat = {
				{1.0f, 0.0f, 0.0f, 0.0f},
				{0.0f, 1.0f, 0.0f, 0.0f},
				{0.0f, 0.0f, 1.0f, 0.0f},
				{0.0f, 0.0f, 0.0f, 1.0f},
			};

			a3real4x4Sum(resultMat, resultMat, translationMat);
			a3real4x4Sum(resultMat, resultMat, rotationMat);
			a3real4x4Product(resultMat, resultMat, scaleMat);

			a3real4x4SetReal4x4(pose_inout->spatialPose[i].transform.m, resultMat);

		}
		return 0;
	}
	return -1;
}

// copy full hierarchy pose
inline a3i32 a3hierarchyPoseCopy(const a3_HierarchyPose* pose_out, const a3_HierarchyPose* pose_in, const a3ui32 nodeCount)
{
	if (pose_out && pose_in && nodeCount)
	{

	}
	return -1;
}


//-----------------------------------------------------------------------------

// update inverse object-space matrices
inline a3i32 a3hierarchyStateUpdateObjectInverse(const a3_HierarchyState *state)
{
	return -1;
}


//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_HIERARCHYSTATE_INL
#endif	// __ANIMAL3D_HIERARCHYSTATE_H