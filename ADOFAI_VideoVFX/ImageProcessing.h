#pragma once
#include <iostream>
#include <string>
using namespace std;

void SaveVideo(const string save_path, const string src_path, const int begin_frame, const int end_frame);

int GetFrame(const string video_path, const string title, const string description, const int begin_pos);

double GetFPS(const string video_path);