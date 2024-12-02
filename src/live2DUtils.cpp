// include(s)
#include "live2DUtils.hpp"
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
double live2DUtils::currentFrame = 0.0;
double live2DUtils::lastFrame = 0.0;
double live2DUtils::deltaTime = 0.0;

// function(s)
csmByte* live2DUtils::loadFileAsBytes(const string filePath, csmSizeInt* outSize)
{
    const char* path = filePath.c_str();

    int size = 0;
    struct stat statBuf;
    if (stat(path, &statBuf) == 0)
    {
        size = statBuf.st_size;

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

    *outSize = size;
    return reinterpret_cast<csmByte*>(buf);
}

void live2DUtils::releaseBytes(csmByte* byteData)
{
    delete[] byteData;
}

csmFloat32  live2DUtils::getDeltaTime()
{
    return static_cast<csmFloat32>(deltaTime);
}

void live2DUtils::updateTime()
{
    currentFrame = SDL_GetTicks64() / 1000.0f;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

void* live2DUtils::Allocate(const csmSizeType size)
{
    return malloc(size);
}

void live2DUtils::Deallocate(void* memory)
{
    free(memory);
}

void* live2DUtils::AllocateAligned(const csmSizeType size, const csmUint32 alignment)
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

void live2DUtils::DeallocateAligned(void* alignedMemory)
{
    void** preamble;

    preamble = static_cast<void**>(alignedMemory);

    Deallocate(preamble[-1]);
}

void live2DUtils::printLog(const csmChar* format, ...)
{
    va_list args;
    csmChar buf[256];
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    std::cout << buf;
    va_end(args);
}

void live2DUtils::printLogLn(const csmChar* format, ...)
{
    va_list args;
    csmChar buf[256];
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    std::cout << buf << std::endl;
    va_end(args);
}

void live2DUtils::printMessage(const csmChar* message)
{
    printLog("%s", message);
}

void live2DUtils::printMessageLn(const csmChar* message)
{
    printLogLn("%s", message);
}
