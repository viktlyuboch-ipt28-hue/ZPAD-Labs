cmake_minimum_required(VERSION 3.12) 
project(CameraVision VERSION 1.0 LANGUAGES CXX)

# налаштовуємо стандарт C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# шукаємо OpenCV
find_package(OpenCV REQUIRED)

#  список усіх вихідних файлів 
set(SOURCES
    src/main.cpp
    src/CameraProvider.cpp
    src/KeyProcessor.cpp
    src/FrameProcessor.cpp
    src/Display.cpp
    src/FaceDetector.cpp
)

# створюємо наш бінарник
add_executable(camera_vision ${SOURCES})

# підключаємо папки із заголовочними файлами
target_include_directories(camera_vision PRIVATE 
    include
    ${OpenCV_INCLUDE_DIRS}
)

# лінкування бібліотеки OpenCV
target_link_libraries(camera_vision PRIVATE 
    ${OpenCV_LIBS}
)

# Налаштування компілятора
if(MSVC)
    target_compile_options(camera_vision PRIVATE /W4 /O2)
else()
    target_compile_options(camera_vision PRIVATE -Wall -Wextra -Wpedantic -O2)
endif()

# Автоматичне копіювання файлів нейромережі в папку збірки після кожного білду
set(FACE_PROTO_URL "https://raw.githubusercontent.com/opencv/opencv/master/samples/dnn/face_detector/deploy.prototxt")
set(FACE_MODEL_URL "https://raw.githubusercontent.com/opencv/opencv_3rdparty/dnn_samples_face_detector_20170830/res10_300x300_ssd_iter_140000.caffemodel")

# Файли будуть завантажуватися безпосередньо в папку build, звідки працює бінарник
set(DNN_TARGET_DIR "${CMAKE_BINARY_DIR}")

if(NOT EXISTS "${DNN_TARGET_DIR}/deploy.prototxt")
    message(STATUS " Завантаження архітектури мережі (deploy.prototxt)")
    file(DOWNLOAD ${FACE_PROTO_URL} "${DNN_TARGET_DIR}/deploy.prototxt" SHOW_PROGRESS STATUS download_status)
    list(GET download_status 0 status_code)
    if(NOT status_code EQUAL 0)
        message(FATAL_ERROR " Не вдалося завантажити deploy.prototxt")
    endif()
endif()

if(NOT EXISTS "${DNN_TARGET_DIR}/res10_300x300_ssd_iter_140000.caffemodel")
    message(STATUS "Завантаження ваг мережі (res10_300x300_ssd_iter_140000.caffemodel)")
    file(DOWNLOAD ${FACE_MODEL_URL} "${DNN_TARGET_DIR}/res10_300x300_ssd_iter_140000.caffemodel" SHOW_PROGRESS STATUS download_status)
    list(GET download_status 0 status_code)
    if(NOT status_code EQUAL 0)
        message(FATAL_ERROR "Не вдалося завантажити caffemodel!")
    endif()
endif()
