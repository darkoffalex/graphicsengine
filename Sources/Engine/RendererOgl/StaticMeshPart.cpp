#include "StaticMeshPart.h"
#include "Defaults.h"

#include <glm/gtc/matrix_transform.hpp>

namespace ogl
{
	/**
	* \brief Получить матрицу вращения 2*2 на угол rotation
	* \param leftHanded Левостороняя система
	* \return Матрица поворота
	*/
	glm::mat4 TextureParameters::getRotMatrix(bool leftHanded) const
	{
		glm::mat2 rot;
		rot[0][0] = glm::cos(glm::radians(this->rotation));
		rot[1][0] = glm::sin(glm::radians(this->rotation)) * (leftHanded ? 1 : -1);
		rot[0][1] = glm::sin(glm::radians(this->rotation)) * (leftHanded ? -1 : 1);
		rot[1][1] = glm::cos(glm::radians(this->rotation));
		return rot;
	}

	/**
	* \brief Конструктор
	* \param geometry Ресурс геометрии
	*/
	StaticMeshPart::StaticMeshPart(StaticGeometryResourcePtr geometry):geometry_(geometry)
	{
		// Параметры текстуры по умолчанию
		TextureParameters defaultParams = {
			nullptr,
			{ 0.0f,0.0f },
			{ 1.0f,1.0f },
			0.0f,
			GL_REPEAT,
			GL_REPEAT
		};

		// Назначить параметры текстуры по умолчанию для всх текстур
		this->diffuseTexture = defaultParams;
		this->detailTexture = defaultParams;
		this->specularTexture = defaultParams;
		this->bumpTexture = defaultParams;

		// Назначить параметры материала по умолчанию
		this->material = defaults::GetMaterialSetings(defaults::DefaultMaterialType::DEFAULT);
	}

	/**
	* \brief Получить ресурс геометрии
	* \return Умный указатель на ресурс
	*/
	StaticGeometryResourcePtr StaticMeshPart::getGeometry() const
	{
		return this->geometry_;
	}
}
