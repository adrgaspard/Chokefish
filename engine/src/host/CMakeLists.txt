set(HOST_NAME host)

file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
)

if(SRC_FILES)
    add_library(${HOST_NAME} ${SRC_FILES})
    target_include_directories(${HOST_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
else()
    message(STATUS "Aucun fichier source trouvé pour le ${HOST_NAME}")
endif()