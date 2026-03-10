cmake_minimum_required(VERSION 3.29)

project(bc7enc)

option(BC7ENC_BUILD_X64 "build 64-bit" TRUE)
option(BC7ENC_SUPPORT_BC7E "support BC7E (requires ispc)" FALSE)
option(BC7ENC_COMPILE_AS_LIBRARY "compile to dll instead of exe" FALSE)
option(BC7ENC_SHARED_LIBRARY "should we compile a shared library if compiling as library" TRUE)

message("Initial BUILD_X64=${BC7ENC_BUILD_X64}")
message("Initial CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
message("Initial BC7ENC_SUPPORT_BC7E=${BC7ENC_SUPPORT_BC7E}")

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release )
endif( NOT CMAKE_BUILD_TYPE )

message( ${PROJECT_NAME} " build type: " ${CMAKE_BUILD_TYPE} )

if (BC7ENC_BUILD_X64)
	message("Building 64-bit")
else()
	message("Building 32-bit")
endif(BC7ENC_BUILD_X64)

if (BC7ENC_SUPPORT_BC7E)
	if (NOT MSVC)
		set(ISPC_OBJECT_FILES ${CMAKE_SOURCE_DIR}/bc7e.o ${CMAKE_SOURCE_DIR}/bc7e_avx.o ${CMAKE_SOURCE_DIR}/bc7e_sse2.o ${CMAKE_SOURCE_DIR}/bc7e_sse4.o ${CMAKE_SOURCE_DIR}/bc7e_avx2.o)
		add_custom_command(OUTPUT ${ISPC_OBJECT_FILES}
			COMMAND ispc -g -O2 ${CMAKE_CURRENT_SOURCE_DIR}/bc7e.ispc -o ${CMAKE_CURRENT_SOURCE_DIR}/bc7e.o -h ${CMAKE_CURRENT_SOURCE_DIR}/bc7e_ispc.h --target=sse2,sse4,avx,avx2 --opt=fast-math --opt=disable-assertions
										 	DEPENDS bc7e.ispc)
	else()
		set(ISPC_OBJECT_FILES ${CMAKE_SOURCE_DIR}/bc7e.obj ${CMAKE_SOURCE_DIR}/bc7e_avx.obj ${CMAKE_SOURCE_DIR}/bc7e_sse2.obj ${CMAKE_SOURCE_DIR}/bc7e_sse4.obj ${CMAKE_SOURCE_DIR}/bc7e_avx2.obj)
		add_custom_command(OUTPUT ${ISPC_OBJECT_FILES}
			COMMAND ispc -g -O2 ${CMAKE_CURRENT_SOURCE_DIR}/bc7e.ispc -o ${CMAKE_CURRENT_SOURCE_DIR}/bc7e.obj -h ${CMAKE_CURRENT_SOURCE_DIR}/bc7e_ispc.h --target=sse2,sse4,avx,avx2 --opt=fast-math --opt=disable-assertions
										 	DEPENDS bc7e.ispc)
	endif()
endif()

# -fno-strict-aliasing shouldn't be necessary, it's here because that is what MSVC uses by default and that's what I've tested with the most.
if (NOT MSVC)
	set(GCC_COMPILE_FLAGS "-fno-strict-aliasing -Wall -Wextra")
	if (NOT BC7ENC_BUILD_X64)
		set(GCC_COMPILE_FLAGS "${GCC_COMPILE_FLAGS} -m32")
	endif()
endif()	

set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${GCC_COMPILE_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE  "${CMAKE_C_FLAGS_RELEASE} ${GCC_COMPILE_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${GCC_COMPILE_FLAGS} -D_DEBUG")

if (BC7ENC_SUPPORT_BC7E)
	set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COMPILE_FLAGS} -DBC7ENC_SUPPORT_BC7E=1")
else()
	set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COMPILE_FLAGS}")
endif()

set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} ${GCC_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${GCC_COMPILE_FLAGS} -D_DEBUG")

set (BC7ENC_SRC_LIST ${COMMON_SRC_LIST}
	bc7enc.cpp
	bc7enc.h
	bc7decomp.cpp
	bc7decomp_ref.cpp
	bc7decomp.h
	lodepng.cpp	
	lodepng.h
	rgbcx.cpp
	rgbcx.h
	rgbcx_table4.h
	rgbcx_table4_small.h
	utils.cpp
	utils.h
	miniz.h
	dds_defs.h
	ert.cpp
	ert.h
	rdo_bc_encoder.cpp
	rdo_bc_encoder.h
	bc7e.ispc
	)

if (NOT BC7ENC_COMPILE_AS_LIBRARY)
	set (BC7ENC_SRC_LIST ${BC7ENC_SRC_LIST} test.cpp)
else()
	set (BC7ENC_SRC_LIST ${BC7ENC_SRC_LIST} libbc7enc.cpp libbc7enc.h)
endif()

if (NOT BC7ENC_COMPILE_AS_LIBRARY)
	message("Compiling as executable")
	add_executable(bc7enc ${BC7ENC_SRC_LIST} ${ISPC_OBJECT_FILES})
else()
	if (BC7ENC_SHARED_LIBRARY)
		message("Compiling as shared library")
		add_library(bc7enc SHARED ${BC7ENC_SRC_LIST} ${ISPC_OBJECT_FILES})
	else()
		message("Compiling as static library")
		add_library(bc7enc STATIC ${BC7ENC_SRC_LIST} ${ISPC_OBJECT_FILES})
	endif(BC7ENC_SHARED_LIBRARY)
endif()

find_package(OpenMP)
if (OpenMP_FOUND)
	message("Found OpenMP.")
	target_link_libraries(bc7enc PRIVATE OpenMP::OpenMP_C OpenMP::OpenMP_CXX)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
	message("Could not find OpenMP. Compiling without.")
endif(OpenMP_FOUND)

if (NOT MSVC)
	target_link_libraries(bc7enc PRIVATE m)
endif()


