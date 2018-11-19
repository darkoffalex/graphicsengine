#include "FileTools.h"

#include <stdexcept>
#include <fstream>
#include <sstream>

// Необходимо для функций получения путей к файлу
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

/**
* \brief Путь к рабочему каталогу
* \return Строка содержащая путь к директории
*/
std::string WorkingDir()
{
	char path[MAX_PATH] = {};
	GetCurrentDirectoryA(MAX_PATH, path);
	PathAddBackslashA(path);
	return std::string(path);
}

/**
* \brief Путь к каталогу с исполняемым файлом (директория содержащая запущенный .exe)
* \return Строка содержащая путь к директории
*/
std::string ExeDir()
{
	char path[MAX_PATH] = {};
	GetModuleFileNameA(nullptr, path, MAX_PATH);
	PathRemoveFileSpecA(path);
	PathAddBackslashA(path);
	return std::string(path);
}

/**
* \brief Абсолютный путь к папке с шейдеами
* \return Строка содержащая путь к директории
*/
std::string ShadersDir()
{
	std::string exeDir = ExeDir();
	return exeDir.append("..\\Shaders\\");
}

/**
* \brief Загрузка текстового файла в строку
* \return Строка с содержимым файла
*/
std::string LoadStringFromFile(std::string filepath)
{
	// Чтение файла
	std::ifstream shaderFileStram;

	// Открыть файл для текстово чтения
	shaderFileStram.open(filepath);

	if (!shaderFileStram.fail())
	{
		// Строковой поток
		std::stringstream sourceStringStream;
		// Считать в строковой поток из файла
		sourceStringStream << shaderFileStram.rdbuf();
		// Закрыть файл
		shaderFileStram.close();
		// Получить данные из строкового поток в строку
		return sourceStringStream.str();
	}

	return "";
}