#include "include/ResourceGraph.hpp"

ResourceGraph::ResourceGraph()
{
	create_graph();

}
//PRIVATE

void ResourceGraph::create_graph()
{
	graph_node_t node;
	graph.reserve(gl_resources.size());
	for (size_t i = 0; i < gl_crafts.size(); i++) {
		for (size_t k = 0; k < gl_crafts[i].output.size(); k++) {
			node.craft=i;
			node.resource.reserve(gl_crafts[i].input.size());
			for (size_t j = 0; j < gl_crafts[i].input.size(); j++) {
				node.resource.push_back(gl_crafts[i].input[j]);
			}
			graph[k].push_back(node);
		}
	}
	for (size_t i = 0; i<graph.size() ; i++) {
		graph[i].shrink_to_fit();	//Don't waste memory
	}
}
