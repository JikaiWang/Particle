//
//  main.cpp
//  Particle2Dv2
//
//  Created by Jikai Wang on 7/3/18.
//  Copyright © 2018 Jikai Wang. All rights reserved.
//

#include "Suspension.h"

int main(int argc, const char *argv[]) {

	suspension system1;

	system1.generateNew();
	system1.printInfo();
	system1.evolve();


	suspension system2;
	system2.fraction = 0.2;
	system2.generateNew();
	system2.printInfo();
	system2.evolve();
	system2.printInfo();
	system2.exportPosition();

	return 0;
}