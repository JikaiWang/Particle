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
    radius = int(0.5 * float(ROW) / frameInfo.sys_h);
}

void videoWriter::writeFrame()
{
    frame = Scalar(255, 255, 255);
    int x;
    int y;
    for (auto par = particle->begin(); par != particle->end(); ++par) {
        x = int(ROW * par->x / frameInfo.sys_h);
        y = int(COL * par->y / frameInfo.sys_w);
        
        if (par->num_kick != 0){
            circle(frame, Point(x, y), radius, activeColor, 1, LINE_AA);
        } else {
            circle(frame, Point(x, y), radius, inactiveColor, 1, LINE_AA);
        }
    }
    writer.write(frame);
}
