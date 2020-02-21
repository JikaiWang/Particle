#include "Grid.hpp"

basic::Grid::Grid(
	double _cellSizeX, 
	double _cellSizeY, 
	int _numCellinX, 
	int _numCellinY,
	vector<par_info> *_particle
)
{
	cellSizeX = _cellSizeX;
	cellSizeY = _cellSizeY;
	numCellinX = _numCellinX;
	numCellinY = _numCellinY;
	grids = new grid_info[numCellinX * numCellinY];
}

basic::Grid::~Grid()
{
    delete[] grids;
}

int basic::Grid::offset(int index)
{
	return grids[index].offset;
}

int basic::Grid::count(int index)
{
	return grids[index].count;
}

void basic::Grid::update()
{
	int count;
	int pre_hash;

	// Generate hash table
	for (auto &par : particle)
		par.hash = int(par.x / cellSizeX) + int(par.y / cellSizeY) * numCellinX;

	// Sort by key using radix sort
	sort(particle.begin(), particle.end(), less_than_key());

	// Get unique count and location
	for (int i = 0; i < numCellinY * numCellinX; ++i)
		grids[i].offset = grids[i].count = 0;
	pre_hash = particle.begin()->hash;
	count = 1;
	grids[pre_hash].offset = 0;
	for (int i = 1; i < particle.size(); ++i)
	{
		if (particle[i].hash == pre_hash) ++count;
		else {
			grids[pre_hash].count = count;
			pre_hash = particle[i].hash;
			grids[pre_hash].offset = i;
			count = 1;
		}
	}
	grids[pre_hash].count = count;
}
