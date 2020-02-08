//
//  Measurement.cpp
//  Particle2Dv2
//
//  Created by Jikai Wang on 12/13/18.
//  Copyright © 2018 Jikai Wang. All rights reserved.
//

#include "Measurement.hpp"

void structuralFactor(render_info *info)
{
	double w = info->sys_w;
	double h = info->sys_h;
	int iw = int(w);
	int ih = int(h);
    vector<double> skxy(4 * iw * ih);
	vector<par_info> *particle = info->p_particle;
	int num = int(particle->size());
	vector<double> sk;
	vector<double> k;
	double kx;
	double ky;
	complex<double> buffer;
	const complex<double> i(0, 1);

	for(int y = 0; y < ih; y++)
	{
		for(int x = 0; x < iw; x++)
		{
			kx = 2.0 * M_PI * (x - w / 2.0);
			ky = 2.0 * M_PI * (y - h / 2.0);
			buffer = complex<double>(0.0, 0.0);
			for(auto par = particle->begin(); par != particle->end(); par++)
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

	int max = int(*max_element(k.begin(), k.end()) / STEPS);
	vector<double> result(max, 0.0);
	vector<double> count(max, 0.0);
	
	for (int n = 0; n < sk.size(); ++n) {
		result[int(k[n] / STEPS)] += sk[n];
		count[int(k[n] / STEPS)] += 1.0;
	}

	char filename[50];
	chrono::system_clock sys_time;
	auto in_time_t = std::chrono::system_clock::to_time_t(sys_time.now());
    sprintf(filename, "./output/sk %f %f %ld.txt", in_time_t);
	ofstream out;
	out.open(filename);
	if (out) {
		out.precision(17);
		for (int n = 0; n < result.size(); ++n) {
            if (count[n] != 0.0)
            {
                out << STEPS * (n + 0.5) << '\t';
                out << result[n]/count[n] << endl;
            }
		}
		out.close();
	}
	else {
		cout << "Cannot open output file" << endl;
	}
}
