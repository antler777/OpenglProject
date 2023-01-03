#version 330 core
out vec4 FragColor;
in vec3 vertexColor;
in vec2 uv;

uniform float newColor; // 在OpenGL程序代码中设定这个变量
uniform sampler2D t1;
uniform sampler2D t2;

void main()
{
	vec3 mainTex1 = texture(t1, uv).xyz;
	vec3 mainTex2 = texture(t2, uv).xyz;
	vec3 fincolor = mix(mainTex1,mainTex2,0);
	FragColor = vec4(fincolor,1.0);
}