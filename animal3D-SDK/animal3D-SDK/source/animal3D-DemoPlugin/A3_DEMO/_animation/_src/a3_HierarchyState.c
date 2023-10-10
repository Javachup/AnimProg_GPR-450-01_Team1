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

		// MOCAPSEGMENT = a3_HierarchyPose
		// MOCAPHEADER = (???)
		// NODE = a3_SpatialPose

		MOCAPSEGMENT mocapsegment;
		MOCAPHEADER mocapheader;

		size_t read;
		a3ui32 i, j, where;
		a3ui32 pos[8];	// position of next char to write
		char line[8][40];	// store attribute and value
		char buffer[4097];
		a3ui32 section = 0;	// which section is being processed
		NODE *tnode;	// setting up base pos and frames
		a3real** base, ** rot, ** arot, ** trot;
		a3real ang[3], num, den;
		a3boolean eof = false;
		a3ui32 currentnode = mocapheader.numOfSegments;

		// allocate memory for arrays using malloc
		base = (a3real **)malloc(3 * sizeof(a3real *));
		rot = (a3real**)malloc(3 * sizeof(a3real*));
		arot = (a3real**)malloc(3 * sizeof(a3real*));
		trot = (a3real**)malloc(3 * sizeof(a3real*));

		/*for (i = 0; i < 3; i++)
{
			base[i] = (a3real*)malloc(3 * sizeof(a3real));
			rot[i] = (a3real*)malloc(3 * sizeof(a3real));
			arot[i] = (a3real*)malloc(3 * sizeof(a3real));
			trot[i] = (a3real*)malloc(3 * sizeof(a3real));
		}*/

		mocapsegment.header->callib = 1.0f;
		mocapsegment.header->scaleFactor = 1.0f;

		FILE* file = fopen(resourceFilePath, "rb");
		if (file)
		{
			// process the "header" section of the file
			read = fread(buffer, 1, 4096, file);
			buffer[read] = '\0';
			i = strstr(buffer, "[Header]");
			i += strcspn(buffer + i, '\n');
			while (buffer[++i] < 32)
			{
				where = pos[0] = pos[1] = pos[2] = pos[3] = pos[4] = pos[5] = pos[6] = pos[7] = 0;
				// process each line in the header
				while (read && !eof)
				{
					while (i < read && !eof)
					{
						if (buffer[i] == '#' || buffer[i] == '\n')
						{
							// process line
							line[1][pos[1]] = line[0][pos[0]] = '\0';
							if (line[0][0] == '[')
							{
								// body struct has been read and ready to process base positions
								// assign the GLOBAL node to the root pointer
								mocapsegment.root = 0;
								for (j = 0; j < currentnode && !mocapsegment.root; ++j)
								{
									if (strcomp(mocapsegment.nodeList[j]->name, "GLOBAL"))
									{
										mocapsegment.root = mocapsegment.nodeList[j]->children[0];
										mocapsegment.root->parent = 0;
										NODE** temp = (NODE**)malloc(sizeof(NODE*) * mocapheader.numOfSegments);

										a3ui32 m;
										for (m = 0; m < j; ++m)
										{
											temp[m] = mocapsegment.nodeList[m];
										}
										for (m = j + 1; m <= mocapheader.numOfSegments; ++m)
										{
											temp[m - 1] = mocapsegment.nodeList[m];
										}

										for (m = 0; m < mocapheader.numOfFrames; ++m)
										{
											free(mocapsegment.nodeList[j]->froset[m]);
											free(mocapsegment.nodeList[j]->freuler[m]);
										}
										free(mocapsegment.nodeList[j]->froset);
										free(mocapsegment.nodeList[j]->freuler);
										free(mocapsegment.nodeList[j]->scale);
										free(mocapsegment.nodeList[j]->name);
										free(mocapsegment.nodeList[j]->children);
										free(mocapsegment.nodeList[j]);
										free(mocapsegment.nodeList);
										mocapsegment.nodeList = temp;
										--currentnode;
									}
								}
							}

							if (section > 2)
							{
								char temp[40];
								j = 1;
								while ((temp[j - 1] = line[0][j]) && line[0][++j] != ']');
								temp[j - 1] = '\0';
								tnode = 0;
								for (j = 0; j < currentnode && !tnode; ++j)
								{
									if (!strcmp(mocapsegment.nodeList[j]->name, temp))
									{
										tnode = mocapsegment.nodeList[j];
									}
								}

								if (!tnode)
								{
									if (strcmp(temp, "EndOfFile"))
									{
										//strcpy(strerror, "Unknown node has been encountered while processing the frames");
										fclose(file);
										return -1;
									}
									else
									{
										eof = true;
									}
								}
							}
						}
						else if (line[0][0] && line[1][0])
						{
							if (!section)
							{
								if (!ProcessHeader(line, pos))
								{
									fclose(file);
									return -1;
								}
							}
							else if (section == 1)
							{
								if (!ProcessSegments(line, pos))
								{
									fclose(file);
									return -1;
								}
							}
							else if (section == 2)
							{
								// setup the base positions for the segment
								line[7][pos[7]] = line[6][pos[6]] = line[5][pos[5]] = line[4][pos[4]] =
									line[3][pos[3]] = line[2][pos[2]] = line[1][pos[1]] = '\0';
								tnode = 0;
								for (j = 0; j < currentnode && !tnode; ++j)
								{
									if (!strcmp(mocapsegment.nodeList[j]->name, '\n'))
									{
										tnode = mocapsegment.nodeList[j];
									}
								}
								SetupColor(tnode);
								SetupOffset(tnode, (a3real)atof(line[1]) * mocapheader.callib, (a3real)atof(line[2]) *
									mocapheader.callib, (a3real)atof(line[3])* mocapheader.callib);
								tnode->euler[0] = (a3real)atof(line[4]);
								tnode->euler[1] = (a3real)atof(line[5]);
								tnode->euler[2] = (a3real)atof(line[6]);
								tnode->length = (a3real)atof(line[7]) * mocapheader.callib;
							}
							else if (section > 2)
							{
								// process the frame information for tnode
								line[7][pos[7]] = line[6][pos[6]] = line[5][pos[5]] = line[4][pos[4]] =
									line[3][pos[3]] = line[2][pos[2]] = line[1][pos[1]] = '\0';
								a3ui32 frame = atol(line[0]) - 1;
								if (!frame)
								{
									ang[0] = tnode->euler[0] * 0.017453f;
									ang[1] = tnode->euler[1] * 0.017453f;
									ang[2] = tnode->euler[2] * 0.017453f;
									CalcRotationMatrix(ang, base, trot, arot, tnode->euler[0], tnode->euler[1]);
								}

								ang[0] = (a3real)atof(line[4]) * 0.017453f;
								ang[1] = (a3real)atof(line[5]) * 0.017453f;
								ang[2] = (a3real)atof(line[6]) * 0.017453f;
								CalcRotationMatrix(ang, rot, trot, arot, tnode->euler[0], tnode->euler[1]);
								matmult(base, rot, trot, 3, 3);

								// decompose into ZYX
								tnode->freuler[frame][0] = (a3real)atan(trot[1][0] / trot[0][0]);
								if (base[0][0] < 0)
								{
									if (base[1][0] < 0)
									{
										tnode->freuler[frame][0] -= 3.141592f;
									}
									else
									{
										tnode->freuler[frame][0] += 3.141592f;
									}
								}

								num = (a3real)(trot[0][2] * sin(tnode->freuler[frame][0]) - trot[1][2] *
									cos(tnode->freuler[frame][0]));
								den = (a3real)(-trot[0][1] * sin(tnode->freuler[frame][0]) + trot[1][1] *
									cos(tnode->freuler[frame][0]));
								tnode->freuler[frame][2] = (a3real)atan(num / den) * 57.2957795f;
								if (den < 0)
								{
									if (num < 0)
									{
										tnode->freuler[frame][2] -= 180;
									}
									else
									{
										tnode->freuler[frame][2] += 180;
									}
								}

								num = -trot[2][0];
								den = (a3real)(trot[0][0] * cos(tnode->freuler[frame][0]) + trot[1][0] *
									sin(tnode->freuler[frame][0]));
								tnode->freuler[frame][1] = (a3real)atan(num / den) * 57.2957795f;
								if (den < 0)
								{
									if (num < 0)
									{
										tnode->freuler[frame][1] -= 180;
									}
									else
									{
										tnode->freuler[frame][1] += 180;
									}
								}
								tnode->freuler[frame][2] *= 57.2957795f;

								tnode->froset[frame][0] = (a3real)atof(line[1]) * mocapheader.callib;
								tnode->froset[frame][1] = (a3real)atof(line[2]) * mocapheader.callib;
								tnode->froset[frame][2] = (a3real)atof(line[3]) * mocapheader.callib;
								tnode->scale[frame] = (a3real)atof(line[7]);
							}
						}

						// move onto the next line and clear current line information
						j = strstr(buffer + 1, '\n');
						if (j == -1)
						{
							if (buffer[4095] != 10)
							{
								read = fread(buffer, 1, 4096, file);
								i = strstr(buffer, '\n');
							}
							else
							{
								read = fread(buffer, 1, 4096, file);
								i = 0;
							}
							buffer[4096] = '\0';
						}
						else
						{
							i += j;
						}
						where = pos[0] = pos[1] = pos[2] = pos[3] = pos[4] = pos[5] = pos[6] = pos[7] = 0;
					}

					if (buffer[i] > 44 && buffer[i] < 123)
					{
						line[where][pos[where]++] = buffer[i++];
					}
					else if ((buffer[i] == 32 || buffer[i] == 9) && pos[where] > 0) // && (where == 0 || section > 1)
					{
						++where;
						++i;
					}
					else
					{
						++i;
					}

					read = fread(buffer, 1, 4096, file);
					buffer[4096] = '\0';
					i = 0;
				}

				mocapheader.degrees = true;
				mocapheader.euler[0][0] = mocapheader.euler[0][1] = 0; 
				mocapheader.euler[0][2] = 1;
				mocapheader.euler[1][0] = mocapheader.euler[1][2] = 0;
				mocapheader.euler[1][1] = 1;
				mocapheader.euler[2][1] = mocapheader.euler[2][2] = 0;
				mocapheader.euler[2][0] = 1;

				fclose(file);
				for (i = 0; i < 3; i++)
				{
					free(base[i]);
					free(rot[i]);
					free(arot[i]);
					free(trot[i]);
				}

				// free dynamically allocated memory when done
				free(base);
				free(rot);
				free(arot);
				free(trot);
				return 1;
			}
		}
		else
		{
			//strcpy(strerror, "Cannot open file");
			return -1;
		}
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
