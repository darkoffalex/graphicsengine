#include "Light.h"
#include <glm/gtc/matrix_transform.hpp>

namespace ogl
{
	/**
	 * \brief Конструктор
	 * \param type Тип источника
	 * \param vPosition Положение
	 * \param vRotation Вращение
	 * \param vColor Цвет
	 */
	Light::Light(LightType type, glm::vec3 vPosition, glm::vec3 vRotation, glm::vec3 vColor) :
		type_(type),
		position(vPosition),
		rotation(vRotation),
		color(vColor)
	{
		this->render = true;
		this->attenuation.linear = 0.22f;
		this->attenuation.quadratic = 0.20f;
		this->cutOffAngle = 40.0f;
		this->cutOffOuterAngle = 45.0f;
	}

	/**
	 * \brief Получить матрицу вращения
	 * \return Матрица
	 */
	glm::mat4 Light::getRotationMatrix4x4() const
	{
		glm::mat4 result(1);
		result = glm::rotate(result, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		result = glm::rotate(result, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		result = glm::rotate(result, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		return result;
	}

	/**
	 * \brief Получить матрицу сдвига
	 * \return Матрица
	 */
	glm::mat4 Light::getTranslationMatrix() const
	{
		glm::mat4 result(1);
		result = glm::translate(result, this->position);

		return result;
	}

	/**
	 * \brief Получить матрицу модели
	 * \return Матрица
	 */
	glm::mat4 Light::getModelMatrix() const
	{
		glm::mat4 scale(1);

		if (this->type_ == LightType::DIRECTIONAL_LIGHT || this->type_ == LightType::SPOT_LIGHT) {
			scale = glm::scale(scale, glm::vec3(1.0f, 1.0f, 2.0f));
		}

		return this->getTranslationMatrix() * this->getRotationMatrix4x4() * scale;
	}

	/**
	 * \brief Получить тип
	 * \return Идентификатор типа
	 */
	LightType Light::getType() const
	{
		return this->type_;
	}

	/**
	 * \brief Получить направление
	 * \return Направление
	 */
	glm::vec3 Light::getDirection() const
	{
		return this->getRotationMatrix4x4() * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
	}
}