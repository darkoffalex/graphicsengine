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

// Максимальные кол-во источников разных типов
#define MAX_POINT_LIGHTS 32
#define MAX_DIRECT_LIGHTS 32
#define MAX_SPOT_LIGHTS 32

// Выход шейдера
layout (location = 0) out vec4 color;

// Структура описывающая параметры материала
struct MaterialSettings
{
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	float shininess;
};

// Структура описывающая параметры фрагмента
struct FragmentSettings
{
	vec3 position;
	vec3 color;
	vec3 normal;
	float specularity;
};

// Структура описывающая параметры точечного источника освещения
struct PointLight
{
	vec3 position;
	vec3 color;
	float linear;
	float quadratic;
};

// Структура описывающая параметры направленного источника освещения
struct DirectLight
{
	vec3 direction;
	vec3 color;
};

// Структура описывающая параметры источника типа "прожектор-фонарик"
struct SpotLight
{
	vec3 position;
	vec3 color;
	vec3 direction;
	float linear;
	float quadratic;
	float cutOffCos;
	float cutOffOuterCos;
	mat4 modelMatrix;
};

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

// UBO-блок с параметрами источников света (точечных)
layout(std140) uniform ptLightsUniform
{
	PointLight[MAX_POINT_LIGHTS] pointLights;
};

// UBO-блок с параметрами источников света (направленных)
layout(std140) uniform dirLightsUniform
{
	DirectLight[MAX_DIRECT_LIGHTS] directionalLights;
};

// UBO-блок с параметрами источников света (прожекторов)
layout(std140) uniform spotLightsUniform
{
	SpotLight[MAX_SPOT_LIGHTS] spotLights;
};

// Текстуры
uniform sampler2D albedoSpecularTexture;
uniform sampler2D positionTexture;
uniform sampler2D normalTexture;

// Вычислить освещенность фрагмента точечным источником
vec3 calculatePointLightComponent(PointLight light, FragmentSettings fragment, MaterialSettings material, vec3 viewPosition)
{
	// Вектор из фрагмента в камеру (обратное направление взгляда)
	vec3 fragmentToView = normalize(viewPosition - fragment.position);

	// Коэффициент затухания (использует расстояние до источника, а так же спец-коэффициенты источника)
	float distance = length(light.position - fragment.position);
	float attenuation = 1.0f / (1.0f + light.linear * distance + light.quadratic * (distance * distance));

	// Фоновый (ambient) цвет просто равномерно заполняет каждый фрагмент
	vec3 ambient = light.color * material.ambientColor * fragment.color;

	// Рассеянный (diffuse) зависит от угла между нормалью фрагмента и вектором падения света на фрагмент
	vec3 lightDir = normalize(light.position - fragment.position);
	float diffuseBrightness = max(dot(fragment.normal, lightDir), 0.0);
	vec3 diffuse = light.color * (diffuseBrightness * material.diffuseColor) * fragment.color;

	// Бликовый (specular) зависит от угла между вектором взгляда и отроженным относительно нормали вектором падения света на фрагмент
	vec3 reflectedLightDir = reflect(-lightDir, fragment.normal);
	float specularBrightness = pow(max(dot(fragmentToView, reflectedLightDir), 0.0), material.shininess);
	vec3 specular = light.color * (specularBrightness * material.specularColor) * fragment.specularity;

	// Вернуть сумму всех компонентов
	return ((ambient * attenuation) + (diffuse * attenuation) + (specular * attenuation));
}

// Основная функция фрагментного шейдера
// Вычисление итогового цвета фрагмента с учетом всех параметров вложений G-буфера
void main()
{
	// Пока-что используется материал по умолчанию, в дальнейшем организуем передачу материалов (что-нибудь намутим)
	MaterialSettings material;
	material.ambientColor = vec3(0.05f, 0.05f, 0.05f);
	material.diffuseColor = vec3(0.8f, 0.8f, 0.8f);
	material.specularColor = vec3(0.4f, 0.4f, 0.4f);
	material.shininess = 16.0f;

	// Записать параметры текущего фрагмента в текстуру
	FragmentSettings fragment;
	fragment.position = texture(positionTexture,fs_in.uv).rgb;
	fragment.color = texture(albedoSpecularTexture,fs_in.uv).rgb;
	fragment.normal = texture(normalTexture,fs_in.uv).rgb;
	fragment.specularity = texture(albedoSpecularTexture,fs_in.uv).a;

	// Результирующий цвет
	vec3 resultColor;

	// Пройтись по массиву точечных источников света
	for(int i = 0; i < MAX_POINT_LIGHTS; i++){
		resultColor += calculatePointLightComponent(pointLights[i], fragment, material, cameraPosition);
	}

	// Итоговый цвет
	color = vec4(resultColor,1.0f);
}

/*FRAGMENT-SHADER-END*/