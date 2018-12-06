#pragma once

#include <vector>
#include <string>

#include "Types.h"

namespace ogl
{
	namespace defaults
	{
		/**
		 * \brief Тип геометрии (для функций получения вершин и индексов)
		 */
		enum DefaultGeometryType
		{
			PLANE,
			CUBE,
			CUBE_SKYBOX,
			PYRAMID,
			SPHERE
		};

		/**
		 * \brief Тип материала по умолчанию
		 */
		enum DefaultMaterialType
		{
			DEFAULT,
			GOLD,
			CHROME,
			BRONZE,
			CYAN_PLASTIC,
			YELLOW_RUBBER
		};

		/**
		 * \brief Тип шейдера по умолчанию
		 */
		enum DefaultShaderType
		{
			SOLID_COLORED,
			FULL_BRIGHT_TEXTURED,
			SKYBOX,
			LIGHT_SHADOW_MAP,
		};

		/**
		 * \brief Получить вершины примитивов по умолчанию
		 * \param type Тип примитива
		 * \param size Размер примитива
		 * \return Массив вершин
		 */
		std::vector<Vertex> GetVertices(DefaultGeometryType type, float size = 1.0f);

		/**
		 * \brief Получить индексы примитивов по умолчанию
		 * \param type Тип примитива
		 * \return Массив индексов
		 */
		std::vector<GLuint> GetIndices(DefaultGeometryType type);

		/**
		 * \brief Получить параметры материалов по умолчанию
		 * \param type Тип материала
		 * \return Структура с параметрами
		 */
		MaterialSettings GetMaterialSetings(DefaultMaterialType type);

		/**
		 * \brief Получить код шейдеров по умолчанию
		 * \param type Тип шейдера
		 * \return Строка с кодом шейдера
		 */
		std::string GetShaderSource(DefaultShaderType type);
	}
}
