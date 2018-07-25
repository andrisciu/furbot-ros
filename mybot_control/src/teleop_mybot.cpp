#include "ros/ros.h"
#include <std_msgs/Float64.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>

#define ANGULAR_RATIO 0.3
#define VELOCITY_RATIO 7

int check1, check2 = 0;
std_msgs::Float64 lin;
std_msgs::Float64 ang;
std_msgs::Float64 old_ang;
std_msgs::Float64 old_lin;
int linear_joy, angular_joy;

void key_callback(const geometry_msgs::Twist::ConstPtr &value)
{

    if (!(abs(old_lin.data + value->linear.x) > 2))
    {
        lin.data = old_lin.data + value->linear.x;
        old_lin.data = lin.data;
    }

    lin.data = value->linear.x;

    if (!(abs(old_ang.data + value->angular.z) > 2))
    {
        ang.data = old_ang.data + value->angular.z;
        old_ang.data = ang.data;
    }

    check1 = 1;
}

void JoyCallback(const sensor_msgs::Joy::ConstPtr &joy)
{

    geometry_msgs::Twist twist_bot;
    twist_bot.angular.z = ANGULAR_RATIO * joy->axes[angular_joy];
    twist_bot.linear.x = VELOCITY_RATIO * joy->axes[linear_joy];

    lin.data = twist_bot.linear.x;
    ang.data = twist_bot.angular.z;
    check2 = 1;
}

int main(int argc, char **argv)
{
    angular_joy = 0;
    linear_joy = 1;
    old_ang.data = 0.0;
    old_lin.data = 0.0;

    ros::init(argc, argv, "aggregate_topics");

    ros::NodeHandle pub;
    ros::NodeHandle sub;

    ros::Publisher front = pub.advertise<std_msgs::Float64>("/mybot/front", 1000);
    ros::Publisher rear = pub.advertise<std_msgs::Float64>("/mybot/rear", 1000);
    ros::Subscriber joy = sub.subscribe<sensor_msgs::Joy>("joy", 10, JoyCallback);
    ros::Subscriber key = sub.subscribe("turtle1/cmd_vel", 1000, key_callback);

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
                std::cout << "ERR: " << msg;
            }

            check1 = 0;
        }

        if (check2 == 1)
        {

            try
            {
                std::cout << "VEL: " << lin.data << "\tANG: " << ang.data << "\n";
                front.publish(ang);
                rear.publish(lin);
            }
            catch (const char *msg)
            {
                ROS_INFO("SOMETHING WENT WRONG!! are you sure that the controllers have strated correctly?");
                std::cout << "ERR: " << msg;
            }

            check2 = 0;
        }

        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}
