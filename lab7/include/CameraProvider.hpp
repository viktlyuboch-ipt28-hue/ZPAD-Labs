#pragma once
#include <opencv2/opencv.hpp>
#include <string>

class CameraProvider {
public:
    explicit CameraProvider(int deviceIndex = 0);
    ~CameraProvider();

    bool open();
    void release();
    bool isOpened() const;
    cv::Mat getFrame();

    int getWidth() const;
    int getHeight() const;
    double getFPS() const;

private:
    cv::VideoCapture cap_;
    int deviceIndex_;
    cv::Mat lastFrame_;
};
