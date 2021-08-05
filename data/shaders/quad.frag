#version 450 core

layout (location = 0) out vec4 fragColor;

in VS_OUT {
	vec2 uv;
} fs_in;

uniform sampler2D albedo;

void main()
{
	fragColor = texture(albedo, fs_in.uv);
	fragColor.a = 1.0;
}