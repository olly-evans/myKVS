
find_package(Threads REQUIRED)

file(GLOB_RECURSE BENCH_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/bench_*.cpp")

add_compile_options(-pg)

foreach(bench_src IN LISTS BENCH_SOURCES)
    get_filename_component(bench_name ${bench_src} NAME_WE)
    add_executable(${bench_name} ${bench_src})
    target_include_directories(${bench_name} PUBLIC ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(${bench_name} 
    PRIVATE Threads::Threads
    PUBLIC app_lib)
endforeach()