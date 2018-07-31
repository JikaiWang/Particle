#include "Suspension.h"

suspension::suspension()
{
	cout << "Instance initialized, waiting for parameters..." << endl;
}


suspension::~suspension()
{
	cout << "Instance destroyed" << endl;
}

void suspension::loadConfig()
{
	;
}

void suspension::generateNew()
{
	accumulated_cycle = 0;
	reversibility = false;
	num = int(sys_w * sys_h * fraction / (M_PI * 0.25));

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(0, 1);

	par_info buffer;

	for (int i = 0; i < num; i++)
	{
		buffer.x = sys_w * dis(gen);
		buffer.y = sys_h * dis(gen);
		buffer.tag = 0;
		buffer.pretag = 1;
		buffer.hash = 0;
		particle.push_back(buffer);
	}

	cout << num << " particles generated" << endl;
}

void suspension::evolve()
{
	cout << "Start evolving..." << endl;

	grid_info *grid = new grid_info[width * height];
	int count;
	int pre_hash;
	int cycle = 0;

	thread t[NUM_THREADS];

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(0, 1);

	while ((active_portion > 0 || sedv > 0) && cycle < cutoffCycle) {

		// Wait for rendering
		if (pause && next_frame) pause = false;
		if (pauseforRender || pause) {
			this_thread::sleep_for(chrono::nanoseconds(1));
			continue;
		}
		pauseforShear = true;

		// Generate hash table
		for (auto par = particle.begin(); par != particle.end(); ++par)
			par->hash = int(par->x / cellSizeX) + int(par->y / cellSizeY) * width;

		// Sedimentation
		if (sedv > 0) {
			for (auto par = particle.begin(); par != particle.end(); ++par) {
				par->y -= sedv;
				if (par->y < 2 * cellSizeY) {
					par->pretag = 1;
					if (par->y < 0.5) par->y = 0.5;
				}
			}
		}

		// Sort by key using radix sort
		sort(particle.begin(), particle.end(), less_than_key());

		// Get unique count and location
		for (int i = 0; i < height * width; ++i)
			grid[i].offset = grid[i].count = 0;
		pre_hash = particle.begin()->hash;
		count = 1;
		grid[pre_hash].offset = 0;
		for (int i = 1; i < particle.size(); ++i)
		{
			if (particle[i].hash == pre_hash) ++count;
			else {
				grid[pre_hash].count = count;
				pre_hash = particle[i].hash;
				grid[pre_hash].offset = i;
				count = 1;
			}
		}
		grid[pre_hash].count = count;

		// Main loop
		if (lrPeriodic) lr_adjust = 1;
		if (udPeriodic) ud_adjust = 1;

		if (NUM_THREADS > 1) {
			for (int i = 0; i < NUM_THREADS; i++)
			{
				t[i] = thread(
					batchcheck,
					&particle,
					grid,
					height,
					width,
					lr_adjust,
					ud_adjust,
					NUM_THREADS,
					i
				);
			}
			for (int i = 0; i < NUM_THREADS; i++) {
				t[i].join();
			}
		}
		else {
			for (int y = 0; y < height + ud_adjust - 1; ++y)
				for (int x = 0; x < width + lr_adjust - 1; ++x)
					cellcheck(&particle, grid, y, x, height, width);
		}
		
		

		// Random kick
		for (auto par = particle.begin(); par != particle.end(); ++par) {
			if (par->tag > 0) {
				disp = dis(gen) * epsilon;
				angle = dis(gen) * 2 * M_PI;
				par->x += disp * cos(angle);
				par->y += disp * sin(angle);
			}
		}

		active_portion = 0;
		for (auto par = particle.begin(); par != particle.end(); ++par) {
			// Count active
			if (par->tag > 0) active_portion += 1;

			// Prepare tags
			par->pretag = par->tag;
			par->tag = 0;

			// Deal with boundary condition
			if (lrPeriodic) {
				if (par->x < 0) par->x += width;
				if (par->x > width) par->x -= width;
			}
			else {
				if (par->x < 0.5) par->x = 1 - par->x;
				if (par->x > width - 0.5)   par->x = 2 * width - 1 - par->x;
			}
			if (udPeriodic) {
				if (par->y < 0) par->y += height;
				if (par->y > height) par->y -= height;
			}
			else {
				if (par->y < 0.5) par->y = 1 - par->y;
				if (par->y > height - 0.5) par->y = 2 * height - 1 - par->y;
			}
		}
		active_portion /= double(particle.size());
		cout << '\r' << ++cycle << '\t' << int(active_portion * 100) << "%   " << flush;

		pauseforShear = false;

		if (!pause && next_frame) {
			pause = true;
			next_frame = false;
		}
	}
	cout << endl;
	accumulated_cycle += cycle;

	delete[] grid;

	cout << "Evolving finished using " << cycle << " cycles" << endl;
	cout << "After " << accumulated_cycle << " cycles --> ";
	if (active_portion > 0) {
		reversibility = false;
		cout << "Irreversible" << endl;
	}
	else {
		reversibility = true;
		cout << "Reversible" << endl;
	}
}

void suspension::printInfo()
{
	cout << "Num of particles: " << num << endl;
	cout << "Packing fraction: " << fraction << endl;
	cout << "System width: " << width << endl;
	cout << "System height: " << height << endl;
	cout << "Random kick size: " << epsilon << endl;
	cout << "Sedimentation velocity: " << sedv << endl;
	cout << "Maximum cycles: " << cutoffCycle << endl;
	cout << "Current cycle: " << accumulated_cycle << endl;
	if (reversibility) cout << "Reversible: Yes" << endl;
	else cout << "Reversible: No" << endl;
	if (lrPeriodic) cout << "Periodic LR: Yes" << endl;
	else cout << "Periodic LR: No" << endl;
	if (udPeriodic) cout << "Periodic UD: Yes" << endl;
	else cout << "Periodic UD: No" << endl;
}

void suspension::exportPosition()
{
	std::vector<std::string> buffer;
	for (auto par = particle.begin(); par != particle.end(); ++par) {
		buffer.push_back(to_string(par->x) + " " + to_string(par->y));
	}

	std::ofstream output_file("./position.txt");
	std::ostream_iterator<std::string> output_iterator(output_file, "\n");
	std::copy(buffer.begin(), buffer.end(), output_iterator);
}
