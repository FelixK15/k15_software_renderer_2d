#ifndef _K15_SOFTWARE_RENDERER_2D_H_
#define _K15_SOFTWARE_RENDERER_2D_H_

#ifndef size_t
# include "stddef.h"
#endif

typedef size_t ksr2_contexthandle;

#define ksr2_kilobyte(x) 		(x * 1024)
#define ksr2_megabyte(x) 		(ksr2_kilobyte(x) * 1024)
#define ksr2_gigabyte(x) 		(ksr2_megabyte(x) * 1024)

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
	K15_RENDERER_2D_RESULT_INVALID_ARGUMENT,
	K15_RENDERER_2D_UNKNOWN_PIXEL_FORMAT
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
	
    unsigned int        backBufferWidth;
    unsigned int        backBufferHeight;
    unsigned int        backBufferCount;
    unsigned int		flags;

	ksr2_pixel_format   backBufferFormat;
	
	ksr2_debug_fnc		debugFnc;
	ksr2_debug_category debugCategoryFilter;

} ksr2_context_parameters;

typedef struct 
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} ksr2_rgba_color;

ksr2_rgba_color ksr2_rgba_color_float(float r, float g, float b, float a);
ksr2_rgba_color ksr2_rgba_color_uint8(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
ksr2_rgba_color ksr2_rgba_color_uint32(unsigned int rgba);

ksr2_rgba_color ksr2_rgb_color_float(float r, float g, float b);
ksr2_rgba_color ksr2_rgb_color_uint8(unsigned char r, unsigned char g, unsigned char b);
ksr2_rgba_color ksr2_rgb_color_uint32(unsigned int rgba);

ksr2_rgba_color ksr2_color_red();
ksr2_rgba_color ksr2_color_green();
ksr2_rgba_color ksr2_color_blue();
ksr2_rgba_color ksr2_color_yellow();
ksr2_rgba_color ksr2_color_magenta();
ksr2_rgba_color ksr2_color_cyan();
ksr2_rgba_color ksr2_color_white();
ksr2_rgba_color ksr2_color_black();

ksr2_result ksr2_init_context(const ksr2_context_parameters* pParameters, ksr2_contexthandle* pOutContextHandle);
void ksr2_destroy_context(ksr2_contexthandle handle);
void ksr2_swap_buffers(ksr2_contexthandle handle);
unsigned char* ksr2_get_presenting_image_data(ksr2_contexthandle handle);

ksr2_result ksr2_resize_swap_chain(ksr2_contexthandle handle, unsigned int width, unsigned int height, ksr2_pixel_format format);

ksr2_result ksr2_draw_line(ksr2_contexthandle handle, int x1, int y1, int x2, int y2, unsigned int thickness, ksr2_rgba_color color);

#ifdef K15_SOFTWARE_RENDERER_2D_IMPLEMENTATION

#ifndef K15_RENDERER_2D_STATIC
#	define ksr2_internal static
#else
#	define ksr2_internal extern
#endif

#ifndef ksr2_nullptr
#	define ksr2_nullptr			((void*)0)
#endif

#define ksr2_use_argument(x)	((void)x)

typedef unsigned    int  	ksr2_u32;
typedef signed      int  	ksr2_s32;
typedef unsigned    char 	ksr2_u8;
typedef signed      char 	ksr2_s8;
typedef unsigned    short	ksr2_u16;
typedef signed      short	ksr2_s16;
typedef unsigned    char 	ksr2_b8;
typedef unsigned    int		ksr2_b32;
typedef unsigned    int    	ksr2_u32;
typedef unsigned 	char	ksr2_byte;

typedef struct
{
	ksr2_u32 			stride;
	ksr2_u32 			alignment;
	ksr2_u8				channelCount;
	ksr2_u8				bitsPerChannel[4u];
	size_t 				memorySizeInBytes;
} ksr2_image_memory_requirements;

typedef struct
{
	ksr2_byte* 				pStartAddress;
	ksr2_byte*				pEndAddress;
	size_t 					memorySizeInBytesStart;
	size_t 					memorySizeInBytesEnd;
	size_t 					memoryCapacityInBytes;
} ksr2_linear_allocator;

typedef struct
{
	void** 								pImages;
	void* 								pImageStart; //FK: in case we need to reset the allocator
	void*								pCurrentImage;
	ksr2_u32 							imageCount;
	ksr2_u32 							imageIndex;
	ksr2_u32 							width;
	ksr2_u32 							height;
	ksr2_pixel_format 					format;
	ksr2_image_memory_requirements 		memoryRequirements;
} ksr2_swap_chain;

typedef struct
{
    void*   				pMemory;
    size_t 					memorySizeInBytes;

	ksr2_linear_allocator 	allocator;
	ksr2_swap_chain			swapChain;

	ksr2_debug_fnc			debugFnc;
	ksr2_debug_category 	debugCategoryFilter;
} ksr2_context;


ksr2_internal ksr2_contexthandle 	ksr2_invalid_context_handle = 0u;
ksr2_internal size_t 				ksr2_default_alignment		= 16u;

ksr2_internal void ksr2_debug_fnc_stub(ksr2_contexthandle contextHandle, ksr2_debug_category category, const char* pMessage)
{
	ksr2_use_argument(contextHandle);
	ksr2_use_argument(category);
	ksr2_use_argument(pMessage);
}

ksr2_internal void ksr2_init_linear_allocator(ksr2_linear_allocator* pOutAllocator, void* pBaseAddress, size_t memorySizeInBytes)
{
	ksr2_linear_allocator allocator 	= {};
	allocator.pStartAddress 			= (ksr2_byte*)pBaseAddress;
	allocator.pEndAddress				= (ksr2_byte*)pBaseAddress + memorySizeInBytes;
	allocator.memorySizeInBytesStart 	= 0u;
	allocator.memorySizeInBytesEnd	 	= 0u;
	allocator.memoryCapacityInBytes 	= memorySizeInBytes;	
	*pOutAllocator = allocator;
}

ksr2_internal void ksr2_set_linear_allocator_front(ksr2_linear_allocator* pAllocator, void* pStartAddress)
{
	ksr2_assert(pAllocator != ksr2_nullptr);
	ksr2_assert(pStartAddress != ksr2_nullptr);

	pAllocator->memorySizeInBytesStart = ((size_t)pStartAddress - (size_t)pAllocator->pStartAddress);
	pAllocator->pStartAddress = pStartAddress;
}

ksr2_internal size_t ksr2_get_linear_allocator_capacity(const ksr2_linear_allocator* pAllocator)
{
	return pAllocator->memoryCapacityInBytes - (pAllocator->memorySizeInBytesStart + pAllocator->memorySizeInBytesEnd);
}

ksr2_internal ksr2_result ksr2_allocate_from_linear_allocator_front(void** pOutPointer, ksr2_linear_allocator* pAllocator, size_t memorySizeInBytes, size_t alignment)
{
	ksr2_use_argument(alignment);

	const size_t allocatorCapacityInBytes = ksr2_get_linear_allocator_capacity(pAllocator);

	if (allocatorCapacityInBytes < memorySizeInBytes)
	{
		return K15_RENDERER_2D_RESULT_OUT_OF_MEMORY;
	}

	*pOutPointer = (void*)(pAllocator->pStartAddress + pAllocator->memorySizeInBytesStart);
	pAllocator->memorySizeInBytesStart += memorySizeInBytes;

	return K15_RENDERER_2D_RESULT_SUCCESS;
}

ksr2_internal ksr2_result ksr2_allocator_from_linear_allocator_back(void** pOutPointer, ksr2_linear_allocator* pAllocator, size_t memorySizeInBytes)
{
	const size_t allocatorCapacityInBytes = ksr2_get_linear_allocator_capacity(pAllocator);

	if (allocatorCapacityInBytes < memorySizeInBytes)
	{
		return K15_RENDERER_2D_RESULT_OUT_OF_MEMORY;
	}

	pOutPointer = (void*)(pAllocator->pEndAddress - pAllocator->memorySizeInBytesEnd - memorySizeInBytes);
	pAllocator->memorySizeInBytesEnd += memorySizeInBytes;

	return K15_RENDERER_2D_RESULT_SUCCESS;	
}

ksr2_internal size_t ksr2_get_pixel_size_in_bytes(ksr2_pixel_format format)
{
	switch(format)
	{
		case K15_RENDERER_2D_PIXEL_FORMAT_RGB8:
			return 3u;

		default:
			ksr2_assert(0u);
	}

	return 0u;
}

ksr2_internal ksr2_result ksr2_query_image_memory_requirements(ksr2_image_memory_requirements* pOutRequirements, ksr2_u32 width, ksr2_u32 height, ksr2_pixel_format format)
{
	ksr2_image_memory_requirements requirements = {};
	
	switch(format)
	{
		case K15_RENDERER_2D_PIXEL_FORMAT_RGB8:
			requirements.bitsPerChannel[0] = 8u;
			requirements.bitsPerChannel[1] = 8u;
			requirements.bitsPerChannel[2] = 8u;
			requirements.channelCount = 3u;
			requirements.stride = width;
			requirements.memorySizeInBytes = height * width * 3u;
			break;

		default:
			return K15_RENDERER_2D_UNKNOWN_PIXEL_FORMAT;
	}

	*pOutRequirements = requirements;

	return K15_RENDERER_2D_RESULT_SUCCESS;
}

ksr2_internal ksr2_result ksr2_init_swap_chain_images(ksr2_swap_chain* pSwapChain, ksr2_linear_allocator* pAllocator, ksr2_u32 width, ksr2_u32 height, ksr2_pixel_format format)
{
	ksr2_image_memory_requirements memoryRequirements = {};
	ksr2_result result = ksr2_query_image_memory_requirements(&memoryRequirements, width, height, format);

	if (result != K15_RENDERER_2D_RESULT_SUCCESS)
	{
		return result;
	}

	for (ksr2_u32 imageIndex = 0u; imageIndex < pSwapChain->imageCount; ++imageIndex)
	{
		void* pImage = ksr2_nullptr;
		result = ksr2_allocate_from_linear_allocator_front(&pImage, pAllocator, memoryRequirements.memorySizeInBytes, memoryRequirements.alignment);

		if (result != K15_RENDERER_2D_RESULT_SUCCESS)
		{
			return result;
		}		

		pSwapChain->pImages[imageIndex] = pImage;
	}

	return K15_RENDERER_2D_RESULT_SUCCESS;
}

ksr2_internal void ksr2_destroy_swap_chain_images(ksr2_swap_chain* pSwapChain, ksr2_linear_allocator* pAllocator)
{
	ksr2_assert(pSwapChain != ksr2_nullptr);
	ksr2_assert(pAllocator != ksr2_nullptr);

	void* pStartAddress = pSwapChain->pImageStart;
	
	ksr2_set_linear_allocator_front(pAllocator, pStartAddress);
	
	for (ksr2_u32 imageIndex = 0u; imageIndex < pSwapChain->imageCount; ++imageIndex)
	{
		pSwapChain->pImages[imageIndex] = ksr2_nullptr;
	}
}

ksr2_internal void* ksr2_get_allocator_front_address(ksr2_linear_allocator* pAllocator)
{
	ksr2_assert(pAllocator != ksr2_nullptr);
	return pAllocator->pStartAddress + pAllocator->memorySizeInBytesStart;
}

ksr2_internal ksr2_result ksr2_init_swap_chain(ksr2_swap_chain* pOutSwapChain, ksr2_linear_allocator* pAllocator, ksr2_u32 width, ksr2_u32 height, ksr2_pixel_format format, ksr2_u32 imageCount)
{
	void** pImages = ksr2_nullptr;
	ksr2_result result = ksr2_allocate_from_linear_allocator_front(&pImages, pAllocator, sizeof(void*) * imageCount, ksr2_default_alignment);

	if (result != K15_RENDERER_2D_RESULT_SUCCESS)
	{
		return result;
	}

	ksr2_swap_chain swapChain = {};
	swapChain.imageCount 			= imageCount;
	swapChain.pImages 				= pImages;
	swapChain.width 				= width;
	swapChain.height 				= height;
	swapChain.format 				= format;
	swapChain.pImageStart 			= ksr2_get_allocator_front_address(pAllocator);
	
	result = ksr2_init_swap_chain_images(&swapChain, pAllocator, width, height, format);

	if (result != K15_RENDERER_2D_RESULT_SUCCESS)
	{
		return result;
	}

	*pOutSwapChain = swapChain;

	return K15_RENDERER_2D_RESULT_SUCCESS;
}

// DECLARED FUNCTIONS

float ksr2_clamp_float(float v, float min, float max)
{
	return v < min ? min : v > max ? max : v;
}

ksr2_rgba_color ksr2_rgba_color_float(float r, float g, float b, float a)
{
	ksr2_rgba_color color;
	color.r = (ksr2_u8)(ksr2_clamp_float(r, 0.0f, 1.0f) * 255.0f);
	color.g = (ksr2_u8)(ksr2_clamp_float(g, 0.0f, 1.0f) * 255.0f);
	color.b = (ksr2_u8)(ksr2_clamp_float(b, 0.0f, 1.0f) * 255.0f);
	color.a = (ksr2_u8)(ksr2_clamp_float(a, 0.0f, 1.0f) * 255.0f);

	return color;
}

ksr2_rgba_color ksr2_rgba_color_uint8(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	ksr2_rgba_color color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;

	return color;
}

ksr2_rgba_color ksr2_rgba_color_uint32(unsigned int rgba)
{
	ksr2_rgba_color color;
	color.r = ((rgba >> 24) & 0xFF);
	color.g = ((rgba >> 16) & 0xFF);
	color.b = ((rgba >>  8) & 0xFF);
	color.a = ((rgba >>  0) & 0xFF);
}

ksr2_rgba_color ksr2_rgb_color_float(float r, float g, float b)
{
	ksr2_rgba_color color;
	color.r = (ksr2_u8)(ksr2_clamp_float(r, 0.0f, 1.0f) * 255.0f);
	color.g = (ksr2_u8)(ksr2_clamp_float(g, 0.0f, 1.0f) * 255.0f);
	color.b = (ksr2_u8)(ksr2_clamp_float(b, 0.0f, 1.0f) * 255.0f);
	color.a = 255u;

	return color;
}

ksr2_rgba_color ksr2_rgb_color_uint8(unsigned char r, unsigned char g, unsigned char b)
{
	ksr2_rgba_color color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = 255u;
	
	return color;
}

ksr2_rgba_color ksr2_rgb_color_uint32(unsigned int rgb)
{
	ksr2_rgba_color color;
	color.r = ((rgb >> 16) & 0xFF);
	color.g = ((rgb >>  8) & 0xFF);
	color.b = ((rgb >>  0) & 0xFF);
	color.a = 255u;
	
	return color;
}

ksr2_rgba_color ksr2_color_red()
{
	return ksr2_rgb_color_uint8(0xFF, 0x00, 0x00);
}

ksr2_rgba_color ksr2_color_green()
{
	return ksr2_rgb_color_uint8(0x00, 0xFF, 0x00);
}

ksr2_rgba_color ksr2_color_blue()
{
	return ksr2_rgb_color_uint8(0x00, 0x00, 0xFF);
}

ksr2_rgba_color ksr2_color_yellow()
{
	return ksr2_rgb_color_uint8(0xFF, 0x00, 0xFF);
}

ksr2_rgba_color ksr2_color_magenta()
{
	return ksr2_rgb_color_uint8(0xFF, 0xFF, 0x00);
}

ksr2_rgba_color ksr2_color_cyna()
{
	return ksr2_rgb_color_uint8(0x00, 0xFF, 0xFF);
}

ksr2_rgba_color ksr2_color_white()
{
	return ksr2_rgb_color_uint8(0xFF, 0xFF, 0xFF);
}

ksr2_rgba_color ksr2_color_black()
{
	return ksr2_rgb_color_uint8(0x00, 0x00, 0x00);
}

ksr2_result ksr2_init_context(const ksr2_context_parameters* pParameters, ksr2_contexthandle* pOutContextHandle)
{
	ksr2_debug_fnc debugFnc = pParameters != ksr2_nullptr ? pParameters->debugFnc : ksr2_nullptr;
	
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
	ksr2_linear_allocator allocator;
	ksr2_init_linear_allocator(&allocator, pParameters->pMemory, pParameters->memorySizeInBytes);

	ksr2_result result = ksr2_allocate_from_linear_allocator_front(&pContext, &allocator, sizeof(ksr2_context), ksr2_default_alignment);

	if (result != K15_RENDERER_2D_RESULT_SUCCESS)
	{
		//debugFnc(ksr2_invalid_context_handle, K15_RENDERER_2D_DEBUG_CATEGORY_ERROR, "Not enough memory. need at least %u bytes for context.\n", sizeof(ksr2_context));
		return result;
	}

	const ksr2_u32 swapChainImageCount = (pParameters->flags & K15_RENDERER_2D_DOUBLE_BUFFERED_FLAG) > 0u ? 2u : 1u;
	result = ksr2_init_swap_chain(&pContext->swapChain, &allocator, pParameters->backBufferWidth, pParameters->backBufferHeight, pParameters->backBufferFormat, swapChainImageCount);

	if (result != K15_RENDERER_2D_RESULT_SUCCESS)
	{
		return result;
	}

	pContext->allocator = allocator;
	ksr2_contexthandle handle = (ksr2_contexthandle)(pContext);
	*pOutContextHandle = handle;

	return K15_RENDERER_2D_RESULT_SUCCESS;
}

void ksr2_destroy_context(ksr2_contexthandle handle)
{
	ksr2_assert(handle != ksr2_invalid_context_handle);
}

void ksr2_swap_buffers(ksr2_contexthandle handle)
{
	ksr2_context* pContext = (ksr2_context*)handle;

	if (pContext->swapChain.imageIndex + 1u == pContext->swapChain.imageCount)
	{
		pContext->swapChain.imageIndex = 0u;
	} 
	else
	{
		++pContext->swapChain.imageIndex;
	}
	
	pContext->swapChain.pCurrentImage = pContext->swapChain.pImages[pContext->swapChain.imageIndex];
}

unsigned char* ksr2_get_presenting_image_data(ksr2_contexthandle handle)
{
	ksr2_context* pContext = (ksr2_context*)handle;
	return (unsigned char*)pContext->swapChain.pCurrentImage;
}

ksr2_result ksr2_resize_swap_chain(ksr2_contexthandle handle, ksr2_u32 width, ksr2_u32 height, ksr2_pixel_format format)
{
	if (handle == ksr2_invalid_context_handle)
	{
		return K15_RENDERER_2D_RESULT_INVALID_ARGUMENT;
	}
	ksr2_context* pContext = (ksr2_context*)(handle);

	if (pContext->swapChain.width == width && pContext->swapChain.height == height && pContext->swapChain.format == format)
	{
		return K15_RENDERER_2D_RESULT_SUCCESS;
	}

	ksr2_destroy_swap_chain_images(&pContext->swapChain, &pContext->allocator);
	
	ksr2_result result = ksr2_init_swap_chain_images(&pContext->swapChain, &pContext->allocator, width, height, format);

	if (result != K15_RENDERER_2D_RESULT_SUCCESS)
	{
		return result;
	}

	return K15_RENDERER_2D_RESULT_SUCCESS;
}

ksr2_result ksr2_draw_line(ksr2_contexthandle handle, int x1, int y1, int x2, int y2, unsigned int thickness, ksr2_rgba_color color)
{
	if (thickness == 0u)
	{
		return K15_RENDERER_2D_RESULT_SUCCESS;
	}

	if (x1 == x2 || y1 == y2)
	{
		return K15_RENDERER_2D_RESULT_SUCCESS;
	}

	ksr2_context* pContext = (ksr2_context*)handle;
	
	return K15_RENDERER_2D_RESULT_SUCCESS;
}

#endif // K15_SOFTWARE_RENDERER_2D_IMPLEMENTATION
#endif // _K15_SOFTWARE_RENDERER_2D_H_