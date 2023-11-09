INCLUDE_DIRECTORIES(${ROSS_SOURCE_DIR} ${ROSS_BINARY_DIR})

SET(coffeeshop_srcs
coffeeshop.c		coffeeshop.h)

ADD_EXECUTABLE(coffeeshop ${coffeeshop_srcs})

TARGET_LINK_LIBRARIES(coffeeshop ROSS m)
