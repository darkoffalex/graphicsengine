/*VERTEX-SHADER-BEGIN*/
#version 330 core

// Арибуты вершины
layout(location = 0) in vec3 position;   // Положение
layout(location = 5) in uint phantom;    // Фантомная вершина

// Выходные значения шейдера
// Почти все эти значения будут интерполироваться для каждого фрагмента
out VS_OUT
{
	vec3 vertexPosLoc;       // Положение вершины в локальных координатах
	uint isPhantom;          // Является ли вершина фантомной
} vs_out;

// Основная функция вершинного шейдера
// В данной ситуации все матричные операции произоводятся в геометрическом шейдере, поэтому просто отдаем ему данные
void main()
{
	// Отдать локальное положение вершины
	vs_out.vertexPosLoc = position;
	// Передаем фрагментному шейдеру является ли данная вершина искуственной (фантомной)
	vs_out.isPhantom = phantom;
}

/*VERTEX-SHADER-END*/

////////////////////////////////////////////////////////////////////////////////////

/*GEOMETRY-SHADER-BEGIN*/
#version 330 core

// На входе треугольники
layout (triangles_adjacency) in;
// На выходе треугольники
layout (triangle_strip, max_vertices = 21) out;

// Uniform-переменные
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPosition;

// Входные значения шейдера
// Значения для каждой вершины треугольника (из вершинного шейдера)
in VS_OUT
{
	vec3 vertexPosLoc;     // Положение вершины в локальных координатах
	uint isPhantom;        // Является ли вершина фантомной (искуственной)
} gs_in[];

// Выходные значения шейдера
// Почти все эти значения будут интерполироваться для каждого фрагмента
out GS_OUT
{
	vec3 color;            // Цвет вершины
	vec3 fragmentPos;      // Положение вершины в мировых координатах
} gs_out;

// Подсчет нормали полигона
vec3 CalcNormal(vec3 v0, vec3 v1, vec3 v2, bool ccw = false)
{
	vec3 edge1 = v1 - v0;
	vec3 edge2 = v2 - v0;
	return normalize(ccw ? cross(edge1,edge2) : cross(edge2,edge1));
}

// Обращен ли полигон к источнику освещения
bool IsPolygonFacingLight(vec3 lightPosition, vec3 v0, vec3 v1, vec3 v2, bool ccw = false)
{
	// Получить вектор межу светом и полигонов (используются сумма векторов между всеми его точками)
	vec3 v0toLight = lightPosition - v0;
	vec3 v1toLight = lightPosition - v1;
	vec3 v2toLight = lightPosition - v2;
	vec3 polygonToLight = normalize(v0toLight + v1toLight + v2toLight);

	// Получить нормаль
	vec3 normal = CalcNormal(v0,v1,v2,ccw);

	// Обращен, если скалярное произведение векторов падения света и нормали - положительно
	return dot(polygonToLight,normal) > 0;
}

// Есть ли у полигона фантомная вершина
bool HasPhantomVertex(uint v0, uint v1, uint v2)
{
	return bool(gs_in[v0].isPhantom) || bool(gs_in[v1].isPhantom) || bool(gs_in[v2].isPhantom);
}

// Примитив линии
void EmitLine(uint v0, uint v1, vec3 color = vec3(1.0f,1.0f,1.0f))
{
	gl_Position = projection * view * model * vec4(gs_in[v0].vertexPosLoc, 1.0);
	gs_out.color = vec3(1.0f,1.0f,1.0f);
	gs_out.fragmentPos = (model * vec4(gs_in[v0].vertexPosLoc, 1.0f)).xyz;
	EmitVertex();

	gl_Position = projection * view * model * vec4(gs_in[v1].vertexPosLoc, 1.0);
	gs_out.color = vec3(1.0f,1.0f,1.0f);
	gs_out.fragmentPos = (model * vec4(gs_in[v1].vertexPosLoc, 1.0f)).xyz;
	EmitVertex();
}

// Полигон передней крышки теневого объема
void ShadowVolumeFrontCapPolygon(vec3 color = vec3(0.6f,0.6f,0.6f))
{
	vec3 positions[3];
	positions[0] = gs_in[0].vertexPosLoc;
	positions[1] = gs_in[2].vertexPosLoc;
	positions[2] = gs_in[4].vertexPosLoc;

	for(int i = 0; i < 3; i++)
	{
		// Генерация вершины
		vec4 position = vec4(positions[i], 1.0f);

		gl_Position = projection * view * model * position;
		gs_out.color = color;
		gs_out.fragmentPos = (model * position).xyz;
		EmitVertex();
	}

	EndPrimitive();
}

// Полигон дальней крышки теневого объема
void ShadowVolumeBackCapPolygon(vec3 lightPosition, vec3 color = vec3(0.6f,0.6f,0.6f))
{
	// Вершины полигона сдвигаются вперед по вектор от источка света
	vec3 positions[3];
	float lengthMul = 1000.0f;
	positions[0] = (normalize(gs_in[0].vertexPosLoc - lightPosition) * lengthMul) + gs_in[0].vertexPosLoc;
	positions[1] = (normalize(gs_in[4].vertexPosLoc - lightPosition) * lengthMul) + gs_in[4].vertexPosLoc;
	positions[2] = (normalize(gs_in[2].vertexPosLoc - lightPosition) * lengthMul) + gs_in[2].vertexPosLoc;

	for(int i = 0; i < 3; i++)
	{
		// Генерация вершины, нужно спроецировать бесконечно далеко вперед (4-й компонент - 0)
		vec4 position = vec4(positions[i], 0.0f);

		gl_Position = projection * view * model * position;
		gs_out.color = color;
		gs_out.fragmentPos = (model * position).xyz;
		EmitVertex();
	}

	EndPrimitive();
}

// Вытянуть ребро вдаль от источника (создать дополнительные грани)
void ShadowVolumePolygon(vec3 lightPosition, vec3 v0, vec3 v1)
{
	// Вычислить "сдвинутые" вершины
	float lengthMul = 1000.0f;
	vec3 v2 = (normalize(v0 - lightPosition) * lengthMul) + v0;
	vec3 v3 = (normalize(v1 - lightPosition) * lengthMul) + v1;
	
	// Последовательность отсрисовки вершин полигона "теневого объема"
	vec3 positions[6];
	positions[0] = v0;
	positions[1] = v2;
	positions[2] = v1;

	positions[3] = v1;
	positions[4] = v2;
	positions[5] = v3;

	// Примитивы (2 треугольника) тенегового объема
	for(int i = 0; i < 6; i++)
	{
		// Если это новые вершины, то их нужно спроецировать бесконечно далеко вперед (4-й компонент - 0)
		vec4 position = vec4(positions[i], (i == 1 || i == 4 || i == 5) ? 0.0f : 1.0f);

		// Генерация вершины
		gl_Position = projection * view * model * position;
		gs_out.color = vec3(0.6f,0.6f,0.6f);
		gs_out.fragmentPos = (model * position).xyz;
		EmitVertex();

		if(i == 2 || i == 5){
			EndPrimitive();
		}
	}
}

// Основная функция (подсчет TBN матрицы для карт нормалей)
void main() 
{
    // Получить положение источника света в локальном пространстве (пространстве модели)
	vec3 lightPositionLoc = (inverse(model) * vec4(lightPosition, 1.0f)).xyz;

	// Направлен ли данный полигон к источнику света
	if(IsPolygonFacingLight(lightPositionLoc, gs_in[0].vertexPosLoc, gs_in[2].vertexPosLoc, gs_in[4].vertexPosLoc))
	{
		// Отрисовка полигонов теневого объема
		// Определение силуэтных ребер, вытягивание их в бесконечность от источника света
		if(bool(gs_in[1].isPhantom) || IsPolygonFacingLight(lightPositionLoc, gs_in[0].vertexPosLoc, gs_in[1].vertexPosLoc, gs_in[2].vertexPosLoc) == false)
		{
			//Ребро 0-2 - силуэтное
			ShadowVolumePolygon(lightPositionLoc, gs_in[0].vertexPosLoc, gs_in[2].vertexPosLoc);
		}
		if(bool(gs_in[3].isPhantom) || IsPolygonFacingLight(lightPositionLoc, gs_in[2].vertexPosLoc, gs_in[3].vertexPosLoc, gs_in[4].vertexPosLoc) == false)
		{
			//Ребро 2-4 - силуэтное
			ShadowVolumePolygon(lightPositionLoc, gs_in[2].vertexPosLoc, gs_in[4].vertexPosLoc);
		}
		if(bool(gs_in[5].isPhantom) || IsPolygonFacingLight(lightPositionLoc, gs_in[4].vertexPosLoc, gs_in[5].vertexPosLoc, gs_in[0].vertexPosLoc) == false)
		{
			//Ребро 4-0 - силуэтное
			ShadowVolumePolygon(lightPositionLoc, gs_in[4].vertexPosLoc, gs_in[0].vertexPosLoc);
		}

		// Полигон, который направлен к источнику может служить передней крышкой теневого ообъема (выводим его как есть)
		ShadowVolumeFrontCapPolygon();

		// Тот же полигон, который является частью передней крышки, может быть и частью задней, 
		// если его спроецировать вперед и обойти вершины в обратном порядке
		ShadowVolumeBackCapPolygon(lightPositionLoc);
	}

	EndPrimitive();
}

/*GEOMETRY-SHADER-END*/

////////////////////////////////////////////////////////////////////////////////////

/*FRAGMENT-SHADER-BEGIN*/
#version 330 core

// Вывод в 3-ee вложение
layout (location = 0) out vec4 outColor;

// Значения принятые на вход с предыдущих этапов
in GS_OUT
{
	vec3 color;            // Цвет вершины
	vec3 fragmentPos;      // Положение вершины в мировых координатах
} fs_in;


// Результрующий цвет фрагмента
//out vec4 color;

// Основная функция фрагментного шейдера
// Помещение значений положений фрагментов (положение,цвет,бликовость,нормаль) в цветовые вложения g-буфера
void main()
{
	// Интенсивность отражения фрагмента
	outColor = vec4(fs_in.color,1.0f);
}
/*FRAGMENT-SHADER-END*/