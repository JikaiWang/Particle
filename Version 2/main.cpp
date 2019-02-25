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
    
    
    suspension system1;
    system1.generateNew();
    system1.evolve();
//    system1.exportPosition();
//    system1.varianceNum();
    system1.structuralFactor();
    
    
//    videoWriter video(&system1.particle, &system1.renderInfo);
//    for (int i = 0; i < 10; ++i) {
//        video.writeFrame();
//        system1.evolve();
//    }
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
