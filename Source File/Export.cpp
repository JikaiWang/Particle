//
//  Export.cpp
//  Particle2Dv2
//
//  Created by Jikai Wang on 7/31/18.
//  Copyright © 2018 Jikai Wang. All rights reserved.
//

#include "Export.hpp"

chrono::system_clock sys_time;

void ExportPosition(vector<par_info> *particle)
{
    char filename [50];
    auto in_time_t = std::chrono::system_clock::to_time_t(sys_time.now());
    sprintf (filename, "./Position %ld.txt", in_time_t);
    ofstream out;
    out.open(filename);
    if (out) {
        out.precision(17);
        for (auto par = particle->begin(); par != particle->end(); ++par) {
            out << par->x << '\t';
            out << par->y << '\t';
            out << par->type;
            out << endl;
        }
        out.close();
    }
    else{
        cout << "Cannot open output file" << endl;
    }
}
