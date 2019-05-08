#pragma once

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

class Color {
public:
	Color() : _red(1.0f), _green(1.0f), _blue(1.0f), _alpha(1.0f) {}
	Color(float r, float g, float b) : _red(r), _green(g), _blue(b), _alpha(1.0f) { }
	Color(float r, float g, float b, float a) : _red(r), _green(g), _blue(b), _alpha(a) { }
	float getRed() { return _red; }
	void setRed(float r) { _red = r; }
	float getGreen() { return _green; }
	void setGreen(float g) { _green = g; }
	float getBlue() { return _blue; }
	void setBlue(float b) { _blue = b; }
	float getAlpha() { return _alpha; }
	void setAlpha(float a) { _alpha = a; }
	glm::vec3 vec3() { return glm::vec3(_red, _green, _blue); }
	glm::vec4 vec4() { return glm::vec4(_red, _green, _blue, _alpha); }
private:
	float _red;
	float _green;
	float _blue;
	float _alpha;
};