/*VERTEX-SHADER-BEGIN*/
#version 330 core

// Арибуты вершины
layout(location = 0) in vec3 position;   // Положение
layout(location = 1) in vec3 color;      // Цвет
layout(location = 2) in vec2 uv;         // Текстурные координаты
layout(location = 3) in vec3 normal;     // Нормаль
layout(location = 4) in vec3 tangent;    // Тангент

// Структура описывающая параметры мапинга текстуры
struct TexMapping
{
	vec2 offset;
	vec2 scale;
	mat2 rot;
};

// Выходные значение (передаваемые фрагментому шейдеру)
// Эти значения будут интерполированы от вершины к вершине для каждого фрагмента
out vec3 vColor;              // Цвет
out vec2 vTextCoordsDiffuse;  // Текстурные координаты (diffuse)
out vec2 vTextCoordsDetail;   // Текстурные координаты (detail)
out vec2 vTextCoordsSpecular; // Текстурные координаты (specular)
out vec2 vTextCoordsBump;     // Текстурные координаты (bump)
out vec3 vNormal;             // Нормаль
out vec3 vFragmentPosition;   // Положение фрагмента
out mat3 TBN;                 // Матрица для преобразования из касательного пространства в мировое

// Uniform-переменные
uniform mat4 model;                    // Матрица модели (трансформация объекта)
uniform mat4 view;                     // Матрица вида (переход в координатную систему камеры)
uniform mat4 projection;               // Матрица проекции (проекция на экран)

uniform TexMapping texMappingDiffuse;  // Маппинг текстуры (diffuse)
uniform TexMapping texMappingDetail;   // Маппинг текстуры (detail)
uniform TexMapping texMappingSpecular; // Маппинг текстуры (specular)
uniform TexMapping texMappingBump;     // Маппинг текстуры (bump)

// Основная функция (подсчет положения вершины)
void main()
{
	// Матрица преобразования нормалей
	// Учитывает поворот и масштабирование объекта (в дальнейшем лучше вынести в основной код)
	mat3 normalMatrix = mat3(transpose(inverse(model)));

	// Вершина подвергается преобразованием через матрицы
	gl_Position = projection * view * model * vec4(position, 1.0);

	// Цвет передается без изменений
	vColor = color;

	// Текстурные для 4-ех типов текстур (diffuse, detail, specular, bump) передаются с учетом коэффициентов маппинга
	vTextCoordsDiffuse = (texMappingDiffuse.rot * (uv - texMappingDiffuse.offset)) * texMappingDiffuse.scale + 2*texMappingDiffuse.offset;
	vTextCoordsDetail = (texMappingDetail.rot * (uv - texMappingDetail.offset)) * texMappingDetail.scale + 2*texMappingDetail.offset;
	vTextCoordsSpecular = (texMappingSpecular.rot * (uv - texMappingSpecular.offset)) * texMappingSpecular.scale + 2*texMappingSpecular.offset;
	vTextCoordsBump = (texMappingBump.rot * (uv - texMappingBump.offset)) * texMappingBump.scale + 2*texMappingBump.offset;

	// Нормаль подвергается преобразованию матрицы нормалей (частичная матрица модели, только вращения и масштаб)
	vNormal = normalize(normalMatrix * normal);

	// Положение подвергается преобразованию (нужны глобальные координаты)
	vFragmentPosition = vec3(model * vec4(position, 1.0f));

	// Построить матрицу касательного-глоабльного пространства
	vec3 T = normalize(vec3(model * vec4(tangent,   0.0)));
	vec3 B = cross(vNormal, T);
	vec3 N = normalize(vec3(model * vec4(vNormal, 0.0)));
	TBN = mat3(T,B,N);
}
/*VERTEX-SHADER-END*/


/*FRAGMENT-SHADER-BEGIN*/
#version 330 core

// Максимальные кол-во источников разных типов
#define MAX_POINT_LIGHTS 10
#define MAX_DIRECT_LIGHTS 2
#define MAX_SPOT_LIGHTS 10

// Интерполированные входные данные
in vec3 vColor;              // Цвет данного фрагмента
in vec2 vTextCoordsDiffuse;  // Текстурные координаты (diffuse)
in vec2 vTextCoordsDetail;   // Текстурные координаты (detail)
in vec2 vTextCoordsSpecular; // Текстурные координаты (specular)
in vec2 vTextCoordsBump;     // Текстурные координаты (bump)
in vec3 vNormal;             // Нормаль от данного фрагмента
in vec3 vFragmentPosition;   // Положение данного фрагмента (абсолютное)
in mat3 TBN;                 // Матрица для преобразования из касательного пространства в мировое

// Описание материала
struct Material
{
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	float shininess;
};

// Точечный источник освещения
struct PointLight
{
	vec3 position;
	vec3 color;
	float linear;
	float quadratic;
};

// Направленный источник освещения
struct DirectLight
{
	vec3 direction;
	vec3 color;
};

// Прожектор (фонарик)
struct SpotLight
{
	vec3 position;
	vec3 color;
	float linear;
	float quadratic;
	vec3 direction;
	float cutOffCos;
	float cutOffOuterCos;
	mat4 modelMatrix;
};

// Uniform-переменные
uniform Material material;                           // Материал

uniform PointLight pointLights[MAX_POINT_LIGHTS];    // Точечные источники света
uniform DirectLight directLights[MAX_DIRECT_LIGHTS]; // Направленные источники света
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];       // Прожекторы/фонарики

uniform vec3 eyePosition;                            // Положение наблюдателя (камеры)

uniform sampler2D diffuseTexture;                    // Диффузная текстура
uniform sampler2D detailTexture;                     // Деатльная текстура
uniform sampler2D specularTexture;                   // Бликовая текстура
uniform sampler2D bumpTexture;                       // Бамп текстура (карта нормалей)
uniform sampler2D flashlightTexture;                 // Текстура пятен фонарика

// Функции подсчета цветов для каждого типа источника
vec3 CalcPointLightComponents(PointLight light, vec3 normal, vec3 viewDir);
vec3 CalcDirectionalLightComponents(DirectLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLightComponents(SpotLight light, vec3 normal, vec3 viewDir);

// Результрующий цвет фрагмента
out vec4 color;

// Основная функция (подсчет цвета фрагмента)
void main()
{
	// Получить нормаль из карты нормалей (используя UV коордианты для текущего фрагмента)
	vec3 normal = normalize(texture(bumpTexture,vTextCoordsBump).rgb * 2.0 - 1.0);

	// Перевести из скасательного в мировое пространство
	normal = TBN * normal;

	// Направление взгляда наблюдателя (на самом деле это вектор из фрагмента в направлении наблюдателя)
	vec3 viewDir = normalize(eyePosition - vFragmentPosition);

	// Результирующий цвет
	vec3 result;

	// Пройтись по массиву точечных источников света
	for(int i = 0; i < MAX_POINT_LIGHTS; i++){
		result += CalcPointLightComponents(pointLights[i], normal, viewDir);
	}

	// Пройтись по массиву направленных источников света
	for(int i = 0; i < MAX_DIRECT_LIGHTS; i++){
		result += CalcDirectionalLightComponents(directLights[i], normal, viewDir);
	}

	// Пройтись по массиву прожекторов-фонариков
	for(int i = 0; i < MAX_SPOT_LIGHTS; i++){
		result += CalcSpotLightComponents(spotLights[i], normal, viewDir);
	}

	// Итоговый цвет с учетом всех состовляющих (ambient,siffuse,specular), цветов вершин и текстурных координат
	color = vec4(vColor * result, 1.0f);
}


// Подсчет освещенности фрагмента точечным источником
vec3 CalcPointLightComponents(PointLight light, vec3 normal, vec3 viewDir)
{
	// Коэффициент затухания (использует расстояние до источника, а так же спец-коэффициенты источника)
	float distance = length(light.position - vFragmentPosition);
	float attenuation = 1.0f / (1.0f + light.linear * distance + light.quadratic * (distance * distance));

	// Фоновый (ambient) цвет просто равномерно заполняет каждый фрагмент
	vec3 ambient = light.color * material.ambientColor * vec3(texture(diffuseTexture,vTextCoordsDiffuse));

	// Рассеянный (diffuse) зависит от угла между нормалью фрагмента и вектором падения света на фрагмент
	vec3 lightDir = normalize(light.position - vFragmentPosition);
	float diffuseBrightness = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.color * (diffuseBrightness * material.diffuseColor) * vec3(texture(diffuseTexture,vTextCoordsDiffuse));

	// Бликовый (specular) зависит от угла между вектором взгляда и отроженным относительно нормали вектором падения света на фрагмент
	vec3 reflectedLightDir = reflect(-lightDir, normal);
	float specularBrightness = pow(max(dot(viewDir, reflectedLightDir), 0.0), material.shininess);
	vec3 specular = light.color * (specularBrightness * material.specularColor) * vec3(texture(specularTexture,vTextCoordsSpecular));

	return ((ambient * attenuation) + (diffuse * attenuation) + (specular * attenuation));
}

// Подсчет освещенности фрагмента направленным светом
vec3 CalcDirectionalLightComponents(DirectLight light, vec3 normal, vec3 viewDir)
{
	// Фоновый (ambient) цвет просто равномерно заполняет каждый фрагмент
	vec3 ambient = light.color * material.ambientColor * vec3(texture(diffuseTexture,vTextCoordsDiffuse));

	// Рассеянный (diffuse) зависит от угла между нормалью фрагмента и вектором падения света на фрагмент
	vec3 lightDir = normalize(light.direction);
	float diffuseBrightness = max(dot(normal, -lightDir), 0.0);
	vec3 diffuse = light.color * (diffuseBrightness * material.diffuseColor) * vec3(texture(diffuseTexture,vTextCoordsDiffuse));

	// Бликовый (specular) зависит от угла между вектором взгляда и отроженным относительно нормали вектором падения света на фрагмент
	vec3 reflectedLightDir = reflect(lightDir, normal); 
	float specularBrightness = pow(max(dot(viewDir, reflectedLightDir), 0.0), material.shininess);
	vec3 specular = light.color * (specularBrightness * material.specularColor) * vec3(texture(specularTexture,vTextCoordsSpecular));

	return (ambient + diffuse + specular);
}

// Подсчет освещенности фрагмента светом прожектора/фонарика
vec3 CalcSpotLightComponents(SpotLight light, vec3 normal, vec3 viewDir)
{
	// Вектор падения света на источник
	vec3 lightDir = normalize(light.position - vFragmentPosition);

	// Косинус угла между вектором падения света и вектором направления источника
	float thetaCos = dot(lightDir, normalize(-light.direction));

	// Разница косинусов между углом внутреннего конуса и углом внешнего
	float epsilon = light.cutOffCos - light.cutOffOuterCos;

	// Свет наиболее интенсивен в центре (где thetaCos - 1, угол между лучем и направлением фонарика - 0)
	// К краям интенсивность спадает. Благодаря коэффициенту epsilon есть так же яркое пятно внутри (внутр. конус)
	float intensity = clamp((thetaCos - light.cutOffOuterCos) / epsilon, 0.0, 1.0);

	// Разница между вектором луча и вектором навпраления источника (для определения центра пятна)
	vec3 d = lightDir - normalize(-light.direction);
	// Перевести этот вектор из мировых координат в координаты источника
	d = (transpose(light.modelMatrix) * vec4(d, 0.0f)).xyz;
	// Вычислить координаты текстуры по вектору разницы
	vec2 flashLightTexCoords = vec2(d.x * -0.5f + 0.5f, d.y * -0.5f + 0.5f);
	// Получить цвет соответствующий текстелу текстуры с данными координатами
	vec3 texIntensity = vec3(texture(flashlightTexture,flashLightTexCoords));

	// Рассеянный (diffuse)
	float diffuseBrightness = max(dot(normal,lightDir), 0.0);
	vec3 diffuse = light.color * (diffuseBrightness * material.diffuseColor) * vec3(texture(diffuseTexture,vTextCoordsDiffuse));

	// Бликовый (specular)
	vec3 reflectedLightDir = reflect(-lightDir, normal);  
	float specularBrightness = pow(max(dot(viewDir, reflectedLightDir), 0.0), material.shininess);
	vec3 specular = light.color * (specularBrightness * material.specularColor) * vec3(texture(specularTexture,vTextCoordsSpecular));

	// Коэффициент затухания (использует расстояние до источника, а так же спец-коэффициенты источника)
	float distance = length(light.position - vFragmentPosition);
	float attenuation = 1.0f / (1.0f + light.linear * distance + light.quadratic * (distance * distance));

	return ((diffuse * attenuation * intensity * texIntensity) + (specular * attenuation * intensity * texIntensity ));
}
/*FRAGMENT-SHADER-END*/