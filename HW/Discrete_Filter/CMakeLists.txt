cmake_minimum_required(VERSION 3.10)

# set the project name
project(Discrete_Filter)

set(RING_BUFFER_DIR ../Ring_Buffer)

# add the executable
add_executable(disc_filter_eval main.c Filter.c ${RING_BUFFER_DIR}/Ring_Buffer.c)

# add include directory for Ring Buffer
target_include_directories(disc_filter_eval PRIVATE  ${RING_BUFFER_DIR} )

