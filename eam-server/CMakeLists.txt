# 后端CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(asset_backend VERSION 0.1.0 LANGUAGES C CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
    -O2
    -Wall
    -Wextra
   # -Werror
    -fstack-protector
    -fPIC
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(
        -O0
        -g
        -DDEBUG
        -fsanitize=address
        -fno-omit-frame-pointer
    )
endif()


#预处理定义
add_definitions(
    -D_FORTIFY_SOURCE=2
  #  -DNDEBUG
    -DPROJECT_VERSION="${PROJECT_VERSION}"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# 依赖查找
#find_package(pqxx REQUIRED)
find_package(OpenSSL REQUIRED)
#find_package(cpp-jwt REQUIRED)
# find_package(Threads REQUIRED)
#find_package(jsoncpp REQUIRED)

set(BCRYPT_INCLUDE_DIR "/usr/local/include")
set(BCRYPT_LIBRARY "/usr/local/lib/libbcrypt.so")

set(CMAKE_PREFIX_PATH "/home/drogon")
set(Drogon_DIR "/home/drogon/lib/cmake/Drogon")
find_package(Drogon REQUIRED)
#find_package(bcrypt REQUIRED)


include_directories(/usr/local/include)

#包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    # ${Crow_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/controllers
    ${CMAKE_CURRENT_SOURCE_DIR}/include/database
    ${CMAKE_CURRENT_SOURCE_DIR}/include/services
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/include/models
)

message(STATUS "CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")

# 源文件
set(SOURCES
    src/main.cpp
    src/controllers/AuthController.cpp
    #src/controllers/UserController.cpp
    src/database/UserDAO.cpp
    src/services/AuthService.cpp
    #src/services/UserService.cpp
    src/utils/JwtUtils.cpp
    src/utils/json_util.cpp
    src/utils/PasswordUtils.cpp
    src/utils/logger.cpp
    src/models/ConfigManager.cpp
    # src/middleware/auth_middleware.cpp
)

# 可执行文件
add_executable(asset_backend ${SOURCES})

set_target_properties(asset_backend PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
)

# 链接库
target_link_libraries(asset_backend
    PRIVATE
   #pqxx::pqxx
    OpenSSL::SSL
    OpenSSL::Crypto
   # cpp-jwt::cpp-jwt

    # Threads::Threads
    # jsoncpp_lib
    Drogon::Drogon
   # ${BCRYPT_LIBRARY}
)

#复制配置文件
set(CONFIG_DIR ${CMAKE_SOURCE_DIR}/config)
set(CONFIG_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/config)
file(MAKE_DIRECTORY ${CONFIG_OUTPUT_DIR})

#复制配置文件到构建目录（开发时使用）
if(EXISTS ${CONFIG_DIR})
    file(GLOB CONFIG_FILES ${CONFIG_DIR}/*.json)
    foreach(config_file ${CONFIG_FILES})
        get_filename_component(config_name ${config_file} NAME)
        configure_file(
            ${config_file}
            ${CONFIG_OUTPUT_DIR}/${config_name}
            COPYONLY
        )
        message(STATUS "Copying config: " ${config_name})
    endforeach()
else()
    message(WARNING "Config directory not found: " ${CONFIG_DIR})
endif()
#安装配置文件到系统目录（部署时使用）
install(DIRECTORY ${CONFIG_DIR}/
    DESTINATION ${CMAKE_INSTALL_BINDIR}/config
    FILES_MATCHING PATTERN "*.json"
)

target_include_directories(asset_backend PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
   # ${BCRYPT_INCLUDE_DIR}
)

#target_compile_definitions(asset_backend PRIVATE USE_BCRYPT)

# 安装
install(TARGETS asset_backend DESTINATION bin)