#!/usr/bin/env python
# coding:utf-8
from __future__ import print_function
import cv2
import numpy as np

import rospy
import roslib

from std_msgs.msg import Header
from sensor_msgs.msg import Image, CompressedImage
from cv_bridge import CvBridge , CvBridgeError
import time

from pynq_dpu import DpuOverlay

_R_MEAN = 123.68
_G_MEAN = 116.78
_B_MEAN = 103.94

MEANS = [_B_MEAN,_G_MEAN,_R_MEAN]

def resize_shortest_edge(image, size):
    H, W = image.shape[:2]
    if H >= W:
        nW = size
        nH = int(float(H)/W * size)
    else:
        nH = size
        nW = int(float(W)/H * size)
    return cv2.resize(image,(nW,nH))

def mean_image_subtraction(image, means):
    B, G, R = cv2.split(image)
    B = B - means[0]
    G = G - means[1]
    R = R - means[2]
    image = cv2.merge([R, G, B])
    return image

def BGR2RGB(image):
    B, G, R = cv2.split(image)
    image = cv2.merge([R, G, B])
    return image

def central_crop(image, crop_height, crop_width):
    image_height = image.shape[0]
    image_width = image.shape[1]
    offset_height = (image_height - crop_height) // 2
    offset_width = (image_width - crop_width) // 2
    return image[offset_height:offset_height + crop_height, offset_width:
                 offset_width + crop_width, :]

def normalize(image):
    image=image/256.0
    image=image-0.5
    image=image*2
    return image

def preprocess_fn(image, crop_height = 224, crop_width = 224):
    image = BGR2RGB(image)
    image = resize_shortest_edge(image, 256)
    image = central_crop(image, crop_height, crop_width)
    image = normalize(image)
    return image

def calculate_softmax(data):
    result = np.exp(data)
    return result

def predict_label(softmax):
    with open("img/words.txt", "r") as f:
        lines = f.readlines()
    return lines[np.argmax(softmax)-1]


overlay = DpuOverlay("dpu.bit")
overlay.load_model("dpu_tf_inceptionv1.xmodel")    
dpu = overlay.runner

inputTensors = dpu.get_input_tensors()
outputTensors = dpu.get_output_tensors()

shapeIn = tuple(inputTensors[0].dims)
shapeOut = tuple(outputTensors[0].dims)
outputSize = int(outputTensors[0].get_data_size() / shapeIn[0])

softmax = np.empty(outputSize)

output_data = [np.empty(shapeOut, dtype=np.float32, order="C")]
input_data = [np.empty(shapeIn, dtype=np.float32, order="C")]
image = input_data[0]

left_right = (200, 0, 0)

def inference(frame):

    preprocessed = preprocess_fn(frame)
    image[0,...] = preprocessed.reshape(shapeIn[1:])
    job_id = dpu.execute_async(input_data, output_data)
    dpu.wait(job_id)
    temp = [j.reshape(1, outputSize) for j in output_data]
    softmax = calculate_softmax(temp[0][0])
    
    # print("Classification: {}".format(predict_label(softmax)))
    
    return predict_label(softmax)
    # cv2.putText(frame, "{}".format(predict_label(softmax)), (10, 50), cv2.FONT_HERSHEY_SIMPLEX, 0.5, left_right, 1, cv2.LINE_AA)


if __name__=="__main__":
    capture = cv2.VideoCapture(0) # 定义摄像头
    capture.set(cv2.CAP_PROP_FRAME_HEIGHT,480)
    capture.set(cv2.CAP_PROP_FRAME_WIDTH,640)
    rospy.init_node('camera_node', anonymous=True) #定义节点
    
    image_pub=rospy.Publisher('/image_view/image_raw_compressed', CompressedImage, queue_size = 10) #定义话题

    while not rospy.is_shutdown():    # Ctrl C正常退出，如果异常退出会报错device busy！
        start = time.time()
        ret, frame = capture.read()
        if ret: # 如果有画面再执行
            # frame = cv2.flip(frame,0)   #垂直镜像操作
            # frame = cv2.flip(frame,1)   #水平镜像操作   
    
            # frame = cv2.resize(frame, (1280, 720), interpolation=cv2.INTER_AREA)


            # print("Classification: {}".format(predict_label(softmax)))
            
            img1 = frame.copy()
            
            cv2.putText(frame, "{}".format(inference(img1)), (10, 50), cv2.FONT_HERSHEY_SIMPLEX, 1.5, left_right, 2, cv2.LINE_AA)

            ros_frame = CompressedImage()
            
            header = Header(stamp = rospy.Time.now())
            header.frame_id = "Camera"
            ros_frame.header=header
            
            ros_frame.format = "jpeg"
            
            # ros_frame.width = 640
            # ros_frame.height = 480
            # ros_frame.encoding = "bgr8"
            # ros_frame.step = 1920
            
            # ros_frame.data = np.array(frame).tostring() #图片格式转换
            ros_frame.data = np.array(cv2.imencode('.jpg', frame)[1]).tostring()
            
            
            image_pub.publish(ros_frame) #发布消息
            
            end = time.time()  
            print("\rcost time:", end-start, end='' ) # 看一下每一帧的执行时间，从而确定合适的rate
            rate = rospy.Rate(30) # 10hz 

    
    del overlay
    del dpu
    
    capture.release()
    cv2.destroyAllWindows() 
    print("quit successfully!")
