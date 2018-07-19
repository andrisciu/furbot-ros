#include "ros/ros.h"
#include <std_msgs/Float64.h>
#include <geometry_msgs/Twist.h>

#define ANGULAR_RATIO 0.3
#define VELOCITY_RATIO 4

int check1 = 0;
std_msgs::Float64 lin;
std_msgs::Float64 ang;
std_msgs::Float64 old_ang;
std_msgs::Float64 old_lin;

void key_callback(const geometry_msgs::Twist::ConstPtr &value)
{

    lin.data = value->linear.x;

    if (!(abs(old_ang.data + value->angular.z) > 2))
    {
        ang.data = old_ang.data + value->angular.z;
        old_ang.data = ang.data;
    }

    check1 = 1;
}

int main(int argc, char **argv)
{
    old_ang.data = 0.0;
    old_lin.data = 0.0;

    ros::init(argc, argv, "aggregate_topics");

    ros::NodeHandle pub;
    ros::NodeHandle sub;

    ros::Publisher front = pub.advertise<std_msgs::Float64>("/mybot/front", 1000);
    ros::Publisher rear = pub.advertise<std_msgs::Float64>("/mybot/rear", 1000);
    ros::Subscriber key = sub.subscribe("/turtle1/cmd_vel", 1000, key_callback);

    ros::Rate loop_rate(50);
    while (ros::ok())
    {
        if (check1 == 1)
        {
            try
            {
                lin.data = lin.data * VELOCITY_RATIO;
                ang.data = ang.data * ANGULAR_RATIO;
                std::cout << "VEL: " << lin.data << "\tANG: " << ang.data << "\n";
                front.publish(ang);
                rear.publish(lin);
            }
            catch (const char *msg)
            {
                ROS_INFO("SOMETHING WENT WRONG!! are you sure that the controllers have strated correctly?");
            }

            check1 = 0;
        }

        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}
