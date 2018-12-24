#pragma once

#include <glm/glm.hpp>

#include "TextureResource.h"
#include "ShaderResource.h"
#include "StaticGeometryResource.h"
#include "Types.h"

namespace ogl
{
	/**
	* \brief Параметры текстуры
	* \details Описывает масштаб, сдаиг, поворот
	*/
	struct TextureParameters
	{
		TextureResourcePtr resource; // Ресурс текстуры
		glm::vec2 offset;            // Сдвиг текстуры
		glm::vec2 scale;             // Масштаб текстуры
		GLfloat rotation;            // Поворот текстуры
		GLint wrapS;                 // Горизонтальный wrapping текстуры (GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER)
		GLint wrapT;                 // Вертикальный wrapping текстуры (GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER)

		/**
		 * \brief Получить матрицу вращения 2*2 на угол rotation
		 * \param leftHanded Левостороняя система
		 * \return Матрица поворота
		 */
		glm::mat2 getRotMatrix(bool leftHanded = false) const;
	};

	/**
	 * \brief Чать статического меша
	 * \details Статический меш должен состоять как минимум из одной части
	 */
	class StaticMeshPart
	{
	private:
		StaticGeometryResourcePtr geometry_;  // Ресурс геометрии

	public:
		MaterialSettings material;            // Параметры материала

		TextureParameters diffuseTexture;     // Параметры diffuse текстуры
		TextureParameters detailTexture;      // Параметры detail текстуры
		TextureParameters specularTexture;    // Параметры specular текстуры
		TextureParameters bumpTexture;        // Параметры bump текстуры

		/**
		 * \brief Конструктор
		 * \param geometry Ресурс геометрии
		 */
		StaticMeshPart(StaticGeometryResourcePtr geometry);

		/**
		 * \brief Деструктор
		 */
		~StaticMeshPart() = default;

		/**
		 * \brief Получить ресурс геометрии
		 * \return Умный указатель на ресурс
		 */
		StaticGeometryResourcePtr getGeometry() const;
	};
}