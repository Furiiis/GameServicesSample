cmake_minimum_required(VERSION 3.26)
project(GameServicesApp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавьте директорию с заголовочными файлами ImGui
include_directories(libs/imgui)

# Укажите директории, где находятся заголовочные файлы
include_directories(libs/sdk/include)

# Укажите, где находятся ваши библиотечные файлы
if(UNIX)
    link_directories(${CMAKE_SOURCE_DIR}/libs/sdk/lib/Linux)
elseif(WIN32)
    link_directories(${CMAKE_SOURCE_DIR}/libs/sdk/lib/Win64)
endif()

# Добавьте файлы ImGui в проект
set(IMGUI_SOURCES
        libs/imgui/imgui.cpp
        libs/imgui/imgui_draw.cpp
        libs/imgui/imgui_widgets.cpp
        libs/imgui/imgui_tables.cpp
)

if(UNIX)
    set(OPENGL_SOURCES
            libs/imgui/backends/imgui_impl_glfw.cpp  # Если используете GLFW
            libs/imgui/backends/imgui_impl_opengl3.cpp # Если используете OpenGL
    )
elseif(WIN32)
    set(OPENGL_SOURCES
            libs/imgui/backends/imgui_impl_glfw.cpp  # Если используете GLFW
            libs/imgui/backends/imgui_impl_opengl3.cpp # Если используете OpenGL
    )
endif()

# Укажите директории с заголовками
include_directories(
        ${CMAKE_SOURCE_DIR}
)

# Добавьте ваши источники
set(SOURCES
        ${IMGUI_SOURCES}
        ${OPENGL_SOURCES}
        ui_models/friends_model.h
        services_connector/connector.cpp
        event_bus/event_bus.cpp
        event_bus/event_bus.h
        services_connector/friends_service.cpp
        services_connector/friends_service.h
        services_connector/connector.h
        event_bus/listener.cpp
        event_bus/listener.h
        ui_models/logs_model.cpp
        ui_models/logs_model.h
        services_connector/login_service.cpp
        services_connector/login_service.h
        ui_models/message_model.h
        os/utils.h
        os/platforms/linux.cpp
        services_connector/messages_connector.cpp
        services_connector/messages_connector.h
        ui_models/profile_model.h
        views/window.h
        ui_models/main_model.h
        os/platforms/windows.cpp
        services_connector/logs_service.cpp
        ui_models/login_model.h
        services_connector/services_models/login_model.h
)

# Условно добавьте файл main в зависимости от платформы
if(WIN32)
    list(APPEND SOURCES win_main.cpp)
elseif(UNIX)
    list(APPEND SOURCES linux_main.cpp)
    #set(SOURCES linux_main.cpp)
endif()

# Создайте исполняемый файл
add_executable(${PROJECT_NAME} ${SOURCES})

add_custom_target(generate_views ALL
        COMMAND python ${CMAKE_SOURCE_DIR}/view_generator.py
        ui_models
        views
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        VERBATIM
        COMMENT "Generating views..."
)
add_dependencies(${PROJECT_NAME} generate_views)

# Укажите директорию, где будут сгенерированы файлы
set(VIEW_DIR ${CMAKE_SOURCE_DIR}/views)

# Добавьте сгенерированные файлы в сборку
file(GLOB_RECURSE GENERATED_FILES "${VIEW_DIR}/*.cpp" "${VIEW_DIR}/*.h")
target_sources(${PROJECT_NAME} PRIVATE ${GENERATED_FILES})

if(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE pros.sdk.linux.x64)
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE pros.sdk.x64 rpcrt4)
endif()

# Свяжите библиотеки, которые вы используете (например, GLFW, OpenGL)
find_package(OpenGL REQUIRED)

if(UNIX)
    find_package(glfw3 REQUIRED)
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL )
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw OpenGL::GL)
endif ()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/libs/sdk/bin/Win64/pros.sdk.x64.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)
