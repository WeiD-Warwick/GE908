//
// Created by W.D. on 20/10/25
//

#pragma once

struct GEColor {
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a = 1;

	GEColor(unsigned char r, unsigned char g, unsigned char b)
		: r(r), g(g), b(b) {
	}

	GEColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
		: r(r), g(g), b(b), a(a) {
	}
};