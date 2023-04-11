#pragma once
#include "json/json.h"
using namespace std;

enum FileType { ADOFAI, VIDEO };

void ReadJson(const std::string& file, Json::Value& root);
void WriteJson(const Json::Value& value, string location);
string OpenFileName(FileType type);
string OpenFolderName();