#pragma once

#include <string>

#include "ext/imgui/imgui.h"

namespace show {
extern long TASK_WAIT_SLEEP_MS;
extern long TASK_SLEEP_MS;
extern float FONT_SIZE;
extern std::string DEFAULT_GRAPH_NAME; 
extern std::string DEFAULT_VAR_NAME_PREFIX; 
extern int HEIGHT_OF_EACH;
extern float HISTORY_DEFAULT;
namespace item {
extern int BUFFER_SIZE;
}
namespace ui {
extern ImVec4 WINDOW_BG;
}
}

