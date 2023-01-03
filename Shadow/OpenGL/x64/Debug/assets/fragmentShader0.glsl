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
	//�۹��
	vec3 direction;
	float cutOff;
	float outerCutOff;
};

//���Դ
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
//ƽ�й�
struct DirLight {
    vec3 position;
    vec3 color;

};  

//in vec3 vertexColor;
//in vec2 uv;
in vec2 UV;
in vec3 Normal;
in vec3 PosWS;
in vec4 FragPosLightSpace;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D shadowMap;
uniform Material material;
uniform DirLight dirlight;
uniform PointLight pointlight[NR_POINT_LIGHTS];

//��Ӱ����
float ShadowCalculation(vec4 fragPosLightSpace)
{
    // ִ��͸�ӳ���
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // �任��[0,1]�ķ�Χ
    projCoords = projCoords * 0.5 + 0.5;
    // ȡ�����������(ʹ��[0,1]��Χ�µ�fragPosLight������)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // ȡ�õ�ǰƬ���ڹ�Դ�ӽ��µ����
    float currentDepth = projCoords.z;
    // ��鵱ǰƬ���Ƿ�����Ӱ��
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    return shadow;
}
vec3 CalcPointLight(PointLight light, float range,vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // ��������ɫ
    float diff = max(dot(normal, lightDir), 0.0);
    // �������ɫ
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // ˥��
    float distance    = length(light.position - fragPos);
    float attenuation = range / (light.constant + light.linear * distance + 
                 light.quadratic * (distance * distance));    
    // �ϲ����
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, UV));
    vec3 diffuse  = light.color  * diff * vec3(texture(material.diffuse, UV));
    vec3 specular = light.specular *spec *10f* vec3(texture(material.specular, UV));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation*2;
    return (ambient + diffuse + specular);
}
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
    float shadow = ShadowCalculation(FragPosLightSpace); 
    
    vec3 lightDir = normalize(light.position - fragPos);
    // ��������ɫ
    float diff = max(dot(normal, lightDir), 0.0);
    // �������ɫ
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // �ϲ����
    vec3 ambient = 0.3 * light.color* vec3(texture(material.diffuse, UV));
    vec3 diffuse  = light.color  * diff * vec3(texture(material.diffuse, UV));
    vec3 specular = 0.1 *light.color * spec * vec3(texture(material.specular, UV));
    return ((ambient+ (diffuse + specular)*(1-shadow))*light.color);
}

void main()
{
    vec3 output;
    vec3 viedir = normalize(viewPos-PosWS);
	//direction Light
	vec3 DircLight = CalcDirLight(dirlight,Normal,viedir,PosWS);
	//point light
	vec3 PointDir = normalize(pointlight[0].position-PosWS);
	float PointNoL = dot(PointDir,Normal);
	vec3 pointLight1 = CalcPointLight(pointlight[0],pointlight[0].range,Normal,PosWS,viedir);

	//vec3 mainTex1 = texture(t1, uv).xyz;
	//vec3 mainTex2 = texture(t2, uv).xyz;
	//vec3 fincolor = mix(mainTex1,mainTex2,0.5);
      
		vec3 finalcol =DircLight +pointLight1;
		FragColor = vec4(finalcol,1);

	
}