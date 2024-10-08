cmake_minimum_required(VERSION 3.22.0)
project(libaenigma VERSION 7.0.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(aenigma SHARED 
./src/cryptography/AsymmetricKey.cc
./src/cryptography/AsymmetricEvpCipherContext.cc
./src/cryptography/SymmetricEvpCipherContext.cc
./src/cryptography/EvpMdContext.cc
./src/cryptography/CryptoContext.cc
./src/cryptography/File.cc
./src/cryptography/Encryption.cc
./src/cryptography/Factories.cc
./src/cryptography/OnionParsing.cc
./src/cryptography/Utils.cc
./src/cryptography/OnionBuilding.cc
)

add_library(aenigma7 STATIC 
./src/cryptography/AsymmetricKey.cc
./src/cryptography/AsymmetricEvpCipherContext.cc
./src/cryptography/SymmetricEvpCipherContext.cc
./src/cryptography/EvpMdContext.cc
./src/cryptography/CryptoContext.cc
./src/cryptography/File.cc
./src/cryptography/Encryption.cc
./src/cryptography/Factories.cc
./src/cryptography/OnionParsing.cc
./src/cryptography/Utils.cc
./src/cryptography/OnionBuilding.cc
)

set_target_properties(aenigma PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 7)
target_link_libraries(aenigma crypto)

if(ANDROID)
    message(STATUS "Skipping kernelkeys library when building for platform Android.")
    message(STATUS "Install rule will not be defined when building for platform Android.")
else ()
    add_library(aenigma-kernelkeys SHARED ./src/kernelkeys/KernelKeys.c)
    add_library(aenigma-kernelkeys1 STATIC ./src/kernelkeys/KernelKeys.c)
    set_target_properties(aenigma-kernelkeys PROPERTIES VERSION 1.0.0 SOVERSION 1)
    target_link_libraries(aenigma-kernelkeys keyutils)
    add_executable(aenigma_test ./tests/AenigmaTest.cc)
    target_link_libraries(aenigma_test aenigma)

    install(FILES
            ${CMAKE_BINARY_DIR}/libaenigma.so
            ${CMAKE_BINARY_DIR}/libaenigma.so.7
            ${CMAKE_BINARY_DIR}/libaenigma.so.7.0.0
            ${CMAKE_BINARY_DIR}/libaenigma-kernelkeys.so
            ${CMAKE_BINARY_DIR}/libaenigma-kernelkeys.so.1
            ${CMAKE_BINARY_DIR}/libaenigma-kernelkeys.so.1.0.0
            DESTINATION "/usr/local/lib")
endif ()
