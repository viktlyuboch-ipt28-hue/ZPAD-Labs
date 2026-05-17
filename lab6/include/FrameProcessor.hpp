#pragma once
#include <opencv2/opencv.hpp>
#include "KeyProcessor.hpp"

class FrameProcessor {
public:
    FrameProcessor();

    // вхід
    cv::Mat process(const cv::Mat& frame, const AppState& state);

    // для використання зуму
    cv::Mat applyZoom(const cv::Mat& frame, double factor);

    // Перекриття всіх елементів HUD (stats, mode name, help)
    void drawHUD(cv::Mat& frame, const AppState& state);

private:
    // Окремі фільтри
    cv::Mat applyGlitch(const cv::Mat& frame) const;
    cv::Mat applyPixelate(const cv::Mat& frame) const;

    // Шари
    void drawLines(cv::Mat& frame, const AppState& state) const;

    // Підказки
    void drawTextWithBackground(cv::Mat& img, const std::string& text,
                                cv::Point pos, double scale,
                                cv::Scalar color, int thickness = 1) const;

    int glitchOffset_ = 0;
    mutable int glitchTick_ = 0;
};
