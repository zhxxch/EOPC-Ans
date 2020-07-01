#include <string.h>
#include <stdio.h>
#include "Eigen/Dense"
int js_edge_header(FILE *out_stream) {
	return fprintf(out_stream, "var edges=[\n");
}
int js_edge_footer(FILE *out_stream) {
	return fprintf(out_stream, "];\n");
}
int js_nodes(FILE *out_stream, const int NumNodes) {
	fprintf(out_stream, "var nodes=[\n");
	for(int i = 0; i < NumNodes; i++) {
		fprintf(out_stream, "{id:\"%i\"},\n", i);
	}
	fprintf(out_stream, "];\n");
	return 0;
}
template<typename T>
int js_edges(FILE *out_stream,
	const Eigen::DenseBase<T> &edges,
	const int NumNodes) {
	for(int dst = 0; dst < NumNodes; dst++) {
		for(int src = dst + 1; src < NumNodes; src++) {
			if(edges(dst, src) < INFINITY)
				fprintf(out_stream,
					"{source:\"%i\",target:\"%i\"},\n",
					src, dst);
		}
	}
	return 0;
}
template<typename T>
int print_graph_js(FILE *out_stream,
	const Eigen::DenseBase<T> &edges,
	const int NumNodes) {
	js_nodes(out_stream, NumNodes);
	js_edge_header(out_stream);
	js_edges(out_stream, edges, NumNodes);
	js_edge_footer(out_stream);
	return 0;
}