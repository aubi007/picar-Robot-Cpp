'''
**********************************************************************
* Filename    : bot
* Description : main picar automation logic
* Author      : 
* Brand       : 
* E-mail      : 
* Website     : 
* Update      : 
**********************************************************************
'''

#from pibot.driver import back_wheels, front_wheels, filedb
import os
import threading    
import time
#import pibot.driver as driver
#from ..driver import camera
from ..stream import stream

class Bot(object):
    # pibot driving and status variables
    bw_status = 0           # 0 stop, 1 forward, -1 backwards
    fw_pos = 0              # front wheel position -90 - 90
    speed = 60              # 0 - 100
    cam_pan = 0             # left-right camea position -90 - 90
    cam_tilt = 0            # uo-down camera position -90 - 90
    
    def __init__(self):
        print ('bot.init')

        # start streaming
        self.str = stream.VideoCamera()

    def backup(self):
        # Build paths inside the project like this: os.path.join(BASE_DIR, ...)
        BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        driver.setup()
                
        # read the configuration file for wheels and camera modules
        db_file = BASE_DIR + "/config"
        self.fw = front_wheels.Front_Wheels(debug=False, db=db_file)
        self.bw = back_wheels.Back_Wheels(debug=False, db=db_file)
        self.cam = camera.Camera(debug=False, db=db_file)
        
        # set the wheels and camera to ready position according to the configuration
        self.cam.ready()
        self.bw.ready()
        self.fw.ready()
      
    
        # create an event for command changed notification
        self.command_changed = threading.Event()
         
        # start the pibot tick thread
        threading.Thread(target=self.tick, args=()).start()

    # status setter with notification
    def set_bw_status(self, bw_status):
        if self.bw_status != bw_status:
            self.bw_status = bw_status
            self.command_changed.set()
    
    # safe get bw_status
    def get_bw_status(self):
        if self.bw_status == -1: return -1
        elif self.bw_status == 0: return 0
        elif self.bw_status == 1: return 1
        else: 
            self.bw_status = 0
            return 0

    # speed setter with notification
    def set_speed(self, speed):
        print ('new speed ',speed)
        if self.speed != speed:
            self.speed = speed
            self.command_changed.set()
            
    # front wheel setter with notification
    def set_fw_pos(self, fw_pos):
        if self.fw_pos != fw_pos:
            self.fw_pos = fw_pos
            self.command_changed.set()
    
    # camera pan setter with notification
    def set_cam_pan(self, cam_pan):
        if self.cam_pan != cam_pan:
            self.cam_pan = cam_pan
            self.command_changed.set()
    
    # camera tilt setter with notification
    def set_cam_tilt(self, cam_tilt):
        if self.cam_tilt != cam_tilt:
            self.cam_tilt = cam_tilt
            self.command_changed.set()

    # move camera to ready position
    def cam_ready(self):
        self.set_cam_pan(0)
        self.set_cam_tilt(0)
    
    def get_speed(self):
        if self.speed < 0: 
            self.speed = 0
            return 0
        elif self.speed > 100: 
            self.speed = 100
            return 100
        else: return self.speed
    
    # get a status formated as a string
    def get_status_line(self):
        status_line = 'BW:{} FW:{} SPD:{} pan:{} tilt:{}'.format(self.bw_status, self.fw_pos, self.speed, self.cam_pan, self.cam_tilt)
        return status_line

    #run a test procedure
    def run_test(self, test):
        threading.Thread(target=self.test1, args=()).start()

    def test2(self):
        if self.str.filter_lane == True:
            self.str.filter_lane = False
        else:
            self.str.filter_lane = True
        
        
    def test1(self):
        self.set_speed(60)
        self.set_bw_status(1)
        self.set_fw_pos(0)
        self.set_cam_pan(0)
        self.set_cam_tilt(0)
        time.sleep(5)
        self.set_fw_pos(-30)
        time.sleep(3)
        self.set_bw_status(-1)
        time.sleep(3)
        self.set_fw_pos(0)
        time.sleep(5)
        self.set_bw_status(0)

    # Endless loop
    def tick(self):
        # initialise driving variables
        bw_status = self.bw_status
        fw_pos = self.fw_pos
        speed = self.speed
        cam_pan = self.cam_pan
        cam_tilt = self.cam_tilt
        
        while True:
            # get the new set of variables
            new_bw_status = self.get_bw_status()
            new_fw_pos = self.fw_pos
            new_speed = self.get_speed()
            new_cam_pan = self.cam_pan
            new_cam_tilt = self.cam_tilt
            
            print ('>>> Status BW: ', new_bw_status, 'FW: ', new_fw_pos, ' SPD: ', new_speed, ' pan: ', new_cam_pan, ' tilt: ', new_cam_tilt)
            
            # check new state of back wheels
            if bw_status != new_bw_status or speed != new_speed:
                
                # new state is forward
                if new_bw_status == 1:
                    self.bw.speed = new_speed
                    self.bw.forward()
                
                # new state is stop
                elif new_bw_status == 0:
                    self.bw.speed = new_speed
                    self.bw.stop()
                    
                # new state is forward
                elif new_bw_status == -1:
                    self.bw.speed = new_speed
                    self.bw.backward()
                                            
                bw_status = new_bw_status
                speed = new_speed
            
            # check new front wheels position
            if fw_pos != new_fw_pos:
                self.fw.wheel.write(90 + new_fw_pos)
                fw_pos = new_fw_pos

            # check new pan position
            if cam_pan != new_cam_pan:
                self.cam.pan_servo.write(90 + new_cam_pan)
                cam_pan = new_cam_pan
            
            # check new tilt position
            if cam_tilt != new_cam_tilt:
                self.cam.tilt_servo.write(90 + new_cam_tilt)
                cam_tilt = new_cam_tilt
                
            
            # sleep until the next tick
            self.command_changed.clear()
            self.command_changed.wait(5)
            
bot = Bot()
