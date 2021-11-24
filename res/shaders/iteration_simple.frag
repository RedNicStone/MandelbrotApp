
#version 450

precision mediump int;
precision highp float;

layout(location = 0) in vec2 UVcoord;

layout(binding = 0) uniform UniformBufferObject {
	dvec2 pos;
	ivec2 res;
	double zoom;
	int iterations;
	int color;

	double time;
} ubo;

layout(location = 0) out vec4 outColor;

uint calcMandel(double startReal, double startImag) {

	double real = startReal;
	double imag = startImag;

	for (int n = 1; n < ubo.iterations + 1; n++) {
		if ((real * real) + (imag * imag) > 4) {
			return n;
		}
		double realTemp = real;

		real = (real * real) - (imag * imag) + startReal;
		imag = 2 * realTemp * imag + startImag;
	}
	return 0;
}

#define RANGE 255
#define PHASE 16
#define H_RANGE RANGE/2

void main() {
	double real = ubo.zoom * (double(gl_FragCoord.x) + 0.5) / ubo.res.x + ubo.pos.x;
	double imag = (ubo.zoom * (double(gl_FragCoord.y) + 0.5) + ubo.res.y * ubo.pos.y) / ubo.res.x;

	uint calc = calcMandel(real, imag);
	outColor.r = float(calc);
}

