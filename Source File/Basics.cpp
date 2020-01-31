#include "Basics.h"

//index rotation
int offset(int y, int x, int ydimension, int xdimension)
{
	// numpy style indexing
	return (y - (y / ydimension) * ydimension) * xdimension
		+ x - (x / xdimension) * xdimension;
}

//check single cell
void cellcheck(vector<par_info> *particle, grid_info *grid,
	int y, int x, int HEIGHT, int WIDTH, double cellsizeY, double cellsizeX, double sys_h, double sys_w, double gamma, double diameter)
{
//    double testrange[5] = {1.0, 0.95, 0.75, 0.7, 0.5};
//    double testrange[5] = {1.0, 1.0, 1.0, 1.0, 1.0};
//    double Diameter[5] = {1.0/4.0, 1.19/4.0, 1.13/4.0, 1.06/4.0, 1.5/4.0};
    
	bool pre_active = false;
	vector<par_info> buffer; // Partilce to test
	grid_info cell[4]; // adjacent cells

    // Load cells
	for (int i = 0; i < 4; ++i) {
		cell[i] = grid[offset(y + i / 2, x + i % 2, HEIGHT, WIDTH)];
		for (int j = 0; j < cell[i].count; ++j)
			buffer.push_back((*particle)[cell[i].offset + j]);
	}
    
    // shift particle near the boundary
    if (x == WIDTH - 1){
        for (auto par = buffer.begin(); par != buffer.end(); ++par){
            if (par->x < cellsizeX) par->x += sys_w;
        }
    }
    if (y == HEIGHT - 1){
        for (auto par = buffer.begin(); par != buffer.end(); ++par){
            if (par->y < cellsizeY) par->y += sys_h;
        }
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
				+ (target->y - par->y)*(target->y - par->y) <
                diameter * diameter)
				target->tag += 1;
            if ((target->x - par->x)*(target->y - par->y) < 0){
                if (abs(target->x - par->x) < sqrt(diameter * diameter - (target->y - par->y)*(target->y - par->y)) + abs(target->y - par->y) * gamma)
                    target->tag += 1;}
//            if (target->type == par->type)
//            {
//                if ((target->x - par->x)*(target->x - par->x)
//                    + (target->y - par->y)*(target->y - par->y) < testrange[target->type] * testrange[target->type])
//                {
//                    target->tag += 1;
//                }
//            }
			++par;
		}

	}
    
    // restore particle position near the boundary
    if (x == WIDTH - 1){
        for (auto par = buffer.begin(); par != buffer.end(); ++par){
            if (par->x > sys_w) par->x -= sys_w;
        }
    }
    if (y == HEIGHT - 1){
        for (auto par = buffer.begin(); par != buffer.end(); ++par){
            if (par->y > sys_h) par->y -= sys_h;
        }
    }
    
	// Copy result back
	auto target = buffer.begin();
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < cell[i].count; ++j)
			(*particle)[cell[i].offset + j] = *target++;
	}
}

//Used for multithreading
void batchcheck(
	vector<par_info> *particle,
	grid_info *grid,
	int HEIGHT,
	int WIDTH,
    double cellsizeY,
    double cellsizeX,
    double sys_h,
    double sys_w,
	int lr_adjust,
	int ud_adjust,
	int num_threads,
	int index_thread,
    double gamma,
    double diameter)
{
	int height = HEIGHT + ud_adjust;
	int wpt = (WIDTH + lr_adjust - 1) / num_threads + 1; // width per thread

	for (int x = index_thread * wpt; x < (index_thread + 1) * wpt; ++x)
		for (int y = 0; y < height - 1; ++y)
		{
			if (x < WIDTH + lr_adjust - 1)
				cellcheck(particle, grid, y, x, HEIGHT, WIDTH, cellsizeY, cellsizeX, sys_h, sys_w, gamma, diameter);
		}
}
