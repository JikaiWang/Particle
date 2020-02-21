//
//  VideoWriter.hpp
//  Particle2Dv2
//
//  Created by Jikai Wang on 2/18/19.
//  Copyright © 2019 Jikai Wang. All rights reserved.
//

#pragma once

#define COLORMODE heatmap

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include <vector>
#include <iostream>
#include "Struct.hpp"

using namespace cv;
class videoWriter
{
public:
	videoWriter(info *_frameInfo);
	~videoWriter();
    void initMovie();
    void writeFrame();
	void writeFrameConnectivity(String filename = "connectivity", 
		double max = 0.0, double min = 1.0);
	
private:
	const int ROW = 2000;
	const int COL = 2000;
	const int RADIUS = 1;
	const int THICKNESS = 1;
	const bool TEXT = false;

	bool imageOnly = true;
    VideoWriter writer;
    std::vector<par_info> *particle;
    info *frameInfo;
    Scalar activeColor;
	Scalar activeFillColor;
	Scalar inactiveColor;
	Scalar inactiveFillColor;
    double radius_;
	double epsilon_;
    Mat frame = Mat(Size(ROW, COL), CV_8UC3);
	Scalar heatmap(double minimum, double maximum, double value);
	Scalar grayscale(double minimum, double maximum, double value);
};
