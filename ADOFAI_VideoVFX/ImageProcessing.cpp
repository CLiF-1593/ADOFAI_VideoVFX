#include "ImageProcessing.h"
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <Windows.h>
using namespace cv;
using namespace std;

void gotoxy(int x, int y){
    COORD pos = { x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void SaveVideo(const string save_path, const string src_path, const int begin_frame, const int end_frame) {
	VideoCapture cap(src_path);
	if (!cap.isOpened()) {
		cout << "Cannot Open Video File" << endl;
		return;
	}

	VideoWriter video(save_path, VideoWriter::fourcc('m', 'p', '4', 'v'), cap.get(CAP_PROP_FPS), Size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT)), true);
    
    cap.set(CAP_PROP_POS_FRAMES, begin_frame);

    int percent = -1;

    while (cap.get(CAP_PROP_POS_FRAMES) <= end_frame) {
        int new_percent = (cap.get(CAP_PROP_POS_FRAMES) - begin_frame) * 1000 / (end_frame - begin_frame);
        if (percent < new_percent) {
            percent = new_percent;
            gotoxy(0, 23);
            cout << "> Progress : " << (double)percent / 10.0 << "%";
        }
        Mat frame;
        cap >> frame;
        if (frame.empty())
            break;
        video.write(frame);
    }
    cout << endl;
    cap.release();
    video.release();
}

int GetFrame(const string video_path, const string title, const string description, const int begin_pos) {
    
    VideoCapture cap(video_path);
    if (!cap.isOpened()) {
        cout << "Cannot Open Video File" << endl;
        return -1;
    }

    int max = cap.get(CAP_PROP_FRAME_COUNT) - 1;
    int min = 0;

    int cur_frame = begin_pos == 1 ? min : max;
    cap.set(CAP_PROP_POS_FRAMES, cur_frame);

    cv::namedWindow(title);
    startWindowThread();
    while (true) {
        Mat frame;
        cap >> frame;
        if (frame.empty())
            break;
        cap.set(CAP_PROP_POS_FRAMES, cur_frame);

        cv::Mat roi = frame(Rect(0, 0, 800, 150));
        cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(0, 0, 0));
        double alpha = 0.65;
        cv::addWeighted(color, alpha, roi, 1.0 - alpha, 0.0, roi);

        putText(frame, title, Point(10, 40), 2, 1.2, Scalar::all(255), 2, LINE_AA);
        putText(frame, description, Point(10, 80), 2, 0.8, Scalar::all(255), 1, LINE_AA);
        putText(frame, "Press the Left or Right Arrow keys to move the frames", Point(10, 115), 2, 0.8, Scalar::all(200), 1, LINE_AA);
        putText(frame, "Frame Number : " + to_string(cur_frame), Point(10, 150), 2, 0.8, Scalar::all(200), 1, LINE_AA);
        
        resize(frame, frame, Size(960, 540));
        imshow(title, frame);

        int key = waitKeyEx();
        switch (key) {
        case ' ':
            cv::destroyWindow(title);
            cap.release();
            return cur_frame;
        case 2424832:
            if (cur_frame > min and cur_frame <= max) cur_frame--;
            cap.set(CAP_PROP_POS_FRAMES, cur_frame);
            break;
        case 2555904:
            if (cur_frame >= min and cur_frame < max) cur_frame++;
            cap.set(CAP_PROP_POS_FRAMES, cur_frame);
            break;
        }
    }
    cv::destroyWindow(title);
    cap.release();
    return 0;
}

double GetFPS(const string video_path) {
    VideoCapture cap(video_path);
    if (!cap.isOpened()) {
        cout << "Cannot Open Video File" << endl;
        return 0.0;
    }
    auto ret = cap.get(CAP_PROP_FPS);
    cap.release();
    return ret;
}
