#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

namespace ImageProcessing {
    extern double video_fps;
    extern int video_width;
    extern int video_height;
    extern int video_frame_num;

    void ReadVideo(const std::string src_path);
	void SaveVideo(const std::string src_path, const std::string save_path, const int begin_frame, const int end_frame);
    void ConvertBitrate(const std::string src_path, const std::string save_path, const std::string bitrate, const std::string max_bitrate);
    int GetFrame(const std::string src_path, const std::string title, const std::string description, const int begin_pos);
};