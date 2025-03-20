
add_library(SpaceBattle INTERFACE
  "ICommand.h"
)

add_subdirectory (IoCContainer)

target_link_libraries(SpaceBattle INTERFACE
	IoC
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SpaceBattle PROPERTY CXX_STANDARD 20)
endif()

