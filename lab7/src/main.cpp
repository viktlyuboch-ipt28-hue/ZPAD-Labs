#include <iostream>
#include <opencv2/opencv.hpp>

#include "CameraProvider.hpp"
#include "KeyProcessor.hpp"
#include "FrameProcessor.hpp"
#include "Display.hpp"

// Глобальний стан для виклику мишки
struct MouseData {
    KeyProcessor* kp;
    AppState*     state;
};

static void mouseCallback(int event, int x, int y, int flags, void* userdata) {
    auto* data = static_cast<MouseData*>(userdata);
    data->kp->processMouse(event, x, y, flags, *data->state);
}



int main(int argc, char* argv[]) {
    int cameraIndex = 0;
    if (argc > 1) cameraIndex = std::stoi(argv[1]);

    // Створення модулів 
    CameraProvider camera(cameraIndex);
    KeyProcessor   keyProc;
    FrameProcessor frameProc;
    Display        display("Camera Vision");
    AppState       state;

    // Ініт 
    if (!camera.open()) {
        std::cerr << "Failed to open camera. Exiting." << std::endl;
        return 1;
    }

    display.init(state);

    // перевизначення зворотнього виклику миші на той що знає KeyProcessor
    static MouseData mouseData{&keyProc, &state};
    cv::setMouseCallback(display.windowName(), mouseCallback, &mouseData);

    std::cout << "\n>:3 Camera Vision started C:<" << std::endl;
    std::cout << "Keys: [0-2] modes | [D] draw mode | [F] Face" << std::endl;
    std::cout << "      [Tab] cycle mode " << std::endl;
    std::cout << "      [X] clear drawings | [Q] / [Esc] = quit" << std::endl;

    // основний цикл
    while (true) {
        // 1. захоплення кадру
        cv::Mat frame = camera.getFrame();
        if (frame.empty()) {
            cv::waitKey(10);
            continue;
        }

        // 2. обробка кадру
        cv::Mat output = frameProc.process(frame, state);

        // 3. дисплей
        display.show(output);

        // 4. обробка клавіатури 
        int key = cv::waitKey(1) & 0xFF;
        if (!keyProc.processKey(key, state)) break;
    }

    //  Очистка
    camera.release();
    display.destroy();
    std::cout << "BYE BYE!" << std::endl;
    return 0;
}
