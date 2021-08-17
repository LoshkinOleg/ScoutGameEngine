#version 450 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in mat4 iModelMatrix;

out VS_OUT {
	vec3 w_normal;
	vec3 w_fragPos;
} vs_out;

uniform mat4 cameraMatrix;

void main()
{
	const mat3 normalMatrix = transpose(inverse(mat3(iModelMatrix)));

	vs_out.w_normal = normalize(normalMatrix * normalize(vNormal));
	vs_out.w_fragPos = (iModelMatrix * vec4(vPos, 1.0)).xyz;
	gl_Position = cameraMatrix * vec4(vs_out.w_fragPos, 1.0);
}