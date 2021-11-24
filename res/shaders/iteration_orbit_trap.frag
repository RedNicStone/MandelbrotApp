
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

#define TRAP z.x*z.x + z.y*z.y
//#define TRAP abs(z.x + z.y)
//#define TRAP abs(z.x)+abs(z.y)
//#define TRAP z.x*z.x*(sin(time)+1.0)+z.y*z.y*(cos(time)+1.0)
//#define TRAP sin(z.x)+z.y*z.y+1.0
//#define TRAP abs(sin(z.x)-z.y)
//#define TRAP mandelbrot(z, maxIterations)
//#define TRAP abs(z.x+z.y)+abs(z.x)+abs(z.y)
//#define TRAP abs((z.x*z.x+z.y*z.y)*(z.x*z.x+z.y*z.y)-2.0*(z.x*z.x-z.y*z.y))
//#define TRAP abs(z.x*z.x*z.x+z.y*z.y*z.y-3.0*z.x*z.y)
//#define TRAP abs((z.x-1.0))
//#define TRAP abs(z.x*z.x+z.y*z.y-0.75)
//#define TRAP abs(z.x*z.x+z.y*z.y-z.y)
//#define TRAP abs(sin(z.y)-z.x)+abs(sin(z.x)-z.y)
//#define TRAP abs(z.x*z.x+z.y*z.y+2.0*z.x+z.x*z.x*z.y)
//#define TRAP abs(z.x*z.x+z.y*z.y+z.x+z.x*z.x)

double mandelbrot(dvec2 p, uint maxIterations)
{
	dvec2 z = dvec2(0.0, 0.0);
	for (int i = 0; i < maxIterations; ++i)
		z = dvec2(z.x*z.x-z.y*z.y, z.x*z.y*2.0) + p;
	return z.x*z.x+z.y*z.y;
}

uint calcMandel(dvec2 c, dvec2 orbit, uint maxIterations) {
	dvec2 z = dvec2(0, 0);
	double dist = 1e20;
	double p = 0.0;

	for (int n = 1; n < ubo.iterations + 1; n++) {
		z = dvec2(z.x*z.x-z.y*z.y, z.x*z.y*2.0) + c;
		p = (TRAP);
		if (p < dist)
			dist = p;
	}
	return int(sqrt(p)*100);
}

#define RANGE 255
#define PHASE 16
#define H_RANGE RANGE/2

void main() {
	double real = ubo.zoom * (double(gl_FragCoord.x) + 0.5) / ubo.res.x + ubo.pos.x;
	double imag = (ubo.zoom * (double(gl_FragCoord.y) + 0.5) + ubo.res.y * ubo.pos.y) / ubo.res.x;

	uint calc = calcMandel(dvec2(real, imag), dvec2(0, 0), ubo.iterations);
	outColor.r = calc;
}

