#include "ObjLoader.h"

#include <fstream>
#include <sstream>

/**
* \brief Загрузка данных из .obj файла
* \param path Путь к файлу
* \param withoutUV Без UV координат
*/
void ObjLoader::LoadFromFile(std::string path, bool withoutUV)
{
	// Попытка открыть файл для чтения
	std::ifstream in;
	in.open(path, std::ifstream::in);
	if (in.fail()) return;

	// Текущая строка
	std::string line;

	while (!in.eof()) {
		// Получить строку
		std::getline(in, line);

		// Получить строковй поток
		std::istringstream iss(line);

		// Переменная для символьного мусора
		char trash;

		// Если строчка начинается с "v " (нет разницы первых 2 символов строки и "v ") - это положения вершин
		if (!line.compare(0, 2, "v ")) {
			// Вписать символ "v" в строковую "мусорку"
			iss >> trash;
			// Далее идут координаты вершин (float), вписать их по очереди
			VertexPosition vp;
			iss >> vp.x;
			iss >> vp.y;
			iss >> vp.z;
			// Добавить положение вершины в список положений
			this->positions_.push_back(vp);
		}
		// Если строчка начинается с "vt" (нет разницы первых 2 символов строки и "vt") - это текстурные координаты
		else if(!withoutUV && !line.compare(0, 2, "vt")) {
			// Вписать символ "vt" в строковую "мусорку"
			iss >> trash >> trash;
			// Далее идут координаты текстуры (float), вписать их по очереди
			VertexTexCoords vtc;
			iss >> vtc.u;
			iss >> vtc.v;
			// Добавить uv-координаты в список координат
			this->texCoords_.push_back(vtc);
		}
		// Если строчка начинается с "vn" (нет разницы первых 2 символов строки и "vn") - это нормали
		else if (!line.compare(0, 2, "vn")) {
			// Вписать символ "vn" в строковую "мусорку"
			iss >> trash >> trash;
			// Далее идут координаты текстуры (float), вписать их по очереди
			VertexNormal vn;
			iss >> vn.x;
			iss >> vn.y;
			iss >> vn.z;
			// Добавить uv-координаты в список координат
			this->normals_.push_back(vn);
		}
		// Если строка начинается с "f "
		else if (!line.compare(0, 2, "f ")) {
			// Массив индексов полигона
			std::vector<int> faceIndices;
			// Вписать символ первое значение строки (это символ "f") в строковую "мусорку"
			iss >> trash;
			// Набор индексов полигона
			Poly p;
			// Индексы (положения, текстурных координат, нормали)
			int positionIndex;
			int textCoordsIndex;
			int normalIndex;

			// Если UV координаты не заданы (формат строки vp//vn vp//vn vp//vn)
			if (withoutUV) {
				// Цикл пройдется по строке до ее окончания (обычно 3 раза)
				while (iss >> positionIndex >> trash >> trash >> normalIndex) {
					// Отнимаем 1, поскольку в файле индексы идут с 1 а в массивах нумерация с ноля
					positionIndex--;
					normalIndex--;
					// Создаем набор индексов вершины
					VertexIndices vi;
					vi.position = positionIndex;
					vi.normal = normalIndex;
					// Добавляем набор вершины в полигон
					p.vertexIndices.push_back(vi);
				}
			}
			// Если UV координаты заданы (формат строки vp/vt/vn vp/vt/vn vp/vt/vn)
			else {
				// Цикл пройдется по строке до ее окончания (3 раза)
				while (iss >> positionIndex >> trash >> textCoordsIndex >> trash >> normalIndex) {
					// Отнимаем 1, поскольку в файле индексы идут с 1 а в массивах нумерация с ноля
					positionIndex--;
					textCoordsIndex--;
					normalIndex--;
					// Создаем набор индексов вершины
					VertexIndices vi;
					vi.position = positionIndex;
					vi.texCoords = textCoordsIndex;
					vi.normal = normalIndex;
					// Добавляем набор вершины в полигон
					p.vertexIndices.push_back(vi);
				}
			}
			
			// После завершения цикла в структуре "p" будет по 3 индекса каждого типа
			// Эту стрктуру можно отправлять в массив полигонов
			this->faces_.push_back(p);
		}
	}
}

/**
* \brief Очистка данных
*/
void ObjLoader::Clear()
{
	this->positions_.clear();
	this->texCoords_.clear();
	this->normals_.clear();
	this->faces_.clear();
}

/**
* \brief Создать ресурс статической геометрии для OpenGL рендерера
* \param inverseOrder Изменить порядок следования вершин в полигонах (против/по часовой стрелке)
* \param recalcNormals Пересчитать нормали (дорогая операция)
* \param adjacency Построить геометрию со смежностями
* \return Указатель на ресурс для OpenGL рендерера
*/
ogl::StaticGeometryResourcePtr ObjLoader::MakeOglRendererResource(bool inverseOrder, bool recalcNormals, bool adjacency)
{
	// Массив вершин
	std::vector<ogl::Vertex> vertices;
	// Массив индексов (индексы полигонов)
	std::vector<GLuint> indices;
	// Массив идентификаторов вершин
	std::vector<VertexIndices> vertexIds;

	// Для всех полигонов
	for (auto face : this->faces_)
	{
		// Временный массив индексов
		// Этот массив будет добавляться к общему массиву индексов, в случае необходимости его порядок можеть быть инвертирован
		std::vector<GLuint> indicesTemp;

		// Для всех наборов индексов вершин (обычно 3)
		for (const VertexIndices& vertexIndices : face.vertexIndices)
		{
			// Найти набор индексов в массиве идентификаторов
			std::vector<VertexIndices>::iterator it = std::find(vertexIds.begin(), vertexIds.end(), vertexIndices);

			// Если такой набор индексов уже ранее рассматривался (по сути проверяем встречалась ли нам такая вершина)
			if(it != vertexIds.end())
			{
				// Индекс элемента массива
				auto elementIndex = std::distance(vertexIds.begin(), it);
				// Добавить индекс в массив индексов
				indicesTemp.push_back(elementIndex);
			}
			// Если встречаем такую вершину впервые
			else
			{
				// Создать новую вершину
				ogl::Vertex v;

				// Получить значения из массивов
				VertexPosition position = this->positions_[vertexIndices.position];
				VertexNormal normal = this->normals_[vertexIndices.normal];
				VertexTexCoords texCoords = this->texCoords_[vertexIndices.texCoords];

				v.position = glm::vec3({
					static_cast<GLfloat>(position.x),
					static_cast<GLfloat>(position.y),
					static_cast<GLfloat>(position.z)
				});

				v.normal = glm::normalize(glm::vec3({
					static_cast<GLfloat>(normal.x),
					static_cast<GLfloat>(normal.y),
					static_cast<GLfloat>(normal.z)
				}));

				if (!inverseOrder) v.normal *= -1;

				v.uv = glm::vec2({
					static_cast<GLfloat>(texCoords.u),
					static_cast<GLfloat>(texCoords.v),
				});

				v.color = { 1.0f,1.0f,1.0f };
				v.tangent = {0.0f,0.0f,0.0f};
				v.phantom = 0;

				// Добавить саму вершину в массив
				vertices.push_back(v);
				// Добавить идентификацию вершины в массив
				vertexIds.push_back(vertexIndices);
				// Добавить индекс новой вершины в массив индексов
				indicesTemp.push_back(vertices.size()-1);
			}
		}

		// Сменить порядок вершин в полигоне (если нужно)
		if(inverseOrder) std::reverse(indicesTemp.begin(), indicesTemp.end());

		// Добавить к общему массиву
		indices.insert(indices.end(), indicesTemp.begin(), indicesTemp.end());
	}

	return ogl::MakeStaticGeometryResource(vertices, indices, false, recalcNormals, false, adjacency);
}

