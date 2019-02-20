cmake_minimum_required(VERSION 2.8)

project(Strategy)

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf)

add_executable(${PROJECT_NAME}
    "main.cpp"
    "SUnit.cpp"
    "SNode.cpp"
    "SNodeGraph.cpp"
    "SRenderer.cpp"
    "SBuilding.cpp"
    "SLoader.cpp"
    "SGameManager.cpp"
    "SSprite.cpp"
    "SPlayer.cpp"
    "SGameInput.cpp"
)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES} pthread)
