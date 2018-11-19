#pragma once

#include <gl/glew.h>
#include <vector>
#include <map>
#include <memory>

namespace ogl
{
	/**
	 * \brief Ресурс шейдерной программы
	 * \details Производит компиляцию шейдеров и сборку программы во время инициализации. Не копируемый
	 */
	class ShaderResource
	{

	private:
		GLuint id_;  // Идентификатор шейдерной программы

		/**
		 * \brief Внутренний метод разбития строки на под-строки
		 * \param str Исходная строка
		 * \param delimiter Символ разделяющий строку
		 * \details Используется во время парсинга шейдерного файла
		 * \return Массив строк
		 */
		static std::vector<std::string> explodeString(const std::string& str, char delimiter);

		/**
		 * \brief Метод распределения шейдерного кода по ассоциативному массиву
		 * \param shaders Указатель на ассоциативный массив содержащий строки с кодами шейдеров
		 * \param source Исходная строка с общим кодом
		 * \details Поскольку все шейдеры (вершинный, фрагментный и прочие) находятся в одном файле, его нужно разбить
		 */
		static void splitCodeToMap(std::map<GLuint, std::string>* shaders, const std::string& source);

		/**
		 * \brief Компиляция шейдера
		 * \param shaderSource Исходный код шейдера
		 * \param type Тип шейдера
		 * \return Идентификатор шейдера
		 */
		static GLuint compileShader(const char * shaderSource, GLuint type);

		/**
		 * \brief Запрет копирования через инициализацию
		 * \param other Ссылка на копируемый объекта
		 */
		ShaderResource(const ShaderResource& other) = delete;

		/**
		 * \brief Запрект копирования через присваивание
		 * \param other Ссылка на копируемый объекта
		 */
		void ShaderResource::operator=(const ShaderResource& other) = delete;

	public:

		/**
		 * \brief Создать шейдерную программу
		 * \param source Исходный код шейдеров
		 */
		ShaderResource(const std::string& source);

		/**
		 * \brief Уничтожить шейдерную программу
		 */
		~ShaderResource();

		/**
		 * \brief Получть ID шейдерной программы
		 * \return Числовой идентификатор
		 */
		GLuint getId() const;
	};

	/**
	 * \brief Тип для умного указателя на ресурс
	 * \details Ресурс лучше не создавать как объект, предпочтительнее использовать функцию создания ресурса и данный тип
	 */
	typedef std::shared_ptr<ShaderResource> ShaderResourcePtr;

	/**
	 * \brief Создать ресурс шейдерной программы
	 * \param source Исходный код шейдеров
	 * \return Умный указатель на ресурс
	 */
	ShaderResourcePtr MakeShaderResource(const std::string& source);
}
