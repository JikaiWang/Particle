//
//  VideoWriter.cpp
//  Particle2Dv2
//
//  Created by Jikai Wang on 2/18/19.
//  Copyright © 2019 Jikai Wang. All rights reserved.
//

#include "VideoWriter.hpp"

videoWriter::videoWriter(vector<par_info> *input, render_info *frame_info, bool imageOnly)
{
	#ifdef _WIN32
	if (!imageOnly) writer.open("output/movie.mov", VideoWriter::fourcc('X', '2', '6', '4'), 60.0, Size(COL, ROW));
	#endif // _WIN32
	#ifdef __APPLE__
	if (!imageOnly) writer.open("./movie.mov", VideoWriter::fourcc('a', 'v', 'c', '1'), 30.0, Size(ROW, COL));
	#endif // __APPLE__

    particle = input;
    frameInfo = frame_info;
    activeColor = Scalar(0, 0, 255);
	activeFillColor = Scalar(100, 100, 255);
    inactiveColor = Scalar(255, 0, 0);
	inactiveFillColor = Scalar(255, 100, 100);
}

void videoWriter::writeFrame(bool imageOnly)
{
    frame = Scalar(255, 255, 255);
    int x;
    int y;
    double x2;
    double y2;
	radius = int(0.5 * float(ROW) / frameInfo->sys_w);

    for (int i = 0; i < frameInfo->sys_w / frameInfo->cellsizeX; ++i) {
        x = int(ROW * double(i) * frameInfo->cellsizeX / frameInfo->sys_w);
        line(frame, Point(x, 0), Point(x, COL - 1), Scalar(0, 0, 0), 1, LINE_AA);
    }
    for (int i = 0; i < frameInfo->sys_h / frameInfo->cellsizeY; ++i) {
        y = int(COL * double(i) * frameInfo->cellsizeY / frameInfo->sys_h);
        line(frame, Point(0, y), Point(ROW - 1, y), Scalar(0, 0, 0), 1, LINE_AA);
    }

    for (auto par = particle->begin(); par != particle->end(); ++par) {
        x = int(ROW * par->x / frameInfo->sys_w);
        y = int(COL * (1 - par->y / frameInfo->sys_h));
        
        if (par->num_kick != 0){
			circle(frame, Point(x, y), radius, activeFillColor, -1, LINE_AA);
			circle(frame, Point(x, y), radius, activeColor, THICKNESS, LINE_AA);
        } else {
			circle(frame, Point(x, y), radius, inactiveFillColor, -1, LINE_AA);
			circle(frame, Point(x, y), radius, inactiveColor, THICKNESS);
        }
        
        if ((par->x < 0.5) || (par->y < 0.5) || (par->x > frameInfo->sys_w - 0.5) || (par->y < frameInfo->sys_h > 0.5)) {
            x2 = par->x;
            y2 = par->y;
            if (par->x < 0.5) x2 += frameInfo->sys_w;
            if (par->y < 0.5) y2 += frameInfo->sys_h;
            if (par->x > frameInfo->sys_w - 0.5) x2 -= frameInfo->sys_w;
            if (par->y > frameInfo->sys_h - 0.5) y2 -= frameInfo->sys_h;
            x = int(ROW * x2 / frameInfo->sys_w);
            y = int(COL * (1 - y2 / frameInfo->sys_h));
			if (par->num_kick != 0) {
				circle(frame, Point(x, y), radius, activeFillColor, -1, LINE_AA);
				circle(frame, Point(x, y), radius, activeColor, THICKNESS, LINE_AA);
			}
			else {
				circle(frame, Point(x, y), radius, inactiveFillColor, -1, LINE_AA);
				circle(frame, Point(x, y), radius, inactiveColor, THICKNESS, LINE_AA);
			}
        }
    }
	if (imageOnly) {
		imwrite("output/particle.jpg", frame);
	}
	else {
		writer.write(frame);
	}
}

void videoWriter::writeFrameConnectivity(bool imageOnly)
{
	frame = Scalar(255, 255, 255);
	int x;
	int y;
	double x2;
	double y2;
	radius = int(0.5 * float(ROW) / frameInfo->sys_w);
	double maximum = 1;
	double minimum = 1;

	for (auto par = particle->begin(); par != particle->end(); ++par) {
		if (par->connectivity > maximum) maximum = par->connectivity;
		if (par->connectivity < minimum) minimum = par->connectivity;
	}
	
	// draw grid line
	for (int i = 0; i < frameInfo->sys_w / frameInfo->cellsizeX; ++i) {
		x = int(ROW * double(i) * frameInfo->cellsizeX / frameInfo->sys_w);
		line(frame, Point(x, 0), Point(x, COL - 1), Scalar(0, 0, 0), 1, LINE_AA);
	}
	for (int i = 0; i < frameInfo->sys_h / frameInfo->cellsizeY; ++i) {
		y = int(COL * double(i) * frameInfo->cellsizeY / frameInfo->sys_h);
		line(frame, Point(0, y), Point(ROW - 1, y), Scalar(0, 0, 0), 1, LINE_AA);
	}

	// draw particles
	for (auto par = particle->begin(); par != particle->end(); ++par) {
		x = int(ROW * par->x / frameInfo->sys_w);
		y = int(COL * (1 - par->y / frameInfo->sys_h));
		circle(frame, Point(x, y), radius, COLORMODE(minimum, maximum, par->connectivity), -1, LINE_AA);
		circle(frame, Point(x, y), radius, (0, 0, 0), THICKNESS, LINE_AA);
		if (TEXT) putText(frame, to_string(par->connectivity), Point(x, y), 
			FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2);
		

		// draw particles across boundary
		if ((par->x < 0.5) || (par->y < 0.5) || (par->x > frameInfo->sys_w - 0.5) || (par->y < frameInfo->sys_h > 0.5)) {
			x2 = par->x;
			y2 = par->y;
			if (par->x < 0.5) x2 += frameInfo->sys_w;
			if (par->y < 0.5) y2 += frameInfo->sys_h;
			if (par->x > frameInfo->sys_w - 0.5) x2 -= frameInfo->sys_w;
			if (par->y > frameInfo->sys_h - 0.5) y2 -= frameInfo->sys_h;
			x = int(ROW * x2 / frameInfo->sys_w);
			y = int(COL * (1 - y2 / frameInfo->sys_h));
			circle(frame, Point(x, y), radius, COLORMODE(minimum, maximum, par->connectivity), -1, LINE_AA);
			circle(frame, Point(x, y), radius, (0, 0, 0), THICKNESS, LINE_AA);
			if (TEXT) putText(frame, to_string(par->connectivity), Point(x, y), 
				FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2);
		}
	}
	if (imageOnly) {
		imwrite("output/connectivity.jpg", frame);
	}
	else {
		writer.write(frame);
	}
}


Scalar videoWriter::heatmap(double minimum, double maximum, double value)
{
	// 5-color heatmap
	double percent = (value - minimum) / (maximum - minimum);
	if (percent < 0.25)
	{
		return Scalar(255, int(4 * 255 * percent), 0);
	}
	else if (percent < 0.5) {
		return Scalar(int(4 * 255 * (0.5 - percent)), 255, 0);
	}
	else if (percent < 0.75) {
		return Scalar(0, 255, int(4 * 255 * (percent - 0.5)));
	}
	else {
		return Scalar(0, int(4 * 255 * (1 - percent)), 255);
	}
}

Scalar videoWriter::grayscale(double minimum, double maximum, double value)
{
	double percent = (value - minimum) / (maximum - minimum);
	return Scalar(int(255 * (1 - percent)), 
		int(255 * (1 - percent)), 
		int(255 * (1 - percent)));
}
