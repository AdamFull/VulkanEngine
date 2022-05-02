#pragma once
#include "OverlayBase.h"

namespace Engine
{
    namespace Core
    {
        namespace Overlay
        {
            class COverlayConsole : public COverlayBase
            {
            public:
                COverlayConsole() = default;
                COverlayConsole(std::string srName);
                ~COverlayConsole();

                void draw() override;

                static int stricmp(const char *s1, const char *s2);
                static int strnicmp(const char *s1, const char *s2, int n);
                static char* strdup(const char *s);
                static void strtrim(char *s);

                static int textEditCallbackStub(ImGuiInputTextCallbackData *data);

            private:
                void clearLog();
                void execCommand(const char *command_line);
                int textEditCallback(ImGuiInputTextCallbackData *data);
                void addLog(const char *fmt, ...) IM_FMTARGS(2);

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