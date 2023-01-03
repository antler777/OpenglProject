#version 330 core
layout (location = 0) in vec3 aPos;           //顶点位置   OS
layout (location = 1) in vec3 aNormal;	
layout (location = 2) in vec2 aUV;	

out vec3 Normal;
out vec3 PosWS;
out vec2 UV;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
	//需要左乘
   gl_Position = projection * view * model *vec4(aPos.x, aPos.y, aPos.z, 1.0f);
   PosWS = (model*vec4(aPos,1.0)).xyz;
   UV = aUV;
   Normal = mat3(transpose(inverse(model))) * aNormal;;
}