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

// Сдвиг текстурных координат для выборки соседних текселей
const float offset = 0.003;  

void main()
{
	// Массив сдвигов UV координат для получения соседних текселей текстуры
	vec2 offsets[9] = vec2[](
		vec2(-offset, offset), vec2( 0.0f, offset), vec2( offset, offset),
		vec2(-offset, 0.0f), vec2( 0.0f, 0.0f), vec2( offset, 0.0f),
		vec2(-offset, -offset), vec2( 0.0f, -offset), vec2( offset, -offset)
	);

	// Коэфициенты умножения (сверточное ядро)
	// Без изменений (исходная картинка)
    float kernel[9] = float[](
        0, 0, 0,
        0, 1, 0,
        0, 0, 0
    );

	// Размытие
	//float kernel[9] = float[](
	//	1.0 / 16, 2.0 / 16, 1.0 / 16,
	//	2.0 / 16, 4.0 / 16, 2.0 / 16,
	//	1.0 / 16, 2.0 / 16, 1.0 / 16  
    //);

	// Подсчет итогового цвета
	vec3 result;
	for(int i = 0; i < 9; i++){
		// Умножаем значение из ядра на значение соответствующего текселя и прибавляем к результату
		result += vec3(texture(screenTexture, vsoUV.xy + offsets[i])) * kernel[i];
	}

	color = vec4(result, 1.0);
}
/*FRAGMENT-SHADER-END*/