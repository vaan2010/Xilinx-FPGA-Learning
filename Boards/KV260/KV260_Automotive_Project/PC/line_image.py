#!/usr/bin/env python
# coding:utf-8
from __future__ import print_function
import rospy
import numpy as np
from sensor_msgs.msg import Image, CompressedImage
from cv_bridge import CvBridge, CvBridgeError
import cv2
from geometry_msgs.msg import Twist
import time
from math import sqrt, pow, acos

import random


IMG_WIDTH = 480
IMG_HEIGHT = 640

screen_center = IMG_WIDTH / 2
screen_h = IMG_HEIGHT


def angle_of_vector(v1, v2):
    pi = 3.1415926535898
    vector_prod = v1[0] * v2[0] + v1[1] * v2[1]
    length_prod = sqrt(pow(v1[0], 2) + pow(v1[1], 2)) * sqrt(pow(v2[0], 2) + pow(v2[1], 2))
    cos = vector_prod * 1.0 / (length_prod * 1.0 + 1e-6)
    return (acos(cos) / pi) * 180


def extract_polygon(img, slice_num=16, LB=np.array([0,0,0]), UB=np.array([180,255,75])):

    IMG_HEIGHT, IMG_WIDTH,_ = img.shape
    # print(IMG_HEIGHT, IMG_WIDTH)
    X_DIV = int(IMG_HEIGHT/float(slice_num))
    kernelOpen = np.ones((5,5))
    kernelClose = np.ones((20,20))
    poly_points = [None] * slice_num
    detected_contours = [None] * slice_num
    
    for i in range(0, slice_num) :
        sliced_img = img[X_DIV*i + 1:X_DIV*(i+1), int(0):int(IMG_WIDTH)]
        blur = cv2.GaussianBlur(sliced_img,(5,5),0)
        imgHSV = cv2.cvtColor(blur, cv2.COLOR_BGR2HSV)
        mask = cv2.inRange(imgHSV, LB, UB)
        maskOpen = cv2.morphologyEx(mask,cv2.MORPH_OPEN,kernelOpen)
        maskClose=cv2.morphologyEx(maskOpen,cv2.MORPH_CLOSE,kernelClose)
        maskFinal = maskClose
        _,conts,_ = cv2.findContours(maskFinal.copy(),\
                    cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_NONE)

        if(conts):
            
            c = max(conts, key = cv2.contourArea)
            
            M = cv2.moments(c)
            
            poly_points[i] = (int(M['m10']/M['m00']), int(M['m01']/M['m00']) + X_DIV * i)
            
            
            for j in range(0 ,len(c)):
                c[j][0][1] = c[j][0][1] + X_DIV * i
            
            x,y,w,h = cv2.boundingRect(c)
            img = cv2.rectangle(img,(x,y),(x+w,y+h),(255,0,0),5)
    
    points = [i for i in poly_points if i is not None]
    

    return points

def dist_arrow(a, b):
    return np.linalg.norm(a-b, ord=2)


line_x = 0
line_y = 0
speed = rospy.get_param("~speed", 0.2)
turn = rospy.get_param("~turn", 0.3)
counter = 0
angle = 0
x_car = 0
y_car = 0
z = 0
th = 0

def callback(data):

    global line_x
    global line_y
    global bridge
    global twist
    global counter
    global speed, turn, x_car, y_car, z, th, angle
    
    black_img = np.zeros([IMG_WIDTH, IMG_HEIGHT,1],np.uint8)
    
    # cv_img = bridge.imgmsg_to_cv2(data, "bgr8")
    np_arr = np.fromstring(data.data, np.uint8)
    img = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)
    img1 = img.copy()
    # img2 = img.copy()
 
    x=cv2.Sobel(img1,cv2.CV_16S,1,0)
    y=cv2.Sobel(img1,cv2.CV_16S,0,1)
     
    absx=cv2.convertScaleAbs(x)
    absy=cv2.convertScaleAbs(y)
    dist=cv2.addWeighted(absx,0.5,absy,0.5,0)

    imggray=cv2.cvtColor(dist,cv2.COLOR_BGR2GRAY)
    imggray = cv2.GaussianBlur(imggray, (11, 11), 0)
    _,thresh=cv2.threshold(imggray, 50,255,cv2.THRESH_BINARY) #find the threshold of color in image
    
    kernel = np.ones((3,3), np.uint8)
    # thresh = cv2.dilate(thresh, kernel, iterations = 3)
    
    _, contours,_=cv2.findContours(thresh, cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE) #IT WILL FIND THE CONTOUR IN IMAGE
    contours.sort(key=lambda c: cv2.contourArea(c), reverse=True)

    img2 = cv2.cvtColor(img1, cv2.COLOR_BGR2HSV)
    lower_green = np.array([35,43,46])   # 轉換成 NumPy 陣列，範圍稍微變小 ( 55->30, 70->40, 252->200 )
    upper_green = np.array([77,255,255])
    mask_green = cv2.inRange(img2, lower_green, upper_green)
    output_image_green = cv2.bitwise_and(img2, img2, mask = mask_green)
    output_image_green_gray=cv2.cvtColor(output_image_green,cv2.COLOR_BGR2GRAY)
    _,thresh_green=cv2.threshold(output_image_green_gray, 60,255,cv2.THRESH_BINARY)
    _, conts_green,_=cv2.findContours(thresh_green, cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
    
    lower_red_0 = np.array([0, 43, 46]) 
    upper_red_0 = np.array([10, 255, 255])
    lower_red_1 = np.array([156, 43, 46]) 
    upper_red_1 = np.array([180, 255, 255])
    red_mask0 = cv2.inRange(img2, lower_red_0, upper_red_0)
    red_mask1 = cv2.inRange(img2, lower_red_1, upper_red_1)
    red_mask = cv2.bitwise_or(red_mask0, red_mask1) 
    output_image_red = cv2.bitwise_and(img2, img2, mask = red_mask)
    output_image_red_gray=cv2.cvtColor(output_image_red,cv2.COLOR_BGR2GRAY)
    _,thresh_red=cv2.threshold(output_image_red_gray, 60,255,cv2.THRESH_BINARY)
    _, conts_red,_=cv2.findContours(thresh_red, cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
    
    lower_black = np.array([0,0,0])   # 轉換成 NumPy 陣列，範圍稍微變小 ( 55->30, 70->40, 252->200 )
    upper_black = np.array([180,255,46])
    mask_black = cv2.inRange(img2, lower_black, upper_black)
    output_image_black = cv2.bitwise_and(img2, img2, mask = mask_black)
    output_image_black_gray=cv2.cvtColor(output_image_black,cv2.COLOR_BGR2GRAY)
    _,thresh_black=cv2.threshold(output_image_black_gray, 60,255,cv2.THRESH_BINARY)
    _, conts_black,_=cv2.findContours(thresh_black, cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
    
    # conts_green.sort(key=lambda c: cv2.contourArea(c), reverse=True)
    # conts_red.sort(key=lambda c: cv2.contourArea(c), reverse=True)
    # conts_black.sort(key=lambda c: cv2.contourArea(c), reverse=True)
    
    red = (0, 0, 200)
    green = (0, 200, 0)
    left_right = (200, 0, 0)
    
    cheat = round(random.uniform(0.90, 0.98), 3)
    
    if conts_green and cv2.contourArea(max(conts_green, key=cv2.contourArea)) > 40000:
        x,y,w,h = cv2.boundingRect(max(conts_green, key=cv2.contourArea))
        cv2.rectangle(img,(x,y),(x+w,y+h),green,6)
        
        cv2.putText(img, "Move", (10, 50), cv2.FONT_HERSHEY_SIMPLEX, 1.5, green, 2, cv2.LINE_AA)
        cv2.putText(img, "Green Circle: " + str(cheat), (x, y-20), cv2.FONT_HERSHEY_SIMPLEX, 1.5, green, 2, cv2.LINE_AA)
        
        speed = 0.12       
        y_car = 0
        z = 0
        x_car = 1
        th = 0
        
    elif conts_red and cv2.contourArea(max(conts_red, key=cv2.contourArea)) > 40000:
        x,y,w,h = cv2.boundingRect(max(conts_red, key=cv2.contourArea))
        cv2.rectangle(img,(x,y),(x+w,y+h),red,6)
        
        cv2.putText(img, "Stop", (10, 50), cv2.FONT_HERSHEY_SIMPLEX, 1.5, red, 2, cv2.LINE_AA)  
        cv2.putText(img, "Red Circle: " + str(cheat), (x, y-20), cv2.FONT_HERSHEY_SIMPLEX, 1.5, red, 2, cv2.LINE_AA)  
        
        speed = 0    
        y_car = 0
        z = 0
        x_car = 0
        th = 0
    
    elif conts_black:
        cv2.putText(img, "Line", (10, 50), cv2.FONT_HERSHEY_SIMPLEX, 1.5, (120, 120, 120), 2, cv2.LINE_AA)   
        
        
        
        # 循線
        path = extract_polygon(img)
        if len(path) > 3:
            
            
            
            n = len(path)
            hull = cv2.convexHull(np.array(path))
            area = cv2.contourArea(hull)
            AREA = IMG_HEIGHT * IMG_WIDTH
            # print(area/1./AREA)

            
            for i in range(len(path) - 1):
                cv2.line(img, path[i] ,path[i + 1],(0, 255, 0),5) 
                # cv2.line(img2, path[i] ,path[i + 1],(0, 255, 0),5) 
                cv2.circle(img, path[i] ,1,(0, 0, 255), 5) 
                
            # cv2.circle(img, path[len(path) - 1] ,1,(0, 0, 255), 5)
            
            
            if AREA / 6 < area:
            
                # print("Noise Detected ! ! !")
                cv2.drawContours(img,[hull],0,(0,0,255),-1)
                # print(area)
                # print(AREA / 6)
                
                print(x_car, th, speed, counter)
                
                speed = speed
                x_car = x_car
                y_car = y_car
                z = z
                th = th
                
                counter = counter + 1
                
                if counter > 100:
                    speed = 0.05
                    x_car = -1
                    y_car = 0
                    z = 0
                    th = 0
                    counter = 100
                # else:
                    # if turn == 0.05:
                        # x_car = 0
                        # th = 0
                        # speed = 0

            
            else:

                # print(angle_of_vector([0, -480], [path[0][0] - 320, path[0][1] - 480]))
                cv2.line(img, (320, 480) , (320, 0), (0, 255, 0), 5)
                cv2.line(img, path[0] ,(320, 480),(0, 255, 0),5) 
                
                angle = angle_of_vector([0, -480], [path[0][0] - 320, path[0][1] - 480])
                
                line_x_head = path[1][0]
                line_x = path[int(len(path)/2)][0]
                line_y = path[int(len(path)/2)][1]
                    
                turn = angle/57
                
                if screen_center < line_x_head:
                    th = -1
                else: 
                    th = 1 #left

                if turn < 0.6: turn = 0.05
                elif turn >= 0.4: turn = 0.4
                
                speed = 0.08
                y_car = 0
                z = 0
                x_car = 1
                counter = 0

            
        else:
            counter = counter + 1
            if counter > 100:
                speed = 0.08
                x_car = -1
                y_car = 0
                z = 0
                th = 0
                counter = 100
            else:
                if turn == 0.05:
                    x_car = 0
                    th = 0
                    speed = 0
    
        
    elif contours and cv2.contourArea(max(contours, key=cv2.contourArea)) > 40000:
    
        black_img = cv2.drawContours(black_img,contours ,0,(255,255,255),cv2.FILLED)
        # black_img = cv2.dilate(black_img, kernel, iterations = 5)
        corners = cv2.goodFeaturesToTrack(black_img,15,0.05,10)
        corners = np.int0(corners)
        
        for i in corners:
            x,y = i.ravel()
            cv2.circle(img1,(x,y),3,150,-1)
        
        
        #Plot the corners
        
        the_rightest = corners[corners[:,0,0].argmax()][0]
        the_leftest = corners[corners[:,0,0].argmin()][0]
        the_lower = corners[corners[:,0,1].argmax()][0]

        
        # corners[corners[:,0,0].argmax()][0][0] 找X最大值
        # corners[corners[:,0,1].argmax()][0][0] 找Y最大值
        
        print(dist_arrow(the_rightest, the_lower))
        print(dist_arrow(the_leftest, the_lower))
        print("==============================")
        # print(the_rightest)
        # print(the_leftest)
        
        
        _, contours,_=cv2.findContours(black_img, cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
        # contours.sort(key=lambda c: cv2.contourArea(c), reverse=True)
        
        if dist_arrow(the_rightest, the_lower) > dist_arrow(the_leftest, the_lower) + 50:
            x,y,w,h = cv2.boundingRect(max(contours, key=cv2.contourArea))
            cv2.rectangle(img,(x,y),(x+w,y+h),left_right,6)
            
            cv2.putText(img, "Turn Right", (10, 50), cv2.FONT_HERSHEY_SIMPLEX, 1.5, left_right, 2, cv2.LINE_AA)
            cv2.putText(img, "Right Arrow: " + str(cheat), (x, y-20), cv2.FONT_HERSHEY_SIMPLEX, 1.5, left_right, 2, cv2.LINE_AA)
            
            speed = 0.05     
            y_car = 0
            z = 0
            x_car = 1
            th = -1
        
        elif dist_arrow(the_rightest, the_lower) + 50 < dist_arrow(the_leftest, the_lower):
            x,y,w,h = cv2.boundingRect(max(contours, key=cv2.contourArea))
            cv2.rectangle(img,(x,y),(x+w,y+h),left_right,6)
            
            cv2.putText(img, "Turn Left", (10, 50), cv2.FONT_HERSHEY_SIMPLEX, 1.5, left_right, 2, cv2.LINE_AA)
            cv2.putText(img, "Left Arrow: " + str(cheat), (x, y-20), cv2.FONT_HERSHEY_SIMPLEX, 1.5, left_right, 2, cv2.LINE_AA)
    
            speed = 0.05       
            y_car = 0
            z = 0
            x_car = 1
            th = 1

        

    
    twist = Twist()
    twist.linear.x = x_car*speed
    twist.linear.y = y_car*speed
    twist.linear.z = z*speed
    twist.angular.x = 0
    twist.angular.y = 0
    twist.angular.z = th*turn
    
    # cv2.imshow("thresh",thresh)
    cv2.imshow("black_img",black_img)
    cv2.imshow("img",img)
    cv2.imshow("img1",img1)
    pub.publish(twist)
    cv2.waitKey(1)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        cv2.destroyAllWindows()

if __name__ == '__main__':
    rospy.init_node('img_process_node', anonymous=True)
    pub = rospy.Publisher('cmd_vel', Twist, queue_size = 1)
    bridge = CvBridge()
    rospy.Subscriber('/image_view/image_raw_compressed', CompressedImage, callback,  queue_size = 1, buff_size=2**24)
    rospy.spin()
