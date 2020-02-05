//
//  main.cpp
//  Particle2Dv2
//
//  Created by Jikai Wang on 7/3/18.
//  Copyright � 2018 Jikai Wang. All rights reserved.
//

#include "Experiment.hpp"

int main(int argc, const char *argv[]) {
    // Export video sample
    suspension system1;
    system1.generateNew();
    system1.updateInternalParam();
    videoWriter video(&system1.particle, &system1.renderInfo, true);
	/*system1.reversible = false;
    while (!system1.reversible) {
		system1.connectivity();
        video.writeFrameConnectivity();
        system1.evolve();
    }*/
	system1.evolve();
	system1.connectivity();
	video.writeFrameConnectivity(true);

	// test live video
    /*suspension system1;
    system1.generateNew();
    system1.printInfo();
    thread job1(&suspension::evolve, &system1);
    initRenderer(system1.renderInfo);
    render();
    job1.join();*/

	// test probability function
	/*double epsilon = 1.5;
	double diameter = 1.0;
	for (size_t i = 0; i < 500; i++)
	{
		cout << 0.01*i << '\t' << probability(0.01*i, diameter, epsilon) << endl;
	}*/
	//probability(1, diameter, epsilon);

	// test angle function
	/*for (size_t i = 0; i < 100; i++)
	{
		cout << 0.01*i << '\t' << angle(0.6, 1.0, 0.01*i) << endl;
	}*/

    return 0;
}
