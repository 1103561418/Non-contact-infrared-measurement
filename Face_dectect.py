# Untitled - By: DELL - Fri Jul 12 2024

import sensor, time, image, pyb, ustruct,os, tf, math, uos, gc

from pyb import UART


uart = UART(3,115200,timeout_char=1000)
sensor.reset() # Initialize the camera sensor.
sensor.set_pixformat(sensor.GRAYSCALE) # or sensor.GRAYSCALE
sensor.set_framesize(sensor.QQVGA) # or sensor.QQVGA (or others)
sensor.set_hmirror(True)              # 设置水平镜像（翻转180度）
sensor.set_vflip(True)
sensor.skip_frames(10) # Let new settings take affect.
sensor.skip_frames(time = 2000)

clock = time.clock()

global flag_count
flag_count=0
RED_LED_PIN = 1
BLUE_LED_PIN = 3
#data_information_start=bytes([0x2C,0x12])
#data_information_over=bytes([0x5B])

#=========================转换为整形数组=======================
def signed_int_to_byte(value):
    b1 = value & 0xFF                      #获取低8位
    b2 = (value >> 8) & 0xFF               #获取高8位
    return bytearray([b1, b2])             #返回字节数组



#===========================发送数据===================================
def sending_data(x):
    global uart                            #将x、y、z转换为字节数组
    x_bytes = signed_int_to_byte(x)
    FH = bytearray([0x2C, 0x12]) + x_bytes + bytearray([0x5B])
    uart.write(FH)                       #将数据存入FH这个数组，数据包格式为两个帧头，三个数据和一个帧尾



#============================接受数据==================================
state = 0
x = 0
tx_flag = 0
data1_function = 0
data2_function = 0
rx_buff = []  # 假设需要一个接收缓冲区
data_function = [0] * 6


def Receive_Prepare():      #data
    global state
    global x
    global tx_flag
    global data_function
    global data1_function
    global data2_function
    if state==0:
        data_function[0]=uart.readchar()
        if data_function[0] == 0xB3:#帧头
            state = 1
        else:
            state = 0
            rx_buff.clear()
    elif state==1:
        data_function[1]=uart.readchar()
        if data_function[1] == 0xB3:#帧头
            state = 2
        else:
            state = 0
            rx_buff.clear()
    elif state==2:
        data_function[2]=uart.readchar()
        data1_function = data_function[x+2]
        state = 3
    elif state==3:
        data_function[3]=uart.readchar()
        data2_function = data_function[x+3]
        state = 4
    elif state == 4:
        data_function[4]=uart.readchar()
        if data_function[4] == 0x5B:
            tx_flag = (data_function)
            state = 5
    elif state == 5:
        state=0

    else:
        state = 0
        rx_buff.clear()

#================================现场学习=====================================#
def take_photos():
    sensor.reset() # Initialize the camera sensor.
    sensor.set_pixformat(sensor.GRAYSCALE) # or sensor.GRAYSCALE
    sensor.set_framesize(sensor.QQVGA) # or sensor.QQVGA (or others)
    sensor.set_hmirror(True)              # 设置水平镜像（翻转180度）
    sensor.set_vflip(True)
    sensor.skip_frames(10) # Let new settings take affect.
    sensor.skip_frames(time = 2000)
    global flag_count
    flag_count= flag_count+1 #设置被拍摄者序号，第一个人的图片保存到s1文件夹，第二个人的图片保存到s2文件夹，以此类推。每次更换拍摄者时，修改num值。

    n = 10 #设置每个人拍摄图片数量。

    #连续拍摄n张照片，每间隔3s拍摄一次。
    while(n):
        #红灯亮
        pyb.LED(RED_LED_PIN).on()
        sensor.skip_frames(time = 2000)

        #红灯灭，蓝灯亮
        pyb.LED(RED_LED_PIN).off()
        #pyb.LED(BLUE_LED_PIN).on()

        #保存截取到的图片到SD卡
        print(n)
        sensor.snapshot().save("singtown/s%s/%s.pgm" % (flag_count, n) )
        n -= 1
        #pyb.LED(BLUE_LED_PIN).off()
        print("Done! Reset the camera to see the saved image.")

    print("\n第%s个人拍照"%(flag_count+1))

#====================================人脸识别========================================
def face_recognition1():
    sensor.reset() # Initialize the camera sensor.
    sensor.set_pixformat(sensor.GRAYSCALE) # or sensor.GRAYSCALE
    sensor.set_framesize(sensor.QQVGA) # or sensor.QQVGA (or others)
    sensor.set_hmirror(True)              # 设置水平镜像（翻转180度）
    sensor.set_vflip(True)
    sensor.skip_frames(10) # Let new settings take affect.
    sensor.skip_frames(time = 5000) #等待5s
    NUM_SUBJECTS = flag_count #图像库中不同人数，一共2人
    NUM_SUBJECTS_IMGS = 10 #每人有10张样本图片

    img = sensor.snapshot()
    d0 = img.find_lbp((0, 0, img.width(), img.height()))
    #d0为当前人脸的lbp特征
    img = None
    pmin = 99999
    num=0

    for s in range(1, NUM_SUBJECTS+1):
        dist = 0
        print("1111")
        for i in range(2, NUM_SUBJECTS_IMGS+1):
            img = image.Image("singtown/s%d/%d.pgm"%(s, i))
            d1 = img.find_lbp((0, 0, img.width(), img.height()))
            #d1为第s文件夹中的第i张图片的lbp特征
            dist += image.match_descriptor(d0, d1)#计算d0 d1即样本图像与被检测人脸的特征差异度。
        print("Average dist for subject %d: %d"%(s, dist/NUM_SUBJECTS_IMGS))

        if (dist/NUM_SUBJECTS_IMGS)<pmin:
            pmin=(dist/NUM_SUBJECTS_IMGS)
            num=s
        print(pmin)

    print(num) # num为当前最匹配的人的编号。

    if num==1:
        #uart.write(data_information_start)
        #uart.write("ZWH")
        #uart.wirte(data_information_over)
        print("You are ZWH")
    elif num==2:
        #uart.write(data_information_start)
        #uart.write("KJY")
        #uart.write(data_information_over)
       print("You are KJY")



#==============================人脸识别：识别队友============================
def face_recognition():
    sensor.reset() # Initialize the camera sensor.
    sensor.set_pixformat(sensor.GRAYSCALE) # or sensor.GRAYSCALE
    sensor.set_framesize(sensor.QQVGA) # or sensor.QQVGA (or others)
    sensor.set_hmirror(True)              # 设置水平镜像（翻转180度）
    sensor.set_vflip(True)
    sensor.skip_frames(10) # Let new settings take affect.
    sensor.skip_frames(time = 5000) #等待5s
    NUM_SUBJECTS = 2 #图像库中不同人数，一共2人
    NUM_SUBJECTS_IMGS = 10 #每人有10张样本图片

    # 拍摄当前人脸。
    img = sensor.snapshot()
    d0 = img.find_lbp((0, 0, img.width(), img.height()))
    #d0为当前人脸的lbp特征
    img = None
    pmin = 999999
    num=0

    for s in range(1, NUM_SUBJECTS+1):
        dist = 0
        for i in range(2, NUM_SUBJECTS_IMGS+1):
            img = image.Image("singtown/s%d/%d.pgm"%(s,i))
            d1 = img.find_lbp((0, 0, img.width(), img.height()))
            #d1为第s文件夹中的第i张图片的lbp特征
            dist += image.match_descriptor(d0, d1)#计算d0 d1即样本图像与被检测人脸的特征差异度。
        print("Average dist for subject %d: %d"%(s, dist/NUM_SUBJECTS_IMGS))
        if (dist/NUM_SUBJECTS_IMGS)<pmin:
            pmin=(dist/NUM_SUBJECTS_IMGS)
            num=s

    print(num) # num为当前最匹配的人的编号。
    #uart.write(str(num))
    sending_data(num)
    if num==1:
        #uart.write(data_information_start)
        #uart.write("You are ZWH")
        #uart.write(data_information_over)
        print("You are ZWH")
    elif num==2:
        #uart.write(data_information_start)
        #uart.write("You are KJY")
        #uart.write(data_information_over)
        print("You are KJY")


#=================================口罩检测==========================================
def mask_detect():
    sensor.reset()                         # Reset and initialize the sensor.
    sensor.set_pixformat(sensor.RGB565)    # Set pixel format to RGB565 (or GRAYSCALE)
    sensor.set_framesize(sensor.QVGA)      # Set frame size to QVGA (320x240)
    sensor.set_windowing((240, 240))       # Set 240x240 window.
    sensor.set_hmirror(True)              # 设置水平镜像（翻转180度）
    sensor.set_vflip(True)
    sensor.skip_frames(time=2000)          # Let the camera adjust.

    net = None
    labels = None
    min_confidence = 0.5
    flag_mask = 0


    try:
        # load the model, alloc the model file on the heap if we have at least 64K free after loading
        net = tf.load("trained.tflite", load_to_fb=uos.stat('trained.tflite')[6] > (gc.mem_free() - (64*1024)))
    except Exception as e:
        raise Exception('Failed to load "trained.tflite", did you copy the .tflite and labels.txt file onto the mass-storage device? (' + str(e) + ')')

    try:
        labels = [line.rstrip('\n') for line in open("labels.txt")]
    except Exception as e:
        raise Exception('Failed to load "labels.txt", did you copy the .tflite and labels.txt file onto the mass-storage device? (' + str(e) + ')')

    colors = [ # Add more colors if you are detecting more than 7 types of classes at once.
        (255,   0,   0),
        (  0, 255,   0),
        (255, 255,   0),
        (  0,   0, 255),
        (255,   0, 255),
        (  0, 255, 255),
        (255, 255, 255),
    ]

    clock = time.clock()
    while(True):
        clock.tick()

        img = sensor.snapshot()

        # detect() returns all objects found in the image (splitted out per class already)
        # we skip class index 0, as that is the background, and then draw circles of the center
        # of our objects

        for i, detection_list in enumerate(net.detect(img, thresholds=[(math.ceil(min_confidence * 255), 255)])):
            if (i == 0): continue # background class
            if (len(detection_list) == 0): continue # no detections for this class?
            if i == 1:  # 如果检测到人脸
               flag_mask = 1
            elif (i ==0 or i == 2):        # 如果检测到口罩
               flag_mask = 0
            sending_data(flag_mask)

            print("********** %s **********" % labels[i])
            for d in detection_list:
                [x, y, w, h] = d.rect()
                center_x = math.floor(x + (w / 2))
                center_y = math.floor(y + (h / 2))
                print('x %d\ty %d' % (center_x, center_y))
                img.draw_circle((center_x, center_y, 12), color=colors[i], thickness=2)




#=============================主函数================================
while (True):
    clock.tick()
    img = sensor.snapshot()# 镜头初始化
    if uart.any():
        Receive_Prepare()
        print(data1_function, data2_function)
        if(data1_function == 1):
            print(take_photos)
            take_photos()       #现场学习功能
        elif(data1_function == 2):
            face_recognition()  #人脸识别功能：识别三个队友
        elif(data1_function == 3):
            face_recognition1() #人脸识别功能：识别现场学习的人员
        elif(data1_function == 4):
            mask_detect()


