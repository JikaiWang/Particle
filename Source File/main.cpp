//
//  main.cpp
//  Particle2Dv2
//
//  Created by Jikai Wang on 7/3/18.
//  Copyright � 2018 Jikai Wang. All rights reserved.
//

#include "Experiment.hpp"

int main(int argc, const char *argv[]) {

//    activation_with_phi();
//    two_type_particle();
//    distri_over_time();
//    for (int i = 0; i < 10; ++i) {
//        active_decay();
//    }
//    active_decay();
//    system1.fraction = 0.4;
//    suspension system1;
//    system1.generateNew();
//    system1.evolve();
//    activation_movie_line();

    struct info
    {
        double sys_w;
        double sys_h;
        double epsilon;
        double fraction;
    };

    info param[27];
    param[0]  = (info){20,    200,    0.5,    0.3};
    param[1]  = (info){100,    400,    5,        0.125};
    param[2]  = (info){10,    100,    0.05,    0.36};
    param[3]  = (info){10,    100,    0.5,    0.3};
    param[4]  = (info){20,    200,    0.5,    0.1};
    param[5]  = (info){20,    200,    0.5,    0.2};
    param[6]  = (info){1000,20,        0.5,    0.25};
    param[7]  = (info){500,    100,    5,        0.125};
    param[8]  = (info){200,    15,        0.05,    0.275};
    param[9]  = (info){100,    20,        0.5,    0.25};
    param[10] = (info){10,    20,        0.5,    0.25};
    param[11] = (info){500,    100,    5,        0.125};
    param[12] = (info){20,    200,    0.5,    0.3};
    param[13] = (info){100,    400,    5,        0.125};
    param[14] = (info){10,    100,    0.05,    0.36};
    param[15] = (info){10,    100,    0.5,    0.3};
    param[16] = (info){20,    200,    0.5,    0.1};
    param[17] = (info){20,    200,    0.5,    0.2};
    param[18] = (info){100,    20,        0.5,    0.25};
    param[19] = (info){100,    400,    4,        0.19};
    param[20] = (info){100,    400,    3,        0.21};
    param[21] = (info){100,    300,    2,        0.25};
    param[22] = (info){100,    200,    1,        0.3};
    param[23] = (info){20,    200,    0.5,    0.34};
    param[24] = (info){100,    400,    10,        0.05};
    param[25] = (info){100,    200,    1,        0.3};
    param[26] = (info){100,    400,    10,        0.05};

    for (int i = 1; i < 27; ++i){
        phi_c(param[i].sys_w,
              param[i].sys_h,
              param[i].epsilon,
              param[i].fraction);
    }

    return 0;
//    max_reach();
//    activation_movie_dot();
//    cout << system1.fraction << endl;
//    activation_movie_line();
//    system1.exportPosition();
//    system1.varianceNum();
//    system1.structuralFactor();
    
    // Export video sample
//    suspension system1;
//    system1.generateNew();
//    system1.updateInternalParam();
//    videoWriter video(&system1.particle, &system1.renderInfo);
//    for (int i = 0; i < 100; ++i) {
//        video.writeFrame();
//        system1.evolve();
//    }
//    ExportPosition(&system1.particle);
//    system1.exportDensityXY();
//    system1.varianceNum();
//    system1.exportVariance();
//    for (int i = 0; i < system1.particle.size(); ++i) {
//        system1.particle[i].y /= 2.0;
//    }
//    system1.printInfo();
//    thread job1(&suspension::evolve, &system1);
//    initRenderer(system1.renderInfo);
//    render();
//    job1.join();

    return 0;
}
