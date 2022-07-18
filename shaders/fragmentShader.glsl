#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct directional_light{ 	
	vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct point_light {
    vec3 position;
	 
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct spot_light {
    vec3 position;
	vec3 direction;
	float cutoff;
	float outerCutoff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

uniform Material material;
uniform vec3 viewPos;
uniform directional_light dirLight;
#define NUMBER_OF_POINT_LIGHTS 4
uniform point_light pointLights[NUMBER_OF_POINT_LIGHTS];
uniform spot_light spotLight;

vec3 calcDirectionalLight(directional_light light, vec3 normal, vec3 viewDir){ 
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal , lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	vec3 ambient  = light.ambient  *        vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords));

	return (ambient + diffuse + specular);
}

vec3 calcPointLight(point_light light, vec3 normal, vec3 fragPos, vec3 viewDir){ 
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(normal , lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	float dist = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + 
    		    light.quadratic * (dist * dist));

	vec3 ambient  = light.ambient  *        vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords));

	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 calcSpotLight(spot_light light, vec3 normal, vec3 fragPos, vec3 viewDir){ 
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(normal , lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	float dist = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + 
    		    light.quadratic * (dist * dist));

	vec3 ambient  = light.ambient  *        vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords));

	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	float theta     = dot(lightDir, normalize(-light.direction));
	float epsilon   = light.cutoff - light.outerCutoff;
	float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
    
	diffuse  *= intensity;
	
	if( theta < light.outerCutoff )
		return ambient;

	return (ambient + diffuse + specular);
}


void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = vec3(0.0); //calcDirectionalLight(dirLight, norm,viewDir);

	for(int i=0; i < NUMBER_OF_POINT_LIGHTS;i++){ 
		result += calcPointLight(pointLights[i], norm, FragPos, viewDir);
	}
	
	result += calcSpotLight(spotLight, norm, FragPos, viewDir);
	
	FragColor = vec4(result, 1.0);
}

