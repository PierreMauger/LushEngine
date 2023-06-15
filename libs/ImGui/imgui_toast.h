#ifndef TOAST_H
#define TOAST_H

#include <string>
#include <vector>

#include "imgui.h"

enum ToastType {
    SUCCESS,
    INFO,
    WARNING,
    ERROR,
};

struct Toast {
        std::string title;
        ToastType status;
        std::string message;
        float duration;
        ImVec4 color;
        int id;
        bool show;
};

void CreateToast(const std::string &title, const std::string &message, ToastType status = ToastType::INFO);
void UpdateToasts(float deltaTime);
void DrawToasts();

#endif // TOAST_H
