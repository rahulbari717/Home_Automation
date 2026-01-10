# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/lenovo/esp/esp-idf/components/bootloader/subproject"
  "/home/lenovo/Documents/Home_Automation/firmware/esp32-common-components/esp_rtos/004_bin_semaphore/build/bootloader"
  "/home/lenovo/Documents/Home_Automation/firmware/esp32-common-components/esp_rtos/004_bin_semaphore/build/bootloader-prefix"
  "/home/lenovo/Documents/Home_Automation/firmware/esp32-common-components/esp_rtos/004_bin_semaphore/build/bootloader-prefix/tmp"
  "/home/lenovo/Documents/Home_Automation/firmware/esp32-common-components/esp_rtos/004_bin_semaphore/build/bootloader-prefix/src/bootloader-stamp"
  "/home/lenovo/Documents/Home_Automation/firmware/esp32-common-components/esp_rtos/004_bin_semaphore/build/bootloader-prefix/src"
  "/home/lenovo/Documents/Home_Automation/firmware/esp32-common-components/esp_rtos/004_bin_semaphore/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/lenovo/Documents/Home_Automation/firmware/esp32-common-components/esp_rtos/004_bin_semaphore/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/lenovo/Documents/Home_Automation/firmware/esp32-common-components/esp_rtos/004_bin_semaphore/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
