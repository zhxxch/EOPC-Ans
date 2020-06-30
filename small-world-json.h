#include <string.h>
#include <stdio.h>
int js_node_header(FILE *out_stream) {
	return fprintf(out_stream, "var nodes=[\n");
}
int js_node_footer(FILE *out_stream) {
	return fprintf(out_stream, "];\n");
}
int js_edge_header(FILE *out_stream) {
	return fprintf(out_stream, "var edges=[\n");
}
int js_edge_footer(FILE *out_stream) {
	return fprintf(out_stream, "];\n");
}
int js_node(FILE *out_stream, const int nodeID) {
	return fprintf(out_stream, "{id:%i},\n", nodeID);
}
int js_edge(FILE *out_stream, const int srcID,
	const int dstID) {
	return fprintf(out_stream,
		"{source:%i,target:%i},\n", srcID, dstID);
}
int HasNode(const int EdgesA[], const int EdgesB[],
	const int NumEdges, const int nodeID);
int print_graph_js(FILE *out_stream, const int EdgesA[], const int EdgesB[], const int NumNodes, const int NumEdges){
	js_node_header(out_stream);
	for(int i = 0; i < NumNodes; i++) {
		if(HasNode(
			   EdgesA, EdgesB, NumEdges, i)) {
			js_node(out_stream, i);
		}
	}
	js_node_footer(out_stream);
	js_edge_header(out_stream);
	for(int i = 0; i < NumEdges; i++) {
		js_edge(out_stream, EdgesA[i], EdgesB[i]);
	}
	js_edge_footer(out_stream);
	return 0;
}