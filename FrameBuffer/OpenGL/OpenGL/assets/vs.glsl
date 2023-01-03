#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;	


out vec2 uv;

uniform mat4 model1;
uniform mat4 view;
uniform mat4 projection;
void main()
{
	//ÐèÒª×ó³Ë
   gl_Position = projection * view * model1 *vec4(aPos.x, aPos.y, aPos.z, 1.0f);

   uv = aUV;
}