//
//  VideoWriter.cpp
//  Particle2Dv2
//
//  Created by Jikai Wang on 2/18/19.
//  Copyright © 2019 Jikai Wang. All rights reserved.
//

#include "VideoWriter.hpp"

videoWriter::videoWriter(vector<par_info> *input, render_info *frame_info)
{
    writer.open("./movie.mov", VideoWriter::fourcc('a', 'v', 'c', '1'), 30.0, Size(ROW, COL));
    particle = input;
    frameInfo = *frame_info;
    activeColor = Scalar(0, 0, 255);
    inactiveColor = Scalar(255, 0, 0);
    radius = int(0.5 * float(ROW) / frameInfo.sys_w);
}

void videoWriter::writeFrame()
{
    frame = Scalar(255, 255, 255);
    int x;
    int y;
    double x2;
    double y2;
    cout << "entered" << endl;
    for (int i = 0; i < frameInfo.sys_w / frameInfo.cellsizeX; ++i) {
        x = int(ROW * double(i) * frameInfo.cellsizeX / frameInfo.sys_w);
        line(frame, Point(x, 0), Point(x, COL - 1), Scalar(0, 0, 0), 1, LINE_AA);
    }
    for (int i = 0; i < frameInfo.sys_h / frameInfo.cellsizeY; ++i) {
        y = int(COL * double(i) * frameInfo.cellsizeY / frameInfo.sys_h);
        line(frame, Point(0, y), Point(ROW - 1, y), Scalar(0, 0, 0), 1, LINE_AA);
    }
    cout << "continued" << endl;
    for (auto par = particle->begin(); par != particle->end(); ++par) {
        x = int(ROW * par->x / frameInfo.sys_w);
        y = int(COL * (1 - par->y / frameInfo.sys_h));
        
        if (par->num_kick != 0){
            circle(frame, Point(x, y), radius, activeColor, THICKNESS, LINE_AA);
        } else {
            circle(frame, Point(x, y), radius, inactiveColor, THICKNESS, LINE_AA);
        }
        
        if ((par->x < 0.5) || (par->y < 0.5) || (par->x > frameInfo.sys_w - 0.5) || (par->y < frameInfo.sys_h > 0.5)) {
            x2 = par->x;
            y2 = par->y;
            if (par->x < 0.5) x2 += frameInfo.sys_w;
            if (par->y < 0.5) y2 += frameInfo.sys_h;
            if (par->x > frameInfo.sys_w - 0.5) x2 -= frameInfo.sys_w;
            if (par->y > frameInfo.sys_h - 0.5) y2 -= frameInfo.sys_h;
            x = int(ROW * x2 / frameInfo.sys_w);
            y = int(COL * (1 - y2 / frameInfo.sys_h));
            if (par->num_kick != 0){
                circle(frame, Point(x, y), radius, activeColor, THICKNESS, LINE_AA);
            } else {
                circle(frame, Point(x, y), radius, inactiveColor, THICKNESS, LINE_AA);
            }
        }
    }
    writer.write(frame);
    cout << "ended" << endl;
}
