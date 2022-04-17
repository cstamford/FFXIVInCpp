add_library(Offsets STATIC
    API.hpp
    Generated/Actions.cpp Generated/Actions.hpp
    Generated/Jobs.hpp
    Memory.cpp Memory.hpp
    Offsets.cpp Offsets.hpp)

target_link_libraries(Offsets minhook)
