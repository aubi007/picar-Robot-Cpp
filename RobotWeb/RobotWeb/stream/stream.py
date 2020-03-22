#!/usr/bin/env python
'''
**********************************************************************
* Filename    : stream.py
* Description : A streamer module base on cv2
* Author      : 
* Brand       : 
* E-mail      : 
* Website     : 
* Update      : 
*               
**********************************************************************
'''
import cv2
import threading
from datetime import datetime
#from pibot.stream import lanefilter, lanefilter2, lanefilter3
import time

class VideoCamera(object):
    frame_width = 0             # frame width
    frame_height = 0            # frame height
    frame = None                # last captured frame
    frame_ts = None             # capture end timestamp
    grabbed = False             # frame captured flag
    filter_lane = False         # apply lane filter    
    
    def __init__(self):
        print ('stream.init')
        # create the videocapture object for the camera with index 0 
        self.video = cv2.VideoCapture(0)
        
        # get the camera resolution
        self.frame_width = self.video.get(3)
        self.frame_height = self.video.get(4)
        print ("frame size ", self.frame_width, "x", self.frame_height)
        
        # capture first frame to return immediately by the get_frame call
        (self.grabbed, self.frame) = self.video.read()
        self.frame_ts = datetime.now()
        
        # start a new thread that continuously captures a frame
        threading.Thread(target=self.update, args=()).start()

    def __del__(self):
        print ('stream.destroy')
        self.video.release()

    def get_frame(self, status):
#        print ('stream.get_frame')
        image = self.frame
        
        if image is not None:
            # put status layer on frame
            font = cv2.FONT_HERSHEY_SIMPLEX
            cv2.putText(image,status,(10,16), font, 0.5,(255,255,255))
            
            ret, jpeg = cv2.imencode('.jpg', image)
            return (jpeg.tobytes(), self.frame_ts)
        else:
            return (None, None)

    def screenshot(self):
        image = self.frame
        
        if image is not None:        
            ret, jpeg = cv2.imencode('.jpg', image)
            d = datetime.now()
            fn = 'screenshots/{:%Y%m%d%H%M%S}.jpg'.format(d)
            print (fn)
            cv2.imwrite(fn,image)
            
        return
    
    def update(self):
        while True:
#            start = datetime.now()
#            print start.time(), 'frame capture started'
            (self.grabbed, frame) = self.video.read()
                        
#            if self.filter_lane: 
#                frame = lanefilter3.pipeline(frame);

                      
            self.frame = frame
            self.frame_ts = datetime.now()
            
#            time.sleep(1)
#            end = datetime.now()
#            print (end.time(), 'frame captured and processed, duration' , (end - start))
#            print end.time(), 'frame capture ended, duration' , (end - start)

    def applyLaneFilter(self):
        print ('booo')
        
        
        
        
    