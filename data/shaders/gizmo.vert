#version 450 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in mat4 iModelMatrix;

uniform mat4 cameraMatrix;

void main()
{
	gl_Position = cameraMatrix * iModelMatrix * vec4(vPos, 1.0);
}