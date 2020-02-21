//
//  Render.hpp
//  Particle2Dv2
//
//  Created by Jikai Wang on 7/18/18.
//  Copyright © 2018 Jikai Wang. All rights reserved.
//

#pragma once

#define WINDOW_TITLE_PREFIX "Suspension system"

#ifdef __APPLE__
#include <GLUT/glut.h>
#elif _WIN32
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif // __APPLE__

#define _USE_MATH_DEFINES
#include "Struct.hpp"
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <iostream>

class render
{
public:
	render(info *_renderInfo);
	~render();
	void initRender();
	void startRender();

private:
	info *Info;
};

