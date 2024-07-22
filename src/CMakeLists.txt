set(HDR)
set(SRC main.c)

# Create a library target for each prime defined in parent PRIMES variable

set(COMP_ASM)
if (OPT_ASM MATCHES YES)
    message(FATAL_ERROR "Unsupported operation") #TODO: implement Karatsuba
    set(COMP_ASM ${COMP_ALG} -D_OPT_ASM_)
endif()

message(STATUS "Creating library target ${CMAKE_PROJECT_NAME}")
set(SRCP)

#Check by architecture

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
    # ARM
    set(SRCP architectures/aarch64_neon/neon_a72.c)

elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "architectures/x86_64")

    # Intel
    if (OPT_ASM MATCHES YES)
        message(FATAL_ERROR "Unsupported operation") #TODO: implement Karatsuba
        #set(SRCP asm/p${P}-opt.s)
    else()
        message(FATAL_ERROR "Not implemented architecture")
        set(SRCP architectures/x86_64/) #TODO: Implement
    endif()

else()
    # Unsupported architecture
    message(FATAL_ERROR "Unsupported architecture")
endif()

#add_library(${PROJECT_NAME} STATIC ${SRC} ${SRCP})
#

#target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>")

message(STATUS "Creating executable: ${TEST_APP}")


add_executable(executable ${SRC} ${SRCP})

target_compile_options(executable PUBLIC "$<$<CONFIG:DEBUG>:${COMMON_FLAGS}>")

