from std_msgs.msg import Header
from sensor_msgs.msg import CompressedImage
from geometry_msgs.msg import Twist
import rospy
import roslib

from pynq_dpu import DpuOverlay

import os
import time
import numpy as np

import cv2
import random
import colorsys
from matplotlib.patches import Rectangle
import matplotlib.pyplot as plt
from PIL import Image
from math import sqrt, pow, acos

# future work:目前採用 source code PIL Image 的處理模式，再跟 OpenCV 做互相轉換，之後優化會以同一圖像處理框架為準

IMG_WIDTH = 480
IMG_HEIGHT = 640

screen_center = IMG_HEIGHT / 2
screen_h = IMG_WIDTH


overlay = DpuOverlay("dpu.bit")
overlay.load_model("car_pt_yolov4-tiny-0508.xmodel")


anchor_list = [10,14,  23,27,  37,58,  81,82,  135,169,  344,319]
anchor_float = [float(x) for x in anchor_list]
anchors = np.array(anchor_float).reshape(-1, 2)
# print(anchors)

'''Get model classification information'''	
def get_class(classes_path):
    with open(classes_path) as f:
        class_names = f.readlines()
    class_names = [c.strip() for c in class_names]
    return class_names
    
classes_path = "img/car_classes.txt"
class_names = get_class(classes_path)

num_classes = len(class_names)
# print(num_classes)

hsv_tuples = [(1.0 * x / num_classes, 1., 1.) for x in range(num_classes)]
colors = list(map(lambda x: colorsys.hsv_to_rgb(*x), hsv_tuples))
colors = list(map(lambda x: 
                  (int(x[0] * 255), int(x[1] * 255), int(x[2] * 255)), 
                  colors))
random.seed(0)
random.shuffle(colors)
random.seed(None)

def cvtColor(image):
    if len(np.shape(image)) == 3 and np.shape(image)[2] == 3:
        return image 
    else:
        image = image.convert('RGB')
        return image 
    
def resize_image(image, size, letterbox_image):
    iw, ih  = image.size
    w, h    = size
    if letterbox_image:
        scale   = min(w/iw, h/ih)
        nw      = int(iw*scale)
        nh      = int(ih*scale)

        image   = image.resize((nw,nh), Image.BICUBIC)
        new_image = Image.new('RGB', size, (128,128,128))
        new_image.paste(image, ((w-nw)//2, (h-nh)//2))
    else:
        new_image = image.resize((w, h), Image.BICUBIC)
    return new_image

def preprocess_input(image):
    image /= 255.0
    return image
    
def pre_process(image, model_image_size):
    image       = cvtColor(image)
    image_data  = resize_image(image, (416, 416), True)
    image_data  = np.expand_dims(preprocess_input(np.array(image_data, dtype='float32')), 0)
    return image_data
    
    
#check
def _get_feats(feats, anchors, num_classes, input_shape):
    num_anchors = len(anchors)
    
    grid_size = np.shape(feats)[1:3]
    
    grid_x = np.tile(np.reshape(np.arange(grid_size[1]), [1, -1, 1, 1]), [grid_size[0], 1, num_anchors, 1])
    grid_y = np.tile(np.reshape(np.arange(grid_size[0]), [-1, 1, 1, 1]), [1, grid_size[1], num_anchors, 1])

    grid = np.concatenate([grid_x, grid_y], axis = -1)
    grid = np.array(grid, dtype=np.float32)
    
    anchors_tensor = np.reshape(np.array(anchors, dtype=np.float32), [1, 1, num_anchors, 2])
    
    nu = num_classes + 5
    predictions = np.reshape(feats, [-1, grid_size[0], grid_size[1], num_anchors, nu])
    

    box_xy = (1/(1+np.exp(-predictions[..., :2])) + grid) / np.array(grid_size[::-1], dtype=np.float32)
    box_wh = np.exp(predictions[..., 2:4]) * anchors_tensor / np.array(input_shape[::-1], dtype=np.float32)
    
    box_confidence = 1/(1+np.exp(-predictions[..., 4:5]))
    box_class_probs = 1/(1+np.exp(-predictions[..., 5:]))
    
    return box_xy, box_wh, box_confidence, box_class_probs

#check
def correct_boxes(box_xy, box_wh, input_shape, image_shape):
    box_yx = box_xy[..., ::-1]
    box_hw = box_wh[..., ::-1]
    input_shape = np.array(input_shape, dtype = np.float32)
    image_shape = np.array(image_shape, dtype = np.float32)
    
    new_shape = np.around(image_shape * np.min(input_shape / image_shape))
    offset = (input_shape - new_shape) / 2. / input_shape
    scale = input_shape / new_shape
    
    box_yx = (box_yx - offset) * scale
    box_hw *= scale

    box_mins = box_yx - (box_hw / 2.)
    box_maxes = box_yx + (box_hw / 2.)
    
    boxes = np.concatenate([
        box_mins[..., 0:1],
        box_mins[..., 1:2],
        box_maxes[..., 0:1],
        box_maxes[..., 1:2]
    ], axis = -1)
    
    boxes *= np.concatenate([image_shape, image_shape], axis = -1)
    return boxes


def boxes_and_scores(feats, anchors, classes_num, input_shape, image_shape):
    box_xy, box_wh, box_confidence, box_class_probs = _get_feats(feats, anchors, classes_num, input_shape)
    boxes = correct_boxes(box_xy, box_wh, input_shape, image_shape)
    
    boxes = np.reshape(boxes, [-1, 4])

    
    box_scores = box_confidence * box_class_probs
    box_scores = np.reshape(box_scores, [-1, classes_num])
    return boxes, box_scores


'''Draw detection frame'''
def draw_bbox(image, bboxes, classes):
    """
    bboxes: [x_min, y_min, x_max, y_max, probability, cls_id] format coordinates.
    """
    num_classes = len(classes)
    image_h, image_w, _ = image.shape
    hsv_tuples = [(1.0 * x / num_classes, 1., 1.) for x in range(num_classes)]
    colors = list(map(lambda x: colorsys.hsv_to_rgb(*x), hsv_tuples))
    colors = list(map(lambda x: (int(x[0] * 255), int(x[1] * 255), int(x[2] * 255)), colors))

    random.seed(0)
    random.shuffle(colors)
    random.seed(None)

    for i, bbox in enumerate(bboxes):
        coor = np.array(bbox[:4], dtype=np.int32)
        fontScale = 0.5
        score = bbox[4]
        class_ind = int(bbox[5])
        bbox_color = colors[class_ind]
        bbox_thick = int(0.6 * (image_h + image_w) / 600)
        c1, c2 = (coor[0], coor[1]), (coor[2], coor[3])
        cv2.rectangle(image, c1, c2, bbox_color, bbox_thick)
    return image


def nms_boxes(boxes, scores):
    """Suppress non-maximal boxes.

    # Arguments
        boxes: ndarray, boxes of objects.
        scores: ndarray, scores of objects.

    # Returns
        keep: ndarray, index of effective boxes.
    """
    x1 = boxes[:, 0]
    y1 = boxes[:, 1]
    x2 = boxes[:, 2]
    y2 = boxes[:, 3]

    areas = (x2-x1+1)*(y2-y1+1)
    order = scores.argsort()[::-1]

    keep = []
    while order.size > 0:
        i = order[0]
        keep.append(i)

        xx1 = np.maximum(x1[i], x1[order[1:]])
        yy1 = np.maximum(y1[i], y1[order[1:]])
        xx2 = np.minimum(x2[i], x2[order[1:]])
        yy2 = np.minimum(y2[i], y2[order[1:]])

        w1 = np.maximum(0.0, xx2 - xx1 + 1)
        h1 = np.maximum(0.0, yy2 - yy1 + 1)
        inter = w1 * h1

        ovr = inter / (areas[i] + areas[order[1:]] - inter)
        inds = np.where(ovr <= 0.3)[0]  # threshold
        order = order[inds + 1]

    return keep

 
def draw_boxes(image, boxes, scores, classes):

    label_classes = ''

    image_h, image_w = image.size[::-1]

    image = cv2.cvtColor(np.asarray(image), cv2.COLOR_RGB2BGR)


    for i, bbox in enumerate(boxes):
        [top, left, bottom, right] = bbox
        width, height = right - left, bottom - top
        center_x, center_y = left + width*0.5, top + height*0.5
        score, class_index = scores[i], classes[i]
        label = '{}: {:.4f}'.format(class_names[class_index], score) 
        
        if width*height > 12000:
        
            label_classes = '{}'.format(class_names[class_index])
        
        # label_classes.append('{}'.format(class_names[class_index]))
        
        color = tuple([color for color in colors[class_index]])
        
        cv2.rectangle(image, (left, top), (left+width, top+height), color, 2)

                               
        cv2.putText(image, label, (int(left)+10, int(bottom)-10), cv2.FONT_HERSHEY_SIMPLEX, 1, color, 2, cv2.LINE_AA)

    return image, label_classes
    
def evaluate(yolo_outputs, image_shape, class_names, anchors):
    score_thresh = 0.3

    anchor_mask = [[3, 4, 5], [1, 2, 3]]
    
    boxes = []
    box_scores = []
    
    input_shape = np.shape(yolo_outputs[0])[1 : 3]
    
    input_shape = np.array(input_shape)*32

    
    for i in range(len(yolo_outputs)):
        _boxes, _box_scores = boxes_and_scores(
            yolo_outputs[i], anchors[anchor_mask[i]], len(class_names), input_shape, image_shape)
        
        boxes.append(_boxes)
        box_scores.append(_box_scores)
        
    boxes = np.concatenate(boxes, axis = 0)
    box_scores = np.concatenate(box_scores, axis = 0)

    mask = box_scores >= score_thresh
    boxes_ = []
    scores_ = []
    classes_ = []
    for c in range(len(class_names)):
        
        class_boxes_np = boxes[mask[:, c]]
        class_box_scores_np = box_scores[:, c]
        class_box_scores_np = class_box_scores_np[mask[:, c]]
        
        nms_index_np = nms_boxes(class_boxes_np, class_box_scores_np) 
        
        class_boxes_np = class_boxes_np[nms_index_np]
        class_box_scores_np = class_box_scores_np[nms_index_np]
        classes_np = np.ones_like(class_box_scores_np, dtype = np.int32) * c
        
        boxes_.append(class_boxes_np)
        scores_.append(class_box_scores_np)
        classes_.append(classes_np)
        
    boxes_ = np.concatenate(boxes_, axis = 0)
    scores_ = np.concatenate(scores_, axis = 0)
    classes_ = np.concatenate(classes_, axis = 0)

    return boxes_, scores_, classes_
    
    
    
dpu = overlay.runner
inputTensors = dpu.get_input_tensors()
outputTensors = dpu.get_output_tensors()
shapeIn = tuple(inputTensors[0].dims)

shapeOut0 = (tuple(outputTensors[0].dims)) # (1, 13, 13, 75)
shapeOut1 = (tuple(outputTensors[1].dims)) # (1, 26, 26, 75)

outputSize0 = int(outputTensors[0].get_data_size() / shapeIn[0]) # 12675
outputSize1 = int(outputTensors[1].get_data_size() / shapeIn[0]) # 50700

input_data = [np.empty(shapeIn, dtype=np.float32, order="C")]

output_data = [np.empty(shapeOut0, dtype=np.float32, order="C"), 
               np.empty(shapeOut1, dtype=np.float32, order="C")]
image = input_data[0]

def inference(input_image):
    
    # Pre-processing

    image_size = input_image.size[::-1]
    image_data = np.array(pre_process(input_image, (416, 416)), dtype=np.float32)
    
    # Fetch data to DPU and trigger it
    image[0,...] = image_data.reshape(shapeIn[1:])
    job_id = dpu.execute_async(input_data, output_data)
    dpu.wait(job_id)
    
    # Retrieve output data
    conv_out0 = np.reshape(output_data[0], shapeOut0)
    conv_out1 = np.reshape(output_data[1], shapeOut1)

    yolo_outputs = [conv_out0, conv_out1]
    
    # Decode output from YOLOv4-tiny
    boxes, scores, classes = evaluate(yolo_outputs, image_size, class_names, anchors)
    
    input_image, labeles = draw_boxes(input_image, boxes, scores, classes)

    
    return input_image, labeles
    
# 以下為循線

def angle_of_vector(v1, v2):
    pi = 3.1415926535898
    vector_prod = v1[0] * v2[0] + v1[1] * v2[1]
    length_prod = sqrt(pow(v1[0], 2) + pow(v1[1], 2)) * sqrt(pow(v2[0], 2) + pow(v2[1], 2))
    cos = vector_prod * 1.0 / (length_prod * 1.0 + 1e-6)
    return (acos(cos) / pi) * 180
    
def extract_polygon(img, slice_num=16, LB=np.array([0,0,0]), UB=np.array([180,255,85])):

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
        conts,_ = cv2.findContours(maskFinal.copy(), cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_NONE)

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




# line_x = 0
# line_y = 0
speed = rospy.get_param("~speed", 0.0)
turn = rospy.get_param("~turn", 0)
counter = 0
angle = 0
x_car = 0
y_car = 0
z = 0
th = 0
switch = 0



if __name__=="__main__":


    # capture = cv2.VideoCapture('output.avi') # 定義影像來源
    capture = cv2.VideoCapture(0) # 定義相機鏡頭
    capture.set(cv2.CAP_PROP_FRAME_HEIGHT,480)
    capture.set(cv2.CAP_PROP_FRAME_WIDTH,640)
    rospy.init_node('camera_node', anonymous=True) # 定義 ROS Node
    
    image_pub=rospy.Publisher('/image_view/image_raw_compressed', CompressedImage, queue_size = 10) # 定義 ROS 發布主題

    pub = rospy.Publisher('cmd_vel', Twist, queue_size = 1)

    while not rospy.is_shutdown():    # Ctrl C 正常退出，若異常會報 Device Busy
        start = time.time()
        ret, frame = capture.read()
        
        dpu_frame = Image.fromarray(cv2.cvtColor(frame, cv2.COLOR_BGR2RGB))
        
        # frame = cv2.imread("pedestrian0213.jpg")
        # input_image = Image.open("img/pedestrian0213.jpg")
        
        if ret: # 如果有畫面再執行
            

            dpu_frame, result_classes = inference(dpu_frame) #Yolov4-tiny Inference
            
            # print(result_classes)
            
            line_frame = frame[380:480, 0:640]
            # line_frame = frame
            
            path = extract_polygon(line_frame)
            
            if switch == 1:
            
                speed = speed
                x_car = x_car
                y_car = y_car
                z = z
                th = th
            
                counter = counter + 1 
                if counter > 20:
                    switch = 0
                    counter = 0
            
            elif result_classes == 'Stop':
                speed = 0
                turn = 0
            
            elif result_classes == 'Right':
                
                z = 0
            
            
                th = -1 #right
                        
                turn = 0.45

                speed = 0.13
                y_car = 0
                
                x_car = 1
                switch = 1
                
            elif result_classes == 'Left':
            
                z = 0
            
                th = 1 #left
                        
                turn = 0.45

                speed = 0.13
                y_car = 0
                
                x_car = 1
                switch = 1
            
            # elif result_classes == 'Start' and len(path) > 3:
            elif len(path) > 3:
            
                n = len(path)
                hull = cv2.convexHull(np.array(path))
                area = cv2.contourArea(hull)
                AREA = IMG_HEIGHT * IMG_WIDTH
                # print(area/1./AREA)

                
                for i in range(len(path) - 1):
                    cv2.line(line_frame, path[i] ,path[i + 1],(0, 255, 0),5) 
                    # cv2.line(img2, path[i] ,path[i + 1],(0, 255, 0),5) 
                    cv2.circle(line_frame, path[i] ,1,(0, 0, 255), 5) 
                    
                # cv2.circle(img, path[len(path) - 1] ,1,(0, 0, 255), 5)
                
                
                if AREA / 6 < area:
                
                    # print("Noise Detected ! ! !")
                    # cv2.drawContours(frame,[hull],0,(0,0,255),-1)
                    # print(area)
                    # print(AREA / 6)
                    
                    # print(x_car, th, speed, counter)
                    
                    speed = speed
                    x_car = x_car
                    y_car = y_car
                    z = z
                    th = th
                    
                    # counter = counter + 1
                    
                    # if counter > 100:
                        # speed = 0.05
                        # x_car = -1
                        # y_car = 0
                        # z = 0
                        # th = 0
                        # counter = 100
                
                else:

                    # print(angle_of_vector([0, -480], [path[0][0] - 320, path[0][1] - 480]))
                    cv2.line(line_frame, (320, 480) , (320, 0), (0, 255, 0), 5)
                    cv2.line(line_frame, path[0] ,(320, 480),(0, 255, 0),5) 
                    
                    angle = angle_of_vector([0, -480], [path[0][0] - 320, path[0][1] - 480])
                    
                    line_x_head = path[1][0]
                    line_x = path[int(len(path)/2)][0]
                    line_y = path[int(len(path)/2)][1]
                        
                    turn = angle/57
                    
                    
                    if screen_center < line_x_head:
                        th = -1
                    else: 
                        th = 1 #left
                        
                    
                    if turn >= 0.1 and turn <= 0.5: turn = 0.1
                    elif turn >= 0.5: turn = 0.5
                    
                    speed = 0.13
                    y_car = 0
                    z = 0
                    x_car = 1
                    counter = 0
                    
            
            else:
                counter = counter + 1
                if counter > 100:
                    speed = 0.13
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
                    
                    
            
            twist = Twist()
            twist.linear.x = x_car*speed
            twist.linear.y = y_car*speed
            twist.linear.z = z*speed
            twist.angular.x = 0
            twist.angular.y = 0
            twist.angular.z = th*turn
            
            pub.publish(twist)
            
            
            transport_image = np.vstack((dpu_frame[0:380, 0:640], line_frame))
            # transport_image = np.hstack((transport_image, dpu_frame[380:480, 490:640]))
            # transport_image = np.vstack((dpu_frame[0:380, 0:640], transport_image))
            
            ros_frame = CompressedImage()
            
            header = Header(stamp = rospy.Time.now())
            header.frame_id = "Camera"
            ros_frame.header=header
            
            ros_frame.format = "jpeg"

            ros_frame.data = np.array(cv2.imencode('.jpg', transport_image)[1]).tobytes()

            image_pub.publish(ros_frame) # 發布 ROS 消息
            
            end = time.time()  
            print("\rcost time:", end-start, end='' ) # fps
            rate = rospy.Rate(30) # 10hz 

    
    del overlay
    del dpu
    
    capture.release()
    cv2.destroyAllWindows() 
    print("quit successfully!")
