//
//  main.cpp
//  Particle2Dv2
//
//  Created by Jikai Wang on 7/3/18.
//  Copyright � 2018 Jikai Wang. All rights reserved.
//

#include "Suspension.h"
#include "Render.h"



int main(int argc, const char *argv[]) {

	chrono::high_resolution_clock timer;
	auto start_t = timer.now();

	suspension system1;

	system1.generateNew();
	system1.printInfo();

	std::thread t1(&suspension::evolve, &system1);

	//initRenderer(system1.renderInfo);
	//render();

	t1.join();
    
//    ExportPosition(&system1.particle);

	auto end_t = timer.now();
	cout << "Elapsed: " << chrono::duration_cast<chrono::seconds>(end_t - start_t).count() << 's' << endl;

	return 0;
}
