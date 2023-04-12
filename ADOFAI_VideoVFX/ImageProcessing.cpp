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

int frame_min = 0;
int frame_max = 0;
int prev_x_pos = 0;
bool mouse_clicked = false;

void OnMouse(int event, int x, int y, int flag, void* userdata) {
    
    switch (event) {
    case EVENT_LBUTTONDOWN:
        mouse_clicked = true;
        break;
    case EVENT_LBUTTONUP:
        mouse_clicked = false;
        break;
    case EVENT_MOUSEMOVE:
        if (mouse_clicked) {
            (*(double*)userdata) += (double)(x - prev_x_pos) / 10.0;

            if ((*(double*)userdata) < frame_min)
                (*(double*)userdata) = frame_min;

            if ((*(double*)userdata) > frame_max)
                (*(double*)userdata) = frame_max;

        }
        prev_x_pos = x;
        break;
    }
}

int GetFrame(const string video_path, const string title, const string description, const int begin_pos) {
    VideoCapture cap(video_path);
    if (!cap.isOpened()) {
        cout << "Cannot Open Video File" << endl;
        return -1;
    }

    frame_max = cap.get(CAP_PROP_FRAME_COUNT) - 1;
    frame_min = 0;

    double cur_frame = begin_pos == 1 ? frame_min : frame_max;
    cap.set(CAP_PROP_POS_FRAMES, (int)cur_frame);

    bool run = true;

    cv::namedWindow(title);
    startWindowThread();
    setMouseCallback(title, OnMouse, &cur_frame);

    while (run) {
        cap.set(CAP_PROP_POS_FRAMES, (int)cur_frame);
        Mat frame;
        cap >> frame;
        if (frame.empty())
            break;
        cap.set(CAP_PROP_POS_FRAMES, (int)cur_frame);

        cv::Mat roi = frame(Rect(0, 0, 800, 150));
        cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(0, 0, 0));
        double alpha = 0.65;
        cv::addWeighted(color, alpha, roi, 1.0 - alpha, 0.0, roi);

        putText(frame, title, Point(10, 40), 2, 1.2, Scalar::all(255), 2, LINE_AA);
        putText(frame, description, Point(10, 80), 2, 0.8, Scalar::all(255), 1, LINE_AA);
        putText(frame, "Press the Left or Right Arrow keys or Drag the Mouse to move the frames", Point(10, 115), 2, 0.8, Scalar::all(200), 1, LINE_AA);
        putText(frame, "Frame Number : " + to_string((int)cur_frame), Point(10, 150), 2, 0.8, Scalar::all(200), 1, LINE_AA);

        resize(frame, frame, Size(960, 540));
        imshow(title, frame);

        int key = waitKeyEx();
        switch (key) {
        case ' ':
            cv::destroyWindow(title);
            cap.release();
            return cur_frame;
        case 2424832:
            if (cur_frame > frame_min and cur_frame <= frame_max) cur_frame--;
            cap.set(CAP_PROP_POS_FRAMES, cur_frame);
            break;
        case 2555904:
            if (cur_frame >= frame_min and cur_frame < frame_max) cur_frame++;
            cap.set(CAP_PROP_POS_FRAMES, cur_frame);
            break;
        }
    }

    cv::destroyWindow(title);
    cap.release();
    return (int)cur_frame;
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
