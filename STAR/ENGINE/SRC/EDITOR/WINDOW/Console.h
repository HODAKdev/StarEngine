#pragma once

#include <string>
#include <vector>
#include <sstream>
#include "../../IMGUI/imgui.h"

enum MessageType : unsigned char
{
	DebugMessage   = 0x10,
	InfoMessage    = 0x50,
	WarningMessage = 0x60,
	ErrorMessage   = 0x70,
};

struct ConsoleMessage
{
	std::string time;
	std::string message;
	unsigned char type = 0x00;
	ConsoleMessage(std::string time, std::string message, unsigned char type)
	{
		this->time = time;
		this->message = message;
		this->type = type;
	}
};

class ConsoleWindow
{
public:
	void Init();
	void Render();
	bool renderConsole = true;
	std::string GetNowTime();
	void ClearConsole();

public:
	void AddDebugMessage(const char* text, ...);
	void AddInfoMessage(const char* text, ...);
	void AddWarningMessage(const char* text, ...);
	void AddErrorMessage(const char* text, ...);

private:
	std::vector<ConsoleMessage> messages;
	ConsoleMessage* selected_message = nullptr;
	size_t idx = -1;

private:
	bool clear_on_play = false;
	bool pause_on_error = false;
	bool view_error = true;
	bool view_warning = true;
	bool view_info = true;

private:
	unsigned int error_count = 0;
	unsigned int warning_count = 0;
	unsigned int info_count = 0;
};

ConsoleWindow& ConsoleClass();