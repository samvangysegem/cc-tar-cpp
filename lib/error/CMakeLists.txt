# Requires CPM support
CPMAddPackage(
        NAME
        boost-leaf
        GIT_TAG
        boost-1.82.0
        GITHUB_REPOSITORY
        boostorg/leaf
)

add_library(error INTERFACE) 
target_include_directories(error INTERFACE include)
target_link_libraries(error INTERFACE boost_leaf)
