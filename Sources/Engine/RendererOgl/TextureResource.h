#pragma once

#include <gl/glew.h>
#include <memory>

namespace ogl
{
	/**
	 * \brief Ресурс текстурных данных
	 * \details Хранит идентификатор текстурного буфера. Может быть использован многократно, но не копируется
	 */
	class TextureResource
	{
	private:
		GLuint id_;     // OpenGL идентфикатор текстуры
		GLuint width_;  // Ширина
		GLuint height_; // Высота
		GLuint bpp_;    // Байт на пиксель
		bool mipmaps_;  // Используется ли авто-генерация мип-мапов

		/**
		* \brief Запрет копирования через инициализацию
		* \param other Ссылка на копируемый объекта
		*/
		TextureResource(const TextureResource& other) = delete;

		/**
		* \brief Запрект копирования через присваивание
		* \param other Ссылка на копируемый объекта
		*/
		void TextureResource::operator=(const TextureResource& other) = delete;

	public:

		/**
		 * \brief Конструктор
		 * \param textureData Байты текстуры (массив цветов) 
		 * \param width Ширина
		 * \param height Высота
		 * \param bpp Байт на пиксель
		 * \param generateMipmaps Автоматически генерировать мип-мапы
		 * \param type Тип данных для инициализации текстуры (GL_UNSIGNED_BYTE, GL_FLOAT)
		 */
		TextureResource(void* textureData, GLuint width, GLuint height, GLuint bpp, bool generateMipmaps = false, GLuint type = GL_UNSIGNED_BYTE);

		/**
		 * \brief Деструктор, уничтожает OpenGL буфер
		 */
		~TextureResource();

		/**
		 * \brief Получить OpenGL ID текстуры
		 * \return Числовой идентификатор
		 */
		GLuint getId() const;

		/**
		 * \brief Получить ширину
		 * \return Целое число
		 */
		GLuint getWidth() const;

		/**
		 * \brief Получить высоту
		 * \return Целое число
		 */
		GLuint getHeight() const;

		/**
		 * \brief Получить кол-во байт на пиксель
		 * \return Целое число
		 */
		GLuint getBpp() const;
	};

	/**
	 * \brief Тип для умного указателя на ресурс
	 * \details Ресурс лучше не создавать как объект, предпочтительнее использовать функцию создания ресурса и данный тип
	 */
	typedef std::shared_ptr<TextureResource> TextureResourcePtr;

	/**
	 * \brief Создание ресурса
	 * \param textureData Байты текстуры (массив цветов) 
	 * \param width Ширина
	 * \param height Высота
	 * \param bpp Байт на пиксель
	 * \param generateMipmaps Автоматически генерировать мип-мапы
	 * \param type Тип данных для инициализации текстуры (GL_UNSIGNED_BYTE, GL_UNSIGNED_FLOAT)
	 * \return Умный указатель на ресурс
	 */
	TextureResourcePtr MakeTextureResource(void* textureData, GLuint width, GLuint height, GLuint bpp, bool generateMipmaps = false, GLuint type = GL_UNSIGNED_BYTE);
}