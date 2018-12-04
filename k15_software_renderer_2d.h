#ifndef _K15_SOFTWARE_RENDERER_2D_H_
#define _K15_SOFTWARE_RENDERER_2D_H_

#ifndef size_t
# include "stddef.h"
#endif

typedef unsigned    int  	ksr2_u32;
typedef signed      int  	ksr2_s32;
typedef unsigned    char 	ksr2_u8;
typedef signed      char 	ksr2_s8;
typedef unsigned    short	ksr2_u16;
typedef signed      short	ksr2_s16;
typedef unsigned    char 	ksr2_b8;
typedef unsigned    int		ksr2_b32;
typedef unsigned    int    	ksr2_u32;

typedef size_t ksr2_contexthandle;

#define ksr2_kilobyte(x) 		(x * 1024)
#define ksr2_megabyte(x) 		(ksr2_kilobyte(x) * 1024)
#define ksr2_gigabyte(x) 		(ksr2_megabyte(x) * 1024)
#define ksr2_internal 			static
#define ksr2_nullptr			((void*)0)
#define ksr2_use_argument(x)	((void)x)

#ifdef K15_RENDERER_2D_NO_ASSERTS
	#define ksr2_assert(x)
#else
	#define ksr2_assert(x) {if (!(x)) {__builtin_trap();}}
#endif

enum
{
    K15_RENDERER_2D_DEFAULT_MEMORY_SIZE_IN_BYTES = ksr2_megabyte(10)
};

typedef enum
{
    K15_RENDERER_2D_PIXEL_FORMAT_RGB8
} ksr2_pixel_format;

typedef enum
{
	K15_RENDERER_2D_DOUBLE_BUFFERED_FLAG = 0x01
} ksr2_context_flags;

typedef enum
{
    K15_RENDERER_2D_RESULT_SUCCESS = 0,
    K15_RENDERER_2D_RESULT_OUT_OF_MEMORY,
	K15_RENDERER_2D_RESULT_INVALID_ARGUMENT
} ksr2_result;

typedef enum
{
	K15_RENDERER_2D_DEBUG_CATEGORY_INFO 	= 0u,
	K15_RENDERER_2D_DEBUG_CATEGORY_WARNING 	= 1u,
	K15_RENDERER_2D_DEBUG_CATEGORY_ERROR 	= 2u,

	K15_RENDERER_2D_DEBUG_CATEGORY_NO_FILTER = K15_RENDERER_2D_DEBUG_CATEGORY_INFO
} ksr2_debug_category;

typedef void(*ksr2_debug_fnc)(ksr2_contexthandle, ksr2_debug_category, const char*); 

typedef struct
{
    void*               pMemory;
    size_t              memorySizeInBytes;
    ksr2_u32            backBufferWidth;
    ksr2_u32            backBufferHeight;
    ksr2_u32            backBufferCount;
    ksr2_u32			flags;

	ksr2_pixel_format   backBufferFormat;
	
	ksr2_debug_fnc		debugFnc;
	ksr2_debug_category debugCategoryFilter;

} ksr2_context_parameters;

typedef struct
{
	void* pBaseAddress;
	size_t memorySizeInBytes;
} ksr2_allocator_block;

typedef struct
{
	ksr2_allocator_block* 	pFirstBlock;
	void* 					pBaseAddress;
	size_t 					memorySizeInBytes;
} ksr2_block_allocator;

typedef struct
{
    void*   			pMemory;
    size_t 				memorySizeInBytes;

	ksr2_debug_fnc		debugFnc;
	ksr2_debug_category debugCategoryFilter;
} ksr2_context;

ksr2_result ksr2_init_context(const ksr2_context_parameters* pParameters, ksr2_contexthandle* pOutContextHandle);


#ifdef K15_SOFTWARE_RENDERER_2D_IMPLEMENTATION

ksr2_internal ksr2_contexthandle ksr2_invalid_context_handle = 0u;

ksr2_internal void ksr2_debug_fnc_stub(ksr2_contexthandle contextHandle, ksr2_debug_category category, const char* pMessage)
{
	ksr2_use_argument(contextHandle);
	ksr2_use_argument(category);
	ksr2_use_argument(pMessage);
}

ksr2_result ksr2_init_block_allocator(ksr2_block_allocator* pOutAllocator, void* pBaseAddress, size_t memorySizeInBytes)
{
	if (memorySizeInBytes < sizeof(ksr2_block_allocator))
	{
		return K15_RENDERER_2D_RESULT_OUT_OF_MEMORY;
	}

	memorySizeInBytes -= sizeof(ksr2_block_allocator);

	ksr2_block_allocator allocator = {};
	allocator.pBaseAddress 		= pBaseAddress;
	allocator.memorySizeInBytes = memorySizeInBytes;
	allocator.pFirstBlock		= pBaseAddress + memorySizeInBytes;

	allocator.pFirstBlock->sizeInBytes 	= memorySizeInBytes;
	allocator.pFirstBlock->pBaseAddress = pBaseAddress;

	*pOutAllocator = allocator;

	return K15_RENDERER_2D_RESULT_SUCCESS;
}

ksr2_result ksr2_allocate_from_block_allocator(ksr2_block_allocator* pAllocator, size_t memorySizeInBytes)
{
	const size_t totalMemorySizeInBytes = memorySizeInBytes + sizeof(ksr2_allocator_block);

	if (totalMemorySizeInBytes <= pAllocator->memorySizeInBytes)
	{
		return K15_RENDERER_2D_RESULT_OUT_OF_MEMORY;
	}

	
}

ksr2_result ksr2_init_context(const ksr2_context_parameters* pParameters, ksr2_contexthandle* pOutContextHandle)
{
	ksr2_debug_fnc debugFnc = pParameters != ksr2_nullptr ? pParameters->debugFnc : ksr2_debug_fnc_stub;
	
	if (debugFnc == ksr2_nullptr)
	{
		debugFnc = ksr2_debug_fnc_stub;
	}
	
	if (pParameters == ksr2_nullptr)
	{
		debugFnc(ksr2_invalid_context_handle, K15_RENDERER_2D_DEBUG_CATEGORY_ERROR, "pParameter in 'ksr2_init_context' is NULL.\n");
		return K15_RENDERER_2D_RESULT_INVALID_ARGUMENT;
	}

	ksr2_context* pContext = ksr2_nullptr;
	ksr2_block_allocator allocator;
	ksr2_result result = ksr2_init_block_allocator(&allocator, pParameters->pMemory, pParameters->memorySizeInBytes);

	if (result != K15_RENDERER_2D_RESULT_SUCCESS)
	{

	}

	result = ksr2_allocate_from_block_allocator(&allocator, sizeof(ksr2_context), 16u);

	if (pOutContextHandle == ksr2_nullptr)
	{

	}
}
#endif // K15_SOFTWARE_RENDERER_2D_IMPLEMENTATION
#endif // _K15_SOFTWARE_RENDERER_2D_H_