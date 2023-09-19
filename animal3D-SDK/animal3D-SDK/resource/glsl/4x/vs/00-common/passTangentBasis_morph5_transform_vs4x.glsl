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
	
	passTangentBasis_morph5_transform_vs4x.glsl
	Calculate and pass tangent basis with morphing.
*/

#version 450

layout (location = 0) in vec4 aPosition0;
layout (location = 1) in vec4 aNormal0;
layout (location = 2) in vec4 aTangent0;

layout (location = 3) in vec4 aPosition1;
layout (location = 4) in vec4 aNormal1;
layout (location = 5) in vec4 aTangent1;

layout (location = 6) in vec4 aPosition2;
layout (location = 7) in vec4 aNormal2;
layout (location = 8) in vec4 aTangent2;

layout (location = 9) in vec4 aPosition3;
layout (location = 10) in vec4 aNormal3;
layout (location = 11) in vec4 aTangent3;

layout (location = 12) in vec4 aPosition4;
layout (location = 13) in vec4 aNormal4;
layout (location = 14) in vec4 aTangent4;

//layout (location = 2) in vec4 aBitangent;	// usually 11
layout (location = 15) in vec4 aTexcoord;	// usually 8

uniform mat4 uP;
uniform mat4 uMV, uMV_nrm;
uniform mat4 uAtlas;
uniform double uTime;

out vbVertexData {
	mat4 vTangentBasis_view;
	vec4 vTexcoord_atlas;
};

flat out int vVertexID;
flat out int vInstanceID;

vec4 lerp(in vec4 v0, in vec4 v1, in float u);
vec4 nlerp(in vec4 v0, in vec4 v1, in float u);
vec4 CatmullRom(in vec4 vP, in vec4 v0, in vec4 v1, in vec4 vN, in float u);
vec4 nCatmullRom(in vec4 vP, in vec4 v0, in vec4 v1, in vec4 vN, in float u);

void main()
{
	// DUMMY OUTPUT: directly assign input position to output position
//	gl_Position = aPosition;

	vec4[5] positions = vec4[5] 
	(
		aPosition0, aPosition1, aPosition2, aPosition3, aPosition4
	);

	vec4[5] normals = vec4[5] 
	(
		aNormal0, aNormal1, aNormal2, aNormal3, aNormal4
	);

	vec4[5] tangents = vec4[5] 
	(
		aTangent0, aTangent1, aTangent2, aTangent3, aTangent4
	);

	int k0 = int(uTime) % 5; // k == clipCtrl->keyframeIndex
	int k1 = (k0 + 1) % 5;
	int kP = (k0 - 1 + 5) % 5; // Don't % a negative number
	int kN = (k0 + 2) % 5;

	float param = float(uTime) - float(int(uTime)); // [0, 1)

	vec4 position = CatmullRom(positions[kP], positions[k0], positions[k1], positions[kP], param);
	vec4 normal = nCatmullRom(normals[kP], normals[k0], normals[k1], normals[kP], param);
	vec4 tangent = nCatmullRom(tangents[kP], tangents[k0], tangents[k1], tangents[kP], param);

	vec4 bitangent = vec4(cross(normal.xyz, tangent.xyz), 0.0);

	vTangentBasis_view = uMV_nrm * mat4(tangent, bitangent, normal, vec4(0.0));
	vTangentBasis_view[3] = uMV * position;
	gl_Position = uP * vTangentBasis_view[3];
	
	vTexcoord_atlas = uAtlas * aTexcoord;

	vVertexID = gl_VertexID;
	vInstanceID = gl_InstanceID;
}
