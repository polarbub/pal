# This file is the build system for pal
# Copyright (C) 2024 polarbub

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as published by
# the Free Software Foundation.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# Please note that this requires ArduinoCoreCMake, but does not add it as a dependency because your upper level project definitly will

# Declare the minimum required CMake version
cmake_minimum_required(VERSION 3.28)

#Enable versioning
cmake_policy(SET CMP0048 NEW)

# set library metadata
project(pal VERSION 0.0.1 DESCRIPTION "polarbub's Arduino library" LANGUAGES CXX)

#Remove header from SRC_FILES regex:
#' src/[^\n ]*\.h'
# Define a variable containing a list of source files for the project
set(SRC_FILES
        src/pal.cpp)

# Define the build target for the library
add_library(${PROJECT_NAME} ${SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} ArduinoCoreCMake)

#target is used to only effect this library.
target_include_directories(${PROJECT_NAME} PUBLIC src/include)