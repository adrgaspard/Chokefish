set(MODULE_NAME core)

file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
)

if(SRC_FILES)
    add_library(${MODULE_NAME} ${SRC_FILES})
    target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
else()
    message(STATUS "Aucun fichier source trouvé pour le module ${MODULE_NAME}")
endif()