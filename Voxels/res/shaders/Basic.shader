#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 uv;

out vec2 v_uv;

void main ()
{
	gl_Position = position;
	v_uv = uv;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_uv;

void main ()
{
	color = vec4(0, v_uv.x, v_uv.y, 1);
	//color = vec4(1, 0, 0, 1);
};