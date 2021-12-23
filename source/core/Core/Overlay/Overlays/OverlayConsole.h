#pragma once
#include "OverlayBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Overlay
        {
            class OverlayConsole : public OverlayBase
            {
            public:
                OverlayConsole() = default;
                OverlayConsole(std::string srName);
                ~OverlayConsole();

                void Draw() override;

                static int Stricmp(const char *s1, const char *s2);
                static int Strnicmp(const char *s1, const char *s2, int n);
                static char *Strdup(const char *s);
                static void Strtrim(char *s);

                static int TextEditCallbackStub(ImGuiInputTextCallbackData *data);

            private:
                void ClearLog();
                void ExecCommand(const char *command_line);
                int TextEditCallback(ImGuiInputTextCallbackData *data);
                void AddLog(const char *fmt, ...) IM_FMTARGS(2);

                char InputBuf[256];
                ImVector<char *> Items;
                ImVector<const char *> Commands;
                ImVector<char *> History;
                int HistoryPos; // -1: new line, 0..History.Size-1 browsing history.
                ImGuiTextFilter Filter;
                bool AutoScroll{true};
                bool ScrollToBottom{false};
            };
        }
    }
}