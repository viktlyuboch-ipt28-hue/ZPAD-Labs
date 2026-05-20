#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include "KeyProcessor.hpp"

// Оголошення структури для отримання данних з миші
struct MouseCallbackData {
    KeyProcessor* keyProcessor;
    AppState*     appState;
};

class Display {
public:
    explicit Display(const std::string& windowName = "Camera Vision");
    ~Display();

    void init(AppState& state);
    void show(const cv::Mat& frame);
    void destroy();

    const std::string& windowName() const { return windowName_; }

    // Статичний зворотний виклик миші (OpenCV вимагає зворотного виклику в стилі C)
    static void onMouse(int event, int x, int y, int flags, void* userdata);

private:
    std::string windowName_;
};
