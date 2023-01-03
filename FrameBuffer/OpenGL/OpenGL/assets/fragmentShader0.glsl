#version 330 core
out vec4 FragColor;


struct Material {
	sampler2D diffuse;
    vec3 ambient;
    sampler2D specular;   
    float shininess;
}; 


struct SpotLight {
    vec3 position;
    vec3 color;
	//聚光灯
	vec3 direction;
	float cutOff;
	float outerCutOff;
};

//点光源
struct PointLight {
    vec3 position;
    vec3 color;
	float range;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
};
#define NR_POINT_LIGHTS 4
//平行光
struct DirLight {
    vec3 direction;
    vec3 color;

};  

//in vec3 vertexColor;
//in vec2 uv;
in vec2 UV;
in vec3 Normal;
in vec3 PosWS;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform Material material;
uniform DirLight dirlight;
uniform PointLight pointlight[NR_POINT_LIGHTS];

vec3 CalcPointLight(PointLight light, float range,vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 衰减
    float distance    = length(light.position - fragPos);
    float attenuation = range / (light.constant + light.linear * distance + 
                 light.quadratic * (distance * distance));    
    // 合并结果
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, UV));
    vec3 diffuse  = light.color  * diff * vec3(texture(material.diffuse, UV));
    vec3 specular = light.specular *spec *10f* vec3(texture(material.specular, UV));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation*2;
    return (ambient + diffuse + specular);
}
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 合并结果
    //vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, UV));
    vec3 diffuse  = light.color  * diff * vec3(texture(material.diffuse, UV));
    vec3 specular = light.color * spec * vec3(texture(material.specular, UV));
    return ( diffuse + specular);
}

void main()
{
    vec3 output;
    vec3 viedir = normalize(viewPos-PosWS);
	//direction Light
	vec3 DircLight = CalcDirLight(dirlight,Normal,viedir);
	//point light
	vec3 PointDir = normalize(pointlight[0].position-PosWS);
	float PointNoL = dot(PointDir,Normal);
	vec3 pointLight1 = CalcPointLight(pointlight[0],pointlight[0].range,Normal,PosWS,viedir);

	//vec3 mainTex1 = texture(t1, uv).xyz;
	//vec3 mainTex2 = texture(t2, uv).xyz;
	//vec3 fincolor = mix(mainTex1,mainTex2,0.5);
    vec3 specular =  vec3(texture(material.specular, UV));
		vec3 finalcol =DircLight +pointLight1;
		FragColor = vec4(finalcol,1);

	
}