add_executable(sandbox main.cpp)
target_link_libraries(sandbox PRIVATE Renderium)

set_target_properties(sandbox PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

#add_custom_command(TARGET sandbox POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        ${CMAKE_SOURCE_DIR}/src/graphics/shader
#        $<TARGET_FILE_DIR:sandbox>/../shader
#}