#include "Basics.h"
#include "Grid.hpp"
#include "Graph.hpp"

// default parameters
double basic::fraction = 0.37;
double basic::gamma = 0.0;
double basic::epsilon = 0.5;
double basic::diameter = 1.0;
double basic::sys_w = 10;
double basic::sys_h = 10;
double basic::cellsize = 1 + int(gamma);
bool basic::lrPeriodic = true;
bool basic::udPeriodic = true;
vector<par_info> basic::particle = vector<par_info>();

//update internal variable
void basic::init() 
{
	width = int(sys_w / cellsize);
	height = int(sys_h / cellsize);
	cellSizeX = sys_w / double(width);
	cellSizeY = sys_h / double(height);
	if (lrPeriodic) { lr_adjust = 1; }
	else lr_adjust = 0;
	if (udPeriodic) { ud_adjust = 1; }
	else ud_adjust = 0;
	delete grid;
	grid = new Grid(cellSizeX, cellSizeY, width, height, &particle);
}

void basic::update()
{
	grid->update();
}

//index rotation
int basic::offset(int y, int x, int ydimension, int xdimension)
{
	// numpy style indexing
	return (y - (y / ydimension) * ydimension) * xdimension
		+ x - (x / xdimension) * xdimension;
}

//check single cell
void basic::cellcheck(int y, int x)
{
	int gridIndex;
	bool pre_active = false;
	vector<par_info> buffer; // Partilce to test

	// Load cells
	for (int i = 0; i < 4; ++i) {
		gridIndex = offset(y + i / 2, x + i % 2, height, width);
		for (int j = 0; j < grid->count(gridIndex); ++j)
            buffer.push_back(particle[grid->offset(gridIndex) + j]);
	}

	// shift particle near the boundary
	if (x == width - 1)
		for (auto &par : buffer)
			if (par.x < cellSizeX) par.x += sys_w;
	if (y == height - 1)
		for (auto &par : buffer)
			if (par.y < cellSizeY) par.y += sys_h;

	// Return when no need to check
	if (buffer.empty()) return;  // return when no particle
	for (auto const &par : buffer)  // check pretag
		if (par.pretag > 0) pre_active = true; 
	if (!pre_active) return;  // return when previously not active

	// Collision check (recurrent)
	for (auto target = buffer.begin(); target != buffer.end(); ++target) {
		auto par = target + 1;
		for (int i = 0; i < buffer.size() - 1; ++i) {
			if (par == buffer.end()) par = buffer.begin();
			if ((target->x - par->x)*(target->x - par->x)
				+ (target->y - par->y)*(target->y - par->y) <
				diameter * diameter)
				target->tag += 1;
			if ((target->x - par->x)*(target->y - par->y) < 0) {
				if (abs(target->x - par->x) < sqrt(diameter * diameter 
					- (target->y - par->y)*(target->y - par->y)) 
					+ abs(target->y - par->y) * gamma)
					target->tag += 1;
			}
			++par;
		}
	}

	// restore particle position near the boundary
	if (x == width - 1)
		for (auto &par : buffer)
			if (par.x > sys_w) par.x -= sys_w;
	if (y == height - 1)
		for (auto &par : buffer)
			if (par.y > sys_h) par.y -= sys_h;

	// Copy result back
	auto target = buffer.begin();
	for (int i = 0; i < 4; ++i) {
		gridIndex = offset(y + i / 2, x + i % 2, height, width);
		for (int j = 0; j < grid->count(gridIndex); ++j)
			particle[grid->offset(gridIndex) + j] = *target++;
	}
}

//Used for multithreading
void basic::batchcheck(int num_threads, int index_thread)
{
	int _height = height + ud_adjust;
	int wpt = (width + lr_adjust - 1) / num_threads + 1; // width per thread

	for (int x = index_thread * wpt; x < (index_thread + 1) * wpt; ++x)
		for (int y = 0; y < _height - 1; ++y)
			if (x < width + lr_adjust - 1)
				cellcheck(y, x);
}


void basic::connectcheck(int y, int x)
{
	int gridIndex;
	double dist;
	vector<par_info> buffer; // Partilce to test

	// Load cells
	for (int i = 0; i < 4; ++i) {
		gridIndex = offset(y + i / 2, x + i % 2, height, width);
		for (int j = 0; j < grid->count(gridIndex); ++j)
			buffer.push_back(particle[grid->offset(gridIndex) + j]);
	}

	// shift particle near the boundary
	if (x == width - 1)
		for (auto &par : buffer)
			if (par.x < cellSizeX) par.x += sys_w;
	if (y == height - 1)
		for (auto &par : buffer)
			if (par.y < cellSizeY) par.y += sys_h;

	// connection check
	for (auto const &target : buffer) {
		for (auto const &par : buffer) {
			dist = (target.x - par.x)*(target.x - par.x)
				+ (target.y - par.y)*(target.y - par.y);
			if (dist < (diameter + epsilon) * (diameter + epsilon)) {
				if (target.id != par.id)
					graph->SetValue(target.id, par.id, 
						probability(pow(dist, 0.5), diameter, epsilon));
			}
		}
	}
}



double basic::angle(double dist, double diameter, double r)
{
	if (r < 1e-9) return 0;
	double result = sqrt(abs(4.0*r*r*diameter*diameter
		- pow(dist*dist - r*r - diameter*diameter, 2))) / (2 * r*dist);
	if (result > 1) {
		return M_PI_2;
	}
	else {
		return asin(result);
	}
}

double basic::integral(double l, double r, double epsilon, double dist, double diameter, bool flag)
{
	double x = l;
	double result = 0;

	if (flag) {
		while (x < r) {
			result += angle(dist, diameter, x) * Increment / (epsilon * M_PI);
			x += Increment;
		}
	}
	else {
		while (x < r) {
			result += (M_PI - angle(dist, diameter, x)) * Increment / (epsilon * M_PI);
			x += Increment;
		}
	}
	return result;
}

double basic::probability(double dist, double diameter, double epsilon)
{
	if (epsilon < 1e-9) return 0;

	if (dist < diameter) {
		if (epsilon < diameter - dist) {
			return 1;
		}
		else if (epsilon < sqrt(diameter * diameter - dist * dist)) {
			return (diameter - dist) / epsilon
				+ integral(diameter - dist, epsilon, epsilon, dist, diameter, false);
		}
		else if (epsilon < diameter + dist) {
			return (diameter - dist) / epsilon
				+ integral(diameter - dist, sqrt(diameter * diameter - dist * dist), epsilon, dist, diameter, false)
				+ integral(sqrt(diameter * diameter - dist * dist), epsilon, epsilon, dist, diameter, true);
		}
		else {
			return (diameter - dist) / epsilon
				+ integral(diameter - dist, sqrt(diameter * diameter - dist * dist), epsilon, dist, diameter, false)
				+ integral(sqrt(diameter * diameter - dist * dist), diameter + dist, epsilon, dist, diameter, true);
		}
	}
	else {
		if (epsilon < dist - diameter) {
			return 0;
		}
		else if (epsilon < dist + diameter) {
			return integral(dist - diameter, epsilon, epsilon, dist, diameter, true);
		}
		else {
			return integral(dist - diameter, dist + diameter, epsilon, dist, diameter, true);
		}
	}
}

basic::basic() {}

basic::~basic() 
{
	delete grid;
}
