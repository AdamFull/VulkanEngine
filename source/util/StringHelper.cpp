#include "StringHelper.h"

std::vector<std::string> StringHelper::Split(const std::string &str, char sep)
{
    std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(str);

	while (std::getline(tokenStream, token, sep))
		tokens.emplace_back(token);
	return tokens;
}

std::string StringHelper::ReplaceFirst(std::string str, std::string_view token, std::string_view to)
{
    const auto startPos = str.find(token);
	if (startPos == std::string::npos)
		return str;

	str.replace(startPos, token.length(), to);
	return str;
}