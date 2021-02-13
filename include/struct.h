#include <vector>
#include <string>
using namespace std;

//---	STRUCT DEF
struct resource_t {
	//unsigned int id;		//it's the same as index in vector
	string name;
	unsigned int stored;
};

struct craft_t{
	//unsigned int id;		it's the same as index in vector
	string name;
	vector<unsigned int> input;
	vector<unsigned int> in_amount;
	vector<unsigned int> output;
	vector<unsigned int> out_amount;
};
struct status_t {
	vector<unsigned int> stock;		//[i] is res_id;
	vector<unsigned int> history;	//[i] us craft_id;
};
struct global_data_t {
	unsigned int n_resources;
	unsigned int n_crafts;
	vector<resource_t> resources;
	vector<craft_t> crafts;
	status_t initial_status;
};

struct VectorHasher {
	size_t operator()(const vector<unsigned int> &V) const {
		int hash = V.size();
		for(auto &i : V)
			hash ^= i + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}
};

struct tree_iteration_statistics_t {
	size_t starvation;
	size_t gain;
	size_t equal;
	size_t exist;
};

//GLOBAL DEFINITION
extern global_data_t global_data;
extern const vector<resource_t> &gl_resources;
extern const vector<craft_t> &gl_crafts;
