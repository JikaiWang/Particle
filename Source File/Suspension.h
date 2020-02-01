#pragma once

#ifndef Suspension_H
#define Suspension_H

#define NUM_THREADS 2
#define NUM_MOV_AVG_BASE 1000000
#define NUM_MOV_AVG_CURR 500000
#define T_INTERVAL 50000

#include "Basics.h"
#include "Render.hpp"
#include "Export.hpp"
#ifdef __linux__
#define CLUSTER true
#else
#define CLUSTER false
#include "VideoWriter.hpp"
#endif

class suspension
{
public:
	suspension();
	~suspension();

	// parameters
    double fraction = 0.37;
    double gamma = 0.0;
	double epsilon = 0.5;
    double diameter = 1.0;
	bool lrPeriodic = true;
	bool udPeriodic = false;
    bool initialOverlap = true;
	double sys_w = 10.8;
    double sys_h = 10.8;
    double cellsize = 1 + int(gamma); //Griding unit size
	double sedv = 0.0;
	int cutoffCycle = 1;
    int cutoffMeasurement = 20;
    double top_blank = 0.2;
    double fact = 0.0;
    double var = 0.0;

    //Status
	int accumulated_cycle = 0;
	double active_portion = 1;
	bool reversible;
	int num = 0; //Number of particles
    chrono::high_resolution_clock timer;
    
    //Container for particle position and status
	vector<par_info> particle;
    vector<var_info> variance;
    
    //Rendering status API
    bool pauseforRender = false;
    bool pauseforShear = true;
    bool pause = false;
    bool next_frame = false;
	render_info renderInfo = {
		sys_w,
		sys_h,
        0,
        0,
		&pauseforRender,
		&pauseforShear,
		&pause,
		&next_frame,
		&particle
	};

	void loadConfig();
    void updateInternalParam();
	void generateNew();
	void evolve();
	void printInfo();
    void varianceNum();
    void structuralFactor();
    void exportDensityXY();
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


