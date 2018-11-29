#include "TextureCubicResource.h"
#include <stdexcept>

namespace ogl
{
	/**
	* \brief Проинициализирован ли GLEW
	*/
	extern bool _isGlewInitialised;

	/**
	 * \brief Конструктор
	 * \param facesData Данные для каждой стороны куба
	 * \param generateMipmaps Генерировать мип-мапы
	 */
	TextureCubicResource::TextureCubicResource(const std::vector<TextureCubicInitFace>& facesData, bool generateMipmaps)
		:mipmaps_(generateMipmaps)
	{
		// Инициализация GLEW
		if (!_isGlewInitialised) {
			glewExperimental = GL_TRUE;
			_isGlewInitialised = glewInit() == GLEW_OK;
		}

		if (!_isGlewInitialised) {
			throw std::runtime_error("OpenGL:TextureCubicResource: Glew is not initialised");
		}

		if(facesData.size() < 6){
			throw std::runtime_error("OpenGL:TextureCubicResource: Wrong data provided. Cubic texture has 6 faces");
		}

		// Генерация идентификатора текстуры
		glGenTextures(1, &id_);
		// Привязываемся к текстуре по идентификатору (работаем с текустурой)
		glBindTexture(GL_TEXTURE_CUBE_MAP, id_);

		// Фильтр при уменьшении (когда один пиксель вмещает несколько текслей текстуры, то есть маленькие объекты)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, this->mipmaps_ ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		// Когда на один тексель текстуры приходится несколько пикселей (объекты рассматриваются вблизи)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Врапинг текстуры по умолчанию (растягивать до краев)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// Пройтись по полигонам
		for(unsigned int i = 0; i < 6; i++)
		{
			// Сохранить параметры текстуры
			this->faces_.push_back({
				static_cast<GLuint>(facesData[i].width),
				static_cast<GLuint>(facesData[i].height),
				static_cast<GLuint>(facesData[i].bpp)
			});

			// Определить формат текстуры грани куба
			GLuint format;
			switch (facesData[i].bpp)
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

			// Загрузить данные для каждой грани куба
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, format, facesData[i].width, facesData[i].height, 0, format, GL_UNSIGNED_BYTE, facesData[i].textureData
			);
		}

		// Генерация мип-мапов (если нужно)
		if (this->mipmaps_) {
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		}

		// Отвязка от текстуры (настроили и прекращаем работать с текстурой)
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	/**
	 * \brief Деструктор, уничтожает OpenGL буфер
	 */
	TextureCubicResource::~TextureCubicResource()
	{
		glDeleteTextures(1, &id_);
	}

	/**
	 * \brief Получить OpenGL ID кубической текстуры
	 * \return Числовой идентификатор
	 */
	GLuint TextureCubicResource::getId() const
	{
		return this->id_;
	}

	/**
	 * \brief Получить ширину
	 * \param faceIndex Номер грани куба
	 * \return Целое число
	 */
	GLuint TextureCubicResource::getWidth(GLuint faceIndex) const
	{
		return this->faces_[faceIndex].width;
	}

	/**
	 * \brief Получить высоту
	 * \param faceIndex Номер грани куба
	 * \return Целое число
	 */
	GLuint TextureCubicResource::getHeight(GLuint faceIndex) const
	{
		return this->faces_[faceIndex].height;
	}

	/**
	 * \brief Получить кол-во байт на пиксель
	 * \param faceIndex Номер грани куба
	 * \return Целое число
	 */
	GLuint TextureCubicResource::getBpp(GLuint faceIndex) const
	{
		return this->faces_[faceIndex].bpp;
	}

	/**
	 * \brief Создание ресурса
	 * \param facesData Данные для каждой стороны куба
	 * \param generateMipmaps Генерировать мип-мапы
	 * \return Умный указатель на ресурс
	 */
	TextureCubicResourcePtr MakeTextureCubicResource(const std::vector<TextureCubicInitFace>& facesData, bool generateMipmaps)
	{
		return std::make_shared<TextureCubicResource>(facesData, generateMipmaps);
	}
}
