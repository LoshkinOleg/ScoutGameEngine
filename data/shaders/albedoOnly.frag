#version 450 core

layout (location = 0) out vec4 fragColor;

in VS_OUT {
	vec2 uv;
} fs_in;

uniform sampler2D albedoMap;

void main()
{
	fragColor = texture(albedoMap, fs_in.uv);
}