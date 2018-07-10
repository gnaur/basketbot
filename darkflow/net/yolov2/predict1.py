import numpy as np
import math
import cv2
import os
import json
import serial
import time
import struct
#from scipy.special import expit
#from utils.box import BoundBox, box_iou, prob_compare
#from utils.box import prob_compare2, box_intersection
from ...utils.box import BoundBox
from ...cython_utils.cy_yolo2_findboxes import box_constructor
servomaxx = 180
servomaxy = 180
servocenterx = 90
servocentery = 110
screenmaxx = 600
screenmaxy = 430
distancex = 1
distancey = 1
incx = 10
incy =10
a =180
b= 180 




def expit(x):
	return 1. / (1. + np.exp(-x))

def _softmax(x):
	e_x = np.exp(x - np.max(x))
	out = e_x / e_x.sum()
	return out

def findboxes(self, net_out):
	# meta
	meta = self.meta
	boxes = list()
	boxes=box_constructor(meta,net_out)
	return boxes

def postprocess(self, net_out, im, save = True):
	"""
	Takes net output, draw net_out, save to disk
	"""
	global val
	boxes = self.findboxes(net_out)

	# meta
	meta = self.meta
	threshold = meta['thresh']
	colors = meta['colors']
	labels = meta['labels']
	if type(im) is not np.ndarray:
		imgcv = cv2.imread(im)
	else: imgcv = im
	h, w, _ = imgcv.shape

	resultsForJSON = []
	flg=0
	maxbox = None
	#boxes.sort()
	m = -2
	for b in boxes:
		boxResults = self.process_box(b, h, w, threshold)
		if  boxResults is not None and boxResults[-1] > m :
			m = boxResults[-1]
			maxbox = boxResults
	if m != -2:
		left, right, top, bot, mess, max_indx, confidence = maxbox
		size = (right + bot - left - top)/4
		x = (left + right)/2
		y = (top + bot)/2
		print(x,y,size)
		thick = int((h + w) // 300)
		"""if self.FLAGS.json:
									resultsForJSON.append({"label": mess, "confidence": float('%.2f' % confidence), "topleft": {"x": left, "y": top}, "bottomright": {"x": right, "y": bot}})
									continue"""

		cv2.rectangle(imgcv,
			(left, top), (right, bot),
			colors[max_indx], thick)
		cv2.putText(imgcv, mess, (left, top - 12),
			0, 1e-3 * h, colors[max_indx],thick//3)
#serial comm to arduino uno
		ser = serial.Serial('/dev/ttyACM0')		
		global a
		if a :
		 posx = a
		 posy = b
		else:
		 posx = 180 
		 posy = 180
		print(posx)
		if x< screenmaxx/2 - incx:
		 if posx >= incx:
		  posx += distancex
		elif x > screenmaxx/2 + incx:
		 if posx <= servomaxx - incx:
		  posx -= distancex
		#if y < screenmaxy/2 - incy:
		 #if posy >= 5:
		  #posy += distancey
		#elif y > screenmaxy/2 + incy:
		 #if posy <= 175:
		 # posy -= distancey
		
		a = posx
		b = posy
		k = str(a)
		a = 4
		print (a)
		ser.write(int(a))
		
		#if x > 300 and x < 500 and size > 80: #do nothing
		 #ser.write(struct.pack('>B', 0))
		#if size > 100: 	# if ball too close reverse
		 #ser.write(struct.pack('>B', 180))
		#elif x > 400 and size > 50: # move right
		 #ser.write(struct.pack('>B', 2))
		#elif x < 200 and size > 50: # move left
		 #ser.write(struct.pack('>B', 1))
		#elif size < 30:	# move forward
		 #ser.write(struct.pack('>B', 3))
		#elif y > 400: #move the arms
		 #ser.write(struct.pack('>B', 5))				
		#if x < 328:
		 #time.sleep(2)
		 #ser.write(struct.pack('>B', 1))
		#elif x > 328:
		 #time.sleep(1)
		 #ser.write(struct.pack('>B', 2))
		


	if not save: return imgcv

	outfolder = os.path.join(self.FLAGS.imgdir, 'out')
	img_name = os.path.join(outfolder, os.path.basename(im))
	if self.FLAGS.json:
		textJSON = json.dumps(resultsForJSON)
		textFile = os.path.splitext(img_name)[0] + ".json"
		with open(textFile, 'w') as f:
			f.write(textJSON)
		return

	cv2.imwrite(img_name, imgcv)
