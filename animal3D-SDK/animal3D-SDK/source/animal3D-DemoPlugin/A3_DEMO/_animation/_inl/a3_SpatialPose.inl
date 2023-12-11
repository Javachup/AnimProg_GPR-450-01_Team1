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
inline a3_SpatialPose* a3spatialPoseSetRotation(a3_SpatialPose* spatialPose, const a3f32 rx_degrees, const a3f32 ry_degrees, const a3f32 rz_degrees)
{
	if (spatialPose)
	{
		spatialPose->angles.x = a3trigValid_sind(rx_degrees);
		spatialPose->angles.y = a3trigValid_sind(ry_degrees);
		spatialPose->angles.z = a3trigValid_sind(rz_degrees);
	}
	return spatialPose;
}

// scale
inline a3_SpatialPose* a3spatialPoseSetScale(a3_SpatialPose* spatialPose, const a3f32 sx, const a3f32 sy, const a3f32 sz)
{
	if (spatialPose)
	{
		spatialPose->scale.x = sx;
		spatialPose->scale.y = sy;
		spatialPose->scale.z = sz;
	}
	return spatialPose;
}

// translation
inline a3_SpatialPose* a3spatialPoseSetTranslation(a3_SpatialPose* spatialPose, const a3f32 tx, const a3f32 ty, const a3f32 tz)
{
	if (spatialPose)
	{
		spatialPose->translation.x = tx;
		spatialPose->translation.y = ty;
		spatialPose->translation.z = tz;
	}
	return spatialPose;
}


//-----------------------------------------------------------------------------

// reset single node pose
inline a3_SpatialPose* a3spatialPoseReset(a3_SpatialPose* spatialPose)
{
	if (spatialPose)
	{
		spatialPose->transform = a3mat4_identity;
		spatialPose->orientation = a3vec4_w;
		spatialPose->angles = a3vec4_zero;
		spatialPose->scale = a3vec4_one;
		spatialPose->translation = a3vec4_w;
		return spatialPose;
	}
	return spatialPose;
}

// convert single node pose to matrix
inline a3_SpatialPose* a3spatialPoseConvert(a3_SpatialPose* spatialPose, const a3_SpatialPoseChannel channel, const a3_SpatialPoseEulerOrder order)
{
	if (spatialPose)
	{
		a3mat4 rx, ry, rz, r;
		a3real4x4SetRotateX(rx.m, spatialPose->angles.x);
		a3real4x4SetRotateY(ry.m, spatialPose->angles.y);
		a3real4x4SetRotateZ(rz.m, spatialPose->angles.z);
		switch (order)
		{
		case a3poseEulerOrder_xyz:
			a3real4x4Product(r.m, rx.m, ry.m);
			a3real4x4Product(spatialPose->transform.m, r.m, rz.m);
			break;
		case a3poseEulerOrder_yzx:
			a3real4x4Product(r.m, ry.m, rz.m);
			a3real4x4Product(spatialPose->transform.m, r.m, rx.m);
			break;
		case a3poseEulerOrder_zxy:
			a3real4x4Product(r.m, rz.m, rx.m);
			a3real4x4Product(spatialPose->transform.m, r.m, ry.m);
			break;
		case a3poseEulerOrder_yxz:
			a3real4x4Product(r.m, ry.m, rx.m);
			a3real4x4Product(spatialPose->transform.m, r.m, rz.m);
			break;
		case a3poseEulerOrder_xzy:
			a3real4x4Product(r.m, rx.m, rz.m);
			a3real4x4Product(spatialPose->transform.m, r.m, ry.m);
			break;
		case a3poseEulerOrder_zyx:
			a3real4x4Product(r.m, rz.m, ry.m);
			a3real4x4Product(spatialPose->transform.m, r.m, rx.m);
			break;
		}
		a3real3MulS(spatialPose->transform.v0.v, spatialPose->scale.x);
		a3real3MulS(spatialPose->transform.v1.v, spatialPose->scale.y);
		a3real3MulS(spatialPose->transform.v2.v, spatialPose->scale.z);
		spatialPose->transform.v3 = spatialPose->translation;
		return spatialPose;
	}
	return spatialPose;
}

// restore single node pose from matrix
inline a3_SpatialPose* a3spatialPoseRestore(a3_SpatialPose* spatialPose, const a3_SpatialPoseChannel channel, const a3_SpatialPoseEulerOrder order)
{
	if (spatialPose)
	{

	}
	return spatialPose;
}

// copy operation for single node pose
inline a3_SpatialPose* a3spatialPoseOPCopy(a3_SpatialPose* spatialPose_out, const a3_SpatialPose* spatialPose_in)
{
	if (spatialPose_out && spatialPose_in)
	{
		*spatialPose_out = *spatialPose_in;
		return spatialPose_out;
	}
	return spatialPose_out;
}

// concat
inline a3_SpatialPose* a3spatialPoseConcat(a3_SpatialPose* spatialPose_out, const a3_SpatialPose* spatialPose_lhs, const a3_SpatialPose* spatialPose_rhs)
{
	if (spatialPose_out && spatialPose_lhs && spatialPose_rhs)
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

		return spatialPose_out;
	}
	return spatialPose_out;
}

// lerp
inline a3_SpatialPose* a3spatialPoseLerp(a3_SpatialPose* spatialPose_out, const a3_SpatialPose* spatialPose_0, const a3_SpatialPose* spatialPose_1, const a3real u)
{
	if (spatialPose_out && spatialPose_0 && spatialPose_1)
	{
		a3real3Lerp(spatialPose_out->angles.v, spatialPose_0->angles.v, spatialPose_1->angles.v, u);

		a3real3Lerp(spatialPose_out->scale.v, spatialPose_0->scale.v, spatialPose_1->scale.v, u);

		a3real3Lerp(spatialPose_out->translation.v, spatialPose_0->translation.v, spatialPose_1->translation.v, u);

		return spatialPose_out;
	}
	return spatialPose_out;
}

//-----------------------------------------------------------------------------

inline a3real SnakeWaveFunction(a3real x, const a3real amp, const a3real freq, const a3real boneLength, const a3ui32 numBones)
{
	if (0 > x && x >= numBones * boneLength)
		return -1;

	//Wave Function
	a3real radians =  x * freq;
	a3real wave = amp * a3sinr(a3trigValid_sinr(radians));

	//Parabola 
	a3real snakeLength = numBones * boneLength;
	a3real xSq = x * x;

	a3real parabola = -(2 / snakeLength) * (xSq)+(2 * x);

	//SnakeWave
	a3real snakeWave = wave * parabola;

	return snakeWave;
}

inline a3real SnakeWaveFunctionDerivative(a3real x, const a3real amp, const a3real freq, const a3real boneLength, const a3ui32 numBones)
{
	if (0 > x && x >= numBones * boneLength)
		return -1;

	//Wave Function
	a3real radians = x * freq;
	a3real wave = amp * a3sinr(a3trigValid_sinr(radians));

	//Wave Derivative
	a3real waveDer = amp * freq * a3cosr(a3trigValid_sinr(radians));

	//Parabola 
	a3real snakeLength = numBones * boneLength;
	a3real xSq = x * x;

	a3real parabola = -(2 / snakeLength) * (xSq)+(2 * x);

	//Parabola Derivative
	a3real parabolaDer = -(4 / snakeLength) * x + 2;

	//SnakeWave Derivative (Product Rule)
	a3real snakeWaveDer = waveDer * parabola + wave * parabolaDer;

	return snakeWaveDer;
}

// finding snake bone position along function wave
inline a3i32 a3SpatialPoseSnakeWave(a3_SpatialPose* spatialPose_out, a3vec2* previousPos_inout, 
	const a3real amp, const a3real freq, const a3f64 deltaTime, const a3real boneLength, const a3ui32 numBones)
{
	//a3real x = pose_out->pose->translation.x;
	a3real y0 = previousPos_inout->y;
	a3real x0 = previousPos_inout->x + (a3real)deltaTime;
	
	
	//a3real der = SnakeWaveFunctionDerivative(x0, amp, freq, boneLength, numBones);
	a3real dx = boneLength;//boneLength / (a3sqrt(1 + (der * der)));

	a3real y = SnakeWaveFunction(x0 + dx, amp, freq, boneLength, numBones);
	a3real dy = y - y0;
	
	/*
	a3real der = a3cosr(a3trigValid_sinr(x0));
	a3real dx = boneLength / (a3sqrt(1 + (der * der)));
	//a3trigValid_sinr()
	a3real y = a3sinr(a3trigValid_sinr(x0 + dx));
	a3real dy = y - y0;
	*/
	//should be lerp
	spatialPose_out->translation.y = dy;
	spatialPose_out->translation.x = dx;

	previousPos_inout->x += dx;
	previousPos_inout->y += dy;

	return 0;
}



#endif	// !__ANIMAL3D_SPATIALPOSE_INL
#endif	// __ANIMAL3D_SPATIALPOSE_H