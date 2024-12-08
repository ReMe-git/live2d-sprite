#ifndef LIVE2D_UTILS
#define LIVE2D_UTILS

#include <CubismFramework.hpp>
#include <cstdlib>
#include <string>

class Live2DUtils : 
    public Csm::ICubismAllocator
{
public:
    // allocator functions
    void* Allocate(const Csm::csmSizeType size);
    void Deallocate(void* memory);
    void* AllocateAligned(const Csm::csmSizeType size, const Csm::csmUint32 alignment);
    void DeallocateAligned(void* aligned_memory);
    
    // tool functions
    static Csm::csmByte* LoadFileAsBytes(const std::string file_path, Csm::csmSizeInt* out_size);
    static void ReleaseBytes(Csm::csmByte* byte_data);
    static Csm::csmFloat32 GetDeltaTime();
    static void UpdateTime();
    static void PrintLog(const Csm::csmChar* format, ...);
    static void PrintLogLn(const Csm::csmChar* format, ...);
    static void PrintMessage(const Csm::csmChar* message);
    static void PrintMessageLn(const Csm::csmChar* message);

private:
    static double current_frame;
    static double last_frame;
    static double delta_time;
}; // live2DUtils

#endif