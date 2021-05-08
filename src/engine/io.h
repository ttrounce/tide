#ifndef TIDE_IO_H
#define TIDE_IO_H

#include <string>

std::string ReadFile(const std::string& path);
void WriteFile(const std::string& path, const std::string& string);

#endif // TIDE_IO_H