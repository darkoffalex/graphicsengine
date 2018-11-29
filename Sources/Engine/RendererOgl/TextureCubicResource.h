#pragma once

#include <gl/glew.h>
#include <memory>
#include <vector>

namespace ogl
{
	/**
	 * \brief Структура для инициализации
	 * \details Описывает сторону куба при инициализациии
	 */
	struct TextureCubicInitFace
	{
		void* textureData;
		GLint width; 
		GLint height; 
		GLint bpp;
	};

	/**
	 * \brief Параметры текстуры соответствующей стороне куба
	 */
	struct TextureCubicFace
	{
		GLuint width;
		GLuint height;
		GLuint bpp;
	};

	/**
	 * \brief Ресурс текстурных данных (для кубической текстуры)
	 * \details Хранит идентификатор текстурного буфера. Может быть использован многократно, но не копируется
	 */
	class TextureCubicResource
	{

	private:
		GLuint id_;                            // OpenGL идентфикатор текстуры
		std::vector<TextureCubicFace> faces_;  // Стороны кубической текстуры (6 сторон куба)
		bool mipmaps_;                         // Используется ли авто-генерация мип-мапов

		/**
		 * \brief Запрет копирования через инициализацию
		 * \param other Ссылка на копируемый объекта
		 */
		TextureCubicResource(const TextureCubicResource& other) = delete;

		/**
		 * \brief Запрект копирования через присваивание
		 * \param other Ссылка на копируемый объекта
		 */
		void TextureCubicResource::operator=(const TextureCubicResource& other) = delete;

	public:

		/**
		 * \brief Конструктор
		 * \param facesData Данные для каждой стороны куба
		 * \param generateMipmaps Генерировать мип-мапы
		 */
		TextureCubicResource(const std::vector<TextureCubicInitFace>& facesData, bool generateMipmaps = false);

		/**
		 * \brief Деструктор
		 */
		~TextureCubicResource();

		/**
		 * \brief Получить OpenGL ID кубической текстуры
		 * \return Числовой идентификатор
		 */
		GLuint getId() const;

		/**
		 * \brief Получить ширину
		 * \param faceIndex Номер грани куба
		 * \return Целое число
		 */
		GLuint getWidth(GLuint faceIndex) const;

		/**
		 * \brief Получить высоту
		 * \param faceIndex Номер грани куба
		 * \return Целое число
		 */
		GLuint getHeight(GLuint faceIndex) const;

		/**
		 * \brief Получить кол-во байт на пиксель
		 * \param faceIndex Номер грани куба
		 * \return Целое число
		 */
		GLuint getBpp(GLuint faceIndex) const;
	};

	/**
	 * \brief Тип для умного указателя на ресурс
	 * \details Ресурс лучше не создавать как объект, предпочтительнее использовать функцию создания ресурса и данный тип
	 */
	typedef std::shared_ptr<TextureCubicResource> TextureCubicResourcePtr;

	/**
	 * \brief Создание ресурса
	 * \param facesData Данные для каждой стороны куба
	 * \param generateMipmaps Генерировать мип-мапы
	 * \return Умный указатель на ресурс
	 */
	TextureCubicResourcePtr MakeTextureCubicResource(const std::vector<TextureCubicInitFace>& facesData, bool generateMipmaps = false);
}