#include "struct.h"
using namespace std;

struct graph_node_t{
	unsigned int craft;
	vector<unsigned int> resource;
};

class ResourceGraph {
private:
	vector<vector<graph_node_t>> graph;
	//FUNCTION
	void create_graph();
public:
	ResourceGraph();
	virtual ~ResourceGraph ();

};
