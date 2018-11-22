/*VERTEX-SHADER-BEGIN*/
#version 330 core

// Арибуты вершины
layout(location = 0) in vec3 position;   // Положение
layout(location = 2) in vec2 uv;         // Текстурные координаты

// Выход шейдера
out vec2 vsoUV;

void main()
{
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
	vsoUV = uv;
}
/*VERTEX-SHADER-END*/

////////////////////////////////////////////////////////////////////////////////////

/*FRAGMENT-SHADER-BEGIN*/
#version 330 core

// Вход шейдера
in vec2 vsoUV;

// Выход шейдера
out vec4 color;

// Текстура экрана
uniform sampler2D screenTexture;

void main()
{
	// Черно-белый фильтр
	vec4 FragColor = texture(screenTexture, vsoUV);
	float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
	color = vec4(average, average, average, 1.0);
}
/*FRAGMENT-SHADER-END*/