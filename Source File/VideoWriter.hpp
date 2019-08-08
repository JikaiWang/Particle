//
//  VideoWriter.hpp
//  Particle2Dv2
//
//  Created by Jikai Wang on 2/18/19.
//  Copyright © 2019 Jikai Wang. All rights reserved.
//

#ifndef VideoWriter_hpp
#define VideoWriter_hpp

#define ROW 1000
#define COL 1000
#define RADIUS 1

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
    videoWriter(vector<par_info> *input, render_info *frame_info);
    void writeFrame();
    
private:
    VideoWriter writer;
    vector<par_info> *particle;
    render_info frameInfo;
    Scalar activeColor;
    Scalar inactiveColor;
    double radius;
    Mat frame = Mat(Size(ROW, COL), CV_8UC3);
};


#endif /* VideoWriter_hpp */
