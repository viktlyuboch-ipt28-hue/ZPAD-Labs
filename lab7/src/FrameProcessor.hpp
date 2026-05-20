#pragma once
#include <opencv2/opencv.hpp>
#include "KeyProcessor.hpp"
#include "FaceDetector.hpp"

class FrameProcessor {
public:
    FrameProcessor();

    // вхід
    cv::Mat process(const cv::Mat& frame, const AppState& state);


    // Перекриття всіх елементів HUD (stats, mode name, help)
    void drawHUD(cv::Mat& frame, const AppState& state) const;

private:

    //коробка
    cv::Mat applyFaceDetection(const cv::Mat& frame) const;
    // Окремі фільтри
    cv::Mat applyGlitch(const cv::Mat& frame) const;
    cv::Mat applyPixelate(const cv::Mat& frame) const;

    // Шари
    void drawLines(cv::Mat& frame, const AppState& state) const;

    // Підказки
    void drawTextWithBackground(cv::Mat& img, const std::string& text,
                                cv::Point pos, double scale,
                                cv::Scalar color, int thickness = 1) const;

    mutable int glitchOffset_ = 0;
    mutable int glitchTick_ = 0;
    FaceDetector faceDetector_;
};
