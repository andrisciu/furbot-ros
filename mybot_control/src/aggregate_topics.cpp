#include "ros/ros.h"
#include <std_msgs/Float64.h>
#include <sstream>

int check1 = 0;
int check2 = 0;
std_msgs::Float64 val1;
std_msgs::Float64 val2;

void front_callback(const std_msgs::Float64::ConstPtr &value)
{
    val1.data = value->data;
    check1 = 1;
}
void rear_callback(const std_msgs::Float64::ConstPtr &value)
{
    val2.data = value->data;
    check2 = 1;
}

int main(int argc, char **argv)
{

    ros::init(argc, argv, "aggregate_topics");

    ros::NodeHandle pub1;
    ros::NodeHandle pub2;
    ros::NodeHandle sub;

    ros::Publisher pub_front_left = pub1.advertise<std_msgs::Float64>("/mybot/leftFrontWheel_position_controller/command", 1000);
    ros::Publisher pub_front_right = pub1.advertise<std_msgs::Float64>("/mybot/rightFrontWheel_position_controller/command", 1000);
    ros::Publisher pub_rear_right = pub2.advertise<std_msgs::Float64>("/mybot/rightRearWheel_velocity_controller/command", 1000);
    ros::Publisher pub_rear_left = pub2.advertise<std_msgs::Float64>("/mybot/leftRearWheel_velocity_controller/command", 1000);

    ros::Subscriber sub_front = sub.subscribe("/mybot/front", 1000, front_callback);
    ros::Subscriber sub_rear = sub.subscribe("/mybot/rear", 1000, rear_callback);

    ros::Rate loop_rate(50);
    ROS_INFO("WAITING FOR MESSAGES");

    while (ros::ok())
    {
        if (check1 == 1)
        {
            ROS_INFO("SETTING ORIENTATION");
            try
            {
                pub_front_left.publish(val1);
                pub_front_right.publish(val1);
            }
            catch(const char* msg){
                ROS_INFO("SOMETHING WENT WRONG!! are you sure that the controllers have strated correctly?");
            }

            check1 = 0;
        }

        if (check2 == 1)
        {
            ROS_INFO("SETTING VELOCITY");

            try
            {
                pub_rear_left.publish(val2);
                pub_rear_right.publish(val2);
            }
            catch(const char* msg){
                ROS_INFO("SOMETHING WENT WRONG!! are you sure that the controllers have strated correctly?");
            }

            check2 = 0;
        }

        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}
