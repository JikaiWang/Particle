#pragma once
#include "Basics.h"
#include <queue>

class network : public basic 
{
public:
	struct NodeProperty
	{
		double x;
		double y;
		double degree = 0;
		double closeness = 0;
		double betweenness = 0;
		double clustering_coefficient = 0;
		double clustering_coefficient_weighted = 0;
	};

    double alpha_ = 1.0;
    
	vector<NodeProperty> result;
	network();
	~network();
	void Measure(double alpha = 1.0);
	void PrintSystemAverage();
    
private:
	double avg_degree_;
	double avg_closeness_;
	double avg_betweeness_;
	double avg_clustering_coefficient_;
	double avg_clustering_coefficient_weighted_ = 0;

	// support method
	void Load();
    void Connectivity();
	void ClusteringCoefficient();
	void Degree();
	void Closeness();
	void Betweenness();
	adjacencyMatrix ShortestPath();
};



