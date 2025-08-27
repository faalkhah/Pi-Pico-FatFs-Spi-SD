# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/jalil/Raspbery/pi-pico/pico-sdk/tools/pioasm"
  "/Users/jalil/prj/Electronic/pico/examples/my_sd_card_py/src/build/pioasm"
  "/Users/jalil/prj/Electronic/pico/examples/my_sd_card_py/src/build/pioasm-install"
  "/Users/jalil/prj/Electronic/pico/examples/my_sd_card_py/src/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/tmp"
  "/Users/jalil/prj/Electronic/pico/examples/my_sd_card_py/src/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp"
  "/Users/jalil/prj/Electronic/pico/examples/my_sd_card_py/src/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src"
  "/Users/jalil/prj/Electronic/pico/examples/my_sd_card_py/src/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/jalil/prj/Electronic/pico/examples/my_sd_card_py/src/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/jalil/prj/Electronic/pico/examples/my_sd_card_py/src/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
