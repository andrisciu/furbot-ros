#include "ros/ros.h"
#include <std_msgs/Float64.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>
#include <math.h>

#define L 0.5
#define T 0.4
#define wheel_radius 0.09
#define VELOCITY_RATIO 10
#define ANGULAR_RATIO 0.4

int check = 0;
std_msgs::Float64 lin;
std_msgs::Float64 ang;
std_msgs::Float64 old_ang;
std_msgs::Float64 old_lin;
std_msgs::Float64 steer[2];
int linear_joy, angular_joy;

void calc_steer_angle(double delta){

    if( delta == 0.0 ){
        delta= 0.00001;
    }

    double radius;
    double delta_zero;
    double delta_one;
    
    radius = L / tan(delta);
    delta_zero = atan(L/(radius + (T/2)));

    steer[0].data = delta_zero;

    radius = L / tan(delta);
    delta_one = atan(L/(radius - (T/2)));

    steer[1].data = delta_one;

    return;
}

void calc_wheel_spin(double vel){

    double spin = (L/wheel_radius)*vel;
    lin.data = spin;

    return;
}


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

    ang.data = joy->axes[angular_joy];
    lin.data = joy->axes[linear_joy];

    calc_steer_angle(ang.data);
    calc_wheel_spin(lin.data);

    check = 2;
}

void CmdVelCallback(const geometry_msgs::Twist::ConstPtr &vel)
{

    ang.data = vel->angular.z;
    lin.data = vel->linear.x;
    
    calc_steer_angle(ang.data);
    calc_wheel_spin(lin.data);

    check = 2;
}

int main(int argc, char **argv)
{
    angular_joy = 0;
    linear_joy = 1;
    old_ang.data = 0.0;
    old_lin.data = 0.0;

    ros::init(argc, argv, "control_furbot");

    ros::NodeHandle pub;
    ros::NodeHandle sub;

    ros::Publisher front_right = pub.advertise<std_msgs::Float64>("/furbot/rightFrontWheel_position_controller/command", 1000);
    ros::Publisher front_left = pub.advertise<std_msgs::Float64>("/furbot/leftFrontWheel_position_controller/command", 1000);
    ros::Publisher rear = pub.advertise<std_msgs::Float64>("/furbot/rear", 1000);
    ros::Publisher front = pub.advertise<std_msgs::Float64>("/furbot/front", 1000);

    ros::Subscriber joy = sub.subscribe<sensor_msgs::Joy>("joy", 10, JoyCallback);
    ros::Subscriber key = sub.subscribe<geometry_msgs::Twist>("turtle1/cmd_vel", 10, key_callback);
    ros::Subscriber vel = sub.subscribe<geometry_msgs::Twist>("cmd_vel", 100, CmdVelCallback);

    ros::Rate loop_rate(50);


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

        if(check == 2){

            try{
              
                std::cout << "VEL: " << lin.data << "\tANG: " << ang.data << "\n";
                
                front_right.publish(steer[0]);
                front_left.publish(steer[1]);
                rear.publish(lin);

            }
            catch(const char* msg){
                
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

