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



	Joey Romanowski:
		Created and implemented a3spatialPoseLerp(...)
		Created and implemented a3spatialPoseSmoothStep(...)
		Created and implemented a3spatialPoseNearest(...)
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

inline a3i32 a3spatialPoseConcat(a3_SpatialPose* spatialPoseL_inout, const a3_SpatialPose* spatialPoseR_in)
{
	if (spatialPoseL_inout && spatialPoseR_in)
	{
		// Add translation and rotation, multiply scale 
		a3real3Add(spatialPoseL_inout->translation, spatialPoseR_in->translation);
		a3real3Add(spatialPoseL_inout->rotation, spatialPoseR_in->rotation);
		a3real3MulComp(spatialPoseL_inout->scale, spatialPoseR_in->scale);
	}
	return 0;
}

// convert single node pose to matrix
inline a3i32 a3spatialPoseConvert(a3_SpatialPose* spatialPose_in, const a3_SpatialPoseChannel channel, const a3_SpatialPoseEulerOrder order)
{
	if (spatialPose_in)
	{
		a3real4x4p rotationMat = {
			{0.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 0.0f}
		};
		a3real4x4SetRotateZYX(rotationMat, spatialPose_in->rotation[0], spatialPose_in->rotation[1], spatialPose_in->rotation[2]);

		a3real4x4 scaleMat = {
			{spatialPose_in->scale[0], 0.0f, 0.0f, 0.0f},
			{0.0f, spatialPose_in->scale[1], 0.0f, 0.0f},
			{0.0f, 0.0f, spatialPose_in->scale[2], 0.0f},
			{0.0f, 0.0f, 0.0f, 1.0f}
		};

		a3real4x4 translationMat = {
			{1.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 1.0f, 0.0f},
			{spatialPose_in->translation[0], spatialPose_in->translation[1], spatialPose_in->translation[2], 1.0f}
		};

		// add translation
		a3real4x4 tmp_rs, tmp_rst;

		// add rotation
		a3real4x4Product(tmp_rs, rotationMat, scaleMat);
		// multiply scale
		a3real4x4Product(tmp_rst, translationMat, tmp_rs);

		//a3_SpatialPose* sPose = spatialPose_in;
		//a3real4* transform = sPose->transform.m;
		a3real4x4SetReal4x4(spatialPose_in->transform.m, tmp_rst);
	}
	return 0;
}

inline a3i32 a3spatialPoseLerp(a3_SpatialPose* samplePose_out, const a3_SpatialPose* keyPose0_in, const a3_SpatialPose* keyPose1_in, const a3real param)
{
	if (samplePose_out && keyPose0_in && keyPose1_in && param >= 0 && param <= 1)
	{
		a3spatialPoseSetTranslation(samplePose_out, 
			a3lerp(keyPose0_in->translation[0], keyPose1_in->translation[0], param),
			a3lerp(keyPose0_in->translation[1], keyPose1_in->translation[1], param),
			a3lerp(keyPose0_in->translation[2], keyPose1_in->translation[2], param));

		a3spatialPoseSetRotation(samplePose_out, 
			a3lerp(keyPose0_in->rotation[0], keyPose1_in->rotation[0], param),
			a3lerp(keyPose0_in->rotation[1], keyPose1_in->rotation[1], param),
			a3lerp(keyPose0_in->rotation[2], keyPose1_in->rotation[2], param));

		a3spatialPoseSetScale(samplePose_out, 
			a3lerp(keyPose0_in->scale[0], keyPose1_in->scale[0], param),
			a3lerp(keyPose0_in->scale[1], keyPose1_in->scale[1], param),
			a3lerp(keyPose0_in->scale[2], keyPose1_in->scale[2], param));
	}
	return 0;
}

inline a3i32 a3spatialPoseSmoothStep(a3_SpatialPose* samplePose_out, const a3_SpatialPose* keyPose0_in, const a3_SpatialPose* keyPose1_in, const a3real param)
{
	if (samplePose_out && keyPose0_in && keyPose1_in && param >= 0 && param <= 1)
	{
		// Smooth step equation from: https://en.wikipedia.org/wiki/Smoothstep
		float newParam = param * param * (3.0f - 2.0f * param);

		a3spatialPoseSetTranslation(samplePose_out,
			a3lerp(keyPose0_in->translation[0], keyPose1_in->translation[0], newParam),
			a3lerp(keyPose0_in->translation[1], keyPose1_in->translation[1], newParam),
			a3lerp(keyPose0_in->translation[2], keyPose1_in->translation[2], newParam));

		a3spatialPoseSetRotation(samplePose_out,
			a3lerp(keyPose0_in->rotation[0], keyPose1_in->rotation[0], newParam),
			a3lerp(keyPose0_in->rotation[1], keyPose1_in->rotation[1], newParam),
			a3lerp(keyPose0_in->rotation[2], keyPose1_in->rotation[2], newParam));

		a3spatialPoseSetScale(samplePose_out,
			a3lerp(keyPose0_in->scale[0], keyPose1_in->scale[0], newParam),
			a3lerp(keyPose0_in->scale[1], keyPose1_in->scale[1], newParam),
			a3lerp(keyPose0_in->scale[2], keyPose1_in->scale[2], newParam));
	}
	return 0;
}

inline a3i32 a3spatialPoseNearest(a3_SpatialPose* samplePose_out, const a3_SpatialPose* keyPose0_in, const a3_SpatialPose* keyPose1_in, const a3real param)
{
	if (samplePose_out && keyPose0_in && keyPose1_in && param >= 0 && param <= 1)
	{
		const a3_SpatialPose* keyPoseToUse = param < a3real_half ? keyPose0_in : keyPose1_in;

		a3spatialPoseSetTranslation(samplePose_out, keyPoseToUse->translation[0], keyPoseToUse->translation[1], keyPoseToUse->translation[2]);
		a3spatialPoseSetRotation(samplePose_out, keyPoseToUse->rotation[0], keyPoseToUse->rotation[1], keyPoseToUse->rotation[2]);
		a3spatialPoseSetScale(samplePose_out, keyPoseToUse->scale[0], keyPoseToUse->scale[1], keyPoseToUse->scale[2]);
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