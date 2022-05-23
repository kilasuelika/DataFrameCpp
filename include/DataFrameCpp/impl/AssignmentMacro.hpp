#pragma once

#define _ASSGNMENT_SWITCH_CASE_(FUNC, obj)                                                         \
    switch (dtype()) {                                                                             \
    case DType::BOOL:                                                                              \
        switch (obj.dtype()) {                                                                     \
        case DType::BOOL:                                                                          \
            FUNC<bool, bool>(obj);                                                                 \
            break;                                                                                 \
        case DType::INT:                                                                           \
            FUNC<int, int>(obj);                                                                   \
            break;                                                                                 \
        case DType::LONGLONG:                                                                      \
            FUNC<long long, long long>(obj);                                                       \
            break;                                                                                 \
        case DType::FLOAT:                                                                         \
            FUNC<float, float>(obj);                                                               \
            break;                                                                                 \
        case DType::DOUBLE:                                                                        \
            FUNC<double, double>(obj);                                                             \
            break;                                                                                 \
        default:                                                                                   \
            break;                                                                                 \
        }                                                                                          \
        break;                                                                                     \
    case DType::INT:                                                                               \
        switch (obj.dtype()) {                                                                     \
        case DType::BOOL:                                                                          \
            FUNC<int, bool>(obj);                                                                  \
            break;                                                                                 \
        case DType::INT:                                                                           \
            FUNC<int, int>(obj);                                                                   \
            break;                                                                                 \
        case DType::LONGLONG:                                                                      \
            FUNC<long long, long long>(obj);                                                       \
            break;                                                                                 \
        case DType::FLOAT:                                                                         \
            FUNC<float, float>(obj);                                                               \
            break;                                                                                 \
        case DType::DOUBLE:                                                                        \
            FUNC<double, double>(obj);                                                             \
            break;                                                                                 \
        default:                                                                                   \
            break;                                                                                 \
        }                                                                                          \
        break;                                                                                     \
    case DType::LONGLONG:                                                                          \
        switch (obj.dtype()) {                                                                     \
        case DType::BOOL:                                                                          \
            FUNC<long long, bool>(obj);                                                            \
            break;                                                                                 \
        case DType::INT:                                                                           \
            FUNC<long long, int>(obj);                                                             \
            break;                                                                                 \
        case DType::LONGLONG:                                                                      \
            FUNC<long long, long long>(obj);                                                       \
            break;                                                                                 \
        case DType::FLOAT:                                                                         \
            FUNC<double, float>(obj);                                                              \
            break;                                                                                 \
        case DType::DOUBLE:                                                                        \
            FUNC<double, double>(obj);                                                             \
            break;                                                                                 \
        default:                                                                                   \
            break;                                                                                 \
        }                                                                                          \
        break;                                                                                     \
    case DType::FLOAT:                                                                             \
        switch (obj.dtype()) {                                                                     \
        case DType::BOOL:                                                                          \
            FUNC<float, bool>(obj);                                                                \
            break;                                                                                 \
        case DType::INT:                                                                           \
            FUNC<float, int>(obj);                                                                 \
            break;                                                                                 \
        case DType::LONGLONG:                                                                      \
            FUNC<double, long long>(obj);                                                          \
            break;                                                                                 \
        case DType::FLOAT:                                                                         \
            FUNC<float, float>(obj);                                                               \
            break;                                                                                 \
        case DType::DOUBLE:                                                                        \
            FUNC<double, double>(obj);                                                             \
            break;                                                                                 \
        default:                                                                                   \
            break;                                                                                 \
        }                                                                                          \
        break;                                                                                     \
    case DType::DOUBLE:                                                                            \
        switch (obj.dtype()) {                                                                     \
        case DType::BOOL:                                                                          \
            FUNC<double, bool>(obj);                                                               \
            break;                                                                                 \
        case DType::INT:                                                                           \
            FUNC<double, int>(obj);                                                                \
            break;                                                                                 \
        case DType::LONGLONG:                                                                      \
            FUNC<double, long long>(obj);                                                          \
            break;                                                                                 \
        case DType::FLOAT:                                                                         \
            FUNC<double, float>(obj);                                                              \
            break;                                                                                 \
        case DType::DOUBLE:                                                                        \
            FUNC<double, double>(obj);                                                             \
            break;                                                                                 \
        default:                                                                                   \
            break;                                                                                 \
        }                                                                                          \
        break;                                                                                     \
    default:                                                                                       \
        break;                                                                                     \
    }