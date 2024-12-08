// include(s)
#include "Live2DUtils.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <Model/CubismMoc.hpp>

// namespace(s)
using namespace Csm;
using namespace std;

// type(s)
// define(s)
// varible(s)
double Live2DUtils::current_frame = 0.0;
double Live2DUtils::last_frame = 0.0;
double Live2DUtils::delta_time = 0.0;

// function(s)
csmByte* Live2DUtils::LoadFileAsBytes(const string file_path, csmSizeInt* out_size)
{
    const char* path = file_path.c_str();

    int size = 0;
    struct stat stat_buf;
    if (stat(path, &stat_buf) == 0)
    {
        size = stat_buf.st_size;

        if (size == 0)
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }

    std::fstream file;
    file.open(path, std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        return NULL;
    }

    char* buf = new char[size];
    file.read(buf, size);
    file.close();

    *out_size = size;
    return reinterpret_cast<csmByte*>(buf);
}

void Live2DUtils::ReleaseBytes(csmByte* byte_data)
{
    delete[] byte_data;
}

csmFloat32  Live2DUtils::GetDeltaTime()
{
    return static_cast<csmFloat32>(delta_time);
}

void Live2DUtils::UpdateTime()
{
    current_frame = SDL_GetTicks64() / 1000.0f;
    delta_time = current_frame - last_frame;
    last_frame = current_frame;
}

void* Live2DUtils::Allocate(const csmSizeType size)
{
    return malloc(size);
}

void Live2DUtils::Deallocate(void* memory)
{
    free(memory);
}

void* Live2DUtils::AllocateAligned(const csmSizeType size, const csmUint32 alignment)
{
    size_t offset, shift, alignedAddress;
    void* allocation;
    void** preamble;

    offset = alignment - 1 + sizeof(void*);

    allocation = Allocate(size + static_cast<csmUint32>(offset));

    alignedAddress = reinterpret_cast<size_t>(allocation) + sizeof(void*);

    shift = alignedAddress % alignment;

    if (shift)
    {
        alignedAddress += (alignment - shift);
    }

    preamble = reinterpret_cast<void**>(alignedAddress);
    preamble[-1] = allocation;

    return reinterpret_cast<void*>(alignedAddress);
}

void Live2DUtils::DeallocateAligned(void* alignedMemory)
{
    void** preamble;

    preamble = static_cast<void**>(alignedMemory);

    Deallocate(preamble[-1]);
}

void Live2DUtils::PrintLog(const csmChar* format, ...)
{
    va_list args;
    csmChar buf[256];
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    std::cout << buf;
    va_end(args);
}

void Live2DUtils::PrintLogLn(const csmChar* format, ...)
{
    va_list args;
    csmChar buf[256];
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    std::cout << buf << std::endl;
    va_end(args);
}

void Live2DUtils::PrintMessage(const csmChar* message)
{
    PrintLog("%s", message);
}

void Live2DUtils::PrintMessageLn(const csmChar* message)
{
    PrintLogLn("%s", message);
}
