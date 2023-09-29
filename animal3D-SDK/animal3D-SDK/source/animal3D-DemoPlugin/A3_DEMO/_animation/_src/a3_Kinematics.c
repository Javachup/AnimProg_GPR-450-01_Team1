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
	
	a3_Kinematics.c
	Implementation of kinematics solvers.
*/

#include "../a3_Kinematics.h"


//-----------------------------------------------------------------------------

// partial FK solver
a3i32 a3kinematicsSolveForwardPartial(const a3_HierarchyState *hierarchyState, const a3ui32 firstIndex, const a3ui32 nodeCount)
{
	if (hierarchyState && hierarchyState->hierarchy && 
		firstIndex < hierarchyState->hierarchy->numNodes && nodeCount)
	{
		// ****TO-DO: implement forward kinematics algorithm
		//	- for all nodes starting at first index
		a3ui32 i, n, j, p; // i = current index, n = last index, j = current node, p = parent node
		for (i = firstIndex, n = firstIndex + nodeCount; i < n; i++)
		{
			const a3_HierarchyNode* node = hierarchyState->hierarchy->nodes + i;
			j = node->index;
			p = node->parentIndex;

			// If node is not root (has parent node)
			if (p >= 0)
			{
				// Object matrix = parent object matrix * local matrix
				a3real4x4Product(hierarchyState->objectSpace.pose[j].m, // target matrix
					hierarchyState->objectSpace.pose[p].m,
					hierarchyState->localSpace.pose[j].m);
			}
			// else (we are the root)
			else
			{
				// Copy local matrix to object matrix
				a3real4x4SetReal4x4(hierarchyState->objectSpace.pose[j].m, hierarchyState->localSpace.pose[j].m); // set local space as the object space
			}
		}
	}
	return -1;
}


//-----------------------------------------------------------------------------

// partial IK solver
a3i32 a3kinematicsSolveInversePartial(const a3_HierarchyState *hierarchyState, const a3ui32 firstIndex, const a3ui32 nodeCount)
{
	if (hierarchyState && hierarchyState->hierarchy &&
		firstIndex < hierarchyState->hierarchy->numNodes && nodeCount)
	{
		// ****TO-DO: implement inverse kinematics algorithm
		//	- for all nodes starting at first index
		//		- if node is not root (has parent node)
		//			- local matrix = inverse parent object matrix * object matrix
		//		- else
		//			- copy object matrix to local matrix
	}
	return -1;
}


//-----------------------------------------------------------------------------
