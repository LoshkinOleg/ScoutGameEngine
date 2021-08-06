#version 450 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;

out VS_OUT {
	vec3 normal;
	vec3 fragPos;
} vs_out;

uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 cameraMatrix;

void main()
{
	vs_out.normal = normalize(normalMatrix * normalize(vNormal));
	vs_out.fragPos = (modelMatrix * vec4(vPos, 1.0)).xyz;
	gl_Position = cameraMatrix * vec4(vs_out.fragPos, 1.0);
}