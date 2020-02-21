#include "Graph.hpp"

// definition start for class adjacencyList
basic::adjacencyList::adjacencyList(size_t size)
{
	dimension_ = int(size);
	p_list_ = new std::vector<Edge>[dimension_];
}

basic::adjacencyList::adjacencyList(const basic::adjacencyList &p)
{
	dimension_ = p.dimension_;
	p_list_ = new std::vector<Edge>[dimension_];
	for (int i = 0; i < dimension_; i++)
		p_list_[i] = p.p_list_[i];
}

basic::adjacencyList &basic::adjacencyList::operator = (const basic::adjacencyList &p)
{
	dimension_ = p.dimension_;
	p_list_ = new std::vector<Edge>[dimension_];
	for (int i = 0; i < dimension_; i++)
		p_list_[i] = p.p_list_[i];
	return *this;
}

basic::adjacencyList::~adjacencyList()
{
	delete[] p_list_;
}

int basic::adjacencyList::Size()
{
	return dimension_;
}

void basic::adjacencyList::SetValue(int i, int j, double value)
{
	for (auto &node : p_list_[i]) {
		if (node.node == j) {
			node.weight = value;

			// change the other node
			for (auto &node : p_list_[j]) {
				if (node.node == i) {
					node.weight = value;
					return;
				}
			}
		}
	}
	// add node if not found
	p_list_[i].push_back({ j, value });
	p_list_[j].push_back({ i, value });
}

vector<int> basic::adjacencyList::ScanNode(int i)
{
	vector<int> linkedNodes;
	for (auto const &node : p_list_[i])
		linkedNodes.push_back(node.node);
	return linkedNodes;
}

double basic::adjacencyList::GetValue(int i, int j)
{
	for (auto const &node : p_list_[i])
		if (node.node == j)
			return node.weight;
	return -1;
}

void basic::adjacencyList::DeleteNode(int i)
{
	for (auto const &v : p_list_[i])
		for (auto p = p_list_[v.node].begin(); p != p_list_[v.node].end(); p++)
			if (p->node == i) {
				p_list_[v.node].erase(p);
				break;
			}
	p_list_[i].clear();
}


// definition start for class adjacencyMatrix
basic::adjacencyMatrix::adjacencyMatrix(int dim)
{
	dimension_ = dim;
	p_mat_ = new double*[dimension_];
	for (size_t i = 0; i < dimension_; i++)
		p_mat_[i] = new double[dimension_];
	for (size_t i = 0; i < dimension_; i++) {
		for (size_t j = 0; j < dimension_; j++) {
			p_mat_[i][j] = 0.0;
		}
	}
}

basic::adjacencyMatrix::adjacencyMatrix(const basic::adjacencyMatrix &p) 
{
	dimension_ = p.dimension_;
	p_mat_ = new double*[dimension_];
	for (size_t i = 0; i < dimension_; i++)
		p_mat_[i] = new double[dimension_];
	for (size_t i = 0; i < dimension_; i++) {
		for (size_t j = 0; j < dimension_; j++) {
			p_mat_[i][j] = p.p_mat_[i][j];
		}
	}
}

basic::adjacencyMatrix &basic::adjacencyMatrix::operator = (const basic::adjacencyMatrix &p)
{
	dimension_ = p.dimension_;
	p_mat_ = new double*[dimension_];
	for (size_t i = 0; i < dimension_; i++)
		p_mat_[i] = new double[dimension_];
	for (size_t i = 0; i < dimension_; i++) {
		for (size_t j = 0; j < dimension_; j++) {
			p_mat_[i][j] = p.p_mat_[i][j];
		}
	}
	return *this;
}

basic::adjacencyMatrix::~adjacencyMatrix()
{
	for (size_t i = 0; i < dimension_; i++)
		delete[] p_mat_[i];
	delete[] p_mat_;
}

int basic::adjacencyMatrix::Size() 
{
	return dimension_;
}

void basic::adjacencyMatrix::SetValue(int i, int j, double value)
{
	p_mat_[i][j] = value;
}

vector<int> basic::adjacencyMatrix::ScanNode(int i)
{
	vector<int> linkedNodes;
	for (int j = 0; j < dimension_; j++)
		if (p_mat_[i][j] > 0) linkedNodes.push_back(j);
	return linkedNodes;
}

double basic::adjacencyMatrix::GetValue(int i, int j)
{
	return p_mat_[i][j];
}

void basic::adjacencyMatrix::DeleteNode(int i)
{
	for (int j = 0; j < dimension_; j++) {
		p_mat_[i][j] = p_mat_[j][i] = 0.0;
	}
}
