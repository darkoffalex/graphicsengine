#pragma once

#include <Windows.h>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderResource.h"
#include "TextureResource.h"
#include "StaticMesh.h"
#include "Light.h"

#define MAX_POINT_LIGHTS 32
#define MAX_DIRECT_LIGHTS 32
#define MAX_SPOT_LIGHTS 32

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

		// Б У Ф Е Р Ы  К А Д Р А

		/**
		 * \brief Идентификатор G-буфера и его вложений
		 * \details В первом проходе рендеринг сцены будет осуществляться в G-буфер, без освещения
		 */
		struct {
			GLuint gBufferId;                // ID буфера
			GLuint gPositionAttachmentId;    // Позиции фрагментов в 3D пространстве
			GLuint gNormalAttachmentId;      // Нормали фрагментов
			GLuint gAlbedoSpecAttachmentId;  // Цвет и интенсивность отражения
			GLuint depthStencilAttachmentId; // Значения глубины-трафарета (для тестов глубины и трафарета)
			struct { GLuint width; GLuint height; } sizes; // Размеры буфера
		} gBuffer_;

		/**
		 * \brief Идентификатор кадрового буфера и его вложений
		 * \details Во втором проходе будет осуществляться рендеринг в кадровый буфер, с учетом данных их G-буфера (для освещения)
		 */
		struct{
			GLuint frameBufferId;
			GLuint colorAttachmentId;
			GLuint depthStencilAttachmentId;
			struct { GLuint width; GLuint height; } sizes; 
		} frameBuffer_;

		// Ш Е Й Д Е Р Ы

		/**
		 * \brief Шейдеры
		 * \details Перечень основных шейдеров используемых при рендеринге, некоторые задаются, некоторые предустановлены
		 */
		struct {
			ShaderResourcePtr shaderGBuffer_;
			ShaderResourcePtr shaderLighting_;
			ShaderResourcePtr shaderPostProcessing_;
			ShaderResourcePtr shaderSolidColor_;
			ShaderResourcePtr shaderShadowVolumes_;
		} shaders_;

		// Г Е О М Е Т Р И Я  П О  У М О Л Ч А Н И Ю

		/**
		 * \brief Геометрия по умолчанию
		 * \details Используется для визуализации различных системных объектов (напрример, квадрат, для пост-обработки и т.п.)
		 */
		struct{
			StaticGeometryResourcePtr quad;
			StaticGeometryResourcePtr cube;
		} defaultGeometry_;

		// Т Е К С Т У Р Ы  П О  У М О Л Ч А Н Ю

		/**
		 * \brief Текстурные ресурсы по умолчанию
		 * \details Если у рисуемых объектов не будет текстуры, они будут взяты отсюда (это однопиксельные текстуры)
		 */
		struct{
			TextureResourcePtr diffuse;  // Белый пиксель
			TextureResourcePtr specular; // Балый пиксель
			TextureResourcePtr bump;     // Синеватный (фиолетовый) пиксель
			TextureResourcePtr displace; // Белый пиксель
		} defaultTextures_;

		// О Б Ъ Е К Т Ы  Д Л Я  В И З У А Л И З А Ц И И

		std::vector<StaticMeshPtr> staticMeshes_;  // Массив статических мешей (указателей)
		std::vector<LightPtr> lights_;             // Массив источников света (указателей)

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		 * \brief Инициализация G-буффера
		 * \param width Ширина буфера
		 * \param height Высота буфера
		 */
		void initGBuffer(GLuint width, GLuint height);

		/**
		 * \brief Очистка G-буфера
		 */
		void freeGBuffer();

		/**
		 * \brief Инициализация фрейм-буфера
		 * \param width Ширина буфера
		 * \param height Высота буфера
		 */
		void initFrameBuffer(GLuint width, GLuint height);

		/**
		 * \brief Очистка фрейм-буфера
		 */
		void freeFrameBuffer();

		/**
		 * \brief Проход для рендеринга геометрии (рендеринг в G-буфер)
		 * \param shaderID шейдер для рендеринга в G-буфер
		 * \param clearColor Цвет очистки
		 * \param clearMask Маска очистки
		 */
		void renderPassGeometry(GLuint shaderID, glm::vec4 clearColor, GLbitfield clearMask);

		/**
		 * \brief Проход для построения теневых объемов и записи инаформации о тени в stencil-буфер
		 * \param light Источник освещения
		 * \param shaderID Шейдер для построения теневых объемов
		 */
		void renderPassShadows(LightPtr light, GLuint shaderID);

		/**
		 * \brief Проход рендеринга освещенности (один источником света)
		 * \details Данный метод вызывается многократно (для нескольких источников), результаты буфера суммируются
		 * \param light Источник света
		 * \param shaderID Шейдер для рендеринга освещения
		 * \param cameraPosition Положение камеры
		 * \param clearColor Цвет очистки
		 * \param clearMask Маска очистки
		 * \param clear Очистить
		 */
		void renderPassLighting(LightPtr light, GLuint shaderID, const glm::vec3& cameraPosition, glm::vec4 clearColor, GLbitfield clearMask, bool clear = false) const;

		/**
		 * \brief Проход для рендеринга системных объектов (напр. источники света)
		 * \param shaderID 
		 */
		void renderPassSysObjects(GLuint shaderID) const;

		/**
		 * \brief Проход рендеринга для финального представления (рендеринг в основной буфер)
		 * \param shaderID шейдер для рендеринга во фрейм-буфер
		 * \param clearColor Цвет очистки
		 * \param clearMask Маска очистки
		 */
		void renderPassFinal(GLuint shaderID, glm::vec4 clearColor, GLbitfield clearMask) const;

		/**
		 * \brief Передать в шейдер структуру маппинга текстуры
		 * \param shaderId ID шейдера
		 * \param mapping Структура маппинга
		 * \param uniformName Наименование uniform переменной
		 */
		void texMappingToShader(GLuint shaderId, const TextureMapping& mapping, std::string uniformName) const;
		
	public:
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
		 * \param geometry Шейдер для рендеринга геометрии
		 * \param lightning Шейдер для подсчета освещенности
		 * \param postProcessing Шейдер для пост-обработки
		 */
		Renderer(HWND hwnd, ShaderResourcePtr geometry, ShaderResourcePtr lightning, ShaderResourcePtr postProcessing, ShaderResourcePtr shadows);

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
