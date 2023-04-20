#include <iostream>
#include <string>
#include <Windows.h>
#include <ctime>
#include <vector>
#include <filesystem>
#include "File.h"
#include "ImageProcessing.h"
#include "EventJson.h"
#include <opencv2/core/utils/logger.hpp>
#include <Windows.h>

int main() {
	cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);
	//SetConsoleOutputCP(CP_UTF8);
	//SetConsoleCP(CP_UTF8);

	cout << "[ ADOFAI Video VFX - 얼불춤 영상 이펙트 ]" << endl << endl;
	cout << "Developer : CLiF (https://www.youtube.com/@clif-1593)" << endl;
	cout << "Version : v1.0" << endl << endl;

	cout << "이 프로그램은 얼불춤 이펙트를 영상으로 치환함으로써 플레이 경험을 증진시키기 위해 제작되었습니다." << endl;
	cout << "This program was developed to improve the game-playing experience by change visual effects to video." << endl << endl;
	cout << "오류를 발견하시거나 추가 기능을 요청하시고 싶으시다면 디스코드 DM 부탁드립니다. (CLiF [클리프] #8404)" << endl;
	cout << "If you find an error or want to request an additional function, please contact by Discord DM." << endl << endl;
	system("pause");
	system("cls");

	cout << "[ ADOFAI Video VFX - 얼불춤 영상 이펙트 ]" << endl << endl;
	cout << "Developer : CLiF (https://www.youtube.com/@clif-1593)" << endl;
	cout << "Version : v1.0" << endl << endl;

	cout << "!! Regulations : 이용 규정 !!" << endl << endl;
	cout << "타인의 영상 및 저작물을 허락 없이 다운로드 하여 활용하는 것은 금물입니다." << endl;
	cout << "반드시 자신이 직접 녹화한 영상 혹은 영상 저작자가 허가한 맵 영상만을 활용해야 합니다." << endl;
	cout << "활용한 영상의 출처를 반드시 표기해야하며, 이를 위반하여 생기는 모든 책임은 이용자에게 있습니다." << endl;
	cout << "기본적으로 맵의 재배포는 엄격히 금지되며, 맵 제작자와 영상 저작자 모두의 허락이 있을 시 재배포가 가능합니다." << endl;
	cout << "이 프로그램의 사용으로 인해 저작권 혹은 그 외 권리 침해 문제가 발생할 시" << endl;
	cout << "개발자의 권한으로 이 프로그램의 사용을 무기한 금지 시킬 수 있습니다." << endl << endl;

	cout << "Downloading and using other people's videos and works without permission is not allowed." << endl;
	cout << "You must use only the video that you recorded yourself or with the author's permission." << endl;
	cout << "The source of the used video must be indicated, and the user is responsible for violating it." << endl;
	cout << "By default, the redistribution of maps is strictly prohibited," << endl;
	cout << "but it is possible with the permission of both map creators and video authors." << endl;
	cout << "If the use of this program causes copyright or other problems," << endl;
	cout << "the developer (CLiF) can indefinitely prohibit the use of this program." << endl << endl;

	cout << "이용 규정을 반드시 읽고 다음으로 넘어가십시오." << endl;
	cout << "Be sure to read the usage regulations and move on." << endl << endl;
	system("pause");
	system("cls");
	cout << "[ ADOFAI Video VFX - 얼불춤 영상 이펙트 ]" << endl << endl;
	cout << "Developer : CLiF (https://www.youtube.com/@clif-1593)" << endl;
	cout << "Version : v1.0" << endl << endl;

	cout << "< Choose the Video File - 영상 파일 선택 >"  << endl;
	ERR_VIDEO_SELECTION:
	string video_path = OpenFileName(VIDEO);
	if (video_path.empty()) {
		cout << "> You should choose the video. - 영상 파일을 선택해야합니다." << endl;
		goto ERR_VIDEO_SELECTION;
	}
	cout << "> Path : " << video_path << endl  << endl;
	ImageProcessing::ReadVideo(video_path);

	cout << "< Choose the ADOFAI File - 얼불춤 파일 선택 >" << endl;
	ERR_ADOFAI_FILE_SELECTION:
	string adofai_path = OpenFileName(ADOFAI);
	if (adofai_path.empty()) {
		cout << "> You should choose the adofai file. - 얼불춤 맵 파일을 선택해야합니다." << endl;
		goto ERR_ADOFAI_FILE_SELECTION;
	}
	string folder_path = adofai_path;
	while (folder_path.back() != '\\') {
		folder_path.pop_back();
	}

	cout << "> Path : " << adofai_path << endl << endl;

	Json::Value adofai;
	ReadJson(adofai_path, adofai);

	cout << "< First Frame - 첫 프레임 선택 >" << endl;
	int begin_frame = ImageProcessing::GetFrame(video_path, "First Frame", "Choose the first frame of the video that you want to edit.", 1);
	cout << "> Frist Frame : " << begin_frame << endl << endl;

	cout << "< Game Start Frame - 게임 시작 프레임 선택 >" << endl;
	int game_start_frame = ImageProcessing::GetFrame(video_path, "Game Start Frame", "Choose the PRECISE frame that game start.", 1);
	cout << "> Game Start Frame : " << game_start_frame << endl << endl;

	cout << "< Last Frame - 마지막 프레임 선택 >" << endl;
	int end_frame = ImageProcessing::GetFrame(video_path, "Last Frame", "Choose the last frame of the video that you want to edit.", -1);
	cout << "> Last Frame : " << end_frame << endl << endl;
	
	cout << "< Playback Speed - 영상 배속 >" << endl;
	double pitch = 0.0;
	ERR_PITCH:
	cout << "> Speed (ex 1.0, 2.5, 0.5) : ";
	cin >> pitch;
	if (pitch <= 0 || cin.fail()) {
		cin.clear();
		cin.ignore(100000, '\n');
		cout << "> The Number of Repetitions is out of the range. - 잘못된 입력 (범위 넘어감)" << endl;
		goto ERR_PITCH;
	}
	cout << endl << endl;

	cout << "< Generating >" << endl << endl;

	cout << "> Saving the Video . . ." << endl;
	string vfx_video_path = folder_path + "[VideoVFX] tmp.mp4";
	ImageProcessing::SaveVideo(video_path, vfx_video_path, begin_frame, end_frame);

	cout << "> Compressing the Video . . ." << endl;
	string vfx_final_video_path = folder_path + "[VideoVFX] video.mp4";
	ImageProcessing::ConvertBitrate(vfx_video_path, vfx_final_video_path, "5500k", "6500k", pitch);
	filesystem::remove(vfx_video_path);
	
	cout << "> Setting ADOFAI File . . ." << endl;

	//Copy ADOFAI
	Json::Value vfx_adofai;
	if (adofai.isMember("angleData")) {
		vfx_adofai["angleData"] = adofai["angleData"];
	}
	else {
		vfx_adofai["pathData"] = adofai["pathData"];
	}
	vfx_adofai["settings"] = adofai["settings"];
	vfx_adofai["actions"].append(RemoveTile());
	for (int i = 0; i < adofai["actions"].size(); i++) {
		string name = adofai["actions"][i]["eventType"].asString();
		if (name == "SetSpeed" || name == "Twirl" || name == "Pause" || name == "AutoPlayTiles" || name == "Checkpoint" || name == "SetHoldSound" || name == "MultiPlanet" || name == "ScaleMargin" || name == "FreeRoam" || name == "FreeRoamTwirl" || name == "FreeRoamRemove" || name == "SetHitsound" || name == "PlaySound") {
			vfx_adofai["actions"].append(adofai["actions"][i]);
		}
	}

	// Set Camera
	vfx_adofai["settings"]["position"][0] = 10000;
	vfx_adofai["settings"]["position"][1] = 10000;

	// Set Video
	double delta_frame = game_start_frame - begin_frame;
	double time_interval = delta_frame / ImageProcessing::video_fps * 1000;
	vfx_adofai["settings"]["bgVideo"] = "[VideoVFX] video.mp4";
	vfx_adofai["settings"]["bgImage"] = "";
	vfx_adofai["settings"]["vidOffset"] = int(time_interval / pitch);
	vfx_adofai["settings"]["showDefaultBGIfNoImage"] = "Disabled";

	cout << "> Saving ADOFAI File . . ." << endl;

	while (adofai_path.back() != '.') adofai_path.pop_back();
	adofai_path.pop_back();

	WriteJson(vfx_adofai, adofai_path + " [Video VFX Ver].adofai");

	cout << endl << endl;
	cout << "< Completed!! - 완료되었습니다 >" << endl << endl;
	system("pause");
	return 0;
}