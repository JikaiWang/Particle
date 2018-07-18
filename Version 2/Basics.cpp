#include "Basics.h"

int offset(int y, int x, int ydimension, int xdimension)
{
	// numpy style indexing
	return (y - (y / ydimension) * ydimension) * xdimension
		+ x - (x / xdimension) * xdimension;
}


void cellcheck(vector<par_info> *particle, grid_info *grid,
	int y, int x, int HEIGHT, int WIDTH)
{
	bool pre_active = false;
	vector<par_info> buffer; // Partilce to test
	grid_info cell[4]; // adjacent cells

					   // Load cells
	for (int i = 0; i < 4; ++i) {
		cell[i] = grid[offset(y + i / 2, x + i % 2, HEIGHT, WIDTH)];
		for (int j = 0; j < cell[i].count; ++j)
			buffer.push_back((*particle)[cell[i].offset + j]);
	}

	// Return when no need to check
	if (buffer.empty()) return; // return when no particle
	for (auto par = buffer.begin(); par != buffer.end(); ++par)
		if (par->pretag > 0) pre_active = true; // check pretag
	if (!pre_active) return; // return when previously not active

							 // Collision check (recurrent)
	for (auto target = buffer.begin(); target != buffer.end(); ++target) {
		auto par = target + 1;
		for (int i = 0; i < buffer.size() - 1; ++i) {
			if (par == buffer.end()) par = buffer.begin();
			if ((target->x - par->x)*(target->x - par->x)
				+ (target->y - par->y)*(target->y - par->y) < 1)
				target->tag = 1;
			++par;
		}
	}

	// Copy result back
	auto target = buffer.begin();
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < cell[i].count; ++j)
			(*particle)[cell[i].offset + j] = *target++;
	}
}