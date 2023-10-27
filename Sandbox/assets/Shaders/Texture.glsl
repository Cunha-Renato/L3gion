#type vertex
#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;

uniform mat4 u_ViewProjection;

out vec3 v_Position;
out vec4 v_Color;
out vec2 v_TexCoord;
out int v_TexIndex;
out float v_TilingFactor;

void main()
{
	v_Position = a_Position;
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_TexIndex = int(a_TexIndex);
	v_TilingFactor = a_TilingFactor;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core
layout(location = 0) out vec4 color;

in vec3 v_Position;
in vec4 v_Color;
in vec2 v_TexCoord;
flat in int v_TexIndex;
in float v_TilingFactor;

uniform sampler2D u_Textures[32];

void main()
{

	for (int i = 1; i < v_TexIndex+1; i++)
	{
		if (i == v_TexIndex)
			color = texture(u_Textures[i], v_TexCoord * v_TilingFactor) * v_Color;
	}

	if (v_TexIndex == 0)
		color = v_Color;
}