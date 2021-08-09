#version 450 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vTangent;
layout (location = 3) in vec3 vBitangent;
layout (location = 4) in vec2 vUv;
layout (location = 5) in mat4 iModelMatrix;

out VS_OUT {
	vec3 t_fragPos;
	vec3 t_viewPos;
	vec2 uv;
} vs_out;

uniform mat4 cameraMatrix;
uniform vec3 viewPos;

void main()
{
	const mat3 normalMatrix = transpose(inverse(mat3(iModelMatrix)));
	const vec3 normal = normalize(normalMatrix * vNormal);
	const vec3 tangent = normalize(normalMatrix * vTangent);
	const vec3 bitangent = normalize(normalMatrix * vBitangent);
	const mat3 tangentMatrix = mat3(tangent, bitangent, normal);

	vec4 w_fragPos = iModelMatrix * vec4(vPos, 1.0);
	vs_out.t_fragPos = tangentMatrix * w_fragPos.xyz;
	vs_out.t_viewPos = tangentMatrix * viewPos;
	vs_out.uv = vUv;
	gl_Position = cameraMatrix * w_fragPos;
}