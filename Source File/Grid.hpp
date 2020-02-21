#pragma once

#include "Basics.h"
#include <vector>

class basic::Grid
{
public:
	struct grid_info { 
		int offset; 
		int count; 
	};

	Grid(
		double _cellSizeX,
		double _cellSizeY,
		int _numCellinX,
		int _numCellinY,
		vector<par_info> *_particle
	);

	~Grid();

	// return the relative address of a cell
	int offset(int index);

	// return the length of a cell
	int count(int index);

	// update grid after particle changed
	void update();

private:
	grid_info *grids;
	double cellSizeX;
	double cellSizeY;
	int numCellinX;
	int numCellinY;

	struct less_than_key {
		inline bool operator() (
			const par_info& par1, 
			const par_info& par2) {
			return (par1.hash < par2.hash);
		}
	};
};






