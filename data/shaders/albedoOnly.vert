#version 450 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vUv;
layout (location = 2) in mat4 iModelMatrix;

out VS_OUT {
	vec2 uv;
} vs_out;

uniform mat4 cameraMatrix;

void main()
{
	vs_out.uv = vUv;
	gl_Position = cameraMatrix * iModelMatrix * vec4(vPos, 1.0);
}