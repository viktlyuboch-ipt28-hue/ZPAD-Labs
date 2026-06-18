#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>

class FaceDetector {
public:
    FaceDetector();
    ~FaceDetector();

    // Передати новий кадр для фонового аналізу
    void updateFrame(const cv::Mat& frame);

    // Забрати останні відомі координати знайдених облич 
    std::vector<cv::Rect> getLatestFaces() const;

private:
    // Головний цикл фонового потоку
    void workerLoop();

    // Компоненти багатопоточності
    std::thread workerThread_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> isRunning_{false};

    // Нейромережа
    cv::dnn::Net net_;

    // Спільні дані між потоками 
    cv::Mat sharedFrame_;
    bool hasNewFrame_{false};
    std::vector<cv::Rect> lastDetectedFaces_;
};
