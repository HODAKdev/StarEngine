#pragma once

#include <string>
#include <d3d11.h>
#include "../../IMGUI/imgui.h"

class AboutWindow
{
public:
	void Init();
	void Render();
	void Shutdown();
	bool renderAbout = false;

private:
	ID3D11ShaderResourceView* _StarBanner = nullptr;
	std::string _VersionText = "";
};

AboutWindow& AboutClass();