/*VERTEX-SHADER-BEGIN*/
#version 330 core

// Арибуты вершины
layout(location = 0) in vec3 position;   // Положение
layout(location = 1) in vec3 color;      // Цвет
layout(location = 2) in vec2 uv;         // Текстурные координаты
layout(location = 3) in vec3 normal;     // Нормаль

// Структура описывающая параметры мапинга текстуры
struct TextureMapping
{
	vec2 offset;
	vec2 origin;
	vec2 scale;
	mat2 rotation;
};

// Uniform-переменные матриц
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Uniform-переменные для параметров маппинга текстур
uniform TextureMapping diffuseTexMapping;
uniform TextureMapping specularTexMapping;
uniform TextureMapping bumpTexMapping;

// Выходные значения шейдера
// Почти все эти значения будут интерполироваться для каждого фрагмента
out VS_OUT
{
	vec3 color;            // Цвет вершины
	vec2 uvDiffuse;        // Координаты diffuse текстуры
	vec2 uvSpecular;       // Координаты specular текстуры
	vec2 uvBump;           // Координаты bump текстуры
	vec3 normal;           // Нормаль вершины
	vec3 vertexPos;        // Положение вершины в мировых координатах
	vec3 vertexPosLoc;     // Положение вершины в локальных координатах
	mat3 normalMatrix;     // Матрица преобразования нормалей (не интерполируется, наверное)
} vs_out;

// Основная функция вершинного шейдера
// Преобразование координат (и прочих параметров) вершин и передача из следующим этапам
void main()
{
	// Координаты вершины после всех преобразований (мировое пространство, видовое, проекция)
	// Полученое значение это 4D вектор, для которого, на этапе растеризации, выполняется перспективное деление (xyz на w)
	gl_Position = projection * view * model * vec4(position, 1.0);

	// Матрица преобразования нормалей
	// Учитывает только поворот, без искажения нормалей в процессе масштабирования
	vs_out.normalMatrix = mat3(transpose(inverse(model)));

	// Цвет вершины (передается как есть)
	vs_out.color = color;

	// Нормаль вершины трансформируется матрицей нормалей и отдается в таком виде
	vs_out.normal = normalize(vs_out.normalMatrix * normal);

	// Отдать положение фрагмента в мировых координатах
	vs_out.vertexPos = (model * vec4(position, 1.0f)).xyz;

	// Отдать локальное положение вершины (в дальнейшем используется в подсчете тангентов)
	vs_out.vertexPosLoc = position;

	// Передать координаты текстур с учетом трансформаций
	vs_out.uvDiffuse = (diffuseTexMapping.rotation * (uv - diffuseTexMapping.origin)) * diffuseTexMapping.scale + diffuseTexMapping.origin + diffuseTexMapping.offset;
	vs_out.uvSpecular = (specularTexMapping.rotation * (uv - specularTexMapping.origin)) * specularTexMapping.scale + specularTexMapping.origin + specularTexMapping.offset;
	vs_out.uvBump = (bumpTexMapping.rotation * (uv - bumpTexMapping.origin)) * bumpTexMapping.scale + bumpTexMapping.origin + bumpTexMapping.offset;
}

/*VERTEX-SHADER-END*/

////////////////////////////////////////////////////////////////////////////////////

/*GEOMETRY-SHADER-BEGIN*/
#version 330 core

// На входе треугольники
layout (triangles_adjacency) in;
// На выходе треугольники
layout (triangle_strip, max_vertices = 3) out;

// Входные значения шейдера
// Значения для каждой вершины треугольника (из вершинного шейдера)
in VS_OUT
{
	vec3 color;            // Цвет вершины
	vec2 uvDiffuse;        // Координаты diffuse текстуры
	vec2 uvSpecular;       // Координаты specular текстуры
	vec2 uvBump;           // Координаты bump текстуры
	vec3 normal;           // Нормаль вершины
	vec3 vertexPos;        // Положение вершины в мировых координатах
	vec3 vertexPosLoc;     // Положение вершины в локальных координатах
	mat3 normalMatrix;     // Матрица преобразования нормалей (не интерполируется, наверное)
} gs_in[];

// Выходные значения шейдера
// Почти все эти значения будут интерполироваться для каждого фрагмента
out GS_OUT
{
	vec3 color;            // Цвет вершины
	vec2 uvDiffuse;        // Координаты diffuse текстуры
	vec2 uvSpecular;       // Координаты specular текстуры
	vec2 uvBump;           // Координаты bump текстуры
	vec3 normal;           // Нормаль вершины
	vec3 fragmentPos;      // Положение вершины в мировых координатах
	mat3 tbnMatrix;        // Матрица для преобразования из касательного пространства в мировое
} gs_out;

// Подсчет тангента для полигона
vec3 calcTangent(vec3 v0, vec3 v1, vec3 v2, vec2 uv0, vec2 uv1, vec2 uv2)
{
	// Грани полигона в виде векторов
	vec3 edge1 = v1 - v0;
	vec3 edge2 = v2 - v0;

	// Дельта UV для каждой грани
	vec2 deltaUV1 = uv1 - uv0;
	vec2 deltaUV2 = uv2 - uv0;

	// Коэффициент для подсчета тангента
	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	// Подсчитать и вернуть тангент
	return normalize(vec3(
		f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
		f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
		f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)));
}

// Ориентировать тангент по нормали вершины
vec3 OrthogonalizeTangent(vec3 tangent, vec3 vertexNormal)
{
	vec3 bitangent = cross(vertexNormal, tangent);
	return cross(bitangent, vertexNormal);
}

// Основная функция (подсчет TBN матрицы для карт нормалей)
void main() 
{
	// Тангент полигона
	vec3 polygonTangent = calcTangent(
		gs_in[0].vertexPosLoc,
		gs_in[2].vertexPosLoc,
		gs_in[4].vertexPosLoc,
		gs_in[0].uvBump,
		gs_in[2].uvBump,
		gs_in[4].uvBump);

	// Пройтись по всем вершинам
	for(int i = 0; i < gl_in.length(); i++)
	{
		switch(i)
		{
			case 0:
			case 2:
			case 4:

			// Пропуск основных параметров во фрагментный шейдер как есть
			gl_Position = gl_in[i].gl_Position;
			gs_out.color = gs_in[i].color;
			gs_out.uvDiffuse = gs_in[i].uvDiffuse;
			gs_out.uvSpecular = gs_in[i].uvSpecular;
			gs_out.uvBump = gs_in[i].uvBump;
			gs_out.normal = gs_in[i].normal;
			gs_out.fragmentPos = gs_in[i].vertexPos;

			// Собрать TBN матрицу (касательного-мирового пространства)
			vec3 T = OrthogonalizeTangent(gs_in[i].normalMatrix * polygonTangent, gs_in[i].normal);
			vec3 B = cross(gs_in[i].normal, T);
			vec3 N = gs_in[i].normal;
			gs_out.tbnMatrix = mat3(T,B,N);

			// Добавить вершину
			EmitVertex();

			default:
			break;
		}
	}
	EndPrimitive();
}

/*GEOMETRY-SHADER-END*/

////////////////////////////////////////////////////////////////////////////////////

/*FRAGMENT-SHADER-BEGIN*/
#version 330 core

// Вывод в 3 разных вложения
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

// Значения принятые на вход с предыдущих этапов
in GS_OUT
{
	vec3 color;            // Цвет вершины
	vec2 uvDiffuse;        // Координаты diffuse текстуры
	vec2 uvSpecular;       // Координаты specular текстуры
	vec2 uvBump;           // Координаты bump текстуры
	vec3 normal;           // Нормаль вершины
	vec3 fragmentPos;      // Положение вершины в мировых координатах
	mat3 tbnMatrix;        // Матрица для преобразования из касательного пространства в мировое
} fs_in;

// Текстуры
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D bumpTexture;

// Результрующий цвет фрагмента
//out vec4 color;

// Основная функция фрагментного шейдера
// Помещение значений положений фрагментов (положение,цвет,бликовость,нормаль) в цветовые вложения g-буфера
void main()
{
	// Получить нормаль из карты нормалей (используя UV коордианты для текущего фрагмента)
	vec3 normal = normalize(texture(bumpTexture,fs_in.uvBump).rgb * 2.0 - 1.0);
	// Перевести нормаль из скасательного в мировое пространство
	normal = fs_in.tbnMatrix * normal;

	// Положение фрагмента
	gPosition = fs_in.fragmentPos;
	// Нормаль фрагмента
	gNormal = normalize(normal);
	// Цвет фрагмента
	gAlbedoSpec.rgb = fs_in.color * texture(diffuseTexture, fs_in.uvDiffuse).rgb;
	// Интенсивность отражения фрагмента
	gAlbedoSpec.a = texture(specularTexture, fs_in.uvSpecular).r;
}
/*FRAGMENT-SHADER-END*/