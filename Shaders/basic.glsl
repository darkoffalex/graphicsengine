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
out vec3 vsoColor;              // Цвет
out vec2 vsoTextCoordsDiffuse;  // Текстурные координаты (diffuse)
out vec2 vsoTextCoordsDetail;   // Текстурные координаты (detail)
out vec2 vsoTextCoordsSpecular; // Текстурные координаты (specular)
out vec2 vsoTextCoordsBump;     // Текстурные координаты (bump)
out vec3 vsoNormal;             // Нормаль
out vec3 vsoFragmentPosition;   // Положение вершины в мир. координатах (интерполируется для фрагментов)
out vec3 vsoLocalPosition;      // Положение вершины (исходное, без трансформаций)
out mat3 vsoNormalMatrix;       // Матрица преобразования нормалей (учет поворота и масштабирования)

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
	vsoNormalMatrix = mat3(transpose(inverse(model)));

	// Вершина подвергается преобразованием через матрицы
	gl_Position = projection * view * model * vec4(position, 1.0);

	// Цвет передается без изменений
	vsoColor = color;

	// Текстурные для 4-ех типов текстур (diffuse, detail, specular, bump) передаются с учетом коэффициентов маппинга
	vsoTextCoordsDiffuse = (texMappingDiffuse.rot * (uv - texMappingDiffuse.offset)) * texMappingDiffuse.scale + 2*texMappingDiffuse.offset;
	vsoTextCoordsDetail = (texMappingDetail.rot * (uv - texMappingDetail.offset)) * texMappingDetail.scale + 2*texMappingDetail.offset;
	vsoTextCoordsSpecular = (texMappingSpecular.rot * (uv - texMappingSpecular.offset)) * texMappingSpecular.scale + 2*texMappingSpecular.offset;
	vsoTextCoordsBump = (texMappingBump.rot * (uv - texMappingBump.offset)) * texMappingBump.scale + 2*texMappingBump.offset;

	// Нормаль подвергается преобразованию матрицы нормалей (частичная матрица модели, только вращения и масштаб)
	vsoNormal = normalize(vsoNormalMatrix * normal);

	// Положение подвергается преобразованию (нужны глобальные координаты)
	vsoFragmentPosition = vec3(model * vec4(position, 1.0f));

	// Исходное положение вершины
	vsoLocalPosition = position;
}
/*VERTEX-SHADER-END*/

////////////////////////////////////////////////////////////////////////////////////

/*GEOMETRY-SHADER-BEGIN*/
#version 330 core

// На входе треугольники
layout (triangles) in;
// На выходе треугольники
layout (triangle_strip, max_vertices = 3) out;

// Интерполированные входные данные
in vec3 vsoColor[];              // Цвет данного фрагмента
in vec2 vsoTextCoordsDiffuse[];  // Текстурные координаты (diffuse)
in vec2 vsoTextCoordsDetail[];   // Текстурные координаты (detail)
in vec2 vsoTextCoordsSpecular[]; // Текстурные координаты (specular)
in vec2 vsoTextCoordsBump[];     // Текстурные координаты (bump)
in vec3 vsoNormal[];             // Нормаль от данного фрагмента
in vec3 vsoFragmentPosition[];   // Положение вершины в мир. координатах (интерполируется для фрагментов)
in vec3 vsoLocalPosition[];      // Положение вершины (исходное, без трансформаций)
in mat3 vsoNormalMatrix[];       // Матрица преобразования нормалей (учет поворота и масштабирования)

// Выходные параметры геом. шейдера
out vec3 gsoColor;              // Цвет
out vec2 gsoTextCoordsDiffuse;  // Текстурные координаты (diffuse)
out vec2 gsoTextCoordsDetail;   // Текстурные координаты (detail)
out vec2 gsoTextCoordsSpecular; // Текстурные координаты (specular)
out vec2 gsoTextCoordsBump;     // Текстурные координаты (bump)
out vec3 gsoNormal;             // Нормаль
out vec3 gsoFragmentPosition;   // Положение фрагмента
out mat3 gsoTBN;                // Матрица для преобразования из касательного пространства в мировое

// Uniform-переменные
uniform mat4 model;             // Матрица модели (трансформация объекта)

// Подсчет тангента для полигона
vec3 calcTangent();

// Ориентировать тангент по нормали вершины
vec3 OrthogonalizeTangent(vec3 tangent, vec3 vertexNormal);

// Основная функция (подсчет TBN матрицы для карт нормалей)
void main() 
{
	// Тангент полигона
	vec3 polygonTangent = calcTangent();

	// Пройтись по всем вершинам
	for(int i = 0; i < 3; i++)
	{
		// Пропуск основных параметров во фрагментный шейдер как есть
		gl_Position = gl_in[i].gl_Position;
		gsoColor = vsoColor[i];
		gsoTextCoordsDiffuse = vsoTextCoordsDiffuse[i];
		gsoTextCoordsDetail = vsoTextCoordsDetail[i];
		gsoTextCoordsSpecular = vsoTextCoordsSpecular[i];
		gsoTextCoordsBump = vsoTextCoordsBump[i];
		gsoNormal = vsoNormal[i];
		gsoFragmentPosition = vsoFragmentPosition[i];

		// Собрать TBN матрицу (касательного-мирового пространства)
		vec3 T = OrthogonalizeTangent(vsoNormalMatrix[i] * polygonTangent, vsoNormal[i]);
		vec3 B = cross(vsoNormal[i], T);
		vec3 N = vsoNormal[i];
		gsoTBN = mat3(T,B,N);

		EmitVertex();
	}
	EndPrimitive();
}

// Подсчет тангента для полигона
vec3 calcTangent()
{
	// Грани полигона в виде векторов
	vec3 edge1 = vsoLocalPosition[1] - vsoLocalPosition[0];
	vec3 edge2 = vsoLocalPosition[2] - vsoLocalPosition[0];

	// Дельта UV для каждой грани
	vec2 deltaUV1 = vsoTextCoordsBump[1] - vsoTextCoordsBump[0];
	vec2 deltaUV2 = vsoTextCoordsBump[2] - vsoTextCoordsBump[0];

	// Коэффициент для подсчета тангента
	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	// Подсчитать и вернуть тангент
	return normalize(vec3(
		f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
		f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
		f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)));
}

// Ориентировать тангент согласно нормали вершины
vec3 OrthogonalizeTangent(vec3 tangent, vec3 vertexNormal)
{
	vec3 bitangent = cross(vertexNormal, tangent);
	return cross(bitangent, vertexNormal);
}

/*GEOMETRY-SHADER-END*/

////////////////////////////////////////////////////////////////////////////////////

/*FRAGMENT-SHADER-BEGIN*/
#version 330 core

// Максимальные кол-во источников разных типов
#define MAX_POINT_LIGHTS 10
#define MAX_DIRECT_LIGHTS 2
#define MAX_SPOT_LIGHTS 10

// Интерполированные входные данные
in vec3 gsoColor;              // Цвет данного фрагмента
in vec2 gsoTextCoordsDiffuse;  // Текстурные координаты (diffuse)
in vec2 gsoTextCoordsDetail;   // Текстурные координаты (detail)
in vec2 gsoTextCoordsSpecular; // Текстурные координаты (specular)
in vec2 gsoTextCoordsBump;     // Текстурные координаты (bump)
in vec3 gsoNormal;             // Нормаль от данного фрагмента
in vec3 gsoFragmentPosition;   // Положение данного фрагмента (абсолютное)
in mat3 gsoTBN;                 // Матрица для преобразования из касательного пространства в мировое

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
	vec3 normal = normalize(texture(bumpTexture,gsoTextCoordsBump).rgb * 2.0 - 1.0);

	// Перевести из скасательного в мировое пространство
	normal = gsoTBN * normal;

	// Направление взгляда наблюдателя (на самом деле это вектор из фрагмента в направлении наблюдателя)
	vec3 viewDir = normalize(eyePosition - gsoFragmentPosition);

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

	// Альфа-канал. Отвечает на смешивание цветов с уже имеющимися во фрейм-буфере. 
	// Берется из diffuse текстуры через ее uv-координаты
	float alpha = texture(diffuseTexture, gsoTextCoordsDiffuse).a;

	// Если альфа канал очень мал - отбросить этот фрагмент
	if(alpha < 0.01f){
		discard;
	}

	// Итоговый цвет с учетом всех составляющих (ambient,siffuse,specular)
	color = vec4(gsoColor * result, alpha);
}


// Подсчет освещенности фрагмента точечным источником
vec3 CalcPointLightComponents(PointLight light, vec3 normal, vec3 viewDir)
{
	// Коэффициент затухания (использует расстояние до источника, а так же спец-коэффициенты источника)
	float distance = length(light.position - gsoFragmentPosition);
	float attenuation = 1.0f / (1.0f + light.linear * distance + light.quadratic * (distance * distance));

	// Фоновый (ambient) цвет просто равномерно заполняет каждый фрагмент
	vec3 ambient = light.color * material.ambientColor * vec3(texture(diffuseTexture,gsoTextCoordsDiffuse));

	// Рассеянный (diffuse) зависит от угла между нормалью фрагмента и вектором падения света на фрагмент
	vec3 lightDir = normalize(light.position - gsoFragmentPosition);
	float diffuseBrightness = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.color * (diffuseBrightness * material.diffuseColor) * vec3(texture(diffuseTexture,gsoTextCoordsDiffuse));

	// Бликовый (specular) зависит от угла между вектором взгляда и отроженным относительно нормали вектором падения света на фрагмент
	vec3 reflectedLightDir = reflect(-lightDir, normal);
	float specularBrightness = pow(max(dot(viewDir, reflectedLightDir), 0.0), material.shininess);
	vec3 specular = light.color * (specularBrightness * material.specularColor) * vec3(texture(specularTexture,gsoTextCoordsSpecular));

	return ((ambient * attenuation) + (diffuse * attenuation) + (specular * attenuation));
}

// Подсчет освещенности фрагмента направленным светом
vec3 CalcDirectionalLightComponents(DirectLight light, vec3 normal, vec3 viewDir)
{
	// Фоновый (ambient) цвет просто равномерно заполняет каждый фрагмент
	vec3 ambient = light.color * material.ambientColor * vec3(texture(diffuseTexture,gsoTextCoordsDiffuse));

	// Рассеянный (diffuse) зависит от угла между нормалью фрагмента и вектором падения света на фрагмент
	vec3 lightDir = normalize(light.direction);
	float diffuseBrightness = max(dot(normal, -lightDir), 0.0);
	vec3 diffuse = light.color * (diffuseBrightness * material.diffuseColor) * vec3(texture(diffuseTexture,gsoTextCoordsDiffuse));

	// Бликовый (specular) зависит от угла между вектором взгляда и отроженным относительно нормали вектором падения света на фрагмент
	vec3 reflectedLightDir = reflect(lightDir, normal); 
	float specularBrightness = pow(max(dot(viewDir, reflectedLightDir), 0.0), material.shininess);
	vec3 specular = light.color * (specularBrightness * material.specularColor) * vec3(texture(specularTexture,gsoTextCoordsSpecular));

	return (ambient + diffuse + specular);
}

// Подсчет освещенности фрагмента светом прожектора/фонарика
vec3 CalcSpotLightComponents(SpotLight light, vec3 normal, vec3 viewDir)
{
	// Вектор падения света на источник
	vec3 lightDir = normalize(light.position - gsoFragmentPosition);

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
	vec3 diffuse = light.color * (diffuseBrightness * material.diffuseColor) * vec3(texture(diffuseTexture,gsoTextCoordsDiffuse));

	// Бликовый (specular)
	vec3 reflectedLightDir = reflect(-lightDir, normal);  
	float specularBrightness = pow(max(dot(viewDir, reflectedLightDir), 0.0), material.shininess);
	vec3 specular = light.color * (specularBrightness * material.specularColor) * vec3(texture(specularTexture,gsoTextCoordsSpecular));

	// Коэффициент затухания (использует расстояние до источника, а так же спец-коэффициенты источника)
	float distance = length(light.position - gsoFragmentPosition);
	float attenuation = 1.0f / (1.0f + light.linear * distance + light.quadratic * (distance * distance));

	return ((diffuse * attenuation * intensity * texIntensity) + (specular * attenuation * intensity * texIntensity ));
}
/*FRAGMENT-SHADER-END*/