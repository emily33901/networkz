#include "console.hh"

#include <imgui.h>

#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <unordered_map>
#include <vector>

namespace console {

static bool Strnicmp(char const *s1, char const *s2, int n) {
    for (int i = 0; i < n; ++i) {
        unsigned char c1 = static_cast<unsigned char>(s1[i]);
        unsigned char c2 = static_cast<unsigned char>(s2[i]);
        if (tolower(c1) != tolower(c2))
            return false;
        if (c1 == '\0' || c2 == '\0')
            break;
    }
    return true;
}
static void Strtrim(char *str) {
    char *str_end = str + strlen(str);
    while (str_end > str && str_end[-1] == ' ') str_end--;
    *str_end = 0;
}

static std::unordered_map<std::string, Callback> commands;
static std::vector<std::string>                  history;
static std::vector<std::string>                  items;
static int                                       historyPos;

static bool open = false;

bool autoScroll;
bool scrollToBottom;

char inputBuf[256];

void Register(const std::string &command, Callback cb) {
    commands[command] = cb;
}

void Log(const char *fmt, ...) {
    // TODO this is badly optimized but it shouldnt matter

    char    buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    buf[IM_ARRAYSIZE(buf) - 1] = 0;
    va_end(args);
    items.push_back(strdup(buf));
}

void ClearLog() {
    items.clear();
}

void Execute(const std::string &command) {
    Log("# %s\n", command.c_str());

    // Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
    historyPos = -1;
    for (int i = history.size() - 1; i >= 0; i--) {
        if (history[i] == command) {
            history.erase(history.begin() + i);
            break;
        }
    }
    history.push_back(command);

    auto firstSpace = command.find(" ");
    auto args       = std::string();

    if (firstSpace == command.npos) {
        firstSpace = command.size();
    } else {
        args = command.substr(firstSpace + 1);
    }

    auto justCommand = command.substr(0, firstSpace);

    auto c = commands.find(justCommand);

    if (c == commands.end()) {
        Log("Unknown command: '%s'\n", justCommand.c_str());
        return;
    }

    c->second(args);

    // Scroll down on execution
    scrollToBottom = true;
}

bool Init() {
    ClearLog();
    memset(inputBuf, 0, sizeof(inputBuf));
    historyPos = -1;

    Register("help", [](auto args) {
        for (const auto &c : commands) {
            Log(" - %s", c.first.c_str());
        }
    });

    return true;
}

int TextEditCallback(ImGuiInputTextCallbackData *data) {
    //Log("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
    switch (data->EventFlag) {
    case ImGuiInputTextFlags_CallbackCompletion: {
        // Example of TEXT COMPLETION

        // Locate beginning of current word
        const char *word_end   = data->Buf + data->CursorPos;
        const char *word_start = word_end;
        while (word_start > data->Buf) {
            const char c = word_start[-1];
            if (c == ' ' || c == '\t' || c == ',' || c == ';')
                break;
            word_start--;
        }

        // TODO clean up

        // Build a list of candidates
        ImVector<const char *> candidates;
        for (auto &x : commands) {
            if (Strnicmp(x.first.c_str(), word_start, (int)(word_end - word_start)) == 0) {
                candidates.push_back(x.first.c_str());
            }
        }

        if (candidates.Size == 0) {
            // No match
            Log("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
        } else if (candidates.Size == 1) {
            // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
            data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
            data->InsertChars(data->CursorPos, candidates[0]);
            data->InsertChars(data->CursorPos, " ");
        } else {
            // Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
            int match_len = (int)(word_end - word_start);
            for (;;) {
                int  c                      = 0;
                bool all_candidates_matches = true;
                for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                    if (i == 0)
                        c = toupper(candidates[i][match_len]);
                    else if (c == 0 || c != toupper(candidates[i][match_len]))
                        all_candidates_matches = false;
                if (!all_candidates_matches)
                    break;
                match_len++;
            }

            if (match_len > 0) {
                data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
            }

            // List matches
            Log("Possible matches:\n");
            for (int i = 0; i < candidates.Size; i++)
                Log("- %s\n", candidates[i]);
        }

        break;
    }
    case ImGuiInputTextFlags_CallbackHistory: {
        // Example of HISTORY
        const int prev_history_pos = historyPos;
        if (data->EventKey == ImGuiKey_UpArrow) {
            if (historyPos == -1)
                historyPos = history.size() - 1;
            else if (historyPos > 0)
                historyPos--;
        } else if (data->EventKey == ImGuiKey_DownArrow) {
            if (historyPos != -1)
                if (++historyPos >= history.size())
                    historyPos = -1;
        }

        // A better implementation would preserve the data on the current input line along with cursor position.
        if (prev_history_pos != historyPos) {
            const char *history_str = (historyPos >= 0) ? history[historyPos].c_str() : "";
            data->DeleteChars(0, data->BufTextLen);
            data->InsertChars(0, history_str);
        }
    }
    }
    return 0;
}

void Frame() {
    // TODO cleanup
    // this is all taken pretty much wholesale from the imgui demo app

    ImGui::SetNextWindowSize(ImVec2(520, 500), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Networkz console", &open)) {
        ImGui::End();
        return;
    }

    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Close Console"))
            open = false;
        ImGui::EndPopup();
    }

    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();             // 1 separator, 1 input text
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::Selectable("Clear")) ClearLog();
        ImGui::EndPopup();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
    // if (copy_to_clipboard)
    //     ImGui::LogToClipboard();
    for (auto &item : items) {
        // if (!Filter.PassFilter(item))
        //     continue;

        // Normally you would store more information in your item (e.g. make Items[] an array of structure, store color/type etc.)
        bool pop_color = false;
        if (item.find("[error]") != item.npos) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
            pop_color = true;
        } else if (strncmp(item.c_str(), "# ", 2) == 0) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f));
            pop_color = true;
        }
        ImGui::TextUnformatted(item.c_str());
        if (pop_color)
            ImGui::PopStyleColor();
    }
    // if (copy_to_clipboard)
    //     ImGui::LogFinish();

    if (scrollToBottom || (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
        ImGui::SetScrollHereY(1.0f);
    scrollToBottom = false;

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();

    bool reclaim_focus = false;
    if (ImGui::InputText("Input", inputBuf, IM_ARRAYSIZE(inputBuf), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, &TextEditCallback, nullptr)) {
        char *s = inputBuf;
        Strtrim(s);
        if (s[0])
            Execute(s);
        strcpy(s, "");
        reclaim_focus = true;
    }

    // Auto-focus on window apparition
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    ImGui::End();
}

} // namespace console