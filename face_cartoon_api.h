#ifndef FACE_CARTOON_API_H
#define FACE_CARTOON_API_H
#include <math.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <time.h>
#include <dirent.h>
#include "face_api.h"
#include "opencv.hpp"
void draw_link_points(cv::Mat &img, std::vector<float> &pts, cv::Scalar cor);

int test_api_1_pic(const char *picfile, const char *savefile, face_360::FacePoints& pts, double scale=1.5);

int test_dir(char *testdir, char *savedir);
#endif // FACE_CARTOON_API_H
