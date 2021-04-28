#include "io.h"

std::string ReadFile(const std::string& path)
{
    FILE* file = fopen(path.c_str(), "rb");
    std::string str = std::string();
    if (file)
    {
        fseek(file, 0, SEEK_END);
        size_t len = (size_t)ftell(file);
        fseek(file, 0, SEEK_SET);

        char* text = new char[len + 1];
        if (fread(text, 1, len, file) == len)
        {
            text[len] = '\0';
            str = std::string(text);
        }
        delete[] text;
    }
    return str;
}
