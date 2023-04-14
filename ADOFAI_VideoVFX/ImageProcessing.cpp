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

namespace ImageProcessing {
    double video_fps;
    int video_width;
    int video_height;
    int video_frame_num;

    void ReadVideo(const string src_path) {
        VideoCapture cap(src_path);
        if (!cap.isOpened()) {
            std::cout << "Cannot Open Video File" << endl;
            return;
        }

        int percent = -1;
        video_frame_num = cap.get(CAP_PROP_FRAME_COUNT);
        video_fps = cap.get(CAP_PROP_FPS);
        video_width = cap.get(CAP_PROP_FRAME_WIDTH);
        video_height = cap.get(CAP_PROP_FRAME_HEIGHT);
    }

    void SaveVideo(const string src_path, const string save_path, const int begin_frame, const int end_frame) {
        VideoCapture cap(src_path);
        if (!cap.isOpened()) {
            std::cout << "Cannot Open Video File" << endl;
            return;
        }
        
        VideoWriter video(save_path, VideoWriter::fourcc('m', 'p', '4', 'v'), video_fps, Size(video_width, video_height), true);
        int percent = -1;

        cap.set(CAP_PROP_POS_FRAMES, begin_frame);
        video.set(VIDEOWRITER_PROP_QUALITY, 10);
        
        for (int i = begin_frame; i <= end_frame; i++) {
            int new_percent = (i - begin_frame) * 10000 / (end_frame - begin_frame);
            if (percent < new_percent) {
                percent = new_percent;
                gotoxy(0, 25);
                cout << "                  ";
                gotoxy(0, 25);
                std::cout << "> Progress : " << (double)percent / 100.0 << "%";
            }
            cv::Mat frame;
            cap >> frame;
            video.write(frame);
        }
        std::cout << endl;
        video.release();
    }

    void ConvertBitrate(const string src_path, const string save_path, const string bitrate, const string max_bitrate, double pitch) {
        string command = "ffmpeg -v quiet -stats -y -i \"" + src_path + "\" -b:v " + bitrate + " -preset fast -maxrate " + max_bitrate + " -vf \"setpts = " + to_string(1 / pitch) + " * PTS\"" + " -bufsize 2000k \"" + save_path + "\"";
        system(command.c_str());
    }

    
    int frame_min = 0;
    int frame_max = video_frame_num - 1;
    
    int prev_x_pos = 0;
    bool mouse_clicked = false;
    bool timeline_clicked = false;

    void OnMouse(int event, int x, int y, int flag, void* userdata) {

        switch (event) {
        case EVENT_LBUTTONDOWN:
            mouse_clicked = true;
            if (y > 530) {
                timeline_clicked = true;
                (*(double*)userdata) = (double)x / 960.0 * (double)frame_max;
                if ((*(double*)userdata) < frame_min)
                    (*(double*)userdata) = frame_min;
                if ((*(double*)userdata) > frame_max)
                    (*(double*)userdata) = frame_max;
            }
            break;
        case EVENT_LBUTTONUP:
            mouse_clicked = false;
            timeline_clicked = false;
            break;
        case EVENT_MOUSEMOVE:
            if (timeline_clicked) {
                (*(double*)userdata) = (double)x / 960.0 * (double)frame_max;
            }
            else if (mouse_clicked) {
                (*(double*)userdata) += (double)(x - prev_x_pos) / 10.0;
            }
            if ((*(double*)userdata) < frame_min)
                (*(double*)userdata) = frame_min;
            if ((*(double*)userdata) > frame_max)
                (*(double*)userdata) = frame_max;
            prev_x_pos = x;
            break;
        }
    }

    int GetFrame(const string src_path, const string title, const string description, const int begin_pos) {
        cout << ";" << endl;
        cout << "이 화면에 뜨는 로그들은 무시해도 됩니다." << endl;
        cout << "Please ignore the logs that appear on this screen." << endl;
        CONSOLE_SCREEN_BUFFER_INFO buffer_info = { 0 };
        DWORD num_character_read = 0;
        COORD first_char_to_read = { 0 };
        char buffer[100000];
        ReadConsoleOutputCharacterA(GetStdHandle(STD_OUTPUT_HANDLE), buffer, sizeof(buffer), first_char_to_read, &num_character_read);
        for (int i = 0; i < 100000; i++) {
            if (buffer[i] == ';') {
                buffer[i] = '\0';
            }
        }

        VideoCapture cap(src_path);
        if (!cap.isOpened()) {
            std::cout << "Cannot Open Video File" << endl;
            return -1;
        }
        
        vector<Mat*> video_frames;
        video_frames.resize(video_frame_num);
        for (int i = 0; i < video_frames.size(); i++) {
            video_frames[i] = nullptr;
        }

        frame_min = 0;
        frame_max = video_frame_num - 1;

        double cur_frame = begin_pos == 1 ? frame_min : frame_max;
        int calc_frame_num = cur_frame;
        bool run = true;

        cv::namedWindow(title);
        startWindowThread();
        setMouseCallback(title, OnMouse, &cur_frame);

        int render_time = clock();
        int fps = 30;

        thread frame_container_manager([&]() {
            while (run) {
                int range = 200;
                int frame_n = calc_frame_num;
                int begin_pos = max(frame_n - range, 0);
                int end_pos = min(frame_n + range, video_frame_num - 1);
                cv::Mat frame;
                cap.set(CAP_PROP_POS_FRAMES, begin_pos);
                for (int i = begin_pos; i <= end_pos; i++) {
                    if (!video_frames[frame_n]) {
                        i = frame_n;
                        cap.set(CAP_PROP_POS_FRAMES, frame_n);
                    }
                    cap >> frame;
                    if (!video_frames[i]) {
                        resize(frame, frame, Size(960, 540));
                        {
                            cv::Mat roi = frame(Rect(0, 0, 960, 100));
                            cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(0, 0, 0));
                            double alpha = 0.6;
                            cv::addWeighted(color, alpha, roi, 1.0 - alpha, 0.0, roi);
                        }
                        {
                            cv::Mat roi = frame(Rect(0, 530, 960, 10));
                            cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(0, 0, 0));
                            double alpha = 1.0;
                            cv::addWeighted(color, alpha, roi, 1.0 - alpha, 0.0, roi);
                        }
                        {
                            cv::Mat roi = frame(Rect(0, 530, 960 * frame_n / frame_max, 10));
                            cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(0, 0, 200));
                            double alpha = 1.0;
                            cv::addWeighted(color, alpha, roi, 1.0 - alpha, 0.0, roi);
                        }

                        putText(frame, title, Point(10, 25), 2, 0.8, Scalar::all(255), 1, LINE_AA);
                        putText(frame, description, Point(10, 50), 2, 0.4, Scalar::all(255), 1, LINE_AA);
                        putText(frame, "Press the Left or Right Arrow keys or Drag the Mouse to move the frames", Point(10, 70), 2, 0.4, Scalar::all(200), 1, LINE_AA);
                        putText(frame, "Frame Number : " + to_string(i), Point(10, 90), 2, 0.4, Scalar::all(200), 1, LINE_AA);

                        video_frames[i] = new cv::Mat();
                        *(video_frames[i]) = frame;
                    }   
                }
                for (int i = 0; i < video_frames.size(); i++) {
                    if (i < begin_pos || i > end_pos) {
                        if (video_frames[i]) {
                            delete video_frames[i];
                            video_frames[i] = nullptr;
                        }
                    }
                }
                Sleep(10);
            }
        });

        cv::Mat loading_frame(Size(960,540), CV_8UC3);
        while (run) {
            if (video_frames[cur_frame]) {
                imshow(title, *video_frames[cur_frame]);
                video_frames[cur_frame]->copyTo(loading_frame);
                cv::Mat roi = loading_frame(Rect(0, 0, 960, 540));
                cv::Mat color(roi.size(), CV_8UC3, Scalar::all(0));
                double alpha = 0.5;
                cv::addWeighted(color, alpha, roi, 1.0 - alpha, 0.0, roi);
                putText(loading_frame, "Loading", Point(415, 280), 2, 1.2, Scalar::all(255), 2, LINE_AA);
            }
            else {
                imshow(title, loading_frame);
            }
            calc_frame_num = cur_frame;

            int key = waitKeyEx(1);
            switch (key) {
            case ' ':
                run = false;
                break;
            case 2424832:
                cur_frame--;
                break;
            case 2555904:
                cur_frame++;
                break;
            }
            if (cur_frame < frame_min)
                cur_frame = frame_min;
            if (cur_frame > frame_max)
                cur_frame = frame_max;
        }

        frame_container_manager.join();
        cap.release();
        cv::destroyWindow(title);
        system("cls");
        cout << buffer;
        for (int i = 0; i < video_frames.size(); i++) {
            if (video_frames[i]) {
                delete video_frames[i];
                video_frames[i] = nullptr;
            }
        }
        video_frames.clear();
        return (int)cur_frame;
    }
}
