#pragma once
#include <opencv2/opencv.hpp>
#include <string>

enum class ProcessingMode {
    NORMAL = 0,
    GLITCH,
    PIXELATE,
    DRAW,
    COUNT
};

std::string modeToString(ProcessingMode mode);

struct AppState {
    ProcessingMode mode = ProcessingMode::NORMAL;

    // Малювання
    bool mouseDrawing = false;
    cv::Point drawStart{-1, -1};
    cv::Point drawEnd{-1, -1};
    std::vector<std::pair<cv::Point, cv::Point>> drawnLines;

    // Зум мишкою
    double zoomFactor = 1.0;

    // Трекінг яскравості
    int brightness = 50;   // від 0 до 100
};

class KeyProcessor {
public:
    KeyProcessor();

    // Повертає false коли користувач хоче вийти
    bool processKey(int key, AppState& state);

    // Допоміжні підказки по миші - викликається зі статичної обгортки
    void processMouse(int event, int x, int y, int flags, AppState& state);

private:
    void cycleModeForward(AppState& state);
    void cycleModeBackward(AppState& state);
};
