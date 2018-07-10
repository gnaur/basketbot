import numpy as np
import math
import cv2
import os
import json
import serial
import time
#from scipy.special import expit
#from utils.box import BoundBox, box_iou, prob_compare
#from utils.box import prob_compare2, box_intersection
from ...utils.box import BoundBox
from ...cython_utils.cy_yolo2_findboxes import box_constructor

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
		x = int(x)
		y = int(y)
		size = int(size)
		print("start")
		port="/dev/rfcomm0"
		bluetooth=serial.Serial(port, timeout = 1)
		print("connecetd")
		bluetooth.flushInput()
		print ("ping")
		if x <100:
			x = str(x)
			x = str(0) + x
			if len(x) == 2:
				x = str(0) + x
		else:
			x = str(x)
		if y <100:
			y = str(y)
			y = str(0) + y
			if len(y) == 2:
				y = str(0) + y
		else:
			y = str(y)
		if size <100:
			size = str(size)
			size = str(0) + size
			if len(size) == 2:
				size = str(0) + size
		else:
			size = str(size)
		data = x + y + size
		print(data)
		bluetooth.write(data.encode())       
		"""input=bluetooth.readline()
		print (input.decode())
		time.sleep(0.1)
		bluetooth.close()
		print("done")"""
     

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
