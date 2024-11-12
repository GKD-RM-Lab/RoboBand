#pragma once

#include <string>

#include "ext/imgui/imgui.h"

namespace show {
namespace param {
constexpr long TASK_WAIT_SLEEP_MS {100};
constexpr long TASK_SLEEP_MS {2};
constexpr float FONT_SIZE {18.0f};
constexpr int BUFFER_SIZE {30000};
constexpr std::string DEFAULT_GRAPH_NAME {"untitled"}; 
constexpr std::string DEFAULT_VAR_NAME_PREFIX {"var"}; 
constexpr int HEIGHT_OF_EACH {300};


const bool USE_TAB = true;
const ImVec4 WINDOW_BG = ImVec4(0.114, 0.098, 0.161, 0.75);
const int SOCKET_PORT = 14514;
const int SOCKET_BUFFERSIZE = 100000;
const float SOCKET_TIMEOUT_MS = 100;

const int GROUP_ID = 0;
const std::string HOST = "localhost";
const int PORT = 14515;
const bool USE_GUI_TIME = false;

const float WIDTH = 1.5f;
const bool FILLED = false;
}
}

