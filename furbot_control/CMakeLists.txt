cmake_minimum_required(VERSION 2.8.3)
project(furbot_control)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  geometry_msgs
  gazebo_msgs
  tf
  sensor_msgs
  nav_msgs
  move_base_msgs
)

catkin_package()

include_directories(${catkin_INCLUDE_DIRS})

add_executable(aggregate_topics src/aggregate_topics.cpp)
target_link_libraries(aggregate_topics ${catkin_LIBRARIES})
add_executable(teleop_furbot src/teleop_furbot.cpp)
target_link_libraries(teleop_furbot ${catkin_LIBRARIES})
add_executable(furbot_odometry src/furbot_odometry.cpp)
target_link_libraries(furbot_odometry ${catkin_LIBRARIES})
