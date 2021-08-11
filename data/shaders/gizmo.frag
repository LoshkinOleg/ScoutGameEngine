#version 450 core

layout (location = 0) out vec4 fragColor;

uniform vec3 baseColor;

void main()
{
  fragColor.rgb = baseColor;
  fragColor.a = 1.0;
}