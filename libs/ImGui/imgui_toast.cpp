#include "imgui_toast.h"

static std::vector<Toast> toasts;
static int toastCounter = 0;

void CreateToast(const std::string &title, const std::string &message, ToastType status)
{
    Toast toast;
    toast.title = title;
    toast.message = message;
    toast.status = status;
    toast.duration = 5.0f;
    toast.id = toastCounter++;
    toast.show = true;
    switch (toast.status) {
    case ToastType::SUCCESS:
        toast.color = ImColor(7, 188, 12);
        break;
    case ToastType::INFO:
        toast.color = ImColor(52, 152, 219);
        break;
    case ToastType::WARNING:
        toast.color = ImColor(241, 196, 15);
        break;
    case ToastType::ERROR:
        toast.color = ImColor(231, 76, 60);
        break;
    default:
        toast.color = ImColor(52, 152, 219);
        break;
    }
    toasts.push_back(toast);
}

void UpdateToasts(float deltaTime)
{
    for (auto it = toasts.begin(); it != toasts.end();) {
        Toast &toast = *it;
        toast.duration -= deltaTime;

        if (toast.duration <= 0.0f || !toast.show)
            it = toasts.erase(it);
        else
            it++;
    }
}

void DrawToasts()
{
    std::size_t it = 0;
    ImVec2 pos = ImVec2(ImGui::GetIO().DisplaySize.x - 10.0f, ImGui::GetIO().DisplaySize.y - 10.0f);
    for (Toast &toast : toasts) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, toast.color);
        ImGui::SetNextWindowPos({pos.x, pos.y - (toasts.size() - it - 1) * 60}, ImGuiCond_Always, ImVec2(1.0f, 1.0f));
        ImGui::Begin(("Toast##" + std::to_string(toast.id)).c_str(), &toast.show,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                         ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);

        float lineWidth = ImGui::GetContentRegionAvail().x * toast.duration / 5.0f;

        ImVec2 linePos = ImGui::GetCursorScreenPos();
        linePos.y -= 5.0f;
        ImGui::GetWindowDrawList()->AddLine(linePos, ImVec2(linePos.x + lineWidth, linePos.y), IM_COL32_WHITE, 2.0f);

        ImGui::Text("%s", toast.title.c_str());
        ImGui::Separator();
        ImGui::Text("%s", toast.message.c_str());

        if (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            toast.show = false;
        }

        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        it++;
    }
}
