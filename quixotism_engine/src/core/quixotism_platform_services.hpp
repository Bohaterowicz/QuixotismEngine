#pragma once
#include "quixotism_c.hpp"

#include <functional>
#include <memory>

/**
 * @brief Data structure returned by the ReadFile platform service
 *
 */
struct read_file_result
{
    std::unique_ptr<uint8[]> Content = nullptr;
    uint32 Size = 0;
};

/**
 * @brief Struct contains pointers to platform services that the engine is gonna use.
 *
 */
struct platform_services
{
    read_file_result (*ReadFile)(const std::string &) = nullptr;
    real64 (*GetTime)() = nullptr;
};