/*VERTEX-SHADER-BEGIN*/
#version 330 core

// Арибуты вершины
layout(location = 0) in vec3 position;   // Положение
layout(location = 2) in vec2 uv;         // Текстурные координаты

// Выходные значения шейдера
out VS_OUT
{
	vec2 uv;
} vs_out;

// Основная функция вершинного шейдера
// По сути передача положений вершин и UV-координат в следующие этапы, как есть
void main()
{
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
	vs_out.uv = uv;
}

/*VERTEX-SHADER-END*/

////////////////////////////////////////////////////////////////////////////////////

/*FRAGMENT-SHADER-BEGIN*/
#version 330 core

// Выход шейдера
layout (location = 0) out vec4 color;

// Значения принятые на вход с предыдущих этапов
in VS_OUT
{
	vec2 uv;
} fs_in;

// Текстуры
uniform sampler2D screenTexture;

// Сдвиг текстурных координат для выборки соседних текселей
const float offset = 0.003;

// Основная функция фрагментного шейдера
// Вычисление итогового цвета фрагмента с учетом пост-обработки
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

	// Результирующий цвет
	vec3 result;

	// Подсчет результирующего цвета (среднее значение из выборки)
	for(int i = 0; i < 9; i++){
		result += texture(screenTexture, fs_in.uv + offsets[i]).rgb * kernel[i];
	}

	// Выход шейдера
	color = vec4(result, 1.0);
}

/*FRAGMENT-SHADER-END*/