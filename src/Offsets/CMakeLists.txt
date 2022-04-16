add_library(Offsets STATIC
    Memory.cpp Memory.hpp
    Offsets.cpp Offsets.hpp)

target_link_libraries(Offsets minhook)
