#pragma once

#include <Windows.h>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderResource.h"
#include "TextureResource.h"
#include "TextureCubicResource.h"
#include "StaticMesh.h"
#include "Light.h"

namespace ogl
{
	/**
	 * \brief Объекты данного класса осуществляют рендеринг
	 * \details Рендерер получает данные об объектах и источниках света, а затем визуализирует сцену
	 */
	class Renderer
	{
	private:
		HWND hwnd_;                          // Хендл WinAPI окна
		glm::mat4 viewMatrix_;               // Матрица вида
		glm::mat4 projectionMatrix_;         // Матрица проекции

		ShaderResourcePtr shaderSolidColor_; // Шейдер для однотонных объектов
		ShaderResourcePtr shaderSkybox_;     // Шейдер для рендеринга скайбокса
		ShaderResourcePtr shaderBasic_;      // Шейдер для основного освещения
		ShaderResourcePtr shaderPostProc_;   // Шейдер для пост-обработки

		struct{
			GLuint fboId;
			GLuint depthStencilAttachmentId;
			GLuint colorAttachmentId;
			glm::ivec2 sizes;
		} frameBuffer_;

		/**
		 * \brief Текстурные ресурсы по умолчанию
		 * \details Если у рисуемых объектов не будет текстуры, они будут взяты отсюда (это однопиксельные текстуры)
		 */
		struct{
			TextureResourcePtr diffuse;
			TextureResourcePtr detail;
			TextureResourcePtr specular;
			TextureResourcePtr bump;
			TextureResourcePtr flashLightTexture;
		} defaultTextures_;


		/**
		 * \brief Геометрия по умолчанию
		 * \details Используется для визуализации различных объектов (напр. источники света). В дальнейшем будет убрано
		 */
		struct{
			StaticGeometryResourcePtr cube;
			StaticGeometryResourcePtr cilinder;
			StaticGeometryResourcePtr sphere;
			StaticGeometryResourcePtr quad;
			StaticGeometryResourcePtr skybox;
		} defaultGeometry_;

		std::vector<StaticMeshPtr> staticMeshes_;  // Массив статических мешей (указателей)
		std::vector<LightPtr> lights_;             // Массив источников света (указателей)

		/**
		* \brief Запрет копирования через инициализацию
		* \param other Ссылка на копируемый объекта
		*/
		Renderer(const Renderer& other) = delete;

		/**
		* \brief Запрект копирования через присваивание
		* \param other Ссылка на копируемый объекта
		*/
		void Renderer::operator=(const Renderer& other) = delete;

		/**
		 * \brief Инициализация кадрового буфера
		 * \param width Ширина буфера
		 * \param height Высота буфера
		 */
		void initFrameBuffer(GLuint width, GLuint height);

		/**
		 * \brief Де-инициализация кадрового буффера
		 */
		void freeFrameBuffer();

	public:
		/**
		 * \brief Текстура пятен фонарика
		 */
		TextureResourcePtr flashLightTexture;

		/**
		 * \brief Текстура фонового скай-бокса
		 */
		TextureCubicResourcePtr backgroundTexture;

		/**
		 * \brief Ширина и высота области вида
		 * \details Соответствует разрешению области показа, позволяет получит пропорцию
		 */
		struct {
			GLuint width;
			GLuint height;
			GLfloat getAspectRatio() const { return static_cast<GLfloat>(width) / static_cast<GLfloat>(height); }
		} viewPort;

		/**
		 * \brief Положение камеры
		 * \details Используется для вычисления бликов/отражений в шейдере
		 */
		glm::vec3 cameraPosition;

		/**
		 * \brief Конструктор
		 * \param hwnd Хендл WinAPI окна
		 * \param shaderBasic Основной шейдер
		 * \param shaderPostProcessing Шейдер для пост-обработки
		 */
		Renderer(HWND hwnd, ShaderResourcePtr shaderBasic, ShaderResourcePtr shaderPostProcessing);

		/**
		 * \brief Освобождение памяти
		 */
		~Renderer();

		/**
		 * \brief Установить матрицу вида
		 * \param matrix Матрица
		 */
		void setViewMatrix(const glm::mat4& matrix);

		/**
		 * \brief Установить матрицу проекции
		 * \param matrix Матрица
		 */
		void setProjectionMatrix(const glm::mat4& matrix);

		/**
		 * \brief Добавить статический меш в список
		 * \param mesh Объект статического меша
		 * \return Указатель на меш в списке
		 */
		StaticMeshPtr addStaticMesh(const StaticMesh& mesh);

		/**
		 * \brief Удаление статического меша из списка
		 * \param meshPtr Указатель на меш в списке
		 */
		void removeStaticMesh(StaticMeshPtr& meshPtr);

		/**
		 * \brief Добавить источник света
		 * \param light Объект источника света
		 * \return Указатель на источник в списке
		 */
		LightPtr addLight(const Light& light);

		/**
		 * \brief Удалить источник света из списка
		 * \param lightPtr Указатель на источник в списке
		 */
		void removeLight(LightPtr& lightPtr);

		/**
		 * \brief Получить массив статических мешей
		 * \return Ссылка на массив указателей
		 */
		std::vector<StaticMeshPtr>& getStaticMeshes();

		/**
		 * \brief Получить массив источников освещения
		 * \return Ссылка на массив указателей
		 */
		std::vector<LightPtr>& getLights();

		/**
		 * \brief Рисование кадра
		 * \param clearColor Цвет очистки кадра
		 * \param clearMask Параметры очистки
		 */
		void drawFrame(glm::vec4 clearColor = { 0.0f,0.0f,0.0f,1.0f }, GLbitfield clearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	};
}
