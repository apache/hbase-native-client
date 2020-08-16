36c36
< find_package(folly CONFIG REQUIRED)
---
> find_package(Folly  REQUIRED)
46a47,54
> 
> if(NOT APPLE)
>   if(NOT WIN32)
>   	set(WHOLE_ARCHIVE "-Wl,--whole-archive")
>     set(NO_WHOLE_ARCHIVE "-Wl,--no-whole-archive")
>   endif()
> endif()
> 
220a229
>     ${WHOLE_ARCHIVE}
221a231
>     ${NO_WHOLE_ARCHIVE}

