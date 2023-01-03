#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;	// ��ourColor����Ϊ���ǴӶ�����������õ���������ɫ
layout (location = 2) in vec2 aUV;


out vec3 vertexColor;
out vec3 ourColor; // ��Ƭ����ɫ�����һ����ɫ
out vec2 uv;
void main()
{
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
   vertexColor = aColor.xyz;
   ourColor = aColor;
   uv = aUV;
}