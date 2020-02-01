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
#include <sys/stat.h>
#include <complex>
#include <cmath>

using namespace std;

struct par_info
{
	double x;
	double y;
	int tag;
	int pretag;
    float accutag = 0.0f;
	int hash;
    int type = 0;
    int num_kick = 0;
    double connectivity = 0;
    int id;
};

struct grid_info
{
	int offset;
	int count;
};

struct var_info
{
    double box_size;
    double var;
};

struct render_info
{
	double sys_w;
	double sys_h;
    double cellsizeX;
    double cellsizeY;
	bool *p_pauseforRender;
	bool *p_pauseforShear;
	bool *p_pause;
	bool *p_next_frame;
	vector<par_info> *p_particle;
};

int offset(int y, int x, int ydimension, int xdimension); //index rotation

struct less_than_key
{
	inline bool operator() (const par_info& par1, const par_info& par2)
	{
		return (par1.hash < par2.hash);
	}
};

struct less_than_id
{
	inline bool operator() (const par_info& par1, const par_info& par2)
	{
		return (par1.id < par2.id);
	}
};

void cellcheck(
    //check single cell
	vector<par_info> *particle, 
	grid_info *grid,
	int y,
	int x, 
	int HEIGHT,
	int WIDTH,
    double cellsizeY,
    double cellsizeX,
    double sys_h,
    double sys_w,
    double gamma,
    double diameter);


void batchcheck(
    //Used for multithreading
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
    double diameter);



#endif // !Basics_H
