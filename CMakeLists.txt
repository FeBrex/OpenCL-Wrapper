cmake_minimum_required(VERSION 2.8)

project(OpenCL-Wrapper)

find_library(OPENCL_LIBRARIES OpenCL HINTS "C:\\Program Files\\NVIDIA GPU Computing Toolkit\\CUDA\\v5.5\\lib\\Win32")
# find_library(OPENCL_LIBRARIES cuda)
# set(OPENCL_LIBRARIES /usr/local/cuda-6.0/targets/x86_64-linux/lib/libOpenCL.so)

find_path(OPENCL_INCLUDE_DIR CL/cl.h HINTS 
	/usr/include 
	/opt/AMDAPP/include 
	/usr/local/cuda/include
  "C:\\Program Files\\NVIDIA GPU Computing Toolkit\\CUDA\\v5.5\\include"
	)

include_directories(SYSTEM ${OPENCL_INCLUDE_DIR})
include_directories(Code/inc)


if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-Wall -Wextra -Werror -pedantic -pedantic-errors -O2
    -Wno-deprecated-declarations
  )

  if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++11 -Weffc++)
  endif()
elseif(MSVC)
  add_definitions(/W3 /WX)

  # Activate CTP for VS 2012.
  if( MSVC_VERSION LESS 1700 ) # VC10-/VS2010-
    message(FATAL_ERROR "The project requires C++11 features. "
      "You need at least Visual Studio 11 (Microsoft Visual Studio 2012), "
      "with Microsoft Visual C++ Compiler Nov 2012 CTP (v120_CTP_Nov2012).")
  elseif( MSVC_VERSION EQUAL 1700 ) # VC11/VS2012
    message(STATUS "VC11: using Microsoft Visual Studio 2012 "
      "with Microsoft Visual C++ Compiler Nov 2012 CTP (v120_CTP_Nov2012)" )
    set(CMAKE_GENERATOR_TOOLSET "v120_CTP_Nov2012" CACHE STRING "Platform Toolset" FORCE)
  else() # VC12+, assuming C++11 supported.
    message(AUTHOR_WARNING "Assuming C++11 support for VC12+")
  endif() 
endif()

set(OclWrapper_HDRS
  Code/inc/ocl_buffer.h
  Code/inc/ocl_context.h
  Code/inc/ocl_device.h
  Code/inc/ocl_device_type.h
  Code/inc/ocl_event.h
  Code/inc/ocl_event_list.h
  Code/inc/ocl_image.h
  Code/inc/ocl_kernel.h
  Code/inc/ocl_memory.h
  Code/inc/ocl_platform.h
  Code/inc/ocl_program.h
  Code/inc/ocl_query.h
  Code/inc/ocl_queue.h
  Code/inc/ocl_sampler.h
  Code/inc/ocl_wrapper.h
  Code/inc/utl_args.h
  Code/inc/utl_assert.h
  Code/inc/utl_dim.h
  Code/inc/utl_flags.h
  Code/inc/utl_matrix.h
  Code/inc/utl_profile_pass.h
  Code/inc/utl_profile_pass_manager.h
  Code/inc/utl_storage.h
  Code/inc/utl_stream.h
  Code/inc/utl_timer.h
  Code/inc/utl_type.h
  Code/inc/utl_utils.h
)

set(OclWrapper_SRCS
  Code/src/ocl_buffer.cpp
  Code/src/ocl_context.cpp
  Code/src/ocl_device.cpp
  Code/src/ocl_device_type.cpp
  Code/src/ocl_event.cpp
  Code/src/ocl_event_list.cpp
  Code/src/ocl_image.cpp
  Code/src/ocl_kernel.cpp
  Code/src/ocl_memory.cpp
  Code/src/ocl_platform.cpp
  Code/src/ocl_program.cpp
  Code/src/ocl_query.cpp
  Code/src/ocl_queue.cpp
  Code/src/ocl_sampler.cpp
  Code/src/utl_args.cpp
  Code/src/utl_dim.cpp
  Code/src/utl_profile_pass.cpp
  Code/src/utl_profile_pass_manager.cpp
  Code/src/utl_storage.cpp
  Code/src/utl_stream.cpp
  Code/src/utl_timer.cpp
  Code/src/utl_type.cpp
)

add_library(OclWrapper STATIC ${OclWrapper_HDRS} ${OclWrapper_SRCS})
set_target_properties(OclWrapper PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Code/lib)

add_executable(platform Tutorial/1.platform/platform.cpp)
target_link_libraries(platform OclWrapper ${OPENCL_LIBRARIES})

add_executable(context Tutorial/2.context/context.cpp)
target_link_libraries(context OclWrapper ${OPENCL_LIBRARIES})

add_executable(queue Tutorial/3.queue/queue.cpp)
target_link_libraries(queue OclWrapper ${OPENCL_LIBRARIES})

add_executable(program Tutorial/4.program/program.cpp)
target_link_libraries(program OclWrapper ${OPENCL_LIBRARIES})

add_executable(buffer Tutorial/5.buffer/buffer.cpp)
target_link_libraries(buffer OclWrapper ${OPENCL_LIBRARIES})

add_executable(kernel Tutorial/6.kernel/kernel.cpp)
target_link_libraries(kernel OclWrapper ${OPENCL_LIBRARIES})

add_executable(events Tutorial/7.events/events.cpp)
target_link_libraries(events OclWrapper ${OPENCL_LIBRARIES})

add_executable(matrix Tutorial/8.matrix/matrix.cpp)
target_link_libraries(matrix OclWrapper ${OPENCL_LIBRARIES})

add_executable(minimum Tutorial/9.minimum/minimum.cpp)
target_link_libraries(minimum OclWrapper ${OPENCL_LIBRARIES})

add_executable(image Tutorial/10.image/image.cpp)
target_link_libraries(image OclWrapper ${OPENCL_LIBRARIES})

add_executable(profile Tutorial/11.profile/profile.h Tutorial/11.profile/profile.cpp)
target_link_libraries(profile OclWrapper ${OPENCL_LIBRARIES})

