#include "TextureResource.h"
#include <stdexcept>

namespace ogl
{
	/**
	* \brief Проинициализирован ли GLEW
	*/
	extern bool _isGlewInitialised;

	/**
	* \brief Конструктор
	* \param textureData Байты текстуры (массив цветов)
	* \param width Ширина
	* \param height Высота
	* \param bpp Байт на пиксель
	* \param generateMipmaps Автоматически генерировать мип-мапы
	* \param type Тип данных для инициализации текстуры (GL_UNSIGNED_BYTE, GL_FLOAT)
	*/
	TextureResource::TextureResource(void* textureData, GLuint width, GLuint height, GLuint bpp, bool generateMipmaps, GLuint type) :
		width_(width),
		height_(height),
		bpp_(bpp),
		mipmaps_(generateMipmaps)
	{
		// Инициализация GLEW
		if (!_isGlewInitialised) {
			glewExperimental = GL_TRUE;
			_isGlewInitialised = glewInit() == GLEW_OK;
		}

		if (!_isGlewInitialised) {
			throw std::runtime_error("OpenGL:TextureResource: Glew is not initialised");
		}

		// Генерация идентификатора текстуры
		glGenTextures(1, &(this->id_));
		// Привязываемся к текстуре по идентификатору (работаем с текустурой)
		glBindTexture(GL_TEXTURE_2D, this->id_);

		// Фильтр при уменьшении (когда один пиксель вмещает несколько текслей текстуры, то есть маленькие объекты)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->mipmaps_ ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		// Когда на один тексель текстуры приходится несколько пикселей (объекты рассматриваются вблизи)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Определить подходящий вормат (так-себе подход)
		GLuint format;
		switch (this->bpp_)
		{
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			format = GL_RGB;
		}

		// Устанавливаем данные тексткры (загрузка в текстурную память)
		glTexImage2D(GL_TEXTURE_2D, 0, format, this->width_, this->height_, 0, format, type, textureData);

		// Генерация мип-мапов (если нужно)
		if (this->mipmaps_) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		// Отвязка от текстуры (настроили и прекращаем работать с текстурой)
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	/**
	* \brief Деструктор, уничтожает OpenGL буфер
	*/
	TextureResource::~TextureResource()
	{
		glDeleteTextures(1, &id_);
	}

	/**
	* \brief Получить OpenGL ID текстуры
	* \return Числовой идентификатор
	*/
	GLuint TextureResource::getId() const
	{
		return this->id_;
	}

	/**
	* \brief Получить ширину
	* \return Целое число
	*/
	GLuint TextureResource::getWidth() const
	{
		return this->width_;
	}

	/**
	* \brief Получить высоту
	* \return Целое число
	*/
	GLuint TextureResource::getHeight() const
	{
		return this->height_;
	}

	/**
	* \brief Получить кол-во байт на пиксель
	* \return Целое число
	*/
	GLuint TextureResource::getBpp() const
	{
		return this->bpp_;
	}

	/**
	* \brief Создание ресурса
	* \param textureData Байты текстуры (массив цветов)
	* \param width Ширина
	* \param height Высота
	* \param bpp Байт на пиксель
	* \param generateMipmaps Автоматически генерировать мип-мапы
	* \param type Тип данных для инициализации текстуры (GL_UNSIGNED_BYTE, GL_FLOAT)
	* \return Умный указатель на ресурс
	*/
	TextureResourcePtr MakeTextureResource(void* textureData, GLuint width, GLuint height, GLuint bpp, bool generateMipmaps, GLuint type)
	{
		return std::make_shared<TextureResource>(textureData, width, height, bpp, generateMipmaps, type);
	}
}