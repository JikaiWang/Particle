//
//  Measurement.cpp
//  Particle2Dv2
//
//  Created by Jikai Wang on 8/23/18.
//  Copyright © 2018 Jikai Wang. All rights reserved.
//
#include "Measurement.hpp"



void distri_over_time()
{
    int NUM_MEASUREMENT = 100;
    
    
    double** output = new double*[100];
    for (int i = 0; i < 100; ++i)
        output[i] = new double[NUM_MEASUREMENT + 1];
    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < NUM_MEASUREMENT + 1; ++j) {
            output[i][j] = 0.0;
        }
    }
    
    suspension system1;
    system1.generateNew();
    double adjustment = M_PI * 0.25 / (system1.sys_h * system1.sys_w / 100.0 );
    
    for (int i = 0; i < system1.particle.size(); ++i) {
        system1.particle[i].y /= 2.0;
    }
    system1.printInfo();
    
    for (int i = 0; i < NUM_MEASUREMENT + 1; ++i) {
        for (auto par = system1.particle.begin(); par != system1.particle.end(); ++par) {
            output[int(100 * par->y / system1.sys_h)][i] += 1.0;
        }
        system1.evolve();
    }
    
    chrono::system_clock sys_time;

	mkdir("output", 0777);

    char filename [50];
    auto in_time_t = std::chrono::system_clock::to_time_t(sys_time.now());
    sprintf (filename, "./output/fronts %ld.txt", in_time_t);
    ofstream out;
    out.open(filename);
    
    if (out) {
        out.precision(4);
        for (int i = 0; i < 100; ++i) {
            for (int j = 0; j < NUM_MEASUREMENT; ++j) {
                out << output[i][j] * adjustment << ' ';
            }
            out << output[i][NUM_MEASUREMENT] << endl;
        }
        out.close();
    }
    else{
        cout << "Cannot open output file" << endl;
    }
    
    for(int i = 0; i < 100; ++i)
        delete[] output[i];
    delete[] output;
}



void activation_with_phi()
{
    mkdir("output", 0777);
    chrono::system_clock sys_time;
    char filename[50];
    auto in_time_t = std::chrono::system_clock::to_time_t(sys_time.now());
    sprintf(filename, "output/activation %ld.txt", in_time_t);
    ofstream out;
    out.open(filename);
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(0, 1);
	int num_average = 3000;
    int num_system = 1;
	int pre_cycle;
	int random_index;
    int index;
	double disp;
	double angle;
	double activation_count;
	suspension system1;
    double *X = new double[int(system1.sys_w * system1.sys_h / (M_PI * 0.25))];
    double *Y = new double[int(system1.sys_w * system1.sys_h / (M_PI * 0.25))];
	for (int i = 1; i < 71; i++)
	{
		system1.fraction = i / 100.0;
        activation_count = 0.0;
        for (int j = 0; j < num_system; j++)
        {
            system1.generateNew();
            system1.cutoffCycle = 100000;
            system1.evolve();
            index = 0;
            for (auto par = system1.particle.begin();
                 par != system1.particle.end();
                 ++par)
            {
                X[index] = par->x;
                Y[index] = par->y;
                index++;
            }
            pre_cycle = system1.accumulated_cycle;
            system1.cutoffCycle = 2;
            for (int i = 0; i < num_average; i++)
            {
                index = 0;
                for (auto par = system1.particle.begin();
                     par != system1.particle.end();
                     ++par)
                {
                    par->x = X[index];
                    par->y = Y[index];
                    par->pretag = 0;
                    index++;
                }
//                system1.particle = sheared_par;
                random_index = int(dis(gen) * (system1.num - 0));
                if (random_index >= system1.particle.size())
                {
                    cout << "access violation" << endl;
                }
                disp = dis(gen) * system1.epsilon;
                angle = dis(gen) * 2 * M_PI;
                system1.particle.at(random_index).x += disp * cos(angle);
                system1.particle.at(random_index).y += disp * sin(angle);
                system1.particle.at(random_index).pretag += 1;
                system1.active_portion = 1.0;
                system1.evolve();
                if (system1.accumulated_cycle - pre_cycle > 1)
                {
                    activation_count += 1.0;
                }
                pre_cycle = system1.accumulated_cycle;
            }
        }
		activation_count /= float(num_average * num_system);

        if (out)
        {
            out.precision(6);
            out << system1.fraction << '\t' << activation_count << endl;
        }
	}
    delete [] X;
    delete [] Y;
    out.close();
}

void two_type_particle()
{
    int num_type = 5;
    suspension system;
//    system.diameter = 0.25;
    double scale = 2650;
    double Fraction[5] = {210, 355, 530, 405, 1100};
    system.epsilon = 0.125;
    
    vector<par_info> alltype;
    for (int i = 0; i < num_type; ++i) {
        system.fraction = Fraction[i] / scale;
        system.generateNew();
        for (auto par = system.particle.begin(); par != system.particle.end(); par++)
        {
            par->type = i;
        }
        alltype.insert(alltype.end(), system.particle.begin(), system.particle.end());
    }
    system.particle = alltype;
//    system.fraction *= num_type;
    system.num = int(system.particle.size());
    
//    thread job1(&suspension::evolve, &system);
//    initRenderer(system.renderInfo);
//    render();
//    job1.join();
    system.evolve();
    
//    for (auto par = system.particle.begin();
//         par != system.particle.end(); par++)
//    {
//        par->x /= system.diameter;
//        par->y /= system.diameter;
//    }
//    ExportPosition(&system.particle);
    system.exportPosition();
}
