#include "Suspension.h"

suspension::suspension(){
    cout << "Instance initialized, waiting for parameters..." << endl;
}

suspension::~suspension(){
    cout << "Instance destroyed" << endl;
}

void suspension::loadConfig(){
    ;
}

void suspension::updateInternalParam(){
    width = int(sys_w / cellsize);
    height = int(sys_h / cellsize);
}



void suspension::generateNew()
{
    particle.clear();
    active_portion = 1;
    accumulated_cycle = 0;
    reversibility = false;
    num = int(sys_w * sys_h * (1 - top_blank) * fraction / (M_PI * 0.25));
	fraction = num * (M_PI * 0.25) / (sys_w * sys_h * (1 - top_blank));
    
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0, 1);

    if (initialOverlap)
    {
        par_info buffer;
        for (int i = 0; i < num; i++)
        {
            buffer.x = sys_w * dis(gen);
            buffer.y = (1 - top_blank) * sys_h * dis(gen);
            buffer.tag = 0;
            buffer.pretag = 1;
            buffer.hash = 0;
            buffer.tag = 0;
            particle.push_back(buffer);
        }
    }
    else
    {
        int index;
        int index_x;
        int index_y;
        int ix;
        int iy;
        par_info buffer;
        par_info *grid = new par_info[int(sys_w * sys_h * 4)];
        int current_num = 0;
        bool overlap;
        for (int i = 0; i < int(sys_w * sys_h * 4); i++)
        {
            grid[i].pretag = 0;
        }
        while (current_num < num)
        {
            buffer.x = sys_w * dis(gen);
            buffer.y = sys_h * dis(gen);
            buffer.tag = 0;
            buffer.pretag = 1;
            buffer.hash = 0;
            index_x = int(buffer.x);
            index_y = int(buffer.y);
            overlap = false;
            for (int x = index_x - 1; x < index_x + 2; x++) {
                for (int y = index_y - 1; y < index_y + 2; y++) {
                    if (x == -1) {
                        ix = sys_w - 1;
                    }
                    else {
                        ix = x;
                    }
                    if (y == -1) {
                        iy = sys_h - 1;
                    }
                    else {
                        iy = y;
                    }
                    index = offset(iy, ix, sys_h, sys_w) * 4;
                    for (int i = 0; i < 4; i++)
                    {
                        if (grid[index + i].pretag == 1)
                        {
                            if ((pow(grid[index + i].x - buffer.x, 2.0)
                                 + pow(grid[index + i].y - buffer.y, 2.0)) < 1.0)
                            {
                                overlap = true;
                            }
                        }
                    }
                }
            }
            if (!overlap)
            {
                for (int i = (index_x + sys_w * index_y) * 4;
                     i < (index_x + sys_w * index_y + 1) * 4; i++)
                {
                    if (grid[i].pretag == 0)
                    {
                        grid[i] = buffer;
                        current_num++;
                        break;
                    }
                }
            }
        }
        for (int i = 0; i < sys_w * sys_h * 4; i++)
        {
            if (grid[i].pretag == 1) particle.push_back(grid[i]);
        }
        delete[] grid;
    }

    cout << num << " particles generated" << endl;
}



void suspension::evolve()
{
	char filename[50];
    chrono::system_clock sys_time;
    auto in_time_t = std::chrono::system_clock::to_time_t(sys_time.now());
    sprintf(filename, "./output/ft_%f_%f_%f_%f_%f_%ld.txt", sys_w, sys_h, gamma, fraction, epsilon, in_time_t);
    ofstream out;
    out.open(filename);
    out.precision(17);
   
    updateInternalParam();
    auto start_t = timer.now();
    cout << "Start evolving..." << endl;

    grid_info *grid = new grid_info[width * height];
    int count;
    int pre_hash;
    int cycle = 0;
	double avg_count = 0.0f;
    double avg_fact = 0.0f;
    double avg_fact2 = 0.0f;
    bool steady = false;
    double moving_avg_base = 0.0f;
    double moving_avg_curr = 0.0f;
    double moving_queue_base[NUM_MOV_AVG_BASE] = {0};
    double moving_queue_curr[NUM_MOV_AVG_CURR] = {0};

    thread t[NUM_THREADS];

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0, 1);
    
    // Prepare the extended x,y coordinates
    for (auto par = particle.begin(); par != particle.end(); ++par) {
        par->x_ext = par->x;
        par->y_ext = par->y;
    }
    
    while ((active_portion > 0 || sedv > 0) && cycle < cutoffCycle && avg_count < cutoffMeasurement) {

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
            //multithreading (compute in parallel)
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
                    i,
                    gamma,
                    diameter
                );
            }
            for (int i = 0; i < NUM_THREADS; i++) {
                t[i].join();
            }
        }
        else {
            //single core safe version
            for (int y = 0; y < height + ud_adjust - 1; ++y)
                for (int x = 0; x < width + lr_adjust - 1; ++x)
                    cellcheck(&particle, grid, y, x, height, width, gamma, diameter);
        }
        
        

        // Random kick
        for (auto par = particle.begin(); par != particle.end(); ++par) {
            if (par->tag > 0) {
                disp = dis(gen) * epsilon;
                angle = dis(gen) * 2 * M_PI;
                par->x += disp * cos(angle);
                par->y += disp * sin(angle);
                par->x_ext += disp * cos(angle);
                par->y_ext += disp * sin(angle);
            }
        }

        active_portion = 0;
        for (auto par = particle.begin(); par != particle.end(); ++par) {
            // Count active
            if (par->tag > 0) active_portion += 1;

            // Prepare tags
            if (par->tag > 0) par->accutag = 1.0f;
            par->accutag -= 0.004f;
            if (par->accutag < 0) par->accutag = 0.0f;
            par->pretag = par->tag;
            par->tag = 0;

            // Deal with boundary condition
            if (lrPeriodic) {
                if (par->x < 0) par->x += sys_w;
                if (par->x > sys_w) par->x -= sys_w;
            }
            else {
                if (par->x < 0.5) par->x = 1 - par->x;
                if (par->x > sys_w - 0.5)   par->x = 2 * sys_w - 1 - par->x;
            }
            if (udPeriodic) {
                if (par->y < 0) par->y += sys_h;
                if (par->y > sys_h) par->y -= sys_h;
            }
            else {
                if (par->y < 0.5) par->y = 1 - par->y;
                if (par->y > sys_h - 0.5) par->y = 2 * sys_h - 1 - par->y;
            }
        }
        active_portion /= double(particle.size());

        // fact and variance
        if (steady && (cycle%T_INTERVAL == 0))
        {
            avg_fact *= avg_count;
            avg_fact += active_portion;
            avg_fact2 *= avg_count;
            avg_fact2 += active_portion * active_portion;
            avg_count += 1.0f;
            avg_fact /= avg_count;
            avg_fact2 /= avg_count;
        }
        
        //moving average base
        moving_avg_base -= moving_queue_base[cycle%NUM_MOV_AVG_BASE]/double(NUM_MOV_AVG_BASE);
        moving_avg_base += active_portion/double(NUM_MOV_AVG_BASE);
        moving_queue_base[cycle%NUM_MOV_AVG_BASE] = active_portion;
        
        //moving average current
        if (cycle > NUM_MOV_AVG_BASE)
        {
            moving_avg_curr -= moving_queue_curr[cycle%NUM_MOV_AVG_CURR]/double(NUM_MOV_AVG_CURR);
            moving_avg_curr += active_portion/double(NUM_MOV_AVG_CURR);
            moving_queue_curr[cycle%NUM_MOV_AVG_CURR] = active_portion;
        }
        
        // threshold of starting measurement
        if (!steady)
        {
            if (moving_avg_curr > moving_avg_base)
            {
                steady = true;
            }
        }
		
		// output fact
    	if (out && (cycle%1000 == 0)) {
			out << cycle << '\t';
			out << active_portion << endl;
    	}
        
        if (!CLUSTER)
        {
        cout << '\r' << cycle << '\t' << int(active_portion * 100) << "%   " << flush;
        }
        ++cycle;

        pauseforShear = false;

        if (!pause && next_frame) {
            pause = true;
            next_frame = false;
        }
    }
    cout << endl;
    accumulated_cycle += cycle;
    
    // Gather statistical data
    fact = avg_fact;
    var = avg_fact2 - avg_fact * avg_fact;
    
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
    auto end_t = timer.now();
    cout << "Elapsed: ";
    cout << chrono::duration_cast<chrono::seconds>(end_t - start_t).count();
    cout << 's' << endl;

	out.close();
}

void suspension::printInfo()
{
    cout << "Num of particles: " << num << endl;
    cout << "Packing fraction: " << fraction << endl;
    cout << "Particle diameter: " << diameter << endl;
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
    cout << "Multithreading: " << NUM_THREADS << endl;
}

void suspension::exportPosition()
{
    std::vector<std::string> buffer;
    for (auto par = particle.begin(); par != particle.end(); ++par) {
        buffer.push_back(to_string(par->x) + " " + to_string(par->y) + " " + to_string(par->type));
    }
    
    std::ofstream output_file("./output/position.txt");
    std::ostream_iterator<std::string> output_iterator(output_file, "\n");
    std::copy(buffer.begin(), buffer.end(), output_iterator);
}


void suspension::varianceNum()
{
    int num_box = 1000;
    int num_decade = 10;
    int num_steps = 20;
    int num_var = num_decade * num_steps;
    double rho;
    double rho_square;
    double center_x;
    double center_y;
    double box_radius;
    double distance;
    double area;
    variance.clear();
    
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0, 1);
    
    for (int l = 0; l < num_var; ++l) {
        cout << int(100 * float(l) / float(num_var)) << "%\r" << flush;
        if (min(sys_h, sys_w) > diameter * (2 + pow(10, double(l)/double(num_steps))))
        {
            box_radius = 0.5 * diameter * pow(10, double(l)/double(num_steps));
            rho = 0.0;
            rho_square = 0.0;
            for (int i = 0; i < num_box; ++i) {
                center_x = dis(gen) * (sys_w - 2 * diameter - 2 * box_radius) + diameter + box_radius;
                center_y = dis(gen) * (sys_h - 2 * diameter - 2 * box_radius) + diameter + box_radius;
                area = 0;
                for (auto par = particle.begin(); par != particle.end(); ++par) {
                    distance = sqrt(pow(par->x - center_x, 2.0) + pow(par->y - center_y, 2.0)) - box_radius;
                    if (distance < 0)
                    {
                        area += M_PI * pow(diameter/2.0, 2.0);
                    }
                }
                area /= M_PI * pow(box_radius, 2.0);
                rho += area;
                rho_square += pow(area, 2.0);
            }
            variance.push_back({2 * box_radius / diameter, rho_square/double(num_box) - pow(rho/double(num_box), 2.0)});
        }
    }
    
    std::vector<std::string> buffer;
    for (auto par = variance.begin(); par != variance.end(); ++par) {
        buffer.push_back(to_string(log10(par->box_size)) + " " + to_string(log10(par->var)));
    }
    
    char filename[50];
    chrono::system_clock sys_time;
    auto in_time_t = std::chrono::system_clock::to_time_t(sys_time.now());
    sprintf(filename, "./output/variance %f %f %ld.txt", epsilon, fraction, in_time_t);
    std::ofstream output_file(filename);
    std::ostream_iterator<std::string> output_iterator(output_file, "\n");
    std::copy(buffer.begin(), buffer.end(), output_iterator);
    
    cout << "Finished varianceNum" << endl;
}

void suspension::structuralFactor()
{
    int iw = int(sys_w);
    int ih = int(sys_h);
    vector<double> skxy(4 * iw * ih);
    vector<double> sk;
    vector<double> k;
    double kx;
    double ky;
    complex<double> buffer;
    const complex<double> i(0, 1);
    
    for(int y = 0; y < ih; y++)
    {
        cout << int(100 * float(y) / float(ih)) << "%\r" << flush;
        for(int x = 0; x < iw; x++)
        {
            kx = 2.0 * M_PI * (x - sys_w / 2.0) / sys_w;
            ky = 2.0 * M_PI * (y - sys_h / 2.0) / sys_h;
            buffer = complex<double>(0.0, 0.0);
            for(auto par = particle.begin(); par != particle.end(); par++)
            {
                buffer += exp(i * (kx * par->x + ky * par->y));
            }
            skxy[y * iw + x] = pow(abs(buffer), 2.0) / double(num);
            if (kx != 0.0 && ky != 0.0)
            {
                sk.push_back(skxy[y * iw + x]);
                k.push_back(sqrt(pow(kx, 2.0) + pow(ky, 2.0)));
            }
        }
    }
    
    int max = int(*max_element(k.begin(), k.end()) * sys_w * 0.5 / M_PI + 1.0);
    vector<double> result(max, 0.0);
    vector<double> count(max, 0.0);
    
    for (int n = 0; n < sk.size(); ++n) {
        result[int(k[n] * sys_w * 0.5 / M_PI)] += sk[n];
        count[int(k[n] * sys_h * 0.5 / M_PI)] += 1.0;
    }
    
    char filename[50];
    chrono::system_clock sys_time;
    auto in_time_t = std::chrono::system_clock::to_time_t(sys_time.now());
    sprintf(filename, "./output/sk %f %f %ld.txt", epsilon, fraction, in_time_t);
    ofstream out;
    out.open(filename);
    if (out) {
        out.precision(17);
        for (int n = 0; n < result.size(); ++n) {
            if (count[n] != 0.0)
            {
                out << 2 * M_PI * n / sys_w << '\t';
                out << result[n]/count[n] << endl;
            }
        }
        out.close();
    }
    else {
        cout << "Cannot open output file" << endl;
    }
    cout << "Finished structuralFactor" << endl;
}


void suspension::exportDensityXY()
{
    double steps_x = 100;
    double steps_y = 1;
    int arrlen_x = int(sys_w / steps_x);
    int arrlen_y = int(sys_h / steps_y);
    double distribution[arrlen_x][arrlen_y];
    string outputStr;
    
    for (int x = 0; x < arrlen_x; ++x) {
        for (int y = 0; y < arrlen_y; ++y) {
            distribution[x][y] = 0.0;
        }
    }
    
    for (auto par = particle.begin(); par != particle.end(); ++par) {
        distribution[int(par->x / steps_x)][int(par->y / steps_y)] += M_PI * pow(diameter, 2.0) * 0.25 / (steps_x * steps_y);
    }
    
    std::vector<std::string> buffer;
    for (int x = 0; x < arrlen_x; ++x) {
        outputStr = "";
        for (int y = 0; y < arrlen_y; ++y) {
            outputStr += to_string(distribution[x][y]) + " ";
        }
        buffer.push_back(outputStr);
    }
    
    std::ofstream output_file("./densityXY.txt");
    std::ostream_iterator<std::string> output_iterator(output_file, "\n");
    std::copy(buffer.begin(), buffer.end(), output_iterator);
}
