#include "Defaults.h"
#include <map>

namespace ogl
{
	/**
	* \brief Получить вершины примитивов по умолчанию
	* \param type Тип примитива
	* \param size Размер примитива
	* \return Массив вершин
	*/
	std::vector<Vertex> defaults::GetVertices(DefaultGeometryType type, float size)
	{
		std::map<DefaultGeometryType, std::vector<Vertex>> vertices;

		vertices[DefaultGeometryType::CUBE] = {
			{ { (size / 2),(size / 2),(size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,1.0f } },
			{ { (size / 2),-(size / 2),(size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,0.0f } },
			{ { -(size / 2),-(size / 2),(size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,0.0f } },
			{ { -(size / 2),(size / 2),(size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,1.0f } },

			{ { (size / 2),(size / 2),-(size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,1.0f } },
			{ { (size / 2),-(size / 2),-(size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,0.0f } },
			{ { (size / 2),-(size / 2),(size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,0.0f } },
			{ { (size / 2),(size / 2),(size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,1.0f } },

			{ { (size / 2),  (size / 2), -(size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,1.0f } },
			{ { (size / 2), (size / 2), (size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,0.0f } },
			{ { -(size / 2), (size / 2), (size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,0.0f } },
			{ { -(size / 2),  (size / 2), -(size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,1.0f } },

			{ { -(size / 2),(size / 2),-(size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,1.0f } },
			{ { -(size / 2),-(size / 2),-(size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,0.0f } },
			{ { (size / 2),-(size / 2),-(size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,0.0f } },
			{ { (size / 2),(size / 2),-(size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,1.0f } },

			{ { -(size / 2),(size / 2),(size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,1.0f } },
			{ { -(size / 2),-(size / 2),(size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,0.0f } },
			{ { -(size / 2),-(size / 2),-(size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,0.0f } },
			{ { -(size / 2),(size / 2),-(size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,1.0f } },

			{ { -(size / 2),  -(size / 2), -(size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,1.0f } },
			{ { -(size / 2), -(size / 2), (size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,0.0f } },
			{ { (size / 2), -(size / 2), (size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,0.0f } },
			{ { (size / 2),  -(size / 2), -(size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,1.0f } },
		};

		vertices[DefaultGeometryType::PLANE] = {
			{ { (size / 2), (size / 2), 0.0f },{ 1.0f,1.0f,1.0f },{ 1.0f,1.0f } },
			{ { (size / 2), -(size / 2), 0.0f },{ 1.0f,1.0f,1.0f },{ 1.0f,0.0f } },
			{ { -(size / 2), -(size / 2), 0.0f },{ 1.0f,1.0f,1.0f },{ 0.0f,0.0f } },
			{ { -(size / 2), (size / 2),  0.0f },{ 1.0f,1.0f,1.0f },{ 0.0f,1.0f } },
		};

		vertices[DefaultGeometryType::CUBE_SKYBOX] = {
			{ { -1.0f,  1.0f, -1.0f },{ 1.0f,1.0f,1.0f } },
			{ { 1.0f, -1.0f, -1.0f },{ 1.0f,1.0f,1.0f } },
			{ { -1.0f, -1.0f, -1.0f },{ 1.0f,1.0f,1.0f } },
			{ { 1.0f, -1.0f, -1.0f },{ 1.0f,1.0f,1.0f } },
			{ { -1.0f,  1.0f, -1.0f },{ 1.0f,1.0f,1.0f } },
			{ { 1.0f,  1.0f, -1.0f },{ 1.0f,1.0f,1.0f } },

			{ { -1.0f, -1.0f,  1.0f },{ 1.0f,1.0f,1.0f } },
			{ { -1.0f,  1.0f, -1.0f },{ 1.0f,1.0f,1.0f } },
			{ { -1.0f, -1.0f, -1.0f },{ 1.0f,1.0f,1.0f } },
			{ { -1.0f,  1.0f, -1.0f },{ 1.0f,1.0f,1.0f } },
			{ { -1.0f, -1.0f,  1.0f },{ 1.0f,1.0f,1.0f } },
			{ { -1.0f,  1.0f,  1.0f },{ 1.0f,1.0f,1.0f } },

			{ { 1.0f, -1.0f, -1.0f },{ 1.0f,1.0f,1.0f } },
			{ { 1.0f,  1.0f,  1.0f },{ 1.0f,1.0f,1.0f } },
			{ { 1.0f, -1.0f,  1.0f },{ 1.0f,1.0f,1.0f } },
			{ { 1.0f,  1.0f,  1.0f },{ 1.0f,1.0f,1.0f } },
			{ { 1.0f, -1.0f, -1.0f },{ 1.0f,1.0f,1.0f } },
			{ { 1.0f,  1.0f, -1.0f },{ 1.0f,1.0f,1.0f } },

			{ { -1.0f, -1.0f,  1.0f },{ 1.0f,1.0f,1.0f } },
			{ { 1.0f,  1.0f,  1.0f },{ 1.0f,1.0f,1.0f } },
			{ { -1.0f,  1.0f,  1.0f },{ 1.0f,1.0f,1.0f } },
			{ { 1.0f,  1.0f,  1.0f },{ 1.0f,1.0f,1.0f } },
			{ { -1.0f, -1.0f,  1.0f },{ 1.0f,1.0f,1.0f } },
			{ { 1.0f, -1.0f,  1.0f },{ 1.0f,1.0f,1.0f } },

			{ { -1.0f,  1.0f, -1.0f },{ 1.0f,1.0f,1.0f } },
			{ { 1.0f,  1.0f,  1.0f },{ 1.0f,1.0f,1.0f } },
			{ { 1.0f,  1.0f, -1.0f },{ 1.0f,1.0f,1.0f } },
			{ { 1.0f,  1.0f,  1.0f },{ 1.0f,1.0f,1.0f } },
			{ { -1.0f,  1.0f, -1.0f },{ 1.0f,1.0f,1.0f } },
			{ { -1.0f,  1.0f,  1.0f },{ 1.0f,1.0f,1.0f } },

			{ { -1.0f, -1.0f, -1.0f },{ 1.0f,1.0f,1.0f } },
			{ { 1.0f, -1.0f, -1.0f },{ 1.0f,1.0f,1.0f } },
			{ { -1.0f, -1.0f,  1.0f },{ 1.0f,1.0f,1.0f } },
			{ { 1.0f, -1.0f, -1.0f },{ 1.0f,1.0f,1.0f } },
			{ { 1.0f, -1.0f,  1.0f },{ 1.0f,1.0f,1.0f } },
			{ { -1.0f, -1.0f,  1.0f },{ 1.0f,1.0f,1.0f } },
		};

		auto it = vertices.find(type);

		if (it == vertices.end()) return{};

		return it->second;
	}

	/**
	* \brief Получить индексы примитивов по умолчанию
	* \param type Тип примитива
	* \return Массив индексов
	*/
	std::vector<GLuint> defaults::GetIndices(DefaultGeometryType type)
	{
		std::map<DefaultGeometryType, std::vector<GLuint>> indices;

		indices[DefaultGeometryType::CUBE] = {
			0,1,2, 0,2,3,
			4,5,6, 4,6,7,
			8,9,10, 8,10,11,
			12,13,14, 12,14,15,
			16,17,18, 16,18,19,
			20,21,22, 20,22,23,
		};

		indices[DefaultGeometryType::CUBE_FULLY_INDEXED] = {
			0,1,2, 2,3,0,
			4,0,3, 3,7,4,
			3,2,6, 6,7,3,
			4,5,1, 1,0,4,
			6,2,1, 1,5,6,
			7,6,5, 5,4,7
		};

		indices[DefaultGeometryType::PLANE] = {
			0,1,2, 0,2,3,
		};

		auto it = indices.find(type);

		if (it == indices.end()) return{};

		return it->second;
	}

	/**
	* \brief Получить параметры материалов по умолчанию
	* \param type Тип материала
	* \return Структура с параметрами
	*/
	MaterialSettings defaults::GetMaterialSetings(DefaultMaterialType type)
	{
		switch (type)
		{
		case ogl::defaults::DefaultMaterialType::GOLD:
			return{
				{ 0.24725f, 0.1995f, 0.0745f },
				{ 0.75164f, 0.60648f,0.22648f },
				{ 0.628281, 0.555802f,0.366065f },
				0.4 * 128.0f
			};
		case ogl::defaults::DefaultMaterialType::CHROME:
			return{
				{ 0.25f, 0.25f, 0.25f },
				{ 0.4f, 0.4f, 0.4f },
				{ 0.774597f, 0.774597f, 0.774597f },
				0.6 * 128.0f
			};
		case ogl::defaults::DefaultMaterialType::BRONZE:
			return{
				{ 0.2125f, 0.1275f, 0.054f },
				{ 0.714f, 0.4284f, 0.18144f },
				{ 0.393548f, 0.271906f, 0.166721f },
				0.2 * 128.0f
			};
		case ogl::defaults::DefaultMaterialType::CYAN_PLASTIC:
			return{
				{ 0.0f, 0.1f, 0.06f },
				{ 0.0f, 0.50980392f, 0.50980392f },
				{ 0.50196078f, 0.50196078f, 0.50196078 },
				0.25 * 128.0f
			};
		case ogl::defaults::DefaultMaterialType::YELLOW_RUBBER:
			return{
				{ 0.05f, 0.05f, 0.0f },
				{ 0.5f, 0.5f, 0.4f },
				{ 0.7f, 0.7f, 0.04f },
				0.078125f * 128.0f
			};
		default:
			return{
				{ 0.05f, 0.05f, 0.05f },
				{ 0.8f, 0.8f, 0.8f },
				{ 0.4f, 0.4f, 0.4f },
				16.0f
			};
		}
	}

	/**
	* \brief Получить код шейдеров по умолчанию
	* \param type Тип шейдера
	* \return Строка с кодом шейдера
	*/
	std::string defaults::GetShaderSource(DefaultShaderType type)
	{
		switch (type)
		{
		case ogl::defaults::DefaultShaderType::SOLID_COLORED:
		default:
			return 		
				"/*VERTEX-SHADER-BEGIN*/\n"
				"#version 330 core\n"
				"layout (location = 0) in vec3 position;\n"
				"layout (location = 1) in vec3 color;\n"
				"layout (location = 2) in vec2 textCoords;\n"
				"uniform mat4 model;\n"
				"uniform mat4 view;\n"
				"uniform mat4 projection;\n"
				"void main(){gl_Position = projection * view * model * vec4(position, 1.0);}\n"
				"/*VERTEX-SHADER-END*/\n"
				"/*FRAGMENT-SHADER-BEGIN*/\n"
				"#version 330 core\n"
				"out vec4 color;\n"
				"uniform vec3 lightColor;\n"
				"void main(){color = vec4(lightColor, 1.0f);}\n"
				"/*FRAGMENT-SHADER-END*/\n";
		case ogl::defaults::DefaultShaderType::SKYBOX:
			return
				"/*VERTEX-SHADER-BEGIN*/\n"
				"#version 330 core\n"
				"layout (location = 0) in vec3 aPos;\n"
				"out vec3 TexCoords;\n"
				"uniform mat4 view;\n"
				"uniform mat4 projection;\n"
				"void main(){TexCoords = aPos; gl_Position = projection * view * vec4(aPos, 1.0);}\n"
				"/*VERTEX-SHADER-END*/\n"
				"/*FRAGMENT-SHADER-BEGIN*/\n"
				"#version 330 core\n"
				"in vec3 TexCoords;\n"
				"out vec4 FragColor;\n"
				"uniform samplerCube skybox;\n"
				"void main(){FragColor = texture(skybox, TexCoords);}\n"
				"/*FRAGMENT-SHADER-END*/\n";
		case ogl::defaults::DefaultShaderType::LIGHT_SHADOW_MAP:
			return
				"/*VERTEX-SHADER-BEGIN*/\n"
				"#version 330 core\n"
				"layout (location = 0) in vec3 position;\n"
				"uniform mat4 model;\n"
				"uniform mat4 view;\n"
				"uniform mat4 projection;\n"
				"void main(){gl_Position = projection * view * model * vec4(position, 1.0);}\n"
				"/*VERTEX-SHADER-END*/\n"
				"/*FRAGMENT-SHADER-BEGIN*/\n"
				"#version 330 core\n"
				"void main(){gl_FragDepth = gl_FragCoord.z;}\n"
				"/*FRAGMENT-SHADER-END*/\n";
		}
	}
}
