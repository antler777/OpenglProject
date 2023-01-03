#version 330 core
layout (location = 0) in vec3 aPos;           //顶点位置   OS



uniform mat4 lightSpaceMatrix;
uniform mat4 model;




void main()
{
	//需要左乘
   gl_Position =lightSpaceMatrix*model* vec4(aPos, 1.0f);
  
}