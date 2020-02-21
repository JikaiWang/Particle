#pragma once

#include <vector>
#include "Basics.h"



// List representation of graph
class basic::adjacencyList
{
protected:
	struct Edge {
		int node;
		double weight;
	};

private:
	std::vector<Edge> *p_list_;
	int dimension_;

public:
	adjacencyList(size_t size);
	adjacencyList(const basic::adjacencyList &p);
	basic::adjacencyList &operator = (const basic::adjacencyList &p);
	~adjacencyList();

	int Size();  // return container size
	void SetValue(int i, int j, double value);
	double GetValue(int i, int j);
	void DeleteNode(int i);
	vector<int> ScanNode(int i);  // return a list of node index
};



// Matrix representation of graph
class basic::adjacencyMatrix
{
private:
	double **p_mat_;
	int dimension_;

public:
	adjacencyMatrix(int dim);
	adjacencyMatrix(const basic::adjacencyMatrix &p);
	basic::adjacencyMatrix &operator = (const basic::adjacencyMatrix &p);
	~adjacencyMatrix();

	int Size();  // return container size
	void SetValue(int i, int j, double value);
	double GetValue(int i, int j);
	void DeleteNode(int i);
	vector<int> ScanNode(int i);  // return a list of node index
};
