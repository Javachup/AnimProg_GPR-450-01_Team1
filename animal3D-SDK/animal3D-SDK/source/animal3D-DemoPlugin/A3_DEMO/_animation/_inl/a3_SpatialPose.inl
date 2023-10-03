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
	
	a3_SpatialPose.inl
	Implementation of inline spatial pose operations.
*/


#ifdef __ANIMAL3D_SPATIALPOSE_H
#ifndef __ANIMAL3D_SPATIALPOSE_INL
#define __ANIMAL3D_SPATIALPOSE_INL


//-----------------------------------------------------------------------------

// set rotation values for a single node pose
inline a3i32 a3spatialPoseSetRotation(a3_SpatialPose* spatialPose, const a3f32 rx_degrees, const a3f32 ry_degrees, const a3f32 rz_degrees)
{
	if (spatialPose)
	{
		a3real3Set(spatialPose->rotation, rx_degrees, ry_degrees, rz_degrees);
	}
	return 0;
}

// scale
inline a3i32 a3spatialPoseSetScale(a3_SpatialPose* spatialPose, const a3f32 sx, const a3f32 sy, const a3f32 sz)
{
	if (spatialPose)
	{
		a3real3Set(spatialPose->scale, sx, sy, sz);
	}
	return 0;
}

// translation
inline a3i32 a3spatialPoseSetTranslation(a3_SpatialPose* spatialPose, const a3f32 tx, const a3f32 ty, const a3f32 tz)
{
	if (spatialPose)
	{
		a3real3Set(spatialPose->translation, tx, ty, tz);
	}
	return 0;
}


//-----------------------------------------------------------------------------

// reset single node pose
inline a3i32 a3spatialPoseReset(a3_SpatialPose* spatialPose)
{
	if (spatialPose)
	{
		a3spatialPoseSetTranslation(spatialPose, 0, 0, 0);
		a3spatialPoseSetRotation(spatialPose, 0, 0, 0);
		a3spatialPoseSetScale(spatialPose, 1, 1, 1);
	}
	return 0;
}

// convert single node pose to matrix
inline a3i32 a3spatialPoseConvert(a3mat4* mat_out, const a3_SpatialPose* spatialPose_in, const a3_SpatialPoseChannel channel, const a3_SpatialPoseEulerOrder order)
{
	if (mat_out && spatialPose_in)
	{

		a3real4x4p rotationMat = 0;
		a3real4x4SetRotateXYZ(rotationMat, spatialPose_in->rotation[0], spatialPose_in->rotation[1], spatialPose_in->rotation[2]);

		a3real4x4 scaleMat = {
			{spatialPose_in->scale[0], 0.0f, 0.0f, 0.0f},
			{0.0f, spatialPose_in->scale[1], 0.0f, 0.0f},
			{0.0f, 0.0f, spatialPose_in->scale[2], 0.0f},
			{0.0f, 0.0f, 0.0f, 1.0f}
		};

		a3real4x4 translationMat = {
			{1.0f, 0.0f, 0.0f, spatialPose_in->translation[0]},
			{0.0f, 1.0f, 0.0f, spatialPose_in->translation[1]},
			{0.0f, 0.0f, 1.0f, spatialPose_in->translation[2]},
			{0.0f, 0.0f, 0.0f, 1.0f}
		};

		// add translation
		a3real4x4Product(mat_out->m, mat_out->m, translationMat);
		// add rotation
		a3real4x4Product(mat_out->m, mat_out->m, rotationMat);
		// multiply scale
		a3real4x4Product(mat_out->m, mat_out->m, scaleMat);

		a3real4x4SetReal4x4(spatialPose_in->transform.m, mat_out);
	}
	return 0;
}

// copy operation for single node pose
inline a3i32 a3spatialPoseCopy(a3_SpatialPose* spatialPose_out, const a3_SpatialPose* spatialPose_in)
{
	if (spatialPose_out && spatialPose_in)
	{
		a3spatialPoseSetTranslation(spatialPose_out, spatialPose_in->translation[0], spatialPose_in->translation[1], spatialPose_in->translation[2]);
		a3spatialPoseSetRotation(spatialPose_out, spatialPose_in->rotation[0], spatialPose_in->rotation[1], spatialPose_in->rotation[2]);
		a3spatialPoseSetScale(spatialPose_out, spatialPose_in->scale[0], spatialPose_in->scale[1], spatialPose_in->scale[2]);
	}
	return 0;
}


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_SPATIALPOSE_INL
#endif	// __ANIMAL3D_SPATIALPOSE_H