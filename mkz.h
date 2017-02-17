#pragma once

// Be 1337
#define let const auto

// make sure this number is part of the instruction stream
#define PI 3.14159265359

#define MOVE_ONLY_DEFAULT(klass) \
    klass(const klass& other) = delete; \
    klass& operator=(const klass& other) = delete; \
    \
    klass(klass&& other) = default; \
    klass& operator=(klass&& other) = default;
