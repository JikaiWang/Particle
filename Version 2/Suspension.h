#pragma once

#ifndef Suspension_H
#define Suspension_H

#define NUM_THREADS 4

#include "Basics.h"
#include "Render.h"
#include "Export.hpp"


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
	double sys_w = 200;
	double sys_h = 200;
	double cellsize = 1;
	double sedv = 0.0;
	int cutoffCycle = 1000;
	int accumulated_cycle = 0;
	double active_portion = 1;
	bool reversibility;
	int num = 0;
	bool pauseforRender = false;
	bool pauseforShear = true;
	bool pause = false;
	bool next_frame = false;

	vector<par_info> particle;
	render_info renderInfo = {
		sys_w,
		sys_h,
		&pauseforRender,
		&pauseforShear,
		&pause,
		&next_frame,
		&particle
	};


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


