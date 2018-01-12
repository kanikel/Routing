#pragma once
#include<iostream>
#include<string>
#include<list>
using namespace std;
const int Max_num=100;
class dijkstra
{
public:
	dijkstra(int graph[5][5],int node_number,int line_number);
	list<int> routing(int source,int dest);
	~dijkstra();

private:
	int graph[Max_num][Max_num];
	int distance_table[Max_num];
	int pred[Max_num];
	int node_number, line_num;

};
