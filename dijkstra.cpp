#include<iostream>
#include<string>
#include"dijkstra.h"
using namespace std;
dijkstra::dijkstra(int graph[5][5], int node_number, int line_number)
{
	this->node_number = node_number;
	this->line_num = line_number;
	for (int i = 0; i < line_num; i++)
	{
		for (int j = 0; j < node_number; j++) {
			this->graph[i][j] = *(*(graph + i) + j);
		}
	}
	for (int i = 0; i < node_number; i++) {
		this->distance_table[i] = -1;
	}
	for (int i = 0; i < node_number; i++) {
		this->pred[i] = -1;
	}
}

dijkstra::~dijkstra()
{
}
list<int> dijkstra::routing(int source,int dest) {
	bool *s = new bool[this->node_number];
	//initialize
	this->distance_table[source] = 0;
	for (int i = 0; i < this->node_number; i++) {
		s[i] = i == source ? 1 : 0;
	}
	int source_node = source;//use for iteration
	while (!s[dest])
	{
		for (int i = 0; i < this->node_number; i++) {
			if (this->graph[source_node][i] > 0) {//there is path
				if (!s[i]) {//not the shortest
					int new_path_weight = this->graph[source_node][i] + this->distance_table[source_node];
					if (this->distance_table[i] < 0) {
						this->distance_table[i] = new_path_weight;
						this->pred[i] = source_node;
					}
					else
					{
						if (this->distance_table[i] > new_path_weight) {
							this->distance_table[i] = new_path_weight;
							this->pred[i] = source_node;
						}
					}
				}
			}
		}
		//choose next source_node
		int min_value = 0;
		for (int i = 0; i < this->node_number; i++) {
			if (!s[i]) {
				if (min_value == 0) {
					min_value = this->distance_table[i];
					source_node = i;
				}
				else {
					if (min_value < this->distance_table[i]) {
						min_value = this->distance_table[i];
						source_node = i;
					}
				}
			}
		}
		s[source_node] = 1;
	}
	list<int> path;
	int flag = dest;
	while (flag!=-1) {
		path.push_front(flag);
		flag = this->pred[flag];
	}
	return path;
}