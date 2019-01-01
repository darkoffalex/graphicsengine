#include "CameraBase.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

/**
* \brief Конструктор
* \param aspectRatio Пропорции экрана
* \param zNear Ближняя грань отсечения
* \param zFar Дальняя грань отсечения
* \param fov Угол обзора
*/
CameraBase::CameraBase(float aspectRatio, float zNear, float zFar, float fov) :
	zNear_(zNear), zFar_(zFar), aspectRatio_(aspectRatio), FOV(fov)
{
	this->position = { 0.0f,0.0f,0.0f };
	this->rotation = { 0.0f,0.0f,0.0f };
}

/**
* \brief Установка пропорций экрана
* \param aspectRatio Пропорции (отношение ширины к длине)
*/
void CameraBase::setAspectRatio(float aspectRatio)
{
	this->aspectRatio_ = aspectRatio;
}

/**
* \brief Установить ближнюю грань (ближе этого расстояния все отбрасывается)
* \param zNear Ближняя грань отсечения
*/
void CameraBase::setZNear(float zNear)
{
	this->zNear_ = zNear;
}

/**
* \brief Установить дальнюю грань (дальше этого расстояния все отбрасывается)
* \param zFar Дальняя грань отсечения
*/
void CameraBase::setZFar(float zFar)
{
	this->zFar_ = zFar;
}

/**
* \brief Получить матрицу проекции
* \param coordSpaceType Тип пространства (право-ручное или лево-ручное)
* \details При помощи данной матрицы точки примитивов из 3Д-пространства проецируются на 2Д-экран (для дальнейшей растеризации)
* \return Матрица
*/
glm::mat4 CameraBase::getProjectionMatrix(CoordSpaceType coordSpaceType) const
{
	switch (coordSpaceType)
	{
	case CoordSpaceType::LEFT_HANDED:
		return glm::perspectiveLH(this->FOV, this->aspectRatio_, this->zNear_, this->zFar_);

	case CoordSpaceType::RIGHT_HANDED:
	default:
		return glm::perspective(this->FOV, this->aspectRatio_, this->zNear_, this->zFar_);
	}
}

/**
* \brief Получить направление камеры (куда смотрит камера)
* \return Вектор
*/
glm::vec3 CameraBase::getDirection() const
{
	// Получить матрицу вращения
	glm::mat4 rot(1);

	rot = glm::rotate(rot, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	rot = glm::rotate(rot, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rot = glm::rotate(rot, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

	// Вектор направления повернуть при помощи матрицы на нужные углы
	glm::vec4 direction = rot * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);

	return glm::normalize(glm::vec3(direction.x, direction.y, direction.z));
}

/**
* \brief Получить вектор-вправо от камеры
* \return Вектор
*/
glm::vec3 CameraBase::getRight() const
{
	return glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), this->getDirection()));
}

/**
* \brief Получить матрицу вида
* \param coordSpaceType Тип пространства (право-ручное или лево-ручное)
* \details При помощи данной матрицы координаты всех точек меняются так, как если бы центр мира был там, где камера
* \return Матрица
*/
glm::mat4 CameraBase::getViewMatrix(CoordSpaceType coordSpaceType) const
{
	// TODO: Используя подобный подход невозможно сделать камеру поворачивающуюся 
	// TODO: вокруг оси Z. Добавление угла по этой оси ведет лишь к искажению поворотов
	// TODO: по остальным углам (из-за того что угол участвует лишь в формирвоании направления,
	// TODO: пока вектор UP указывает на абсолютный верх). Нужно устранить это дерьмо

	switch (coordSpaceType)
	{
	case CoordSpaceType::LEFT_HANDED:
		return glm::lookAtLH(this->position, this->position + this->getDirection(), glm::vec3(0.0f, 1.0f, 0.0f));

	case CoordSpaceType::RIGHT_HANDED:
	default:
		return glm::lookAt(this->position, this->position + this->getDirection(), glm::vec3(0.0f, 1.0f, 0.0f));
	}
}
