#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
using namespace std;

namespace ImageProcessing {
    extern double video_fps;
    extern int video_width;
    extern int video_height;
    extern int video_frame_num;

    void ReadVideo(const string src_path);
	void SaveVideo(const string src_path, const string save_path, const int begin_frame, const int end_frame);
	int GetFrame(const string src_path, const string title, const string description, const int begin_pos);
};