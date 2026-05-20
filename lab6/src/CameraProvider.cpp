#include "CameraProvider.hpp"
#include <iostream>

CameraProvider::CameraProvider(int deviceIndex)
    : deviceIndex_(deviceIndex) {}

CameraProvider::~CameraProvider() {
    release();
}

bool CameraProvider::open() {
    cap_.open(deviceIndex_);
    if (!cap_.isOpened()) {
        std::cerr << "[CameraProvider] Cannot open camera with index " << deviceIndex_ << std::endl;
        return false;
    }
    cap_.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap_.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    std::cout << "[CameraProvider] Camera opened: "
              << getWidth() << "x" << getHeight()
              << " @ " << getFPS() << " FPS" << std::endl;
    return true;
}

void CameraProvider::release() {
    if (cap_.isOpened())
        cap_.release();
}

bool CameraProvider::isOpened() const {
    return cap_.isOpened();
}

cv::Mat CameraProvider::getFrame() {
    cv::Mat frame;
    cap_ >> frame;
    if (frame.empty()) {
        std::cerr << "[CameraProvider] Empty frame received" << std::endl;
        return lastFrame_.clone();
    }
    lastFrame_ = frame.clone();
    return frame;
}

int CameraProvider::getWidth() const {
    return static_cast<int>(cap_.get(cv::CAP_PROP_FRAME_WIDTH));
}

int CameraProvider::getHeight() const {
    return static_cast<int>(cap_.get(cv::CAP_PROP_FRAME_HEIGHT));
}

double CameraProvider::getFPS() const {
    return cap_.get(cv::CAP_PROP_FPS);
}
