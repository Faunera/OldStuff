#version 330 core

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

uniform vec3 viewPos;
uniform bool blinn;

in vec3 normal;
in vec3 FragPos;
in vec2 texCoord;

out vec4 FragColor;

void main()
{
	float ambientStr = 0.1f;
	vec3 ambient = ambientStr * lightColor;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - FragPos);  
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	float specularStr = 0.1f;
	
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 16);
	if (blinn)
	{
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(viewDir, halfwayDir), 0.0f), 16);
	}
	
	vec3 specular = specularStr * spec * lightColor;

	vec3 result = (ambient + diffuse + specular)* objectColor;
	
	vec4 texel0 = texture2D(texture0, texCoord);
    vec4 texel1 = texture2D(texture1, texCoord);
	vec4 texResult = mix( texel0, texel1, 0.2); 
	
    FragColor = texResult + vec4(result, 1.0f);
	
  //  FragColor = texture2D(texture0, texCoord) + vec4(result, 1.0f);
}