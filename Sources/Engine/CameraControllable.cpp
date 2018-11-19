#include "CameraControllable.h"

/**
* \brief Конструктор
* \param speed Скорость
* \param sensitivity Чувствительность
* \param aspectRatio Пропорции экрана
* \param zNear Ближняя грань отсечения
* \param zFar Дальняя гранб отсечения
* \param fov Угол обзора
*/
CameraControllable::CameraControllable(float speed, float sensitivity, float aspectRatio, float zNear, float zFar, float fov) :
	CameraBase(aspectRatio, zNear, zFar, fov),
	speed_(speed),
	sensitivity_(sensitivity)
{
	this->movement = { 0.0f,0.0f,0.0f };
}

/**
* \brief Установить скорость перемещения
* \param speed Скорость (ед/с)
*/
void CameraControllable::setSpeed(float speed)
{
	this->speed_ = speed;
}

/**
* \brief Получить скорость перемещения
* \return Скорость (ед/с)
*/
float CameraControllable::getSpeed() const
{
	return this->speed_;
}

/**
* \brief Установить чувствительность
* \param sensitivity Чувствительность
*/
void CameraControllable::setSensitivity(float sensitivity)
{
	this->sensitivity_ = sensitivity;
}

/**
* \brief Получить чувствительность
* \return Чувствительность
*/
float CameraControllable::getSensitivity() const
{
	return this->sensitivity_;
}

/**
* \brief Обновить положение в пространстве
* \details Приращение значений положения с учетом скорости и времени
* \param frameDeltaMs Время которое пршло за 1 кадр
*/
void CameraControllable::updatePositions(float frameDeltaMs)
{
	// Если камера двигается
	if(this->movement.x != 0 || this->movement.y != 0 || this->movement.z != 0)
	{
		// Сложить движения в каждом направлении
		glm::vec3 mov = (this->getDirection() * this->movement.z) + (this->getRight() * this->movement.x) + (glm::vec3(0.0f, 1.0f, 0.0f) * this->movement.y);
		// Получить приращение положения с учетом скорости
		mov *= (this->speed_ / 1000.0f * frameDeltaMs);
		// Прирастить
		this->position += mov;
	}

}
