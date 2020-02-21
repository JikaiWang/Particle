//
//  main.cpp
//  Particle2Dv2
//
//  Created by Jikai Wang on 7/3/18.
//  Copyright � 2018 Jikai Wang. All rights reserved.
//

#include "Experiment.hpp"

int main(int argc, const char *argv[]) {
 
    suspension system;
	system.fraction = 0.376;
	system.epsilon = 0.5;
	system.sys_h = system.sys_w = 120;
	system.initialOverlap = false;
	system.generateNew();
	system.updateInfo();
	system.Network.Measure(0.5);
	system.Network.PrintSystemAverage();
	system.VideoWriter.writeFrameConnectivity(
		"b05w " + to_string(system.sys_h) + ' ' + to_string(system.epsilon) + " init", 0.12);
	system.initialOverlap = true;
	system.generateNew();
	system.evolve();
	system.Network.Measure(0.5);
	system.Network.PrintSystemAverage();
	system.VideoWriter.writeFrameConnectivity(
		"b05w " + to_string(system.sys_h) + ' ' + to_string(system.epsilon) + " crit", 0.12);
	
    return 0;
}
