#pragma once

#define NUM_THREADS 2
#define NUM_MOV_AVG_BASE 1000000
#define NUM_MOV_AVG_CURR 500000
#define T_INTERVAL 50000

#include "Basics.h"
#include "Struct.hpp"
#include "Render.hpp"
#include "Export.hpp"
#include "network.hpp"
#ifdef __linux__
#define CLUSTER true
#else
#define CLUSTER false
#include "VideoWriter.hpp"
#endif


class suspension : public basic
{
public:

	bool initialOverlap = true;
	int cutoffCycle = 1000000;
	int cutoffMeasurement = 20;
	double top_blank = 0.0;
	double sedv = 0.0;
    double fact = 0.0;
    double var = 0.0;

    //Status
	int accumulated_cycle = 0;
	double active_portion = 1;
	bool reversible;
	int num = 0; //Number of particles

    suspension();
    ~suspension();
    
    videoWriter VideoWriter = videoWriter(&Info);
    render Render = render(&Info);
	network Network;

	void loadConfig();
	void generateNew();
	void evolve(); // shearing function
	void evolveLive(); // evolve with live view
	void printInfo();
    void varianceNum();
    void structuralFactor();
    void exportDensityXY();
    void exportPosition();
	// call before shear or after parameter update
	void updateInfo();

private:
	double angle;
	double disp;
	chrono::high_resolution_clock timer;
	vector<var_info> variance;

	//Rendering status API
	bool pauseforRender = false;
	bool pauseforShear = true;
	bool pause = false;
	bool next_frame = false;

	info Info = {
		sys_w,
		sys_h,
		cellSizeX,
		cellSizeY,
		diameter,
		epsilon,
		&pauseforRender,
		&pauseforShear,
		&pause,
		&next_frame,
		&particle,
	};
};


