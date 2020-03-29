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
	if 'action' in request.GET:
		action = request.GET['action']
#		print ('"%s" command received' % action)
		# ========== Calibration init =========
		if action == 'calibration':
			bot.calibration()
		# ========== Calibration confirmation =========
		elif action == 'calibration_ok':
			bot.calibration_ok()

		# ========== Camera calibration =========
		elif action == 'camcaliup':
			bot.calibration_camup()
		elif action == 'camcalidown':
			bot.calibration_camdown()
		elif action == 'camcalileft':
			bot.calibration_camleft()
		elif action == 'camcaliright':
			bot.calibration_camright()

		# ========= Front wheel calibration ===========
		elif action == 'fwcalileft':
			bot.calibration_fwleft()
		elif action == 'fwcaliright':
			bot.calibration_fwright()

		# ========= Back wheel calibration ===========
		elif action == 'bwcalileft':
			bot.calibration_bwleft()
		elif action == 'bwcaliright':
			bot.calibration_bwright()

		else:
			print ('command error, error command "%s" received' % action)
	return render_to_response("cali.html")

def connection_test(request):
	return HttpResponse('OK')