#version 450 core

layout (location = 0) in vec2 vPos;

out VS_OUT {
	vec2 uv;
} vs_out;

void main()
{
	vs_out.uv = vPos * 0.5 + 0.5;
	gl_Position = vec4(vPos, 0.0, 1.0);
}