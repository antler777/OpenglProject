#version 330 core
layout (location = 0) in vec3 aPos;           //顶点位置   OS
layout (location = 1) in vec2 aUV;	


out vec2 UV;



void main()
{
	//需要左乘
   gl_Position = vec4(aPos.xy, 0.0f, 1.0f);

   UV = aUV;
  
}