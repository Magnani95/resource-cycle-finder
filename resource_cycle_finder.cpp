#include <vector>
#include <string>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include <utility>

#include <iostream>
#include <fstream>
#include <stdexcept>

#include <omp.h>
#include "yaml-cpp/yaml.h"
#include "include/debug.hpp"
#include "include/struct.h"
using namespace std;

#define STABLE
#if defined STABLE && defined DEBUG
	#define STORE_DEBUG
	#undef DEBUG
#endif

//TODO Optimize vector.reserve in Craft vectors
global_data_t global_data;
const vector<resource_t> &gl_resources = global_data.resources;
const vector<craft_t> &gl_crafts = global_data.crafts;
//---
//---	FUNCTION DECLARATION
bool file_reader(const char *resource_file_name);
void data_gathering(YAML::Node resource_file);
void tree_generation(vector<status_t> &before_status, vector<status_t> &after_status);

void print_status(status_t s);
void print_craft(craft_t c);
//---	MAIN
int main(int argc, char const *argv[])
{
	if (argc != 2) {
		cout<<"\nOne parameter expected: resourcesFile.yaml\n";
		return -1;
	}
	//global_data_t global_data;

	//Input parser
	const char *resource_file_name =argv[1];
	if(!file_reader(resource_file_name)){
		cout<<"\nError in parsing resourse file. Check its consistency.\n";
		return -1;
	}
	//STEP(, main,After YAML reading)
	unordered_map<string,int> map;

	vector<status_t> before_status, after_status;
	before_status.push_back(global_data.initial_status);

	//setbuf(stdout, NULL);

	//tree_generation(before_status, after_status);

	cout<<endl;

	return 0;
}
//---
//---	FUNCTION DEFINITION
bool file_reader(const char *resource_file_name)
{
	//Allocation
	YAML::Node resource_file = YAML::LoadFile(resource_file_name);
	resource_t new_resource;
	craft_t new_craft;
	vector<resource_t> &resources = global_data.resources;
	vector<craft_t> &crafts = global_data.crafts;
	status_t &initial_status = global_data.initial_status;
	//Craft-only
	unordered_map<string,unsigned int> resource_map;
	string craft_res;
	unsigned int craft_res_id, craft_amount;
	string selector;
	//initialization
	global_data.n_resources=resource_file["resources"].size();
	global_data.n_crafts=resource_file["crafts"].size();
	//mem pre-allocation
	resource_map.rehash(global_data.n_resources);
	resources.reserve(global_data.n_resources);
	crafts.reserve(global_data.n_crafts);
	initial_status.stock.reserve(global_data.n_resources);


	for (size_t i = 0; i < global_data.n_resources ; i++) {
		//new_resource.id=i;
		new_resource.name = resource_file["resources"][i]["name"].as<string>();
		new_resource.stored = resource_file["resources"][i]["stored"].as<unsigned int>();
		initial_status.stock.push_back(new_resource.stored);
		resources.push_back(new_resource);
		resource_map[new_resource.name] = i;
	}

	//STEP(, file_reader, PRE-CRAFTS)
	for (size_t i = 0; i < global_data.n_crafts; i++) {
		for(int s=0; s<=1; s++){
			if(s == 0 ){
				//new_craft.id=i;
				new_craft.name = resource_file["crafts"][i]["name"].as<string>();
				selector="input";
			}else{
				selector="output";
			}
			for (size_t j = 0; j < resource_file["crafts"][i][selector].size(); j++) {
				craft_res = resource_file["crafts"][i][selector][j]["resource"].as<string>();
				craft_amount = resource_file["crafts"][i][selector][j]["amount"].as<unsigned int>();
				try{
					craft_res_id = resource_map.at(craft_res);
				}catch(const out_of_range& oor){
					cout<<"\nError: Invalide input resource in crafts\n["<<new_craft.name<<" - "<<craft_res<<"]\n";
					return false;
				}
				if(s == 0){
					new_craft.input.push_back(craft_res_id);
					new_craft.in_amount.push_back(craft_amount);
				}else{ //(s==1)
					new_craft.output.push_back(craft_res_id);
					new_craft.out_amount.push_back(craft_amount);
				}
			}
		}
		crafts.push_back(new_craft);
		new_craft.input.clear(); new_craft.in_amount.clear();
		new_craft.output.clear(); new_craft.out_amount.clear();
	}

	return true;
}

void tree_generation(vector<status_t> &before_status, vector<status_t> &after_status)
{
	status_t actual_status;
	enum iteration_result_e{CONTINUE, EQUAL, GAIN, STARVATION} iteration_result;
	//set<vector<unsigned int>> visited_status;
	unordered_set<vector<unsigned int>,VectorHasher> visited_status;
	//tree_iteration_statistics_t statistics {0,0,0,0};
	while (before_status.size()) {
		//PRINT(before_status.size(),NEW ITERATION)
		//cout<<endl<<before_status.size();
		iteration_result=CONTINUE;
		//statistics.starvation=0;
		//statistics.gain=0;
		//statistics.equal=0;
		//statistics.exist=0;
		//STATUS-LOOP
		#pragma omp parallel for schedule(static)num_threads(12) firstprivate(actual_status,iteration_result)
		for (size_t n_status = 0; n_status < before_status.size() ; n_status++) {
			//CRAFT
			//MESSAGE("POSTLOOP")
			for (size_t n_craft = 0; n_craft < global_data.n_crafts; n_craft++) {
				actual_status = before_status[n_status];
				iteration_result=CONTINUE;
				//input
				//PRINTF("\nIT:\t(%lu) \t[%d]%lu:%lu", before_status.size(),omp_get_thread_num(),n_status,n_craft);
				for (size_t res = 0; res < global_data.crafts[n_craft].input.size(); res++) {
					if(actual_status.stock[global_data.crafts[n_craft].input[res]] < gl_crafts[n_craft].in_amount[res]){
						//PRINTF("\tSTARVATION");
						iteration_result=STARVATION;
						break;
					}else{
						actual_status.stock[global_data.crafts[n_craft].input[res]] -= gl_crafts[n_craft].in_amount[res];
					}
				} //INPUT DONE
				if(iteration_result==STARVATION){
					//statistics.starvation++;
					continue; //EXIT FROM LOCAL CRAFT
				}
				//output
				for (size_t res = 0; res < global_data.crafts[n_craft].output.size(); res++){
					actual_status.stock[gl_crafts[n_craft].output[res]] += gl_crafts[n_craft].out_amount[res];
				}
				// Known status insert
				if( visited_status.contains(actual_status.stock)){
					//MESSAGE("ALREADY VISITED")
					//statistics.exist++;
					continue;
				}else{
					actual_status.history.push_back(n_craft);
					#pragma omp critical
					{
						visited_status.insert(actual_status.stock);
					}
				}

				//GAIN check
				for (size_t res = 0; res < global_data.initial_status.stock.size(); res++) {
					if(actual_status.stock[res] < global_data.initial_status.stock[res]){
						//PRINTF("[CONTINUE]");
						iteration_result=CONTINUE;
						break; //EXIT FROM GAIN CHECK
					}else if(actual_status.stock[res] == global_data.initial_status.stock[res]){
						//PRINTF("[EQUAL]");
						iteration_result = (iteration_result==GAIN)? GAIN:EQUAL;
					} else{ //(higher than)
						iteration_result=GAIN;
						//PRINTF("[GAIN]");
					}

				}//END GAIN check
				if (iteration_result == CONTINUE){
					//PRINTF("\tCONTINUE");
					//print_status(actual_status);
					#pragma omp critical
					{
						after_status.push_back(actual_status);
					}
					continue;
				}else if(iteration_result == EQUAL){
				//	PRINTF("\tEQUAL");
					//statistics.equal++;
					continue;
				}else if(iteration_result == GAIN){
				//	PRINTF("\tGAIN");
					//statistics.gain++;
					continue;
				}else{
					printf("\nError in tree_generation():impossible iteration_result branch\n");
					//PRINT(iteration_result,)
					exit(-99);
				}
			}//END CRAFT
		}//END STATUS-LOOP
		//next iteration commands
		//cout<<"\t"<<statistics.starvation<<":"<<statistics.gain<<":"<<statistics.equal<<":"<<statistics.exist;
		before_status=move(after_status);
	}//END WHILE LOOP
}
//--- SECONDARY FUNCTIONS
void print_status(status_t status)
{
	cout<<"\n"<<"Stock:\t";
	for (size_t i = 0; i < status.stock.size(); i++) {
		if(i != status.stock.size()-1)
			cout<<status.stock[i]<<":";
		else
			cout<<status.stock[i];
	}
}
void print_craft(craft_t craft)
{
	cout<<"\nCraft\t"<<craft.name;
	//Print Crafts
	cout<<"\n\tIN";
	for (size_t j = 0; j < craft.input.size(); j++)
		cout<<":"<<craft.input[j]<<"("<<craft.in_amount[j]<<")";
	cout<<"\n\tOUT";
	for (size_t j = 0; j < craft.input.size(); j++)
		cout<<":"<<craft.output[j]<<"("<<craft.out_amount[j]<<")";

}

#if defined STORE_DEBUG && !defined DEBUG
	#define DEBUG
	#undef STORE_DEBUG
#endif
