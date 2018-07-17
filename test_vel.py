#!/usr/bin/env python

import rospy
import numpy as np
from geometry_msgs.msg import Pose, Point, Quaternion,Twist
from std_msgs.msg import Float64
import time
import os
import sys


class TestVelocity():

    def __init__(self):


        self._pub1 = rospy.Publisher('/mybot/leftRearWheel_effort_controller/command',Float64, queue_size=3)
        self._pub3 = rospy.Publisher('/mybot/rightRearWheel_effort_controller/command',Float64, queue_size=3)
        self._pub2 = rospy.Publisher('/mybot/leftFrontWheel_effort_controller/command',Float64, queue_size=3)
        self._pub4 = rospy.Publisher('/mybot/rightFrontWheel_effort_controller/command',Float64, queue_size=3)

    def set_velocity(self,velocity):

        print ('set velocity')

        ##publish the message
        self._pub1.publish(velocity)
        self._pub3.publish(velocity)

    def set_orientation(self,orientation):

        print ('set orientation')

        ##publish the message
        self._pub2.publish(orientation)
        self._pub4.publish(orientation)


    def main(self):
        i=0
        vel=1
        orient=0.4

        while (i<5):
            
            print ('increase velocity')
            #vel= vel+1
            self.set_velocity(vel)
            i=i+1
            time.sleep(2)
            orient=orient*-1
            self.set_orientation(orient)
            time.sleep(3)
    
        self.set_velocity(0)
        
        rospy.spin()


if __name__ == '__main__':
    rospy.init_node('TestVelocity')

    test = TestVelocity()
    test.main()

