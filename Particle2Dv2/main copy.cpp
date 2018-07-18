//
//  main.cpp
//  Particle2Dv2
//
//  Created by Jikai Wang on 7/3/18.
//  Copyright © 2018 Jikai Wang. All rights reserved.
//

#define _USE_MATH_DEFINES
#define FRACTION 0.38
#define EPSILON 0.5
#define LRPERIODIC true
#define UDPERIODIC true
#define SYS_W 100
#define SYS_H 100
#define CELLSIZE 1
#define SEDV 0.01
#define CUTOFFCYCLE 10000


#include <iostream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <fstream>
#include <chrono>
#include <math.h>

using namespace std;


struct par_info
{
    double x;
    double y;
    int tag;
    int pretag;
    int hash;
};

struct grid_info
{
    int offset;
    int count;
};

par_info init()
{
    // Random generator
    static random_device rd;
    static mt19937 gen(rd());
    uniform_real_distribution<> dis(0, 1);
    
    par_info particle;
    
    particle.x = SYS_W * dis(gen);
    particle.y = SYS_H * dis(gen);
    particle.tag = 0;
    particle.pretag = 1;
    particle.hash = 0;
    
    return particle;
}

int offset(int y, int x, int ydimension, int xdimension)
{
    // numpy style indexing
    return (y - (y / ydimension) * ydimension) * xdimension
            + x - (x / xdimension) * xdimension;
}

struct less_than_key
{
    inline bool operator() (const par_info& par1, const par_info& par2)
    {
        return (par1.hash < par2.hash);
    }
};

void cellcheck(vector<par_info> *particle, grid_info *grid, 
               int y, int x, int HEIGHT, int WIDTH)
{
    bool pre_active = false;
    vector<par_info> buffer; // Partilce to test
    grid_info cell[4]; // adjacent cells

    // Load cells
    for (int i = 0; i < 4; ++i) {
        cell[i] = grid[offset(y+i/2, x+i%2, HEIGHT, WIDTH)];
        for (int j = 0; j < cell[i].count; ++j)
            buffer.push_back((*particle)[cell[i].offset + j]);
    }

    // Return when no need to check
    if (buffer.empty()) return; // return when no particle
    for (auto par = buffer.begin(); par != buffer.end(); ++par)
        if (par->pretag > 0) pre_active = true; // check pretag
    if (!pre_active) return; // return when previously not active

    // Collision check (recurrent)
    for (auto target = buffer.begin(); target != buffer.end(); ++target) {
        auto par = target + 1;
        for (int i = 0; i < buffer.size() - 1; ++i) {
            if (par == buffer.end()) par = buffer.begin();
            if ((target->x - par->x)*(target->x - par->x)
                     + (target->y - par->y)*(target->y - par->y) < 1)
                target->tag = 1;
            ++par;
        }
    }

    // Copy result back
    auto target = buffer.begin();
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < cell[i].count; ++j)
            (*particle)[cell[i].offset + j] = *target++;
    }
}

int main(int argc, const char *argv[]) {
    
    int WIDTH = SYS_W / CELLSIZE;
    int HEIGHT = SYS_H / CELLSIZE;
    double CELLSIZEX = SYS_W / double(WIDTH);
    double CELLSIZEY = SYS_H / double(HEIGHT);
    int NUM = SYS_W * SYS_H * FRACTION / (M_PI * 0.25);
    int lr_adjust = 0;
    int ud_adjust = 0;
    double active_portion = 1;
    int cycle = 1;
    double angle;
    double disp;

    grid_info *grid = new grid_info[WIDTH * HEIGHT];
    int count;
    int pre_hash;

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0, 1);
    
    cout << "WIDTH = " << WIDTH << 'x' << CELLSIZEX << endl;
    cout << "HEIGHT = " << HEIGHT << 'x' << CELLSIZEY << endl;
    
    // Generate particles
    vector<par_info> particle(NUM);
    generate(particle.begin(), particle.end(), init);
    cout << "Array size: particle -> " << particle.size() << endl;
    
    while ((active_portion > 0 || SEDV > 0) && cycle < CUTOFFCYCLE) {
        // Generate hash table
        for (auto par = particle.begin(); par != particle.end(); ++par)
            par->hash = int(par->x / CELLSIZEX) + int(par->y / CELLSIZEY) * WIDTH;

        // Sedimentation
        if (SEDV > 0) {
            for (auto par = particle.begin(); par != particle.end(); ++par) {
                par->y -= SEDV;
                if (par->y < 2 * CELLSIZEY) {
                    par->pretag = 1;
                    if (par->y < 0.5) par->y = 0.5;
                }
            } 
        } 

        // Sort by key using radix sort
        sort(particle.begin(), particle.end(), less_than_key());

        // Get unique count and location
        for (int i = 0; i < HEIGHT * WIDTH; ++i)
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
        if (LRPERIODIC) lr_adjust = 1;
        if (UDPERIODIC) ud_adjust = 1;
        for (int y = 0; y < HEIGHT + ud_adjust - 1; ++y)
            for (int x = 0; x < WIDTH + lr_adjust - 1; ++x)
                cellcheck(&particle, grid, y, x, HEIGHT, WIDTH);


        // Print particle info
        // for (auto i = particle.begin(); i != particle.end(); ++i)
        // {
        //     cout << i->x << "  \t";
        //     cout << i->y << "  \t";
        //     cout << i->tag << "\t";
        //     cout << i->pretag << "\t";
        //     cout << i->hash << "\t";
        //     cout << endl;
        // }


        // Random kick
        for (auto par = particle.begin(); par != particle.end(); ++par) {
            if (par->tag > 0) {
                disp = dis(gen) * EPSILON;
                angle = dis(gen) * 2 * M_PI;
                par->x += disp * cos(angle);
                par->y += disp * sin(angle);
            }
        }

        active_portion = 0;
        for (auto par = particle.begin(); par != particle.end(); ++par){
            // Count active
            if (par->tag > 0) active_portion += 1;

            // Prepare tags
            par->pretag = par->tag;
            par->tag = 0;

            // Deal with boundary condition
            if (LRPERIODIC) {
                if (par->x < 0)             par->x += SYS_W;
                if (par->x > SYS_W)         par->x -= SYS_W;
            } else {
                if (par->x < 0.5)           par->x = 1 - par->x;
                if (par->x > SYS_W - 0.5)   par->x = 2*SYS_W - 1 - par->x; 
            }
            if (UDPERIODIC) {
                if (par->y < 0)             par->y += SYS_H;
                if (par->y > SYS_H)         par->y -= SYS_H;
            } else {
                if (par->y < 0.5)           par->y = 1 - par->y;
                if (par->y > SYS_H - 0.5)   par->y = 2*SYS_H - 1 - par->y; 
            }
        }
        active_portion /= double(particle.size());
        cout << '\r' << cycle << '\t' << int(active_portion*100) << '%' << flush;
        ++cycle;
    }
    cout << endl;

    // for (int i = 0; i < WIDTH * HEIGHT; ++i)
    // {
    //     cout << grid[i].offset << '\t' << grid[i].count << endl;
    // }


    delete[] grid;
    
    return 0;
}
