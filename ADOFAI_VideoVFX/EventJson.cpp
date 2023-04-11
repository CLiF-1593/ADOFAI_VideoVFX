#include "EventJson.h"

Json::Value RemoveTile() {
    Json::Value ret;
    ret["floor"] = 0;
    ret["eventType"] = "RecolorTrack";
    ret["startTile"][0] = 0;
    ret["startTile"][1] = "Start";
    ret["endTile"][0] = 0;
    ret["endTile"][1] = "End";
    ret["trackColorType"] = "Single";
    ret["trackColor"] = "debb7b00";
    ret["secondaryTrackColor"] = "ffffff";
    ret["trackColorAnimDuration"] = 0;
    ret["trackColorPulse"] = "None";
    ret["trackPulseLength"] = 0;
    ret["trackStyle"] = "Standard";
    ret["trackGlowIntensity"] = 0;
    ret["angleOffset"] = 0;
    ret["eventTag"] = "";
    return ret;
}
