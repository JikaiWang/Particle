//
//  Measurement.hpp
//  Particle2Dv2
//
//  Created by Jikai Wang on 8/23/18.
//  Copyright © 2018 Jikai Wang. All rights reserved.
//

#ifndef Experiment_hpp
#define Experiment_hpp

#include "Suspension.h"

void distri_over_time();
void activation_with_phi();
void two_type_particle();
void density_dist_over_time();
void active_phi();
void active_decay();
void max_reach_dot(double fraction);
void max_reach_line(double fraction);
#endif /* Experiment_hpp */

#ifndef __linux__
void activation_movie_dot();
void activation_movie_line();
#endif
