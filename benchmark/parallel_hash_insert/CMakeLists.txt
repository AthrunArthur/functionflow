SET(PHASH_LIST parallel_hash_insert/ff/main.cpp
  parallel_hash_insert/ff/HashTable.cpp)
ADD_EXECUTABLE(phash ${PHASH_LIST})
TARGET_LINK_LIBRARIES(phash ff pthread) 
