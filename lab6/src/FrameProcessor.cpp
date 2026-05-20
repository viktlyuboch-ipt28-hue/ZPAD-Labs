#include "FrameProcessor.hpp"
#include <cmath>
#include <cstdlib>

FrameProcessor::FrameProcessor() : glitchOffset_(0), glitchTick_(0) {}

//  Мейн
//yo
cv::Mat FrameProcessor::process(const cv::Mat& frame, const AppState& state) {
    if (frame.empty()) return frame;

    // застосування яскравості
    double alpha = 0.5 + static_cast<double>(state.brightness) / 100.0;
    int    beta  = static_cast<int>((state.brightness - 50) * 1.5);
    cv::Mat adjusted;
    frame.convertTo(adjusted, -1, alpha, beta);

    cv::Mat result;
    switch (state.mode) {
        case ProcessingMode::GLITCH:   result = applyGlitch(adjusted);   break;
        case ProcessingMode::PIXELATE: result = applyPixelate(adjusted); break;
        default:                       result = adjusted.clone();         break;
    }

    drawLines(result, state);
    drawHUD(result, state);
    return result;
}

//Фільтри

cv::Mat FrameProcessor::applyGlitch(const cv::Mat& frame) const {
    glitchTick_++;
    cv::Mat result = frame.clone();
    std::vector<cv::Mat> channels(3);
    cv::split(result, channels);

    if (glitchTick_ % 3 == 0)
        const_cast<FrameProcessor*>(this)->glitchOffset_ = (rand() % 30) - 15;

    int shift = glitchOffset_;
    int w = frame.cols;

    if (shift > 0)
        channels[2](cv::Rect(0, 0, w - shift, frame.rows)).copyTo(
            channels[2](cv::Rect(shift, 0, w - shift, frame.rows)));
    else if (shift < 0) {
        int s = -shift;
        channels[2](cv::Rect(s, 0, w - s, frame.rows)).copyTo(
            channels[2](cv::Rect(0, 0, w - s, frame.rows)));
    }

    int s2 = std::abs(shift) / 2 + 3;
    if (w > s2)
        channels[0](cv::Rect(s2, 0, w - s2, frame.rows)).copyTo(
            channels[0](cv::Rect(0, 0, w - s2, frame.rows)));

    if (glitchTick_ % 5 == 0) {
        int y = rand() % frame.rows;
        int h = std::min(rand() % 8 + 2, frame.rows - y);
        int dx = (rand() % 40) - 20;
        if (dx > 0 && dx < w) {
            cv::Mat strip = result(cv::Rect(0, y, w, h)).clone();
            cv::Mat dst   = result(cv::Rect(0, y, w, h));
            strip(cv::Rect(0, 0, w - dx, h)).copyTo(dst(cv::Rect(dx, 0, w - dx, h)));
        }
    }

    cv::merge(channels, result);
    return result;
}

cv::Mat FrameProcessor::applyPixelate(const cv::Mat& frame) const {
    int block = 16;
    cv::Mat small, big;
    cv::resize(frame, small, {frame.cols / block, frame.rows / block}, 0, 0, cv::INTER_LINEAR);
    cv::resize(small, big, {frame.cols, frame.rows}, 0, 0, cv::INTER_NEAREST);
    return big;
}

//Шари

void FrameProcessor::drawLines(cv::Mat& frame, const AppState& state) const {
    for (const auto& [p1, p2] : state.drawnLines) {
        cv::line(frame, p1, p2, {0, 255, 0}, 2, cv::LINE_AA);
        cv::circle(frame, p1, 3, {0, 200, 0}, -1);
        cv::circle(frame, p2, 3, {0, 200, 0}, -1);
    }
    if (state.mouseDrawing && state.drawStart.x >= 0)
        cv::line(frame, state.drawStart, state.drawEnd, {0, 255, 255}, 2, cv::LINE_AA);
}

// HUD 

void FrameProcessor::drawHUD(cv::Mat& frame, const AppState& state) const {
    std::string modeStr = "Mode: " + modeToString(state.mode);
    int baseline = 0;
    cv::Size ts = cv::getTextSize(modeStr, cv::FONT_HERSHEY_SIMPLEX, 0.65, 1, &baseline);
    drawTextWithBackground(frame, modeStr, {(frame.cols - ts.width) / 2, 30}, 0.65, {255, 255, 0});

    std::string help = "[0] Normal  [1] Glitch  [2] Pixelate  [D] Draw  [Tab] cycle  [X] clear  [slider] brightness  [Q] quit";
    drawTextWithBackground(frame, help, {5, frame.rows - 8}, 0.38, {180, 180, 180});
}

void FrameProcessor::drawTextWithBackground(cv::Mat& img, const std::string& text,
                                             cv::Point pos, double scale,
                                             cv::Scalar color, int thickness) const {
    int baseline = 0;
    cv::Size ts = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, scale, thickness, &baseline);
    cv::Rect bg(pos.x - 2, pos.y - ts.height - 2, ts.width + 4, ts.height + baseline + 4);
    bg &= cv::Rect(0, 0, img.cols, img.rows);
    if (bg.width > 0 && bg.height > 0)
        img(bg) *= 0.4;
    cv::putText(img, text, pos, cv::FONT_HERSHEY_SIMPLEX, scale, color, thickness, cv::LINE_AA);
}
