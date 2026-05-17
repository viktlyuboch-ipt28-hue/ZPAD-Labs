#include "Display.hpp"
#include <iostream>

Display::Display(const std::string& windowName)
    : windowName_(windowName) {}

Display::~Display() {
    destroy();
}
//so, sup, why r u here?:D
void Display::init(AppState& state) {
    cv::namedWindow(windowName_, cv::WINDOW_NORMAL);
    cv::resizeWindow(windowName_, 640, 480);

    // трекбар для яскравості
    cv::createTrackbar("Brightness", windowName_, &state.brightness, 100, nullptr);

    // зворотний звязок з мишкою
    cbData_.appState = &state;
    cbData_.keyProcessor = nullptr;  
    // зберігаємо вказівник безпосередньо на стан; тут викликається KeyProcessor::processMouse
    cv::setMouseCallback(windowName_, &Display::onMouse, &state);

    std::cout << "[Display] Window \"" << windowName_ << "\" created." << std::endl;
}

void Display::show(const cv::Mat& frame) {
    if (frame.empty()) return;
    cv::imshow(windowName_, frame);
}

void Display::destroy() {
    cv::destroyWindow(windowName_);
}

// Статичний зворотний виклик миші — пересилає до KeyProcessor через AppState*
// Ми підключаємо логіку обробки подій миші безпосередньо сюди, використовуючи обхідний шлях C API без лямбда-виразів.
// Фактична зміна стану виконується через глобальний обробник миші у main().
void Display::onMouse(int /*event*/, int /*x*/, int /*y*/, int /*flags*/, void* /*userdata*/) {
   //тут порожньо — справжній зворотний виклик встановлюється в main() після створення KeyProcessor
}
