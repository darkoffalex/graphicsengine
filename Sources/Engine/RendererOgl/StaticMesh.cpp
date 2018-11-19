#include "StaticMesh.h"
#include <glm/gtc/matrix_transform.hpp>

namespace ogl
{
	/**
	* \brief Конструктор
	* \param part Первая часть статического меша
	*/
	StaticMesh::StaticMesh(StaticMeshPart part) :
		isRendering(true),
		origin(glm::vec3(0.0f, 0.0f, 0.0f)),
		rotation(glm::vec3(0.0f, 0.0f, 0.0f)),
		position(glm::vec3(0.0f, 0.0f, 0.0f)),
		scale(glm::vec3(1.0f, 1.0f, 1.0f))
	{
		this->parts_.push_back(part);
	}

	/**
	* \brief Конструктор
	* \param parts Части статического меша (массив)
	*/
	StaticMesh::StaticMesh(const std::vector<StaticMeshPart>& parts) :
		isRendering(true),
		origin(glm::vec3(0.0f, 0.0f, 0.0f)),
		rotation(glm::vec3(0.0f, 0.0f, 0.0f)),
		position(glm::vec3(0.0f, 0.0f, 0.0f)),
		scale(glm::vec3(1.0f, 1.0f, 1.0f))
	{
		this->parts_ = parts;
	}

	/**
	* \brief Получить части
	* \return Массив частей меша
	*/
	std::vector<StaticMeshPart>& StaticMesh::getParts()
	{
		return this->parts_;
	}

	/**
	* \brief Получить матрицу вращения 4*4
	* \return Матрица
	*/
	glm::mat4 StaticMesh::getRotationMatrix4x4() const
	{
		glm::mat4 result(1);
		result = glm::rotate(result, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		result = glm::rotate(result, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		result = glm::rotate(result, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		return result;
	}

	/**
	* \brief Получить матрицу вращения 3*3
	* \return Матрица
	*/
	glm::mat3 StaticMesh::getRotationMatrix3x3() const
	{
		glm::mat3 result(1);
		//TODO: получить матрицу поворота 3*3
		return result;
	}

	/**
	* \brief Получить матрицу сдвига
	* \return Матрица
	*/
	glm::mat4 StaticMesh::getTranslationMatrix() const
	{
		glm::mat4 result(1);
		result = glm::translate(result, this->position);
		return result;
	}

	/**
	* \brief Получить матрицу масштаба
	* \return Матрица
	*/
	glm::mat4 StaticMesh::getScaleMatrix() const
	{
		glm::mat4 result(1);
		result = glm::scale(result, this->scale);
		return result;
	}

	/**
	* \brief Получить матрицу модели
	* \details Результат перемножения всех матриц с учетом локального центра
	* \return Матрица
	*/
	glm::mat4 StaticMesh::getModelMatrix() const
	{
		return glm::translate(glm::mat4(1), this->origin) * 
			this->getTranslationMatrix() * 
			this->getRotationMatrix4x4() * 
			this->getScaleMatrix() * 
			glm::translate(glm::mat4(1), -this->origin);
	}
}
