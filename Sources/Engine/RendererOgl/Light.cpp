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
		this->shadows = false;
		this->renderScale = 1.0f;
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
	* \param additionalScale Дополнительный множитель размера
	* \return Матрица
	*/
	glm::mat4 Light::getModelMatrix(float additionalScale) const
	{
		// Масштабирование зависит от типа источника (пока что так)
		glm::vec3 scaleVector = this->type_ == LightType::POINT_LIGHT ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(1.0f, 1.0f, 2.0f);
		// Сформировать матрицу масштабирования с учетом доп. параметров масштаба
		glm::mat4 scale = glm::scale(glm::mat4(1), scaleVector * this->renderScale * additionalScale);
		// Матрица модели (учитывает масштабирование, поворот, сдвиг)
		return this->getTranslationMatrix() * this->getRotationMatrix4x4() * scale;
	}

	glm::mat4 Light::getViewMatrix() const
	{
		// TODO: Используя подобный подход невозможно сделать камеру поворачивающуюся 
		// TODO: вокруг оси Z. Добавление угла по этой оси ведет лишь к искажению поворотов
		// TODO: по остальным углам (из-за того что угол участвует лишь в формирвоании направления,
		// TODO: пока вектор UP указывает на абсолютный верх). Нужно устранить это дерьмо

		return glm::lookAt(this->position, this->position + this->getDirection(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::mat4 Light::getProjectionMatrix(GLfloat zNear, GLfloat zFar) const
	{
		if(this->type_ == LightType::DIRECTIONAL_LIGHT)
		{
			return glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, zNear, zFar);
		}

		return glm::perspective(90.0f, 1.0f, zNear, zFar);
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