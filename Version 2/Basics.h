#pragma once

#ifndef Basics_H
#define Basics_H

#include <vector>

using namespace std;

struct par_info
{
	double x;
	double y;
	int tag;
	int pretag;
	int hash;
};

struct grid_info
{
	int offset;
	int count;
};

int offset(int y, int x, int ydimension, int xdimension);

struct less_than_key
{
	inline bool operator() (const par_info& par1, const par_info& par2)
	{
		return (par1.hash < par2.hash);
	}
};

void cellcheck(vector<par_info> *particle, grid_info *grid,
	int y, int x, int HEIGHT, int WIDTH);



#endif // !Basics_H
