//
//  VideoWriter.hpp
//  Particle2Dv2
//
//  Created by Jikai Wang on 2/18/19.
//  Copyright © 2019 Jikai Wang. All rights reserved.
//

#pragma once

#ifndef VideoWriter_hpp
#define VideoWriter_hpp

#define ROW 2160
#define COL 2160
#define RADIUS 1
#define THICKNESS 1
#define TEXT false
#define COLORMODE heatmap

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "Basics.h"

using namespace cv;
class videoWriter
{
public:
    videoWriter(vector<par_info> *input, render_info *frame_info, bool imageOnly = false);
    void writeFrame(bool imageOnly = false);
	void writeFrameConnectivity(bool imageOnly = false);
	Scalar heatmap(double minimum, double maximum, double value);
	Scalar grayscale(double minimum, double maximum, double value);
    
private:
    VideoWriter writer;
    vector<par_info> *particle;
    render_info *frameInfo;
    Scalar activeColor;
	Scalar activeFillColor;
    Scalar inactiveColor;
	Scalar inactiveFillColor;
    double radius;
    Mat frame = Mat(Size(ROW, COL), CV_8UC3);
};


#endif /* VideoWriter_hpp */
