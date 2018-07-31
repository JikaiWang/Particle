#pragma once

#ifndef Basics_H
#define Basics_H

#define _USE_MATH_DEFINES
#include <iostream>
#include <math.h>
#include <vector>
#include <thread>
#include <iomanip>
#include <algorithm>
#include <random>
#include <fstream>
#include <chrono>
#include <iterator>
#include <string>

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

struct render_info
{
	double sys_w;
	double sys_h;
	bool *p_pauseforRender;
	bool *p_pauseforShear;
	bool *p_pause;
	bool *p_next_frame;
	vector<par_info> *p_particle;
};

int offset(int y, int x, int ydimension, int xdimension);

struct less_than_key
{
	inline bool operator() (const par_info& par1, const par_info& par2)
	{
		return (par1.hash < par2.hash);
	}
};

void cellcheck(
	vector<par_info> *particle, 
	grid_info *grid,
	int y,
	int x, 
	int HEIGHT,
	int WIDTH);

void batchcheck(
	vector<par_info> *particle, 
	grid_info *grid,
	int HEIGHT,
	int WIDTH, 
	int lr_adjust, 
	int ud_adjust,
	int num_threads,
	int index_thread);

void check(
	vector<par_info> *particle,
	grid_info *grid,
	int HEIGHT,
	int WIDTH,
	int lr_adjust,
	int ud_adjust,
	int num_threads,
	int index_thread);

#endif // !Basics_H
