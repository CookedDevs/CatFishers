# Recursively gather all .cpp and .h files in the src directory.
file(GLOB_RECURSE CLIENT_FILES
    "${CMAKE_SOURCE_DIR}/Server/src/**.cpp"
    "${CMAKE_SOURCE_DIR}/Server/src/**.h"
)

# Create the executable.
add_executable(Server ${CLIENT_FILES})

target_link_libraries(Server 
    PRIVATE 
    CatCore
    raylib
)

target_include_directories(Server PRIVATE ${CMAKE_SOURCE_DIR}/Server/src ${CORE_LIB_DIR})

add_dependencies(Server CatCore raylib ENet6)

# Function to automatically assign source groups based on folder structure.
function(assign_source_groups)
    foreach(file IN LISTS ARGN)
        # Ensure we have an absolute file path.
        get_filename_component(file_abs "${file}" ABSOLUTE)
        # Get the file path relative to the src directory.
        file(RELATIVE_PATH rel_path "${CMAKE_SOURCE_DIR}/Server/src" "${file_abs}")
        # Get the directory part (if any) from the relative path.
        get_filename_component(dir_path "${rel_path}" DIRECTORY)
        # Replace forward slashes with backslashes for Visual Studio.
        string(REPLACE "/" "\\" group "${dir_path}")
        # Create the source group for the file.
        source_group("${group}" FILES "${file_abs}")
    endforeach()
endfunction()

# Call the function by expanding the CLIENT_FILES list.
assign_source_groups(${CLIENT_FILES})