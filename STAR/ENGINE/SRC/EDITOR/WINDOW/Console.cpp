#include "Console.h"
#include "../Editor.h"
#include "../../IMGUI/imgui_stdlib.h"
#include <iomanip>
#include <chrono>

static ConsoleWindow console;

ConsoleWindow& ConsoleClass()
{
	return console;
}

///////////////////////////////////////////////////////////////

static Editor* editor = &EditorClass();

void ConsoleWindow::Init()
{
	AddInfoMessage("Welcome to Star Engine!");
}

void ConsoleWindow::Render()
{
	if (!renderConsole)
		return;

	ImVec2 safe = ImGui::GetStyle().WindowPadding;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(NULL, NULL));
	{
		ImGui::Begin("Console");
		{
			/**********************************************************************************************************************/
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + safe.x, ImGui::GetCursorPosY() + safe.y));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 4.0f));
			{
				if (ImGui::Button("Clear"))
				{
					ClearConsole();
				}
				/***********************************************************/
				ImGui::SameLine();
				if (clear_on_play)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
					if (ImGui::Button("Clear on Play"))
						clear_on_play = false;
					ImGui::PopStyleColor();
				}
				else
				{
					if (ImGui::Button("Clear on Play"))
						clear_on_play = true;
				}
				/***********************************************************/
				ImGui::SameLine();
				if (pause_on_error)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
					if (ImGui::Button("Pause on Error"))
						pause_on_error = false;
					ImGui::PopStyleColor();
				}
				else
				{
					if (ImGui::Button("Pause on Error"))
						pause_on_error = true;
				}
				/***********************************************************/
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
				if (view_error)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
					if (ImGui::Button(std::string(std::to_string(error_count) + " Errors").c_str()))
						view_error = false;
					ImGui::PopStyleColor();
				}
				else
				{
					if (ImGui::Button(std::string(std::to_string(error_count) + " Errors").c_str()))
						view_error = true;
				}
				ImGui::PopStyleColor();
				/***********************************************************/
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.4f, 1.0f));
				if (view_warning)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
					if (ImGui::Button(std::string(std::to_string(warning_count) + " Warnings").c_str()))
						view_warning = false;
					ImGui::PopStyleColor();
				}
				else
				{
					if (ImGui::Button(std::string(std::to_string(warning_count) + " Warnings").c_str()))
						view_warning = true;
				}
				ImGui::PopStyleColor();
				/***********************************************************/
				ImGui::SameLine();
				if (view_info)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
					if (ImGui::Button(std::string(std::to_string(info_count) + " Messages").c_str()))
						view_info = false;
					ImGui::PopStyleColor();
				}
				else
				{
					if (ImGui::Button(std::string(std::to_string(info_count) + " Messages").c_str()))
						view_info = true;
				}
				/***********************************************************/
			}
			ImGui::PopStyleVar();
			ImGui::Separator();
			/**********************************************************************************************************************/
			float reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
			if (selected_message != nullptr)
				ImGui::BeginChild("Messages", ImVec2(ImGui::GetWindowSize().x - 4.0f, -(reserve + 24.0f)));
			else
				ImGui::BeginChild("Messages", ImVec2(ImGui::GetContentRegionAvail().x - 4.0f, ImGui::GetContentRegionAvail().y));
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);
				for (size_t i = 0; i < messages.size(); i++)
				{
					if (!view_error)
						if (messages[i].type == ErrorMessage)
							continue;
					if (!view_warning)
						if (messages[i].type == WarningMessage)
							continue;
					if (!view_info)
						if (messages[i].type == InfoMessage)
							continue;

					ImGui::PushID((int)i);
					{
						if      (messages[i].type == InfoMessage)    { /* none */ }
						else if (messages[i].type == WarningMessage) { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.4f, 1.0f)); }
						else if (messages[i].type == ErrorMessage)   { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); }
						else if (messages[i].type == DebugMessage)   { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.4f, 1.0f, 1.0f)); }
						/******/
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + safe.x);
						std::string str = "(" + messages[i].time + ")" + " " + messages[i].message; /* time + space + message */
						bool result = ImGui::Selectable(str.c_str(), (idx == i));
						if (result)
						{
							selected_message = &messages[i]; /* for data */
							idx = i;
						}
						/******/
						if      (messages[i].type == InfoMessage)    { /* none */ }
						else if (messages[i].type == WarningMessage) { ImGui::PopStyleColor(); }
						else if (messages[i].type == ErrorMessage)   { ImGui::PopStyleColor(); }
						else if (messages[i].type == DebugMessage)   { ImGui::PopStyleColor(); }
					}
					ImGui::PopID();
				}
				ImGui::PopStyleVar();
				{
					if (ImGui::IsWindowHovered() && !ImGui::IsItemHovered())
						if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
						{
							selected_message = nullptr;
							idx = -1;
						}
				}
			}
			ImGui::EndChild();
			/**********************************************************************************************************************/
			if (selected_message != nullptr)
			{
				ImGui::InputTextMultiline(
					"##MessageBuffer",
					&selected_message->message,
					ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y),
					ImGuiInputTextFlags_ReadOnly);
			}
			/**********************************************************************************************************************/
		}
		ImGui::End();
	}
	ImGui::PopStyleVar();
}

void ConsoleWindow::AddDebugMessage(const char* text, ...)
{
#if defined(_DEBUG)
	if (std::string(text).empty())
		return;

	char buff[MAX_PATH];
	va_list args;
	va_start(args, text);
	vsnprintf(buff, MAX_PATH, text, args);
	va_end(args);
	messages.push_back(ConsoleMessage(GetNowTime(), buff, DebugMessage));
#endif
}

void ConsoleWindow::AddInfoMessage(const char* text, ...)
{
	if (std::string(text).empty())
		return;

	char buff[MAX_PATH];
	va_list args;
	va_start(args, text);
	vsnprintf(buff, MAX_PATH, text, args);
	va_end(args);
	messages.push_back(ConsoleMessage(GetNowTime(), buff, InfoMessage));
	info_count++;
}

void ConsoleWindow::AddWarningMessage(const char* text, ...)
{
	if (std::string(text).empty())
		return;

	char buff[MAX_PATH];
	va_list args;
	va_start(args, text);
	vsnprintf(buff, MAX_PATH, text, args);
	va_end(args);
	messages.push_back(ConsoleMessage(GetNowTime(), buff, WarningMessage));
	warning_count++;
}

void ConsoleWindow::AddErrorMessage(const char* text, ...)
{
	if (std::string(text).empty())
		return;

	char buff[MAX_PATH];
	va_list args;
	va_start(args, text);
	vsnprintf(buff, MAX_PATH, text, args);
	va_end(args);
	messages.push_back(ConsoleMessage(GetNowTime(), buff, ErrorMessage));
	error_count++;
}

std::string ConsoleWindow::GetNowTime()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%X");
	return ss.str();
}

void ConsoleWindow::ClearConsole()
{
	messages.clear();
	selected_message = nullptr;
	info_count = 0;
	warning_count = 0;
	error_count = 0;
	idx = -1;
}