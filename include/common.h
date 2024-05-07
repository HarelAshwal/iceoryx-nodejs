#ifndef ICEORYX_NODEJS_COMMON_H
#define ICEORYX_NODEJS_COMMON_H

#include <napi.h>

constexpr std::uint32_t HW_MANAGER_REQ_BUFFER_SIZE{300000U};

// @brief api data structure
struct ApiData
{
    char data[HW_MANAGER_REQ_BUFFER_SIZE];
};

// @brief context data structure
struct ContextData
{
    int64_t requestSequenceId = 0;
    int64_t expectedResponseSequenceId = requestSequenceId;
};

#endif