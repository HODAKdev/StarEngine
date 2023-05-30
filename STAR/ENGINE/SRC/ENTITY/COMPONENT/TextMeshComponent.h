#pragma once

#include "../../IMGUI/imgui.h"
#include "../../IMGUI/imgui_stdlib.h"
#include "../../TTF2MESH/ttf2mesh.h"

enum AlignGlyph : unsigned char
{
	AlignLeft = 0x10,
	AlignCenter = 0x20,
	AlignRight = 0x30,
};

enum MeshQuality : unsigned int
{
	MeshHigh = 50,
	MeshNormal = 20,
	MeshLow = 10,
};

struct TextMeshComponent
{
public:
	void Render();

public:
	void          SetText(const char* text);
	const char*   GetText();
	void          SetQuality(unsigned char quality);
	unsigned char GetQuality();
	void          SetFont(const char* path);
	const char*   GetFont();
	void          SetAlign(unsigned char align);
	unsigned char GetAlign();
	void          SetMeshDepth(float value);
	float         GetMeshDepth();
	void          SetCharSpacing(float value);
	float         GetCharSpacing();
	void          SetLineSpacing(float value);
	float         GetLineSpacing();
	void          SetWordSpacing(float value);
	float         GetWordSpacing();

private:
	int item_current = 1;
	const char* items[3] = { "High", "Normal", "Low" };

private:
	std::string buffer;
	std::string fontPath;
	ImVec2 offset = ImVec2(NULL, NULL);
	float y_max = NULL;
	unsigned char meshQuality = MeshQuality::MeshNormal;
	unsigned char alignGlyph = AlignGlyph::AlignLeft;
	float meshDepth = 0.1f;
	float charSpacing = 0.0f;
	float lineSpacing = 1.0f;
	float wordSpacing = 0.5f;
	bool autoCharSpacing = true;

private:
	float GetMax(ttf_t* ttf);
	bool SetupTextMesh();
};