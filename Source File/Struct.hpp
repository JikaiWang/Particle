#pragma once

#include <vector>

struct par_info
{
	double x;  // coordinates
	double y;
	int tag;  // active
	int pretag;  // active in last cycle
	float accutag = 0.0f;  // for rendering purposes
	int hash;  // hash based on x-y coordinates
	int type = 0;
	int num_kick = 0;  // number of kicks during shear
	double connectivity = 0;  // area fraction of surrounding particles
	double connectedNum = 0;  // number of surrounding particles
	int id; // used for graph generation
};

struct info
{
	double sys_w;
	double sys_h;
	double cellSizeX;
	double cellSizeY;
	double diameter;
	double epsilon;
	bool *p_pauseforRender;
	bool *p_pauseforShear;
	bool *p_pause;
	bool *p_next_frame;
	std::vector<par_info> *p_particle;
};