36c36,37
< find_package(folly CONFIG REQUIRED)
---
> find_package(Folly  REQUIRED)
> find_package(Boost REQUIRED)
46a48,55
> 
> if(NOT APPLE)
>   if(NOT WIN32)
>     set(WHOLE_ARCHIVE "-Wl,--whole-archive")
>     set(NO_WHOLE_ARCHIVE "-Wl,--no-whole-archive")
>   endif()
> endif()
> 
204a214
>     ${Boost_INCLUDE_DIRS}
216a227
>     ${Boost_LIBRARIES}
220a232
>     ${WHOLE_ARCHIVE}
221a234
>     ${NO_WHOLE_ARCHIVE}
