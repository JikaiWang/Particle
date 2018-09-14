//
//  Render.hpp
//  Particle2Dv2
//
//  Created by Jikai Wang on 7/18/18.
//  Copyright © 2018 Jikai Wang. All rights reserved.
//

#ifndef Render_hpp
#define Render_hpp

#define WINDOW_TITLE_PREFIX "Suspension system"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Basics.h"

void processNormalKeys(unsigned char key, int x, int y);
void renderScene();
void initRenderer(render_info renderInfo);
void render();


#endif /* Render_hpp */
