cmake_minimum_required(VERSION 3.0)
project(shaco C)

if( DEFINED USE_SSL)
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)
add_compile_definitions(USE_SSL)
endif()


if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("Compiling in debug mode")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og")
    add_compile_definitions(DEBUG)
else()
    message("Compiling in release mode")
    #error on compile -static: openssl
    #    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -Oz -s -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fpic -static -static-libgcc -Oz -s  -Wno-write-strings -fno-exceptions -fmerge-all-constants -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s -Wl,-Bstatic ")

    #    add_compile_definitions(DEBUG)
    add_compile_definitions(Release)
endif()


set(
    SHACO_SOURCES

    Common/ministd.c
    Common/Base64.c
    Common/shaco_stdlib.c
    Common/Default.c
    Common/Network.c

    Config/OSConfig.c
    Config/Settings.c

    Helpers/Protect.c
    Helpers/Http.c

    Protocol/Packer.c

    Core/Core.c
    Core/Communication.c
    Core/Commands.c
)


add_executable( ${PROJECT_NAME} shaco.c ${SHACO_SOURCES})

if(OpenSSL_FOUND)
    message("Openssl found")
    target_link_libraries( ${PROJECT_NAME} PRIVATE OpenSSL::Crypto OpenSSL::SSL)
else()
    message("Openssl Not found")
endif()
