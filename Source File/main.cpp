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
//    phi_c();
//    max_reach();
//    activation_movie_dot();
//    cout << system1.fraction << endl;
//    activation_movie_line();
//    system1.exportPosition();
//    system1.varianceNum();
//    system1.structuralFactor();
    
    // Export video sample
    suspension system1;
    system1.generateNew();
    system1.updateInternalParam();
    videoWriter video(&system1.particle, &system1.renderInfo);
    for (int i = 0; i < 100; ++i) {
        video.writeFrame();
        system1.evolve();
    }
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
