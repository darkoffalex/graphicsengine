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

// Типы источника освещения
#define LIGHT_POINT 1
#define LIGHT_DIRECTIONAL 2
#define LIGHT_SPOT 3

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

// Структура описывающая параметры источника света
struct Light
{
	uint type;
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

// Uniform-переменная для положения камеры
uniform vec3 cameraPosition;

// Uniform-переменные для источника света
uniform Light light;

// Текстуры из G-буфера
uniform sampler2D albedoSpecularTexture;
uniform sampler2D positionTexture;
uniform sampler2D normalTexture;

// Вычислить освещенность фрагмента точечным источником
vec3 calculatePointLightComponent(Light light, FragmentSettings fragment, MaterialSettings material, vec3 viewPosition)
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

	// Бликовый (specular) зависит от угла между вектором взгляда и отраженным относительно нормали вектором падения света на фрагмент
	vec3 reflectedLightDir = reflect(-lightDir, fragment.normal);
	float specularBrightness = pow(max(dot(fragmentToView, reflectedLightDir), 0.0), material.shininess);
	vec3 specular = light.color * (specularBrightness * material.specularColor) * fragment.specularity;

	// Вернуть сумму всех компонентов
	return ((ambient * attenuation) + (diffuse * attenuation) + (specular * attenuation));
}

// Вычислить освещенность фрагмента направленным источником
vec3 calculateDirectLightComponent(Light light, FragmentSettings fragment, MaterialSettings material, vec3 viewPosition)
{
	// Вектор из фрагмента в камеру (обратное направление взгляда)
	vec3 fragmentToView = normalize(viewPosition - fragment.position);

	// Фоновый (ambient) цвет просто равномерно заполняет каждый фрагмент
	vec3 ambient = light.color * material.ambientColor * fragment.color;

	// Рассеянный (diffuse) зависит от угла между нормалью фрагмента и вектором падения света на фрагмент
	vec3 lightDir = normalize(light.direction);
	float diffuseBrightness = max(dot(fragment.normal, -lightDir), 0.0);
	vec3 diffuse = light.color * (diffuseBrightness * material.diffuseColor) * fragment.color;

	// Бликовый (specular) зависит от угла между вектором взгляда и отраженным относительно нормали вектором падения света на фрагмент
	vec3 reflectedLightDir = reflect(lightDir, fragment.normal);
	float specularBrightness = pow(max(dot(fragmentToView, reflectedLightDir), 0.0), material.shininess);
	vec3 specular = light.color * (specularBrightness * material.specularColor) * fragment.specularity;

	// Вернуть сумму всех компонентов
	return ambient + diffuse + specular;
}

// Вычислить освещенность фрагмента фонариком-прожектором
vec3 calculateSpotLightComponent(Light light, FragmentSettings fragment, MaterialSettings material, vec3 viewPosition)
{
	// Вектор из фрагмента в камеру (обратное направление взгляда)
	vec3 fragmentToView = normalize(viewPosition - fragment.position);

	// Вектор падения света (от фрагмента к источнику)
	vec3 lightDir = normalize(light.position.xyz - fragment.position);

	// Косинус угла между вектором падения света и вектором направления источника
	float thetaCos = dot(lightDir, normalize(-light.direction.xyz));

	// Разница косинусов между углом внутреннего конуса и углом внешнего
	float epsilon = light.cutOffCos - light.cutOffOuterCos;

	// Свет наиболее интенсивен в центре (где thetaCos - 1, угол между лучем и направлением фонарика - 0)
	// К краям интенсивность спадает. Благодаря коэффициенту epsilon есть так же яркое пятно внутри (внутр. конус)
	float intensity = clamp((thetaCos - light.cutOffOuterCos) / epsilon, 0.0, 1.0);

	//TODO: Интенсивность из текстурной карты фонарика

	// Рассеянный (diffuse)
	float diffuseBrightness = max(dot(fragment.normal,lightDir), 0.0);
	vec3 diffuse = light.color.rgb * (diffuseBrightness * material.diffuseColor) * fragment.color;

	// Бликовый (specular)
	vec3 reflectedLightDir = reflect(-lightDir, fragment.normal);  
	float specularBrightness = pow(max(dot(fragmentToView, reflectedLightDir), 0.0), material.shininess);
	vec3 specular = light.color.rgb * (specularBrightness * material.specularColor) * fragment.specularity;

	// Коэффициент затухания (использует расстояние до источника, а так же спец-коэффициенты источника)
	float distance = length(light.position.xyz - fragment.position);
	float attenuation = 1.0f / (1.0f + light.linear * distance + light.quadratic * (distance * distance));

	// Вернуть сумму всех компонентов
	return ((diffuse * intensity * attenuation) + (specular * intensity * attenuation));
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

	// В зависимости от типа источника
	switch(light.type)
	{
		case uint(LIGHT_POINT):
		resultColor = calculatePointLightComponent(light, fragment, material, cameraPosition);
		break;

		case uint(LIGHT_DIRECTIONAL):
		resultColor = calculateDirectLightComponent(light, fragment, material, cameraPosition);
		break;

		case uint(LIGHT_SPOT):
		resultColor = calculateSpotLightComponent(light, fragment, material, cameraPosition);
		break;
	}

	// Итоговый цвет + альфа
	color = vec4(resultColor,1.0f);
}

/*FRAGMENT-SHADER-END*/