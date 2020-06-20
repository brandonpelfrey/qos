#pragma once
#include <types.h>

static const size_t KERNEL_CODE_SIZE = (32 * 1024 * 1024);
static const size_t KERNEL_STACK_SIZE = (8 * 1024 * 1024);
static const size_t KERNEL_STATIC_DATA_SIZE = (16 * 1024 * 1024);

static const size_t KERNEL_BASE = 0xFFFFFFFF80000000;
static const size_t KERNEL_STACK_BASE = KERNEL_BASE + KERNEL_CODE_SIZE;
static const size_t KERNEL_STACK_TOP = KERNEL_STACK_BASE + KERNEL_STACK_SIZE;
static const size_t KERNEL_STATIC_DATA = KERNEL_STACK_TOP + KERNEL_STATIC_DATA_SIZE;