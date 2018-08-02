#include "ros/ros.h"
#include <std_msgs/Float64.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>

int check = 0;
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

    check = 1;
}

void JoyCallback(const sensor_msgs::Joy::ConstPtr &joy)
{

    geometry_msgs::Twist bot;
    bot.angular.z = joy->axes[angular_joy];
    bot.linear.x = joy->axes[linear_joy];

    lin.data = bot.linear.x;
    ang.data = bot.angular.z;
    check = 1;
}

void CmdVelCallback(const geometry_msgs::Twist::ConstPtr &vel)
{

    geometry_msgs::Twist bot;
    bot.angular.z = vel->angular.z;
    bot.linear.x = vel->linear.x;

    lin.data = bot.linear.x;
    ang.data = bot.angular.z;
    check = 1;
}

int main(int argc, char **argv)
{
    angular_joy = 0;
    linear_joy = 1;
    old_ang.data = 0.0;
    old_lin.data = 0.0;

    ros::init(argc, argv, "teleop_furbot");

    ros::NodeHandle pub;
    ros::NodeHandle sub;

    ros::Publisher front = pub.advertise<std_msgs::Float64>("/furbot/front", 1000);
    ros::Publisher rear = pub.advertise<std_msgs::Float64>("/furbot/rear", 1000);
    ros::Subscriber joy = sub.subscribe<sensor_msgs::Joy>("joy", 10, JoyCallback);
    ros::Subscriber key = sub.subscribe<geometry_msgs::Twist>("turtle1/cmd_vel", 10, key_callback);
    ros::Subscriber vel = sub.subscribe<geometry_msgs::Twist>("cmd_vel", 100, CmdVelCallback);

    ros::Rate loop_rate(50);
    double VELOCITY_RATIO = 10;
    double ANGULAR_RATIO = 0.4;

    while (ros::ok())
    {
        if (check == 1)
        {
            try
            {
                std::cout << "VEL: " << lin.data << "\tANG: " << ang.data << "\n";
                lin.data = lin.data * VELOCITY_RATIO;
                ang.data = ang.data * ANGULAR_RATIO;
                front.publish(ang);
                rear.publish(lin);
            }
            catch (const char *msg)
            {
                ROS_INFO("SOMETHING WENT WRONG!! are you sure that the controllers have strated correctly?");
                std::cout << "ERR: " << msg;
            }

            check = 0;
        }

        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}
