#pragma once

#define _USE_MATH_DEFINES
#include <iostream>
#include <math.h>
#include <vector>
//#include <thread>
#include <iomanip>
#include <algorithm>
#include <random>
#include <fstream>
#include <chrono>
#include <iterator>
//#include <string>
//#include <sys/stat.h>
//#include <complex>
//#include <cmath>

#include "Struct.hpp"

#ifdef _WIN32
#define WIN true
#endif


using namespace std;

class basic // support for shearing function
{
public:
	// simulation parameters
	static double fraction;
	static double gamma;
	static double epsilon;
	static double diameter;
	static double sys_w ;
	static double sys_h ;
	static double cellsize; //Griding unit size
	static bool lrPeriodic;
	static bool udPeriodic;

	struct var_info { double box_size; double var; };

	//Used for multithreading
	void batchcheck(int num_threads, int index_thread);

	//update internal variable
	void init();
	void update();

	basic();
	~basic();

protected:
	// support class
	class adjacencyMatrix;
	class adjacencyList;

	adjacencyList *graph;

	int height;
	int width;
	double cellSizeY;
	double cellSizeX;
	int lr_adjust;
	int ud_adjust;
	double radius;

	//Container for particle position and status
	static vector<par_info> particle;

	//index rotation
	int offset(int y, int x, int ydimension, int xdimension);

	//check single cell
	void cellcheck(int y, int x); 

	//check single cell
	void connectcheck(int y, int x);


private:
	const double Increment = 0.0001;  // step size for numerical integration

	double angle(double dist, double diameter, double r);
	double integral(double l, double r, double epsilon, double dist, double diameter, bool flag);
	double probability(double dist, double diameter, double epsilon);

	class Grid;
	Grid *grid = nullptr;
};
