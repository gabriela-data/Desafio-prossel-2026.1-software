cmake_minimum_required(VERSION 3.14)
project(SimuladorFutebol)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Habilita o log de download
set(FETCHCONTENT_QUIET FALSE)

# Fetch SFML via URL 
FetchContent_Declare(
    SFML
    URL https://github.com/SFML/SFML/archive/refs/tags/2.6.1.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# Fetch BehaviorTree.CPP via URL 
FetchContent_Declare(
    BehaviorTreeCPP
    URL https://github.com/BehaviorTree/BehaviorTree.CPP/archive/refs/tags/4.8.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(SFML_BUILD_AUDIO FALSE)
set(SFML_BUILD_NETWORK FALSE)
set(SFML_BUILD_DOC FALSE)
set(SFML_BUILD_EXAMPLES FALSE)

# Opções para BehaviorTree.CPP
set(BTCPP_EXAMPLES FALSE CACHE BOOL "" FORCE)
set(BUILD_TESTING FALSE CACHE BOOL "" FORCE)
set(BTCPP_UNIT_TESTS FALSE CACHE BOOL "" FORCE)
set(BTCPP_SAMPLES FALSE CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SFML BehaviorTreeCPP)

# Busca arquivos do simulador e solução
file(GLOB_RECURSE SIMULADOR_SOURCES "simulador/src/*.cpp")
file(GLOB_RECURSE SOLUCAO_SOURCES "solucao/src/*.cpp")

# Add Executable
add_executable(SimuladorFutebol
    ${SIMULADOR_SOURCES}
    ${SOLUCAO_SOURCES}
)

# Inclui os diretórios de headers
target_include_directories(SimuladorFutebol PRIVATE 
    simulador/include 
    solucao/include
)

target_link_libraries(SimuladorFutebol PRIVATE 
    sfml-graphics 
    sfml-window 
    sfml-system
    behaviortree_cpp
)
