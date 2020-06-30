#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stat-synth.h"
#include "small-world-json.h"
typedef int invalid_id_t;
int HasNode(const int EdgesA[], const int EdgesB[],
	const int NumEdges, const int nodeID) {
	if(NumEdges == 0) return 0;
	if(*EdgesA == nodeID || *EdgesB == nodeID)
		return 1;
	return HasNode(
		EdgesA + 1, EdgesB + 1, NumEdges - 1, nodeID);
}
typedef int node_list_len_t;
node_list_len_t AddNode(int EdgesA[], int EdgesB[],
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
typedef int edge_list_len_t;
edge_list_len_t AddEdges(int EdgesA[], int EdgesB[],
	const int NumEdges, const int nodeA,
	const int nodeB) {
	EdgesA[NumEdges] = nodeA;
	EdgesB[NumEdges] = nodeB;
	return NumEdges + 1;
}
typedef int node_id_t;
node_id_t GetNodesIter(const int EdgesA[],
	const int EdgesB[], const int NumEdges,
	const int minID, const int maxID) {
	if(minID >= maxID) return (invalid_id_t)maxID;
	if(HasNode(EdgesA, EdgesB, NumEdges, minID)) {
		return minID;
	}
	return GetNodesIter(
		EdgesA, EdgesB, NumEdges, minID + 1, maxID);
}
typedef int edge_id_t;
edge_id_t GetNodeEdgeIter(const int EdgesA[],
	const int EdgesB[], const int NumEdges,
	const int nodeID, const int minEdge) {
	if(minEdge >= NumEdges)
		return (invalid_id_t)NumEdges;
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
double Metric(const int EdgesA[], const int EdgesB[],
	const int nodeA, const int nodeB,
	const int NumEdges, int metric_pass_node[]) {
	double min_metric = INFINITY;
	int pass_node = -1;
	for(int min_edge_id = 0; min_edge_id < NumEdges;) {
		const int edge_id = GetNodeEdgeIter(EdgesA,
			EdgesB, NumEdges, nodeA, min_edge_id);
		const int neighbor_node = GetNeighbor(
			EdgesA, EdgesB, nodeA, edge_id);
		if(neighbor_node == nodeB) {
			*metric_pass_node = neighbor_node;
			return 1.0;
		}
		const double metric = 1.0
			+ Metric(EdgesA, EdgesB, neighbor_node,
				nodeB, NumEdges, metric_pass_node + 1);
		if(metric < min_metric) {
			min_metric = metric;
			pass_node = neighbor_node;
		}
	}
	*metric_pass_node = pass_node;
	return min_metric;
}
int AddRadiusLEdge(int EdgesA[], int EdgesB[],
	const int NumEdges, const int nodeID,
	const double radious, const int maxEdgesZ) {
	return 0;
}
int main(int ac, char *av[]) {
	const int MaxEdges = 10000;
	int *EdgesA = calloc(sizeof(int), MaxEdges);
	int *EdgesB = calloc(sizeof(int), MaxEdges);
	int NumEdges = 0;
	for(int i = 0; i < 20; i++) {
		NumEdges
			= AddNode(EdgesA, EdgesB, NumEdges, i);
	}
	const int Sphere1EdgesNum = NumEdges;
	print_graph_js(stdout, EdgesA,EdgesB,20,Sphere1EdgesNum);
	return 0;
}