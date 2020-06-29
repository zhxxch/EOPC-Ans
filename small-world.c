#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stat-synth.h"

int HasNode(const int EdgesA[], const int EdgesB[],
	const int NumEdges, const int nodeID) {
	if(NumEdges == 0) return 0;
	if(*EdgesA == nodeID || *EdgesB == nodeID)
		return 1;
	return HasNode(
		EdgesA + 1, EdgesB + 1, NumEdges - 1, nodeID);
}
int AddNode(int EdgesA[], int EdgesB[],
	const int NumEdges, const int nodeID) {
	if(NumEdges == 0) {
		EdgesA[0] = nodeID;
		EdgesB[0] = nodeID;
		return 1;
	}
	const int B = EdgesB[0];
	EdgesB[0] = nodeID;
	EdgesA[NumEdges] = nodeID;
	EdgesB[NumEdges] = B;
	return NumEdges + 1;
}
int AddEdges(int EdgesA[], int EdgesB[],
	const int NumEdges, const int nodeA,
	const int nodeB) {
	EdgesA[NumEdges] = nodeA;
	EdgesB[NumEdges] = nodeB;
	return NumEdges + 1;
}
int GetNodesIter(const int EdgesA[],
	const int EdgesB[], const int NumEdges,
	const int minID, const int maxID) {
	if(minID >= maxID) return maxID;
	if(HasNode(EdgesA, EdgesB, NumEdges, minID)) {
		return minID;
	}
	return GetNodesIter(
		EdgesA, EdgesB, NumEdges, minID + 1, maxID);
}
int GetNodeEdgeIter(const int EdgesA[],
	const int EdgesB[], const int NumEdges,
	const int nodeID, const int minEdge) {
	if(minEdge >= NumEdges) return NumEdges;
	if(EdgesA[minEdge] == nodeID
		|| EdgesB[minEdge] == nodeID)
		return minEdge;
	return GetNodeEdgeIter(
		EdgesA, EdgesB, NumEdges, nodeID, minEdge + 1);
}
int GetNeighbor(const int EdgesA[], const int EdgesB[],
	const int nodeID, const int EdgeID) {
	const int nodeA = EdgesA[EdgeID];
	const int nodeB = EdgesB[EdgeID];
	return (nodeA == nodeID) ? nodeB : nodeA;
}
int main(int ac, char *av[]) {
	const int MaxEdges = 10000;
	int *EdgesA = calloc(sizeof(int), MaxEdges);
	int *EdgesB = calloc(sizeof(int), MaxEdges);
	int NumEdges = 0;
	for(int i = 1; i < 20; i++) {
		NumEdges
			= AddNode(EdgesA, EdgesB, NumEdges, i);
	}
	for(int i = 0; i < 30; i++) {
		const int edgeID = GetNodesIter(
			EdgesA, EdgesB, NumEdges, i, 30);
		if(edgeID < NumEdges) {
			printf("Node at edge %i-%i\n",
				EdgesA[edgeID], EdgesB[edgeID]);
		}
	}
	return 0;
}