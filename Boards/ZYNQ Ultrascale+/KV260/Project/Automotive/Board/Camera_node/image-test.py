from std_msgs.msg import Header
from sensor_msgs.msg import CompressedImage
# from cv_bridge import CvBridge , CvBridgeError
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



overlay = DpuOverlay("dpu.bit")
overlay.load_model("pt_yolov4-tiny-0208.xmodel")


anchor_list = [10,14,  23,27,  37,58,  81,82,  135,169,  344,319]
anchor_float = [float(x) for x in anchor_list]
anchors = np.array(anchor_float).reshape(-1, 2)
print(anchors)

'''Get model classification information'''	
def get_class(classes_path):
    with open(classes_path) as f:
        class_names = f.readlines()
    class_names = [c.strip() for c in class_names]
    return class_names
    
classes_path = "img/voc_classes.txt"
class_names = get_class(classes_path)

num_classes = len(class_names)
print(num_classes)

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
    # _, ax = plt.subplots(1)
#     ax.imshow(cv2.cvtColor(image, cv2.COLOR_BGR2RGB))
    # ax.imshow(image)
#     image_h, image_w, _ = image.shape
    image_h, image_w = image.size[::-1]

    image = cv2.cvtColor(np.asarray(image), cv2.COLOR_RGB2BGR)

    for i, bbox in enumerate(boxes):
        [top, left, bottom, right] = bbox
        width, height = right - left, bottom - top
        center_x, center_y = left + width*0.5, top + height*0.5
        score, class_index = scores[i], classes[i]
        label = '{}: {:.4f}'.format(class_names[class_index], score) 
        color = tuple([color/255 for color in colors[class_index]])
        cv2.rectangle(image, (left, top), (left+width, top+height), (0, 255, 0), 2)
        
        # ax.add_patch(Rectangle((left, top), width, height,
                               # edgecolor=color, facecolor='none'))
                               
        cv2.putText(image, label, (int(left), int(top)-10), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2, cv2.LINE_AA)
                               
        # ax.annotate(label, (center_x, center_y), color=color, weight='bold', 
                    # fontsize=12, ha='center', va='center')
    return image
    
def evaluate(yolo_outputs, image_shape, class_names, anchors):
    score_thresh = 0.7
    
#     anchor_mask = [[6, 7, 8], [3, 4, 5], [0, 1, 2]]

    anchor_mask = [[3, 4, 5], [1, 2, 3]]
    
    boxes = []
    box_scores = []
    input_shape = np.shape(yolo_outputs[0])[1 : 3]
#     input_shape = np.shape(yolo_outputs[0])[1 : 2]
    input_shape = np.array(input_shape)*32

#     print(len(yolo_outputs))
    
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
    
image_folder = 'img'
original_images = sorted([i for i in os.listdir(image_folder) if i.endswith("jpg")])
total_images = len(original_images)
print(original_images)

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

def run(image_index, display=False):
    # Read input image

    input_image = Image.open(os.path.join(image_folder, original_images[image_index]))
    
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
    
    print(yolo_outputs)
    
    # Decode output from YOLOv3
    boxes, scores, classes = evaluate(yolo_outputs, image_size, class_names, anchors)
    
    if display:
        input_image = draw_boxes(input_image, boxes, scores, classes)
    print("Number of detected objects: {}".format(len(boxes)))
    
    cv2.imwrite("./test4.jpg", input_image)
    
    return input_image
    

start = time.time()

frame = run(0, display=True)
cv2.imwrite("./test5.jpg", frame)

rospy.init_node('camera_node', anonymous=True) #定义节点
image_pub=rospy.Publisher('/image_view/image_raw_compressed', CompressedImage, queue_size = 10) #定义话题

ros_frame = CompressedImage()
        
header = Header(stamp = rospy.Time.now())
header.frame_id = "Camera"
ros_frame.header=header

ros_frame.format = "jpeg"

ros_frame.data = np.array(cv2.imencode('.jpg', frame)[1]).tostring()

image_pub.publish(ros_frame) #发布消息

end = time.time()  
print("\rcost time:", end-start, end='' ) # 看一下每一帧的执行时间，从而确定合适的rate
rate = rospy.Rate(30) # 10hz


del overlay
del dpu

print("quit successfully!")