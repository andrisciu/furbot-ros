cmake_minimum_required(VERSION 2.8.3)
project(mybot_control)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
)

catkin_package()

include_directories(${catkin_INCLUDE_DIRS})

add_executable(aggregate_topics src/aggregate_topics.cpp)
target_link_libraries(aggregate_topics ${catkin_LIBRARIES})

