#version 450 core

layout (location = 0) out vec4 fragColor;

in VS_OUT {
	vec3 t_fragPos;
	vec3 t_viewPos;
	vec3 t_lightDir;
	vec2 uv;
} fs_in;

uniform sampler2D alphaMap;
uniform sampler2D albedoMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;
uniform float shininess;

const float AMBIENT_FACTOR = 0.01;

void main()
{
	const vec3 fragPos = fs_in.t_fragPos;
	const vec3 viewPos = fs_in.t_viewPos;
	const vec3 lightDir = fs_in.t_lightDir;
	const float alpha = texture(alphaMap, fs_in.uv).r;
	if (alpha < 0.01) discard;
	const vec3 albedo = texture(albedoMap, fs_in.uv).rgb;
	const vec3 specular = texture(specularMap, fs_in.uv).rgb;
	const vec3 normal = normalize(texture(normalMap, fs_in.uv).xyz * 2.0 - 1.0);

	const vec3 viewDir = normalize(fragPos - viewPos);
	const vec3 halfway = normalize(-lightDir - viewDir);
	const vec3 relfectDir = reflect(lightDir, normal);

	const float diffuseIntensity = max(dot(-lightDir, normal), 0.0);
	const float specularIntensity = pow(max(dot(normal, halfway), 0.0), shininess);

	const vec3 ambientColor = AMBIENT_FACTOR * albedo;
	const vec3 diffuseColor = diffuseIntensity * albedo;
	const vec3 specularColor = specularIntensity * specular;

	fragColor.rgb = ambientColor + diffuseColor + specularColor;
	fragColor.a = alpha;
}