//
// Created by W.D. on 20/10/25
//

#pragma once
struct GEColor {
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	GEColor(unsigned char r, unsigned char g, unsigned char b);

	// color presets
	static const GEColor BackgroundDark;   // Main BG
	static const GEColor BackgroundLight;  // UI BG
	static const GEColor White;            // Main UI
	static const GEColor Red;              // HP, Negative Status
	static const GEColor Green;            // HP, Positive Status
	static const GEColor Blue;             // MP, Magic
	static const GEColor Yellow;           // Gold, Interactive Item
	static const GEColor Orange;           // Notice, Rare Item
};
