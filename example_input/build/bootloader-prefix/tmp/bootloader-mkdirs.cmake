# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Juan/semestre_8/rts/esp_idf/v5.3.1/esp-idf/components/bootloader/subproject"
  "D:/Juan/semestre_8/rts/Projects/example_input/build/bootloader"
  "D:/Juan/semestre_8/rts/Projects/example_input/build/bootloader-prefix"
  "D:/Juan/semestre_8/rts/Projects/example_input/build/bootloader-prefix/tmp"
  "D:/Juan/semestre_8/rts/Projects/example_input/build/bootloader-prefix/src/bootloader-stamp"
  "D:/Juan/semestre_8/rts/Projects/example_input/build/bootloader-prefix/src"
  "D:/Juan/semestre_8/rts/Projects/example_input/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Juan/semestre_8/rts/Projects/example_input/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Juan/semestre_8/rts/Projects/example_input/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
