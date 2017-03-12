import cv2
import numpy as np
import numpy.random as rand
import math

class random_cropper:
	def __init__(self, crop_num, crop_width, crop_height, min_obj_height = 0.97, max_obj_height = 0.9999, translate_range = 0.02, 
                 rotation_range = 30, horizontal_flip = True, vertical_flip = True, color_jitter = True):
		self._crop_num = crop_num
		self._crop_width = crop_width
		self._crop_height = crop_height
		self._has_gaussian = False
		self._horizontal_flip = horizontal_flip
		self._max_obj_height = max_obj_height
		self._min_obj_height = min_obj_height
		self._next_gaussian = 0
		self._rotation_range = rotation_range
		self._translate_range = translate_range
		self._vertical_flip = vertical_flip
		self._color_jitter = color_jitter		

	def crop(self, img):
		imgs = []
		for i in range(0, self._crop_num):
			imgs.append(self._crop(img))

		return imgs		

	def set_crop_height(self, val):
		self._crop_height = val

	def set_crop_num(self, val):
		self._crop_num = val

	def set_crop_width(self, val):
		self._crop_width = val

	def set_max_obj_height(self, val):
		self._max_obj_height = val

	def set_min_obj_height(self, val):
		self._min_obj_height = val

	def set_rotation_range(self, val):
		self._rotation_range = val

	def set_translate_range(self, val):
		self._translate_range = val	

	def _crop(self, img):
		output_img = self._random_crop(img)

		if(rand.random() > 0.5):
			output_img = self._random_rotate(output_img) if rand.random() > 0.5 else self._random_rotate_bound(output_img)		
		#	output_img = self._random_rotate(output_img)
		if(self._vertical_flip and rand.random() > 0.5):
			output_img = cv2.flip(output_img, 1)

		output_img = cv2.resize(output_img, (self._crop_width, self._crop_height))
		if(self._horizontal_flip and rand.random() > 0.5):
			output_img = cv2.flip(output_img, 0)
		
		if(rand.random() > 0.5):
			output_img = self._random_translate(output_img)
		if(self._color_jitter and rand.random() > 0.5):
			output_img = self._jitter_color(output_img)

		#print "random crop img shape 0:" + str(output_img.shape)
		#print "random crop img shape 1:" + str(self._crop_width) + "," + str(self._crop_height)
		return output_img

	def _get_random_gaussian(self):
		if(self._has_gaussian):
			self._has_gaussian = False
			return self._next_gaussian

		max_int32_num = np.iinfo(np.int32).max		
		rndmax = float(np.iinfo(np.uint32).max)
		x1 = 0
		x2 = 0
		w = 0
		while True:
			rnd1 = rand.randint(0, max_int32_num)/rndmax
			rnd2 = rand.randint(0, max_int32_num)/rndmax

			x1 = 2.0 * rnd1 - 1.0
			x2 = 2.0 * rnd2 - 1.0
			w = x1 * x1 + x2 * x2
			if(w < 1.0):
				break
		w = math.sqrt( (-2.0 * math.log(w)) / w )
		self._next_gaussian = x2 * w
		self._has_gaussian = True

		return x1 * w

	def _jitter_color(self, img):		
		tform = np.matrix([[-66.379,    25.094,   6.79698], [-68.0492, -0.302309,  -13.9539], [-68.4907,  -24.0199,   7.27653]])
		v = np.matrix([self._get_random_gaussian(), self._get_random_gaussian(), self._get_random_gaussian()]).T
		#print "v1"
		#print v
		v = np.round(tform*0.1*v)
		#print "v2" 
		#print v
		roffset = v[0]
		goffset = v[1]
		boffset = v[2]
		img[:,:,2] = cv2.add(img[:,:,2], roffset)
		img[:,:,1] = cv2.add(img[:,:,1], goffset)
		img[:,:,0] = cv2.add(img[:,:,0], boffset)		
		        
		return img
		
	def _random_crop(self, img):
		scale = self._min_obj_height + np.random.random() * (self._max_obj_height - self._min_obj_height)
		height, width = img.shape[:2]
		size = int(scale * min(height, width))
		#print str(scale) + ":" + str(size)
		x_offset = np.random.randint(0, 3200000) % (width - size)
		y_offset = np.random.randint(0, 3200000) % (height - size)
		#print "x_offset,y_offset:" + str(x_offset) + "," + str(y_offset)
		return img[y_offset:, x_offset:]

	def _random_rotate_bound(self, img):
		# grab the dimensions of the image and then determine the center
		(h, w) = img.shape[:2]
		(cX, cY) = (w // 2, h // 2)
 
		# grab the rotation matrix (applying the negative of the
		# angle to rotate clockwise), then grab the sine and cosine
		# (i.e., the rotation components of the matrix)
		sign = -1 if rand.random() > 0.5 else 1
		angle = rand.random() * self._rotation_range * sign
		M = cv2.getRotationMatrix2D((cX, cY), -angle, 1.0)
		cos = np.abs(M[0, 0])
		sin = np.abs(M[0, 1])
 
		# compute the new bounding dimensions of the image
		nW = int((h * sin) + (w * cos))
		nH = int((h * cos) + (w * sin))
 
		# adjust the rotation matrix to take into account translation
		M[0, 2] += (nW / 2) - cX
		M[1, 2] += (nH / 2) - cY
 
		# perform the actual rotation and return the image
		return cv2.warpAffine(img, M, (nW, nH))

	def _random_rotate(self, img):
		rows, cols = img.shape[:2]
		sign = -1 if rand.random() > 0.5 else 1
		angle = rand.random() * self._rotation_range * sign		
		M = cv2.getRotationMatrix2D((cols/2,rows/2),angle,1)

		return cv2.warpAffine(img,M,(cols,rows))


	def _random_translate(self, img):
		(h,w) = img.shape[:2]
		scale = rand.random()
		M = np.float32([[1,0, w*self._translate_range*scale ],[0,1,h*self._translate_range*scale]])

		return cv2.warpAffine(img,M, (w,h))

