//
//  VideoWriter.cpp
//  Particle2Dv2
//
//  Created by Jikai Wang on 2/18/19.
//  Copyright © 2019 Jikai Wang. All rights reserved.
//

#include "VideoWriter.hpp"

videoWriter::videoWriter(info *_frameInfo)
{
	activeColor = Scalar(0, 0, 255);
	activeFillColor = Scalar(100, 100, 255);
	inactiveColor = Scalar(255, 0, 0);
	inactiveFillColor = Scalar(255, 100, 100);
    frameInfo = _frameInfo;
}

videoWriter::~videoWriter()
{
	;
}

void videoWriter::initMovie()
{
	#ifdef _WIN32
	writer.open("output/movie.mov", 
		VideoWriter::fourcc('X', '2', '6', '4'), 60.0, Size(COL, ROW));
	#endif // _WIN32
	#ifdef __APPLE__
	writer.open("./movie.mov", 
		VideoWriter::fourcc('a', 'v', 'c', '1'), 30.0, Size(ROW, COL));
	#endif // __APPLE__
	imageOnly = false;
}

void videoWriter::writeFrame()
{
	particle = frameInfo->p_particle;
    frame = Scalar(255, 255, 255);
    int x;
    int y;
    double x2;
    double y2;
	radius_ = 0.5 * frameInfo->diameter;
	epsilon_ = frameInfo->epsilon;
	int radius = int(radius_ * float(ROW) / frameInfo->sys_w);

    for (int i = 0; i < frameInfo->sys_w / frameInfo->cellSizeX; ++i) {
        x = int(ROW * double(i) * frameInfo->cellSizeX / frameInfo->sys_w);
        line(frame, Point(x, 0), Point(x, COL - 1), Scalar(0, 0, 0), 1, LINE_AA);
    }
    for (int i = 0; i < frameInfo->sys_h / frameInfo->cellSizeY; ++i) {
        y = int(COL * double(i) * frameInfo->cellSizeY / frameInfo->sys_h);
        line(frame, Point(0, y), Point(ROW - 1, y), Scalar(0, 0, 0), 1, LINE_AA);
    }

    for (auto const &par : *particle) {
        x = int(ROW * par.x / frameInfo->sys_w);
        y = int(COL * (1 - par.y / frameInfo->sys_h));
        
        if (par.num_kick != 0){
			circle(frame, Point(x, y), radius, activeFillColor, -1, LINE_AA);
			circle(frame, Point(x, y), radius, activeColor, THICKNESS, LINE_AA);
        } else {
			circle(frame, Point(x, y), radius, inactiveFillColor, -1, LINE_AA);
			circle(frame, Point(x, y), radius, inactiveColor, THICKNESS);
        }
        
		// draw particles across boundary
		if ((par.x < radius_) ||
			(par.y < radius_) ||
			(par.x > frameInfo->sys_w - radius_) ||
			(par.y < frameInfo->sys_h > radius_))
		{
            x2 = par.x;
            y2 = par.y;
            if (par.x < 0.5) x2 += frameInfo->sys_w;
            if (par.y < 0.5) y2 += frameInfo->sys_h;
            if (par.x > frameInfo->sys_w - 0.5) x2 -= frameInfo->sys_w;
            if (par.y > frameInfo->sys_h - 0.5) y2 -= frameInfo->sys_h;
            x = int(ROW * x2 / frameInfo->sys_w);
            y = int(COL * (1 - y2 / frameInfo->sys_h));
			if (par.num_kick != 0) {
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

void videoWriter::writeFrameConnectivity(String filename, double max, double min)
{
	particle = frameInfo->p_particle;
	frame = Scalar(255, 255, 255);
	int x;
	int y;
	double x2;
	double y2;
	epsilon_ = frameInfo->epsilon;
	radius_ = 0.5 * (frameInfo->diameter);
	int radius = int(radius_ * float(ROW) / frameInfo->sys_w);
	double maximum = max;
	double minimum = min;

	for (auto &par : *particle) {
		if (par.connectivity > maximum) maximum = par.connectivity;
		if (par.connectivity < minimum) minimum = par.connectivity;
	}
	std::cout << "maximum: " << maximum << std::endl;
	std::cout << "minimum: " << minimum << std::endl;
	
	// draw grid line
	//for (int i = 0; i < frameInfo->sys_w / frameInfo->cellSizeX; ++i) {
	//	x = int(ROW * double(i) * frameInfo->cellSizeX / frameInfo->sys_w);
	//	line(frame, Point(x, 0), Point(x, COL - 1), Scalar(0, 0, 0), 1, LINE_AA);
	//}
	//for (int i = 0; i < frameInfo->sys_h / frameInfo->cellSizeY; ++i) {
	//	y = int(COL * double(i) * frameInfo->cellSizeY / frameInfo->sys_h);
	//	line(frame, Point(0, y), Point(ROW - 1, y), Scalar(0, 0, 0), 1, LINE_AA);
	//}

	// draw particles
	for (auto par = particle->begin(); par != particle->end(); ++par) {
		x = int(ROW * par->x / frameInfo->sys_w);
		y = int(COL * (1 - par->y / frameInfo->sys_h));
		circle(frame, Point(x, y), radius, 
			COLORMODE(minimum, maximum, par->connectivity), -1, LINE_AA);
		circle(frame, Point(x, y), radius, (0, 0, 0), THICKNESS, LINE_AA);
		if (TEXT) putText(frame, std::to_string(par->connectivity), Point(x, y), 
			FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2);
		

		// draw particles across boundary
		if ((par->x < radius_) || 
			(par->y < radius_) || 
			(par->x > frameInfo->sys_w - radius_) || 
			(par->y < frameInfo->sys_h > radius_)) 
		{
			x2 = par->x;
			y2 = par->y;
			if (par->x < radius_) x2 += frameInfo->sys_w;
			if (par->y < radius_) y2 += frameInfo->sys_h;
			if (par->x > frameInfo->sys_w - radius_) x2 -= frameInfo->sys_w;
			if (par->y > frameInfo->sys_h - radius_) y2 -= frameInfo->sys_h;
			x = int(ROW * x2 / frameInfo->sys_w);
			y = int(COL * (1 - y2 / frameInfo->sys_h));
			circle(frame, Point(x, y), radius, 
				COLORMODE(minimum, maximum, par->connectivity), -1, LINE_AA);
			circle(frame, Point(x, y), radius, (0, 0, 0), THICKNESS, LINE_AA);
			if (TEXT) putText(frame, std::to_string(par->connectivity), Point(x, y), 
				FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2);
		}
	}
	if (imageOnly) imwrite("output/" + filename + ".jpg", frame);
	else writer.write(frame);
}


Scalar videoWriter::heatmap(double minimum, double maximum, double value)
{
	// 5-color heatmap
	double percent = (value - minimum) / (maximum - minimum);
	if (percent < 0.25) return Scalar(255, int(4 * 255 * percent), 0);
	else if (percent < 0.5) return Scalar(int(4 * 255 * (0.5 - percent)), 255, 0);
	else if (percent < 0.75) return Scalar(0, 255, int(4 * 255 * (percent - 0.5)));
	else return Scalar(0, int(4 * 255 * (1 - percent)), 255);
}

Scalar videoWriter::grayscale(double minimum, double maximum, double value)
{
	double percent = (value - minimum) / (maximum - minimum);
	return Scalar(int(255 * (1 - percent)), 
		int(255 * (1 - percent)), 
		int(255 * (1 - percent)));
}
