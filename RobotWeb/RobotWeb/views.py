'''
**********************************************************************
* Filename    : views
* Description : views for server
* Author      : Cavon
* Brand       : SunFounder
* E-mail      : service@sunfounder.com
* Website     : www.sunfounder.com
* Update      : Cavon    2016-09-13    New release
**********************************************************************
'''

from django.shortcuts import render_to_response
from django.http import HttpResponse
from django.http import StreamingHttpResponse

from RobotApp import RobotApp

# create a RobotApp class instance
bot = RobotApp()

def home(request):
	return render_to_response("base.html")

# yields a streaming generator from camera frames
def yield_generator():
	last_ts = None			# timestamp of the last processed frame
	while True:
		ret = bot.getFrame();
		# yield a new frame only when timestamp has changed
		if ret.data is not None and ret.ts != last_ts:
			last_ts = ret.ts
			yield(b'--frame\r\n'
				  b'Content-Type: image/jpeg\r\n\r\n' + bytes(ret.data) + b'\r\n\r\n')
			
def livefe(request):
	try:
		gen = yield_generator()
		return StreamingHttpResponse(gen, content_type="multipart/x-mixed-replace;boundary=frame")
	except:
		print ('error')
		pass

def run(request):
	debug = ''
	if 'action' in request.GET:
		action = request.GET['action']
		# ============== Back wheels =============
		if action == 'bwready':
			bot.setBWStatus(0)
		elif action == 'forward':
			bot.setBWStatus(1)
		elif action == 'backward':
			bot.setBWStatus(-1)
		elif action == 'stop':
			bot.setBWStatus(0)

		# ============== Front wheels =============
		elif action == 'fwready':
			bot.setFWPos(0)
		elif action == 'fwleft':
			bot.setFWPos(-40)
		elif action == 'fwright':
			bot.setFWPos(40)
		elif action == 'fwstraight':
			bot.setFWPos(0)
			
		# ================ Camera =================
		elif action == 'camready':
			bot.camReady()
		elif action == "camleft":
			bot.setCamPan(40)
		elif action == 'camright':
			bot.setCamPan(-40)
		elif action == 'camup':
			bot.setCamTilt(20)
		elif action == 'camdown':
			bot.setCamTilt(-20)
		
		# ================ test commands =============
		elif action == 'screenshot':
			bot.screenshot()
		elif action == 'test':
			bot.test(1)
			
	# ================ Speed =================	
	if 'speed' in request.GET:
		# print ('speeed')
		speed = int(request.GET['speed'])
		if speed < 0:
			speed = 0
		if speed > 100:
			speed = 100
		bot.setSpeed(speed)	
			
	return render_to_response("run.html")
			

	
def cali(request):
	return render_to_response("cali.html")

#	if 'action' in request.GET:
#		action = request.GET['action']
#		# ========== Camera calibration =========
#		if action == 'camcali':
#			print ('"%s" command received' % action)
#			cam.calibration()
#		elif action == 'camcaliup':
#			print ('"%s" command received' % action)
#			cam.cali_up()
#		elif action == 'camcalidown':
#			print ('"%s" command received' % action)
#			cam.cali_down()
#		elif action == 'camcalileft':
#			print ('"%s" command received' % action)
#			cam.cali_left()
#		elif action == 'camcaliright':
#			print ('"%s" command received' % action)
#			cam.cali_right()
#		elif action == 'camcaliok':
#			print ('"%s" command received' % action)
#			cam.cali_ok()

#		# ========= Front wheel cali ===========
#		elif action == 'fwcali':
#			print ('"%s" command received' % action)
#			fw.calibration()
#		elif action == 'fwcalileft':
#			print ('"%s" command received' % action)
#			fw.cali_left()
#		elif action == 'fwcaliright':
#			print ('"%s" command received' % action)
#			fw.cali_right()
#		elif action == 'fwcaliok':
#			print ('"%s" command received' % action)
#			fw.cali_ok()

#		# ========= Back wheel cali ===========
#		elif action == 'bwcali':
#			print ('"%s" command received' % action)
#			bw.calibration()
#		elif action == 'bwcalileft':
#			print ('"%s" command received' % action)
#			bw.cali_left()
#		elif action == 'bwcaliright':
#			print ('"%s" command received' % action)
#			bw.cali_right()
#		elif action == 'bwcaliok':
#			print ('"%s" command received' % action)
#			bw.cali_ok()
#		else:
#			print ('command error, error command "%s" received' % action)



def connection_test(request):
	return HttpResponse('OK')