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

# Копіюємо ресурси в папку збірки після того як проєкт скомпілюється
add_custom_command(TARGET camera_vision POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "${CMAKE_BINARY_DIR}/assets"
    COMMENT "Copying assets folder to build directory..."
)
