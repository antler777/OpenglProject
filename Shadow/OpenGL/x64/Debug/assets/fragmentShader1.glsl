#version 330 core
out vec4 FragColor;
in vec3 ourColor;
uniform vec3 lightcolor;

void main()
{
	FragColor = vec4(lightcolor, 1.0);
}