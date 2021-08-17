#version 450 core

layout (location = 0) out vec4 fragColor;

uniform vec3 color;

void main()
{
  fragColor.rgb = color;
  fragColor.a = 1.0;
}