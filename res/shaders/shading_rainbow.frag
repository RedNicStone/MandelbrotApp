
#version 450

precision mediump int;

layout(location = 0) in vec2 UVcoord;

layout(binding = 0) uniform UniformBufferObject {
	dvec2 pos;
	ivec2 res;
	double zoom;
	int iterations;
	int color;

	double time;
} ubo;

layout (input_attachment_index = 0, set = 0, binding = 1) uniform subpassInput iterationValue;

layout(location = 0) out vec4 outColor;

//  Function from Iñigo Quiles
//  https://www.shadertoy.com/view/MsS3Wc
vec3 hsb2rgb( in vec3 c ){
	vec3 rgb = clamp(abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),
	6.0)-3.0)-1.0,
	0.0,
	1.0 );
	rgb = rgb*rgb*(3.0-2.0*rgb);
	return c.z * mix(vec3(1.0), rgb, c.y);
}

#define RANGE 255
#define PHASE 16
#define H_RANGE RANGE/2

void main() {
	int hit = int(subpassLoad(iterationValue).r != 0);
	outColor = vec4(hsb2rgb(vec3(float(subpassLoad(iterationValue).r) / RANGE + ubo.time / PHASE, 1.0, 1.0)) * hit, 1.0);
}

