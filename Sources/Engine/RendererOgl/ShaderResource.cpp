#include "ShaderResource.h"
#include <map>
#include <sstream>

namespace ogl
{
	/**
	* \brief Проинициализирован ли GLEW
	*/
	extern bool _isGlewInitialised;

	/**
	* \brief Внутренний метод разбития строки на под-строки
	* \param str Исходная строка
	* \param delimiter Символ разделяющий строку
	* \details Используется во время парсинга шейдерного файла
	* \return Массив строк
	*/
	std::vector<std::string> ShaderResource::explodeString(const std::string& str, char delimiter)
	{
		std::vector<std::string> result;

		std::istringstream iss(str);

		for (std::string token; std::getline(iss, token, delimiter);)
		{
			result.push_back(std::move(token));
		}

		return result;
	}

	/**
	* \brief Метод распределения шейдерного кода по ассоциативному массиву
	* \param shaders Указатель на ассоциативный массив содержащий строки с кодами шейдеров
	* \param source Исходная строка с общим кодом
	* \details Поскольку все шейдеры (вершинный, фрагментный и прочие) находятся в одном файле, его нужно разбить
	*/
	void ShaderResource::splitCodeToMap(std::map<GLuint, std::string>* shaders, const std::string& source)
	{
		// Итератор ассоциативного массива
		std::map<GLuint, std::string>::iterator it;

		// Пройтись по всем элементам ассоциативного массива
		for (it = shaders->begin(); it != shaders->end(); ++it)
		{
			// Получить ключ и значение элемента
			GLuint key = it->first;
			std::string value = it->second;

			// Установить пустую строку в значение
			(*shaders)[key] = "";

			// Если корректно указаны метки начала и конца шейдера
			if (value.size() > 0 && value.find('|') != std::string::npos)
			{
				// Получить метки начала и конца
				std::vector<std::string> startAndEndLabels = ShaderResource::explodeString(value, '|');

				// Получить позиции начала и конца шейдера
				std::size_t start = source.find(startAndEndLabels[0]);
				std::size_t end = source.find(startAndEndLabels[1]);

				// Если позиции начала и конца корректны - получить подстроку
				if (start != std::string::npos && end != std::string::npos) {
					(*shaders)[key] = source.substr(start, end - start);
				}
			}
		}
	}

	/**
	* \brief Компиляция шейдера
	* \param shaderSource Исходный код шейдера
	* \param type Тип шейдера
	* \return Идентификатор шейдера
	*/
	GLuint ShaderResource::compileShader(const char* shaderSource, GLuint type)
	{
		// Идентификатор шейдера
		GLuint id;

		// Инициализация GLEW
		if (!_isGlewInitialised) {
			glewExperimental = GL_TRUE;
			_isGlewInitialised = glewInit() == GLEW_OK;
		}

		if (!_isGlewInitialised) {
			throw std::runtime_error("OpenGL:ShaderResource: Glew is not initialised");
		}

		// Зарегистрировать шейдер нужного типа
		id = glCreateShader(type);

		// Связать исходный код и шейдер
		glShaderSource(id, 1, &shaderSource, nullptr);

		// Компиляция шейдера
		glCompileShader(id);

		// Успешна ли копиляция
		GLint success;
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);

		// Если не скомпилирован - генерация исключения
		if (!success) {
			GLsizei messageLength = 0;
			GLchar message[1024];
			glGetShaderInfoLog(id, 1024, &messageLength, message);

			throw std::runtime_error(std::string("OpenGL:ShaderResource:Compilation: ").append(message));
		}

		return id;
	}

	/**
	* \brief Создать шейдерную программу
	* \param source Исходный код шейдеров
	*/
	ShaderResource::ShaderResource(const std::string& source)
	{
		// Инициализация GLEW
		if (!_isGlewInitialised) {
			glewExperimental = GL_TRUE;
			_isGlewInitialised = glewInit() == GLEW_OK;
		}

		if (!_isGlewInitialised) {
			throw std::runtime_error("OpenGL:ShaderResource: Glew is not initialised");
		}

		// Ассоциативный массив исходных кодов шейдеров
		std::map<GLuint, std::string> shaderSources;
		shaderSources[GL_VERTEX_SHADER] = "/*VERTEX-SHADER-BEGIN*/|/*VERTEX-SHADER-END*/";
		shaderSources[GL_FRAGMENT_SHADER] = "/*FRAGMENT-SHADER-BEGIN*/|/*FRAGMENT-SHADER-END*/";
		shaderSources[GL_GEOMETRY_SHADER] = "/*GEOMETRY-SHADER-BEGIN*/|/*GEOMETRY-SHADER-END*/";

		// Поделить весь файл на куски относящиеся к соответствующим типам шейдеров
		ShaderResource::splitCodeToMap(&shaderSources, source);

		// Зарегестрировать шейдерную программу
		this->id_ = glCreateProgram();

		// Итератор ассоциативного массива
		std::map<GLuint, std::string>::iterator it;

		// ID'ы шейдеров (чтобы освободить память после линковки программы)
		std::vector<GLuint> shaderIds;

		// Пройтись по всем элементам ассоциативного массива
		for (it = shaderSources.begin(); it != shaderSources.end(); ++it)
		{
			// Если код шейдера не пуст
			if(it->second.length() > 0)
			{
				// Скомпилировать шейдер
				GLuint shaderId = ShaderResource::compileShader(it->second.c_str(), it->first);
				// Добавить шейдер к программе
				glAttachShader(this->id_, shaderId);
				// Добавить в список ID'ов
				shaderIds.push_back(shaderId);
			}
		}

		// Собрать шейдерную программу
		glLinkProgram(this->id_);

		// Проверка ошибок сборки шейдерной программы
		GLint success;
		glGetProgramiv(this->id_, GL_LINK_STATUS, &success);

		// Если не удалось собрать программу
		if (!success) {
			GLsizei messageLength = 0;
			GLchar message[1024];
			glGetProgramInfoLog(this->id_, 1024, &messageLength, message);

			throw std::runtime_error(std::string("OpenGL:ShaderResource:Linking: ").append(message));
		}

		// Удалить шейдеры (после сборки шейдерной программы они уже не нужны в памяти)
		for (const GLuint& shaderId : shaderIds) {
			glDeleteShader(shaderId);
		}
	}

	/**
	* \brief Уничтожить шейдерную программу
	*/
	ShaderResource::~ShaderResource()
	{
		if (id_) glDeleteProgram(id_);
	}

	/**
	* \brief Получть ID шейдерной программы
	* \return Числовой идентификатор
	*/
	GLuint ShaderResource::getId() const
	{
		return this->id_;
	}

	/**
	* \brief Создать ресурс шейдерной программы
	* \param source Исходный код шейдеров
	* \return Умный указатель на ресурс
	*/
	ShaderResourcePtr MakeShaderResource(const std::string& source)
	{
		return std::make_shared<ShaderResource>(source);
	}
}
