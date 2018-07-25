#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <gazebo_msgs/GetModelState.h>
#include <tf/transform_datatypes.h>

int main(int argc, char **argv)
{
  ros::init(argc, argv, "odometry_publisher");

  ros::NodeHandle n1;
  ros::NodeHandle n2;

  ros::Publisher odom_pub = n1.advertise<nav_msgs::Odometry>("odom", 50);
  tf::TransformBroadcaster odom_broadcaster;

  double x = 0.0;
  double y = 0.0;
  double th = 0.0;

  double yaw, pitch, roll = 0.0;

  ros::Time current_time, last_time;
  current_time = ros::Time::now();
  last_time = ros::Time::now();

  ros::ServiceClient client = n2.serviceClient<gazebo_msgs::GetModelState>("/gazebo/get_model_state");
  gazebo_msgs::GetModelState gms;
  gms.request.model_name = "mybot";

  ros::Rate r(10);
  while (n1.ok())
  {
    try
    {
      ROS_INFO("ODOMETRY POSITIONS");
      client.call(gms);
      std::cout << "POSITIONS X: " << gms.response.pose.position.x << "\tY: " << gms.response.pose.position.y << "\tZ: " << gms.response.pose.position.z << "\n";

      ros::spinOnce();
      current_time = ros::Time::now();

      tf::Quaternion q(gms.response.pose.orientation.x, gms.response.pose.orientation.y,
                       gms.response.pose.orientation.z, gms.response.pose.orientation.w);
      tf::Matrix3x3 m(q);
      m.getRPY(roll, pitch, yaw);
      
      x = gms.response.pose.position.x;
      y = gms.response.pose.position.y;
      th = yaw;

      geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(th);

      //first, we'll publish the transform over tf
      geometry_msgs::TransformStamped odom_trans;
      odom_trans.header.stamp = current_time;
      odom_trans.header.frame_id = "odom";
      odom_trans.child_frame_id = "chassis";

      odom_trans.transform.translation.x = x;
      odom_trans.transform.translation.y = y;
      odom_trans.transform.translation.z = 0.0;
      odom_trans.transform.rotation = odom_quat;

      //send the transform
      odom_broadcaster.sendTransform(odom_trans);

      //next, we'll publish the odometry message over ROS
      nav_msgs::Odometry odom;
      odom.header.stamp = current_time;
      odom.header.frame_id = "odom";

      //set the position
      odom.pose.pose.position.x = x;
      odom.pose.pose.position.y = y;
      odom.pose.pose.position.z = 0.0;
      odom.pose.pose.orientation = odom_quat;

      //set the velocity
      odom.child_frame_id = "chassis";
      odom.twist.twist.linear.x = gms.response.twist.linear.x;
      odom.twist.twist.linear.y = gms.response.twist.linear.y;
      odom.twist.twist.angular.z = gms.response.twist.angular.z;

      //publish the message
      odom_pub.publish(odom);

      last_time = current_time;
      r.sleep();
    }
    catch (const char *msg)
    {
      std::cout << "ERROR OCCOURED DURING ODOMETRY CALCULATION! ERR: " << msg;
      return 0;
    }
  }
}
