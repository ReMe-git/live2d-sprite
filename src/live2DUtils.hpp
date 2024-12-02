#ifndef LIVE2D_UTILS
#define LIVE2D_UTILS

#include <CubismFramework.hpp>
#include <cstdlib>
#include <string>

class live2DUtils : 
    public Csm::ICubismAllocator
{
public:
    // allocator functions
    void* Allocate(const Csm::csmSizeType size);
    void Deallocate(void* memory);
    void* AllocateAligned(const Csm::csmSizeType size, const Csm::csmUint32 alignment);
    void DeallocateAligned(void* alignedMemory);
    
    // tool functions
    static Csm::csmByte* loadFileAsBytes(const std::string filePath, Csm::csmSizeInt* outSize);
    static void releaseBytes(Csm::csmByte* byteData);
    static Csm::csmFloat32 getDeltaTime();
    static void updateTime();
    static void printLog(const Csm::csmChar* format, ...);
    static void printLogLn(const Csm::csmChar* format, ...);
    static void printMessage(const Csm::csmChar* message);
    static void printMessageLn(const Csm::csmChar* message);

private:
    static double currentFrame;
    static double lastFrame;
    static double deltaTime;
}; // live2DUtils

#endif