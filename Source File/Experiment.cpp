//
//  Measurement.cpp
//  Particle2Dv2
//
//  Created by Jikai Wang on 8/23/18.
//  Copyright © 2018 Jikai Wang. All rights reserved.
//
#include "Experiment.hpp"



void distri_over_time()
{
	int NUM_MEASUREMENT = 1000;


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
	double adjustment = M_PI * 0.25 / (system1.sys_h * system1.sys_w / 100.0);

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

	#ifndef _WIN32
	mkdir("output", 0777);
	#endif

	char filename[100];
	auto in_time_t = std::chrono::system_clock::to_time_t(sys_time.now());
	sprintf(filename, "./output/fronts %ld.txt", in_time_t);
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
	else {
		cout << "Cannot open output file" << endl;
	}

	for (int i = 0; i < 100; ++i)
		delete[] output[i];
	delete[] output;
}



void activation_with_phi()
{
	#ifndef _WIN32
	mkdir("output", 0777);
	#endif
	chrono::system_clock sys_time;
	char filename[100];
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
	delete[] X;
	delete[] Y;
	out.close();
}

void two_type_particle()
{
	int num_type = 5;
	suspension system;
	//    system.diameter = 0.25;
	double scale = 2650;
	double Fraction[5] = { 210, 355, 530, 405, 1100 };
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

void density_dist_over_time()
{
	suspension system;
	system.generateNew();
	system.evolve();

}

void active_phi()
{
	suspension system;
	for (int i = 1; i < 66; ++i) {
		system.fraction = i * 0.01;
		system.generateNew();
		system.evolve();
	}
	char filename[100];
	chrono::system_clock sys_time;
	auto in_time_t = std::chrono::system_clock::to_time_t(sys_time.now());
	sprintf(filename, "./output/fact %ld.txt", in_time_t);
	ofstream out;
	out.open(filename);
	if (out) {
		out.precision(17);
		for (int i = 1; i < 66; ++i) {
			system.fraction = i * 0.01;
			system.generateNew();
			system.evolve();
			out << system.fraction << '\t';
			out << system.active_portion << endl;
		}
		out.close();
	}
	else {
		cout << "Cannot open output file" << endl;
	}
}

void active_decay()
{
	// measure 100 times ==> 100*cutoffCycle
	suspension system;
	system.generateNew();
	char filename[100];
	chrono::system_clock sys_time;
	auto in_time_t = std::chrono::system_clock::to_time_t(sys_time.now());
	sprintf(filename, "./output/actdecay %ld.txt", in_time_t);
	ofstream out;
	out.open(filename);
	if (out) {
		out.precision(17);
		for (int i = 0; i < 100; ++i) {
			system.evolve();
			out << (i + 1)*system.cutoffCycle << '\t';
			out << system.active_portion << endl;
		}
		out.close();
	}
	else {
		cout << "Cannot open output file" << endl;
	}
}

void max_reach_dot(double fraction)
{
	int total_kicks;
	int total_active;
	int time_settle;
	int random_index;
	double maximum_reach;
    double SS_dist;
	double disp;
	double dist;
	double angle;
	double x, y;
	char filename[100];
	chrono::system_clock sys_time;
	ofstream out1;
	ofstream out2;
	out1.precision(17);
	out2.precision(17);

	// initialize sheared system
	suspension system;
	system.lrPeriodic = true;
	system.udPeriodic = true;
	system.fraction = fraction;
	system.generateNew();
	system.evolve();
	if (!system.reversible) return;
	vector<par_info> buffer = system.particle;
	sort(buffer.begin(), buffer.end(), less_than_id());

	// random generator
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(0, 1);
	srand(time(nullptr));

	// file handling
	auto in_time_t = std::chrono::system_clock::to_time_t(sys_time.now());
	sprintf(filename, "./output/corrlen_%f_%f_%f_%f_%f_%f_%ld.txt",
		system.sys_w, system.sys_h, system.gamma, system.fraction,
		system.epsilon, system.sedv, in_time_t);
	out1.open(filename);

	// simulation
	for (int i = 0; i < 1000; ++i) {
		system.accumulated_cycle = 0;
		system.active_portion = 1;
		total_kicks = 0;
		total_active = 0;
		time_settle = 0;
		maximum_reach = 0.0;
        SS_dist = 0.0;

		// perturb a particle
		system.particle = buffer;
		random_index = (int)rand() / double(RAND_MAX) * double(system.num);
		cout << random_index << endl;
		x = system.particle.at(random_index).x;
		y = system.particle.at(random_index).y;
		disp = rand() / double(RAND_MAX) * system.epsilon;
		angle = rand() / double(RAND_MAX) * 2 * M_PI;
		x += disp * cos(angle);
		y += disp * sin(angle);
		// boundary condition
		if (x < 0) x += system.sys_w;
		if (x > system.sys_w) x -= system.sys_w;
		if (y < 0) y += system.sys_h;
		if (y > system.sys_h) y -= system.sys_h;

		system.particle.at(random_index).x = x;
		system.particle.at(random_index).y = y;
		system.particle.at(random_index).pretag = 1;
		system.evolve();

		// shift first perturbed particle to the center
		for (auto par = system.particle.begin(); par != system.particle.end(); ++par) {
			par->x += system.sys_w / 2.0 - x;
			par->y += system.sys_h / 2.0 - y;
			if (par->x < 0) par->x += system.sys_w;
			if (par->x > system.sys_w) par->x -= system.sys_w;
			if (par->y < 0) par->y += system.sys_h;
			if (par->y > system.sys_h) par->y -= system.sys_h;
		}

		// export corrlen measurement
		time_settle = system.accumulated_cycle - 1;
		for (auto par = system.particle.begin(); par != system.particle.end(); ++par) {
			if (par->num_kick > 0) {
				total_active += 1;
				dist = (par->x - system.sys_w / 2.0) * (par->x - system.sys_w / 2.0)
					+ (par->y - system.sys_h / 2.0) * (par->y - system.sys_h / 2.0);
                SS_dist += dist;
				dist = sqrt(dist);
				if (dist > maximum_reach) {
					maximum_reach = dist;
				}
			}
			total_kicks += par->num_kick;
		}
        out1 << total_kicks << '\t';
        out1 << total_active << '\t';
        out1 << time_settle << '\t';
        out1 << maximum_reach << '\t';
        out1 << sqrt(SS_dist / double(total_active)) << endl;
	}
	out1.close();
}

void max_reach_line(double fraction)
{
	int total_kicks;
	int total_active;
	int time_settle;
	int random_index;
	double maximum_reach;
    double SS_dist;
	double disp;
	double dist;
	double angle;
	double x, y;
	double rand_y;
	char filename[100];
	chrono::system_clock sys_time;
	ofstream out1;
	ofstream out2;
	out1.precision(17);
	out2.precision(17);

	// initialize sheared system
	suspension system;
	system.lrPeriodic = true;
	system.udPeriodic = true;
	system.fraction = fraction;
	system.generateNew();
	system.evolve();
	if (!system.reversible) return;
	vector<par_info> buffer = system.particle;
	sort(buffer.begin(), buffer.end(), less_than_id());

	// random generator
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(0, 1);
	srand(time(nullptr));

	// file handling
	auto in_time_t = std::chrono::system_clock::to_time_t(sys_time.now());
	sprintf(filename, "./output/corrlenline_%f_%f_%f_%f_%f_%f_%ld.txt",
		system.sys_w, system.sys_h, system.gamma, system.fraction,
		system.epsilon, system.sedv, in_time_t);
	out1.open(filename);

	// simulation
	for (int i = 0; i < 1000; ++i) {
		system.accumulated_cycle = 0;
		system.active_portion = 1;
		total_kicks = 0;
		total_active = 0;
		time_settle = 0;
		maximum_reach = 0.0;
        SS_dist = 0.0;
		system.particle = buffer;

		// perturb a line of particles
		rand_y = rand() / double(RAND_MAX) * (system.sys_h - 1.0) + 0.5;
		for (auto par = system.particle.begin(); par != system.particle.end(); ++par) {
			if (abs(par->y - rand_y) < 0.5) {
				x = par->x;
				y = par->y;
				disp = rand() / double(RAND_MAX) * system.epsilon;
				angle = rand() / double(RAND_MAX) * 2 * M_PI;
				x += disp * cos(angle);
				y += disp * sin(angle);
				// boundary condition
				if (x < 0) x += system.sys_w;
				if (x > system.sys_w) x -= system.sys_w;
				if (y < 0) y += system.sys_h;
				if (y > system.sys_h) y -= system.sys_h;
				par->x = x;
				par->y = y;
				par->pretag = 1;
			}
		}
		system.evolve();

		// shift particles to central horizontal line
		for (auto par = system.particle.begin(); par != system.particle.end(); ++par) {
			par->y -= rand_y - system.sys_h / 2.0;
			if (par->y < 0) par->y += system.sys_h;
			if (par->y > system.sys_h) par->y -= system.sys_h;
		}

		// export corrlen measurement
		time_settle = system.accumulated_cycle - 1;
		for (auto par = system.particle.begin(); par != system.particle.end(); ++par) {
			if (par->num_kick > 0) {
				total_active += 1;
				dist = abs(par->y - system.sys_h / 2.0);
                SS_dist += dist*dist;
				if (dist > maximum_reach) {
					maximum_reach = dist;
				}
			}
			total_kicks += par->num_kick;
		}
		out1 << total_kicks << '\t';
		out1 << total_active << '\t';
		out1 << time_settle << '\t';
        out1 << maximum_reach << '\t';
        out1 << sqrt(SS_dist / double(total_active)) << endl;
	}    
	out1.close();
}


void phi_c(double sys_w, double sys_h, double epsilon, double fraction)
{
    // binary search
    int num_trial = 5;
    bool pass;
    double left = 0.001;
    double right = 0.6;
    
    suspension system;
    system.fraction = fraction;
    system.epsilon = epsilon;
    system.sys_w = sys_w;
    system.sys_h = sys_h;
    system.generateNew();
    int N = system.num;
    system.top_blank = 0.0;
    system.cutoffCycle = 10000;
    
    char filename[100];
    chrono::system_clock sys_time;
    ofstream out2;
    out2.precision(4);
    auto in_time_t = std::chrono::system_clock::to_time_t(sys_time.now());
    sprintf(filename, "./output/phic_%f_%f_%f_%f_%f_%f_%ld.txt", system.sys_w, system.sys_h, system.gamma, system.fraction, system.epsilon, system.sedv, in_time_t);
    out2.open(filename);
    
    while (right - left > 0.001)
    {
        pass = false;
        system.fraction = 0.5 * (left + right);
        out2 << system.fraction << endl;
        system.sys_h = N * M_PI * 0.25 / (system.fraction * system.sys_w);
        system.generateNew();
        for (int i = 0; i < num_trial; ++i)
        {
            system.evolve();
            if (system.reversible){
                pass = true;
                break;
            }
            if (system.active_portion > 0.3) break;
        }
        if (pass) left = system.fraction;
        if (!pass) right = system.fraction;
    }
    out2 << left << endl;
    out2.close();
}


#ifndef __linux__

void activation_movie_dot()
{
	int random_index;
	double maximum_reach;
	double disp;
	double angle;
	double x, y;

	// initialize sheared system
	suspension system;
	system.lrPeriodic = true;
	system.udPeriodic = true;
	system.generateNew();
	system.evolve();
	if (!system.reversible) return;
	vector<par_info> buffer = system.particle;
	sort(buffer.begin(), buffer.end(), less_than_id());

	// random generator
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(0, 1);
	srand(time(nullptr));
	system.accumulated_cycle = 0;
	system.active_portion = 1;
	maximum_reach = 0.0;

	// perturb a particle
	system.particle = buffer;
	random_index = (int)rand() / double(RAND_MAX) * double(system.num);
	cout << random_index << endl;
	x = system.particle.at(random_index).x;
	y = system.particle.at(random_index).y;
	disp = rand() / double(RAND_MAX) * system.epsilon;
	angle = rand() / double(RAND_MAX) * 2 * M_PI;
	x += disp * cos(angle);
	y += disp * sin(angle);
	// boundary condition
	if (x < 0) x += system.sys_w;
	if (x > system.sys_w) x -= system.sys_w;
	if (y < 0) y += system.sys_h;
	if (y > system.sys_h) y -= system.sys_h;

	system.particle.at(random_index).x = x;
	system.particle.at(random_index).y = y;
	system.particle.at(random_index).pretag = 1;

	// shift first perturbed particle to the center
	for (auto par = system.particle.begin(); par != system.particle.end(); ++par) {
		par->x += system.sys_w / 2.0 - x;
		par->y += system.sys_h / 2.0 - y;
		if (par->x < 0) par->x += system.sys_w;
		if (par->x > system.sys_w) par->x -= system.sys_w;
		if (par->y < 0) par->y += system.sys_h;
		if (par->y > system.sys_h) par->y -= system.sys_h;
	}

	system.cutoffCycle = 1;
	videoWriter writer(&(system.particle), &(system.renderInfo));
	while (system.active_portion > 0) {
		system.evolve();
		writer.writeFrame();
	}
}

void activation_movie_line()
{
	double maximum_reach;
	double disp;
	double angle;
	double x, y;
	double rand_y;

	// initialize sheared system
	suspension system;
	system.lrPeriodic = true;
	system.udPeriodic = true;
	system.generateNew();
	system.evolve();
	if (!system.reversible) return;
	vector<par_info> buffer = system.particle;
	sort(buffer.begin(), buffer.end(), less_than_id());

	// random generator
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(0, 1);
	srand(time(nullptr));

	system.accumulated_cycle = 0;
	system.active_portion = 1;
	maximum_reach = 0.0;
	system.particle = buffer;

	// perturb a line of particles
	rand_y = rand() / double(RAND_MAX) * (system.sys_h - 1.0) + 0.5;
	for (auto par = system.particle.begin(); par != system.particle.end(); ++par) {
		if (abs(par->y - rand_y) < 0.5) {
			x = par->x;
			y = par->y;
			disp = rand() / double(RAND_MAX) * system.epsilon;
			angle = rand() / double(RAND_MAX) * 2 * M_PI;
			x += disp * cos(angle);
			y += disp * sin(angle);
			// boundary condition
			if (x < 0) x += system.sys_w;
			if (x > system.sys_w) x -= system.sys_w;
			if (y < 0) y += system.sys_h;
			if (y > system.sys_h) y -= system.sys_h;
			par->x = x;
			par->y = y;
			par->pretag = 1;
		}
	}

	// shift particles to central horizontal line
    for (auto par = system.particle.begin(); par != system.particle.end(); ++par) {
        par->y -= rand_y - system.sys_h / 2.0;
        if (par->y < 0) par->y += system.sys_h;
        if (par->y > system.sys_h) par->y -= system.sys_h;
    }
    for (auto par = buffer.begin(); par != buffer.end(); ++par) {
        par->y -= rand_y - system.sys_h / 2.0;
        par->num_kick = 0;
        if (par->y < 0) par->y += system.sys_h;
        if (par->y > system.sys_h) par->y -= system.sys_h;
    }

	system.cutoffCycle = 1;
	videoWriter writer(&(system.particle), &(system.renderInfo));
	while (system.active_portion > 0) {
		system.evolve();
		writer.writeFrame();
        sort(system.particle.begin(), system.particle.end(), less_than_id());
        for (int i = 0; i < system.num; ++i){
            buffer.at(i).num_kick += system.particle.at(i).num_kick;
        }
	}
    
    char filename[50];
    chrono::system_clock sys_time;
    ofstream out2;
    out2.precision(17);
    auto in_time_t = std::chrono::system_clock::to_time_t(sys_time.now());
    sprintf(filename, "./posidiff_%f_%f_%f_%f_%f_%f_%ld.txt", system.sys_w, system.sys_h, system.gamma, system.fraction, system.epsilon, system.sedv, in_time_t);
    out2.open(filename);
    out2 << system.accumulated_cycle << endl;
    sort(system.particle.begin(), system.particle.end(), less_than_id());
    sort(buffer.begin(), buffer.end(), less_than_id());
    for (int i = 0; i < system.num; ++i)
    {
        out2 << buffer.at(i).x << '\t';
        out2 << buffer.at(i).y << '\t';
        out2 << system.particle.at(i).x << '\t';
        out2 << system.particle.at(i).y << '\t';
        out2 << buffer.at(i).num_kick << endl;
    }
    out2.close();
    this_thread::sleep_for(chrono::seconds(1));
}



#endif
