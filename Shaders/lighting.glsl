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

// UBO-блок с положениями
layout (std140) uniform positions
{
	vec3 cameraPosition;
};

// Текстуры
uniform sampler2D albedoSpecularTexture;
uniform sampler2D positionTexture;
uniform sampler2D normalTexture;

// Основная функция фрагментного шейдера
// Вычисление итогового цвета фрагмента с учетом всех параметров вложений G-буфера
void main()
{
	color = vec4(texture(albedoSpecularTexture,fs_in.uv).rgb,1.0);
}

/*FRAGMENT-SHADER-END*/