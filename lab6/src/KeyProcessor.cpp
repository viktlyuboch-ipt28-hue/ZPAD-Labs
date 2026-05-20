#include "KeyProcessor.hpp"
#include <iostream>
#include <algorithm>

std::string modeToString(ProcessingMode mode) {
    switch (mode) {
        case ProcessingMode::NORMAL:   return "Normal";
        case ProcessingMode::GLITCH:   return "Glitch";
        case ProcessingMode::PIXELATE: return "Pixelate";
        case ProcessingMode::DRAW:     return "Draw (LMB drag)";
        default:                       return "Unknown";
    }
}

KeyProcessor::KeyProcessor() {}

bool KeyProcessor::processKey(int key, AppState& state) {
    if (key == -1) return true;

    switch (key) {
        case 27:   // вихід через esc
        case 'q':
        case 'Q':
            return false;

        case '0': state.mode = ProcessingMode::NORMAL;   break;
        case '1': state.mode = ProcessingMode::GLITCH;   break;
        case '2': state.mode = ProcessingMode::PIXELATE; break;
        case 'd':
        case 'D': state.mode = ProcessingMode::DRAW;     break;

        // Стерти рисунки
        case 'x':
        case 'X':
            state.drawnLines.clear();
            break;

        // Tab наступний фільтр
        case 9:
            cycleModeForward(state);
            break;

        // back - минулий фільтр
        case '`':
            cycleModeBackward(state);
            break;

        default:
            break;
    }

    return true;
}

void KeyProcessor::processMouse(int event, int x, int y, int /*flags*/, AppState& state) {

    // для малювання
    if (state.mode == ProcessingMode::DRAW) {
        if (event == cv::EVENT_LBUTTONDOWN) {
            state.mouseDrawing = true;
            state.drawStart = {x, y};
            state.drawEnd   = {x, y};
        } else if (event == cv::EVENT_MOUSEMOVE && state.mouseDrawing) {
            state.drawEnd = {x, y};
        } else if (event == cv::EVENT_LBUTTONUP && state.mouseDrawing) {
            state.drawEnd = {x, y};
            state.drawnLines.push_back({state.drawStart, state.drawEnd});
            state.mouseDrawing = false;
            state.drawStart = {-1, -1};
            state.drawEnd   = {-1, -1};
        }
        return;
    }
}

void KeyProcessor::cycleModeForward(AppState& state) {
    int next = (static_cast<int>(state.mode) + 1) % static_cast<int>(ProcessingMode::COUNT);
    state.mode = static_cast<ProcessingMode>(next);
}

void KeyProcessor::cycleModeBackward(AppState& state) {
    int prev = static_cast<int>(state.mode) - 1;
    if (prev < 0) prev = static_cast<int>(ProcessingMode::COUNT) - 1;
    state.mode = static_cast<ProcessingMode>(prev);
}
