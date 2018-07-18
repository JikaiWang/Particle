#pragma once

#ifndef Suspension_H
#define Suspension_H

#define _USE_MATH_DEFINES

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <fstream>
#include <chrono>
#include <math.h>
#include <iterator>
#include <string>


#include "Basics.h"


class suspension
{
public:
	suspension();
	~suspension();

	// parameters
	double fraction = 0.38;
	double epsilon = 0.5;
	bool lrPeriodic = true;
	bool udPeriodic = true;
	double sys_w = 100;
	double sys_h = 100;
	double cellsize = 1;
	double sedv = 0.0;
	int cutoffCycle = 1000;
	int accumulated_cycle = 0;
	double active_portion = 1;
	bool reversibility;
	int num = 0;

	vector<par_info> particle;

	void loadConfig();
	void generateNew();
	void evolve();
	void printInfo();
	void exportPosition();
	
protected:
	int width = int(sys_w / cellsize);
	int height = int(sys_h / cellsize);
	double cellSizeX = sys_w / double(width);
	double cellSizeY = sys_h / double(height);
	int lr_adjust = 0;
	int ud_adjust = 0;
	double angle;
	double disp;
};


#endif // !Suspension_H


