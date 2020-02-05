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
	int y, int x, int HEIGHT, int WIDTH, double cellsizeY, double cellsizeX, 
	double sys_h, double sys_w, double gamma, double diameter)
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
	if (x == WIDTH - 1) {
		for (auto par = buffer.begin(); par != buffer.end(); ++par) {
			if (par->x < cellsizeX) par->x += sys_w;
		}
	}
	if (y == HEIGHT - 1) {
		for (auto par = buffer.begin(); par != buffer.end(); ++par) {
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
			if ((target->x - par->x)*(target->y - par->y) < 0) {
				if (abs(target->x - par->x) < sqrt(diameter * diameter - (target->y - par->y)*(target->y - par->y)) 
					+ abs(target->y - par->y) * gamma)
					target->tag += 1;
			}
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
	if (x == WIDTH - 1) {
		for (auto par = buffer.begin(); par != buffer.end(); ++par) {
			if (par->x > sys_w) par->x -= sys_w;
		}
	}
	if (y == HEIGHT - 1) {
		for (auto par = buffer.begin(); par != buffer.end(); ++par) {
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


double angle(double dist, double diameter, double r)
{
	if (r < 1e-9) return 0;
	double result = sqrt(abs(4.0*r*r*diameter*diameter - pow(dist*dist - r*r - diameter*diameter, 2))) / (2 * r*dist);
	//cout << dist << '\t' << diameter << '\t' << r << '\t' << result << endl;
	if (result > 1) {
		return M_PI_2;
	}
	else {
		return asin(result);
	}
}

double integral(double l, double r, double epsilon, double dist, double diameter, bool flag)
{
	double x = l;
	double result = 0;

	if (flag)
	{
		while (x < r)
		{
			result += angle(dist, diameter, x) * INCREMENT / (epsilon * M_PI);
			x += INCREMENT;
		}
	}
	else {
		while (x < r)
		{
			result += (M_PI - angle(dist, diameter, x)) * INCREMENT / (epsilon * M_PI);
			x += INCREMENT;
		}
	}
	//cout << result << endl;
	return result;
}

double probability(double dist, double diameter, double epsilon)
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



void connectcheck(vector<par_info> *particle, grid_info *grid, double radius,
	int y, int x, int HEIGHT, int WIDTH, double cellsizeY, double cellsizeX, 
	double sys_h, double sys_w, double gamma, double diameter)
{
	int shift_offset;
	double dist;
	double prob;
	vector<par_info> buffer; // Partilce to test
	vector<par_info> center; // Partilce in the center cell
	grid_info cell[9]; // adjacent cells

	// Load cells
	for (int i = 0; i < 9; ++i) {
		shift_offset = offset(y + i / 3 - 1, x + i % 3 - 1, HEIGHT, WIDTH);
		if (y + i / 3 - 1 < 0) shift_offset += WIDTH * HEIGHT;
		if (x + i % 3 - 1 < 0) shift_offset += WIDTH;
		cell[i] = grid[shift_offset];
		if (i == 4) {
			for (int j = 0; j < cell[i].count; ++j)
				center.push_back((*particle)[cell[i].offset + j]);
		}
		else {
			for (int j = 0; j < cell[i].count; ++j)
				buffer.push_back((*particle)[cell[i].offset + j]);
		}
	}

	// shift particle near the boundary
	if (x == WIDTH - 1) {
		for (auto par = buffer.begin(); par != buffer.end(); ++par) {
			if (par->x < cellsizeX) par->x += sys_w;
		}
	}
	if (y == HEIGHT - 1) {
		for (auto par = buffer.begin(); par != buffer.end(); ++par) {
			if (par->y < cellsizeY) par->y += sys_h;
		}
	}
	if (x == 0) {
		for (auto par = buffer.begin(); par != buffer.end(); ++par) {
			if (par->x > sys_w - cellsizeX) par->x -= sys_w;
		}
	}
	if (y == 0) {
		for (auto par = buffer.begin(); par != buffer.end(); ++par) {
			if (par->y > sys_h - cellsizeY) par->y -= sys_h;
		}
	}

	// Return when no need to check
	//if (buffer.empty()) return; // return when no particle

	// connection check
	// adjacent cells
	for (auto target = center.begin(); target != center.end(); ++target) {
		for (auto par = buffer.begin(); par != buffer.end(); ++par) {
			dist = (target->x - par->x)*(target->x - par->x)
				+ (target->y - par->y)*(target->y - par->y);
			if (dist < radius * radius) {
				target->connectedNum += 1;
				target->connectivity += probability(dist, diameter, radius - diameter);
			}
		}
	}
	// self-check
	for (auto target = center.begin(); target != center.end(); ++target) {
		for (auto par = center.begin(); par != center.end(); ++par) {
			dist = (target->x - par->x)*(target->x - par->x)
				+ (target->y - par->y)*(target->y - par->y);
			if (dist < radius * radius) {
				target->connectedNum += 0.5;
				par->connectedNum += 0.5;
				prob = probability(dist, diameter, radius - diameter) * 0.5;
				target->connectivity += prob;
				par->connectivity += prob;
			}
		}
	}

	// Copy result back
	auto target = center.begin();
	for (int j = 0; j < cell[4].count; ++j) {
		(*particle)[cell[4].offset + j] = *target++;
	}
}


AdjacencyMatrix::AdjacencyMatrix(int dim)
{
	dimension = dim;
	mat = new double*[dim];
	for (size_t i = 0; i < dim; i++)
		mat[i] = new double[dim];
}

AdjacencyMatrix::~AdjacencyMatrix()
{
	for (size_t i = 0; i < dimension; i++)
		delete[] mat[i];
	delete[] mat;
}

void AdjacencyMatrix::setValue(int i, int j, double value)
{
	mat[i][j] = value;
}

void AdjacencyMatrix::addProperty()
{
	;
}


