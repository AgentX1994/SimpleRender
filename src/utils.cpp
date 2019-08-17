#include "utils.h"

#include <fstream>
#include <optional>
#include <spdlog/spdlog.h>
#include <string>

std::optional<std::string> read_file(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file) {
        spdlog::error("Could not open \"{}\"", filename);
        return {};
    }

    file.seekg(0, std::ios::end);
    auto size = file.tellg();

    std::string buffer(size, '\0');

    file.seekg(0);
    file.read(&buffer[0], size);

    return buffer;
}
