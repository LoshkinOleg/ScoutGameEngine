#version 450 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vTangent;
layout (location = 3) in vec2 vUv;
layout (location = 4) in mat4 iModelMatrix;

out VS_OUT {
	vec3 t_fragPos;
	vec3 t_viewPos;
	vec3 t_lightDir;
	vec2 uv;
} vs_out;

uniform mat4 cameraMatrix;
uniform vec3 viewPos;

const vec3 lightDir = normalize(vec3(-1.0, -1.0, -1.0));

void main()
{
	const mat3 normalMatrix = transpose(inverse(mat3(iModelMatrix)));
	const mat3 tangentMatrix = mat3(iModelMatrix);
	const vec3 normal = normalize(normalMatrix * vNormal);
	const vec3 tangent = normalize(tangentMatrix * vTangent);
	const vec3 bitangent = cross(normal, tangent);
	const mat3 tbnMatrix = transpose(mat3(tangent, bitangent, normal));

	vec4 w_fragPos = iModelMatrix * vec4(vPos, 1.0);
	vs_out.t_fragPos = tbnMatrix * w_fragPos.xyz;
	vs_out.t_viewPos = tbnMatrix * viewPos;
	vs_out.t_lightDir = tbnMatrix * lightDir;
	vs_out.uv = vUv;
	gl_Position = cameraMatrix * w_fragPos;
}