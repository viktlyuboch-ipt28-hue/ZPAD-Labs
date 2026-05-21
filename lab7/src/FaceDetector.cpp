#include "FaceDetector.hpp"
#include <iostream>

FaceDetector::FaceDetector() {
    try {
        net_ = cv::dnn::readNetFromCaffe("deploy.prototxt", "res10_300x300_ssd_iter_140000.caffemodel");
        if (!net_.empty()) {
            isRunning_ = true;
            // Запускаємо фоновий потік
            workerThread_ = std::thread(&FaceDetector::workerLoop, this);
            std::cout << " Асинхронний FaceDetector успішно запущено у фоні" << std::endl;
        } else {
            std::cerr << " Error. Не виходить завантажити архітектуру Ші" << std::endl;
        }
    } catch (const cv::Exception& e) {
        std::cerr << "Помилка ініціалізації DNN у FaceDetector: " << e.what() << std::endl;
    }
}

FaceDetector::~FaceDetector() {
    if (isRunning_) {
        isRunning_ = false;
        cv_.notify_one(); // Розбудити потік, якщо він спить, для безпечного завершення
        if (workerThread_.joinable()) {
            workerThread_.join(); // Чекаємо на повну зупинку потоку
        }
    }
    std::cout << " Фоновий потік FaceDetector зупинено." << std::endl;
}

void FaceDetector::updateFrame(const cv::Mat& frame) {
    if (frame.empty() || !isRunning_) return;

    std::lock_guard<std::mutex> lock(mutex_);
    // Передаємо копію кадру у спільну змінну для обробки
    frame.copyTo(sharedFrame_);
    hasNewFrame_ = true;
    cv_.notify_one(); // Сповіщаємо фоновий потік, що з'явилася робота
}

std::vector<cv::Rect> FaceDetector::getLatestFaces() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return lastDetectedFaces_;
}

void FaceDetector::workerLoop() {
    while (isRunning_) {
        cv::Mat frameToProcess;

        // Блок очікування нового кадру
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this]() { return !isRunning_ || hasNewFrame_; });
            
            if (!isRunning_) break;

            // Забираємо кадр у локальну матрицю потоку та скидаємо прапорець
            frameToProcess = sharedFrame_.clone();
            hasNewFrame_ = false;
        }

        if (frameToProcess.empty()) continue;

        // Виконуємо інференс нейромережі (важка математика)
        cv::Mat blob = cv::dnn::blobFromImage(frameToProcess, 1.0, cv::Size(300, 300), cv::Scalar(104.0, 177.0, 123.0));
        net_.setInput(blob);
        cv::Mat detections = net_.forward();

        cv::Mat detectionMat(detections.size[2], detections.size[3], CV_32F, detections.ptr<float>());
        std::vector<cv::Rect> localDetectedFaces;

        for (int i = 0; i < detectionMat.rows; i++) {
            float confidence = detectionMat.at<float>(i, 2);

            // Поріг чутливості (35% для стабільності за будь-якого освітлення)
            if (confidence > 0.35f) {
                int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frameToProcess.cols);
                int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frameToProcess.rows);
                int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frameToProcess.cols);
                int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frameToProcess.rows);

                // Захист від виходу за межі екрана
                x1 = std::max(0, std::min(x1, frameToProcess.cols - 1));
                y1 = std::max(0, std::min(y1, frameToProcess.rows - 1));
                x2 = std::max(0, std::min(x2, frameToProcess.cols - 1));
                y2 = std::max(0, std::min(y2, frameToProcess.rows - 1));

                localDetectedFaces.push_back(cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2)));
            }
        }

        // Оновлюємо глобальні координати під м'ютексом
        {
            std::lock_guard<std::mutex> lock(mutex_);
            lastDetectedFaces_ = std::move(localDetectedFaces);
        }
    }
}
