#include "network.hpp"
#include "Graph.hpp"

network::network()
{
	double temp = cellsize;
	cellsize = epsilon + diameter;
	init();  // generate grids
	cellsize = temp;
}

network::~network()
{
}

void network::Measure(double alpha)
{
	alpha_ = alpha;
	init();
	update();  // update hash and grids
	graph = new adjacencyList(particle.size());
	Connectivity();
	Load();
	Degree();
	ClusteringCoefficient();
	Closeness();
	Betweenness();
	delete graph;

	// for videoWriter
	for (size_t i = 0; i < graph->Size(); i++) {
		particle[i].connectivity = result[i].betweenness;
	}
}

void network::Load()
{
	result.clear();
	// load particle positions
	for (auto &par : particle)
		result.push_back({ par.x, par.y });
}

void network::Degree()
{
	double degree;
	double strength;
	for (int i = 0; i < graph->Size(); i++) {
		degree = strength = 0.0;
		for (auto &j : graph->ScanNode(i)) {
			strength += graph->GetValue(i, j);
			if (graph->GetValue(i, j)) degree += 1.0;
		}
		result[i].degree = pow(degree, 1 - alpha_) * pow(strength, alpha_);
	}
	avg_degree_ = 0;
	for (auto const &node : result)
		avg_degree_ += node.degree;
	avg_degree_ /= double(result.size());
}

void network::Connectivity()
{
	int index = 0;
	for (auto &par : particle)
		par.id = index++;

	// only work with periodic boundary
	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
			connectcheck(y, x);
}

void network::ClusteringCoefficient()
{
	double numTriplet;
	double strength;
	double numTriangle;
	double weighted;
	double dimension;

	for (int i = 0; i < graph->Size(); i++) {
		numTriplet = 0.0;
		numTriangle = 0.0;
		strength = 0.0;
		dimension = -1.0;
		weighted = 0.0;
		for (auto &j : graph->ScanNode(i)) {
			strength += graph->GetValue(i, j);
			dimension += 1.0;
			for (auto &k : graph->ScanNode(i)) {
				if (j != k) {
					numTriplet += 1.0;
					if (graph->GetValue(j, k) > 0) {
						numTriangle += 1.0;
						weighted += graph->GetValue(i, j);
						weighted += graph->GetValue(i, k);
					}
				}
			}
		}
		if (numTriplet > 0) {
			result[i].clustering_coefficient = numTriangle / numTriplet;
			result[i].clustering_coefficient_weighted
				= 0.5 * weighted / (strength * dimension);
		}
		else {
			result[i].clustering_coefficient = 0;
			result[i].clustering_coefficient_weighted = 0;
		}
	}
}

void network::Closeness()
{
	adjacencyMatrix spt_set = ShortestPath();

	double dist;
	for (int i = 0; i < spt_set.Size(); i++) {
		dist = 0.0;
		for (int j = 0; j < spt_set.Size(); j++) {
			if (i != j && spt_set.GetValue(i, j) > 0) dist += 1 / spt_set.GetValue(i, j);
		}
		result[i].closeness = dist;
	}
	avg_closeness_ = 0;
	for (auto const &node : result)
		avg_closeness_ += node.closeness;
	avg_closeness_ /= double(result.size());
}

void network::Betweenness()
{
	adjacencyMatrix spt_set = ShortestPath();

	int num_on_path;
	int num_connected;
	for (int source = 0; source < graph->Size(); source++)  {
		num_on_path = 0;
		num_connected = 0;
		for (int i = 0; i < graph->Size(); i++) {
			for (int j = i; j < graph->Size(); j++) {
				if (i != source && j != source) {
					if (spt_set.GetValue(i, j) > 0) {
						++num_connected;
						if (spt_set.GetValue(i, j) == spt_set.GetValue(source, i) + spt_set.GetValue(source, j)) {
							++num_on_path;
						}
					}
				}
			}
		}
		result[source].betweenness = double(num_on_path) / double(num_connected);
	}
	avg_betweeness_ = 0;
	for (auto const &node : result)
		avg_betweeness_ += node.betweenness;
	avg_betweeness_ /= double(result.size());
}


basic::adjacencyMatrix network::ShortestPath()
{
	struct Node {
		int id;
		double cost;
	};

	struct CompareCost {
		bool operator()(Node const& p1, Node const& p2) {
			return p1.cost > p2.cost;
		}
	};

	int count;
	int pre_id;
	bool end_of_queue;
	double pre_cost;
	Node target;
	adjacencyMatrix spt_set(graph->Size());
	adjacencyList spare_set = *graph;
	priority_queue<Node, vector<Node>, CompareCost> p;

	for (int source = 0; source < graph->Size(); source++)
	{
		count = 1;
		spare_set = *graph;  // make a copy
		p = priority_queue<Node, vector<Node>, CompareCost>();
		spt_set.SetValue(source, source, 0.0);  // add source
		pre_id = source;
		pre_cost = 0.0;

		while (count < graph->Size())
		{
			for (auto const &cur_id : spare_set.ScanNode(pre_id)) {
				p.push({ cur_id, pre_cost + pow(spare_set.GetValue(pre_id, cur_id), -alpha_) });
			}
			spare_set.DeleteNode(pre_id);  // delete node added
			if (p.empty()) break;
			end_of_queue = false;
			target = p.top();  // select the node with smallest cost
			while (spt_set.GetValue(source, target.id) > 0) {
				if (p.empty()) {
					end_of_queue = true;
					break;
				}
				p.pop();
				target = p.top();
			}
			if (end_of_queue) break;  // exit when empty queue
			p.pop();  // pop from pq
			spt_set.SetValue(source, target.id, target.cost);  // add node
			pre_id = target.id;
			pre_cost = target.cost;
			++count;
		}
	}

	return spt_set;
}

void network::PrintSystemAverage()
{
	cout << "Average degree: " << avg_degree_ << endl;
	cout << "Average closeness: " << avg_closeness_ << endl;
	cout << "Average betweeness: " << avg_betweeness_ << endl;
}