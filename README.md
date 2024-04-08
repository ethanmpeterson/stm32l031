# stm32l031
Example Bare Metal Project for the Nucleo STM32L031 Development Board.

# Notes
- Linker Script was generated in STM32Cube IDE
- FreeRTOS
  - Generated project showed heap_4 is the correct selection for this micro.
- Assembly startup code matching the linker script is also found in the Cube IDE generated project
- Take an example clang complete file from my bare-metal repo. Also check the
  dotfiles repo for the correct emacs config to get autocomplete working.
