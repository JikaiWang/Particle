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

#ifdef _WIN32
#define WIN true
#endif

#define INCREMENT 0.0001

using namespace std;

struct par_info
{
	double x; // coordinates
	double y;
	int tag; // active
	int pretag; // active in last cycle
    float accutag = 0.0f; // for rendering purposes
	int hash; // hash based on x-y coordinates
    int type = 0;
    int num_kick = 0; // number of kicks during shear
    double connectivity = 0; // area fraction of surrounding particles
	double connectedNum = 0; // number of surrounding particles
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
	double radius;
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

void connectcheck(
	//check single cell
	vector<par_info> *particle,
	grid_info *grid,
	double radius,
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

struct nodeProperty
{
	double connectedNumber;
	double connectedArea;
	double clusteringCoefficient = 0;
	double clusteringCoefficientWeighted = 0;
	double betweenness = 0;
	double betweennessWeighted = 0;
};

class AdjacencyMatrix
{
public:
	double **mat;
	int dimension;
	vector<nodeProperty> graph;

	AdjacencyMatrix(int dim);
	~AdjacencyMatrix();

	void setValue(int i, int j, double value);
	void addProperty();
};

double angle(double dist, double diameter, double r);
double integral(double l, double r, double epsilon, double dist, double diameter, bool flag);
double probability(double dist, double diameter, double epsilon);

#endif // !Basics_H
