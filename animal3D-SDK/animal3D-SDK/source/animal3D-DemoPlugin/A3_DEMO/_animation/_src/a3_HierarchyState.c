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
	
	a3_HierarchyState.c
	Implementation of transform hierarchy state.
*/

#include "../a3_HierarchyState.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


//-----------------------------------------------------------------------------

// initialize pose set given an initialized hierarchy and key pose count
a3i32 a3hierarchyPoseGroupCreate(a3_HierarchyPoseGroup *poseGroup_out, const a3_Hierarchy *hierarchy, const a3ui32 poseCount)
{
	// validate params and initialization states
	//	(output is not yet initialized, hierarchy is initialized)
	if (poseGroup_out && hierarchy && !poseGroup_out->hierarchy && hierarchy->nodes)
	{
		// determine memory requirements
		// The raw data is the spactial poses that is wrapped in a hierarchy pose
		const size_t size = sizeof(a3_HierarchyPose) * poseCount +
			sizeof(a3_SpatialPose) * poseCount * hierarchy->numNodes;
		// allocate everything (one malloc)
		void* mem = malloc(size);
		if (mem == NULL)
			return -1;

		// set pointers
		poseGroup_out->hierarchy = hierarchy;
		// Staring address for heirarchical poses
		a3_HierarchyPose* hposeBase = (a3_HierarchyPose*)mem;
		// Starting address for spatial poses
		a3_SpatialPose* poseBase = (a3_SpatialPose*)(hposeBase + poseCount);
		// Link the 2 together: each hpose get numNodes number of spatial poses
		poseGroup_out->hpose = hposeBase;

		poseGroup_out->hposeCount = poseCount;

		// For each hpose, set the spatialPose pointer
		for (a3ui32 i = 0; i < poseCount; i++)
		{
			// Cast to non-const for the initialization
			a3_HierarchyPose* hpose = (poseGroup_out->hpose + i);

			// Start at poseBase and add the size of every hierarchy pose before it (i + numNodes) to end up at that hpose's spatial pose memeory
			hpose->spatialPose = poseBase + i * hierarchy->numNodes;
		}

		// reset all data
		// For every spatialPose, reset it
		// number of spatial poses = numPoses * numNodes
		for (a3ui32 i = 0; i < poseCount * hierarchy->numNodes; i++)
		{
			a3spatialPoseReset(poseBase + i);
		}

		// done
		return 1;
	}
	return -1;
}

// release pose set
a3i32 a3hierarchyPoseGroupRelease(a3_HierarchyPoseGroup *poseGroup)
{
	// validate param exists and is initialized
	if (poseGroup && poseGroup->hierarchy)
	{
		// release everything (one free)
		free(poseGroup->hpose); // poseGroup->hpose is the start of the memory block

		// reset pointers
		poseGroup->hierarchy = 0;

		// done
		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// initialize hierarchy state given an initialized hierarchy
a3i32 a3hierarchyStateCreate(a3_HierarchyState *state_out, const a3_Hierarchy *hierarchy)
{
	// validate params and initialization states
	//	(output is not yet initialized, hierarchy is initialized)
	if (state_out && hierarchy && !state_out->hierarchy && hierarchy->nodes)
	{
		// determine memory requirements
		// The raw data is the spactial poses that is wrapped in a hierarchy pose
		const size_t size = sizeof(a3_SpatialPose) * 2 * hierarchy->numNodes;
		// allocate everything (one malloc)
		void* mem = malloc(size);
		if (mem == NULL)
			return -1;

		// set pointers
		state_out->hierarchy = hierarchy;

		//// For each hpose, set the spatialPose pointer
		//for (a3ui32 i = 0; i < hierarchy->numNodes; i++)
		//{
		//	// Cast to non-const for the initialization
		//	a3_HierarchyPose* hpose = (a3_HierarchyPose*)(poseGroup_out->hpose + i);
		//
		//	// Start at poseBase and add the size of every hierarchy pose before it (i + numNodes) to end up at that hpose's spatial pose memeory
		//	hpose->spatialPose = poseBase + i * hierarchy->numNodes;
		//}

		a3_SpatialPose* poseBase = (a3_SpatialPose*)mem;
		state_out->localSpace.spatialPose = poseBase;
		state_out->objectSpace.spatialPose = poseBase + hierarchy->numNodes;



		// reset all data
		// For every spatialPose, reset it
		// number of spatial poses = numPoses * numNodes
		for (a3ui32 i = 0; i < hierarchy->numNodes * 2; i++)
		{
			a3spatialPoseReset(poseBase + i);
		}

		// done
		return 1;
	}
	return -1;
}

// release hierarchy state
a3i32 a3hierarchyStateRelease(a3_HierarchyState *state)
{
	// validate param exists and is initialized
	if (state && state->hierarchy)
	{
		// release everything (one free)
		free(state->localSpace.spatialPose); // poseGroup->hpose is the start of the memory block

		// reset pointers
		state->hierarchy = 0;

		// done
		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// load HTR file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadHTR(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath)
{
	if (poseGroup_out && !poseGroup_out->hposeCount && hierarchy_out && !hierarchy_out->numNodes && resourceFilePath && *resourceFilePath)
	{
		/*_____ignore any data following the # character_____*/
		// [SegmentNames&Hierarchy] defines hierarchical struct of skeleton
			// left is child, right is parent
		// [BasePosition] indicates the beginning of the base pose section
			// segment name, translation: xyz, rotation: xyz, bone length
		// Motion data needed to drive the animation
			// [BoneName]: int frameNumber, trans: xyz, rot: xyz, bone scale
		// [EndOfFile]: no more data to be processed

		// header variables, however not all are used
		char fileType[256], dataType[256], euler[256],
			calib[256], rot[256], axis[256], boneLength[256];
		a3ui32 fileVersion, numSegments, numFrames, dataFrame;
		a3real scaleFactor;

		// manually set up the skeleton
		a3ui32 j, p, jointIndex = 0;
		a3i32 jointParentIndex = -1;
		char objectParent[256], object[256];
		a3_SpatialPose* spatialPose = 0;
		a3real spatialPoseScale;

		FILE* file;
		char line[4096];	// max line length
		char section[1024];	// current section name

		// open the file for reading
		file = fopen(resourceFilePath, "r");
		if (file == NULL)
		{
			perror("Error opening file");
			return -1;
		}

		// read each line from the file
		while (fgets(line, sizeof(line), file) != NULL)
		{
			// remove trailing newline characters
			line[strcspn(line, "\n")] = '\0';

			// ignore all comments
			if (line[0] == '#')
			{
				continue;
			}

			// check if the line contains a section header
			if (line[0] == '[' && line[strlen(line) - 1] == ']')
			{
				strcpy(section, line);	//save the current section name
			}
			else
			{
				if (strcmp(section, "[Header]") == 0)
				{
					// parse the header
					char keyWord[256];
					int result = sscanf(line, "%s", keyWord);

					if (result == 1)
					{
						if (strcmp(keyWord, "FileType") == 0)
						{
							int result = sscanf(line, "FileType %255s", fileType);
							printf("File Type: %s\n", fileType);
						}
						else if (strcmp(keyWord, "DataType") == 0)
						{
							int result = sscanf(line, "DataType %255s", dataType);
							printf("Data Type: %s\n", dataType);
						}
						else if (strcmp(keyWord, "FileVersion") == 0)
						{
							int result = sscanf(line, "FileVersion %d", &fileVersion);
							printf("File Version: %d\n", fileVersion);
						}
						else if (strcmp(keyWord, "NumSegments") == 0)
						{
							int result = sscanf(line, "NumSegments %d", &numSegments);
							printf("Number of Segments: %d\n", numSegments);
							a3hierarchyCreate(hierarchy_out,numSegments, 0);

							if (hierarchy_out->numNodes == numSegments)
							{
								printf("Number of nodes has been set\n");
							}
						}
						else if (strcmp(keyWord, "NumFrames") == 0)
						{
							int result = sscanf(line, "NumFrames %d", &numFrames);

							// more frames because we need room for the base pose
							numFrames += 1;
							a3hierarchyPoseGroupCreate(poseGroup_out, hierarchy_out, numFrames);
							// set the base position
							poseGroup_out->hierarchy = hierarchy_out;

							printf("Number of Frames: %d\n", numFrames);
						}
						else if (strcmp(keyWord, "DataFrameRate") == 0)
						{
							int result = sscanf(line, "DataFrameRate %d", &dataFrame);
							printf("Data Frame Rate: %d\n", dataFrame);
						}
						else if (strcmp(keyWord, "EulerRotationOrder") == 0)
						{
							int result = sscanf(line, "EulerRotationOrder %255s", euler);
							printf("Euler Rotation Order: %s\n", euler);
						}
						else if (strcmp(keyWord, "CalibrationUnits") == 0)
						{
							int result = sscanf(line, "CalibrationUnits %255s", calib);
							printf("Calibration Units: %s\n", calib);
						}
						else if (strcmp(keyWord, "RotationUnits") == 0)
						{
							int result = sscanf(line, "RotationUnits %255s", rot);
							printf("Rotation Units: %s\n", rot);
						}
						else if (strcmp(keyWord, "GlobalAxisofGravity") == 0)
						{
							int result = sscanf(line, "GlobalAxisofGravity %255s", axis);
							printf("Global Axis of Gravity: %s\n", axis);
						}
						else if (strcmp(keyWord, "BoneLengthAxis") == 0)
						{
							int result = sscanf(line, "BoneLengthAxis %255s", boneLength);
							printf("Bone Length Axis: %s\n", boneLength);
						}
						else if (strcmp(keyWord, "ScaleFactor") == 0)
						{
							int result = sscanf(line, "ScaleFactor %f", &scaleFactor);
							printf("Scale Factor: %f\n", scaleFactor);
							spatialPoseScale = scaleFactor;
						}
					}
				}
				else if (strcmp(section, "[SegmentNames&Hierarchy]") == 0)
				{
					// link the segments into the hierarchy
					int result = sscanf(line, "%s %s", object, objectParent);

					// if the object is main, set the joint parent index to -1
					if (strcmp(objectParent, "GLOBAL") == 0)
					{
						jointParentIndex = -1;
					}
					else
					{
						jointParentIndex = a3hierarchyGetNodeIndex(hierarchy_out, objectParent);
					}

					a3hierarchySetNode(hierarchy_out, jointIndex++, jointParentIndex, object);

					//printf("Object Name: %s		Parent Name: %s\n", object, objectParent);
				}
				else if (strcmp(section, "[BasePosition]") == 0)
				{
					char jointName[256];
					a3f32 tX, tY, tZ, rX, rY, rZ, boneLength;

					int result = sscanf(line, "%s %f %f %f %f %f %f %f", jointName, &tX, &tY, &tZ, &rX, &rY, &rZ, &boneLength);

					// Scale translation by 0.1f since calibration scale is mm
					tX *= 0.1f;
					tY *= 0.1f;
					tZ *= 0.1f;

					p = 0;
					j = a3hierarchyGetNodeIndex(hierarchy_out, jointName);
					spatialPose = poseGroup_out->hpose[p].spatialPose + j;
					a3spatialPoseSetTranslation(spatialPose, tX, tY, tZ);
					a3spatialPoseSetRotation(spatialPose, rX, rY, rZ);
					a3spatialPoseSetScale(spatialPose, spatialPoseScale, spatialPoseScale, spatialPoseScale);

					//printf("%s %f %f %f %f %f %f %f\n", jointName, tX, tY, tZ, rX, rY, rZ, boneLength);

				}
				else if (strcmp(section, "[EndOfFile]") == 0)
				{
					// if the end of the file is reached then break while loop
					break;
				}
				else
				{
					// process the motion data
					a3ui32 index;
					a3f32 tX, tY, tZ, rX, rY, rZ, scaleFactor;
					char jointName[256];

					// get rid of the "[" and "]" on either side of the section name
					strcpy(jointName, section);
					size_t len = strlen(jointName);
					// shift all chars to the left by 1
					for (int l = 0; l < len - 1; l++)
					{
						jointName[l] = jointName[l + 1];
					}
					// null terminate the string
					jointName[len - 2] = '\0';

					int result = sscanf(line, "%d %f %f %f %f %f %f %f", &index, &tX, &tY, &tZ, &rX, &rY, &rZ, &scaleFactor);

					tX *= 0.1f;
					tY *= 0.1f;
					tZ *= 0.1f;

					p = ++index;
					j = a3hierarchyGetNodeIndex(hierarchy_out, jointName);
					//printf("Node: %s		Hierarchy Pose: %d\n", jointName, index);

					spatialPose = poseGroup_out->hpose[p].spatialPose + j;
					a3spatialPoseSetTranslation(spatialPose, tX, tY, tZ);
					a3spatialPoseSetRotation(spatialPose, rX, rY, rZ);
					a3spatialPoseSetScale(spatialPose, scaleFactor, scaleFactor, scaleFactor);

					//printf("%f %f %f %f %f %f %f\n", tX, tY, tZ, rX, rY, rZ, scaleFactor);
				}
			}
		}
		fclose(file);
		return 1;
	}
	return -1;
}

// load BVH file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadBVH(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath)
{
	if (poseGroup_out && !poseGroup_out->hposeCount && hierarchy_out && !hierarchy_out->numNodes && resourceFilePath && *resourceFilePath)
	{

	}
	return -1;
}


//-----------------------------------------------------------------------------
