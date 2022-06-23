import picamera
import picamera.array
import cv2
import numpy as np
import time

with picamera.PiCamera() as camera:
	with picamera.array.PiRGBArray(camera) as stream:
		camera.resolution=(320,240)
		t_1=0#before time
		t=0#now time
		i=1
		c='a'
		firstloopfl=True
		t_1=time.time()
		t=time.time()
		dt=0
		while(1):
			#get the time
			t_1=t
			t=time.time()
			dt=t-t_1
			#Save previous image
			if i==2:
				gray_1=gray
			elif i==3:
				gray_2=gray_1
				gray_1=gray
			elif i>=4:
				gray_3=gray_2
				gray_2=gray_1
				gray_1=gray
			#get image
			camera.capture(stream,'bgr',use_video_port=True)
			im=cv2.GaussianBlur(stream.array,(5,5),0)#GaussianFilter
			im1=cv2.bilateralFilter(im,9,75,75)
			gray=cv2.cvtColor(im1, cv2.COLOR_RGB2GRAY)

			if i==1:#1st Getting image
				gray_1=gray
				bi=gray#Register in the background image
				hsv = np.zeros_like(im)
				hsv[...,1] = 255
			elif i>=4:
				#Background subtraction method
				db=cv2.absdiff(gray,bi)
				db_1=cv2.absdiff(gray_1,bi)
				
				#OpticalFlow
				flow = cv2.calcOpticalFlowFarneback(db,db_1, None, 0.5, 3, 15, 3, 5, 1.2, 0)
				
				#Convert from vector to coordinate system
				mag, ang = cv2.cartToPolar(flow[...,0], flow[...,1])
				hsv[...,0] = ang*180/np.pi/2
				hsv[...,2] = cv2.normalize(mag,None,0,255,cv2.NORM_MINMAX)
				rgb1 = cv2.cvtColor(hsv,cv2.COLOR_HSV2BGR)
				rgb=cv2.GaussianBlur(rgb1,(5,5),0)
				img2=[hsv[...,0],hsv[...,0],hsv[...,0]]
				
				angle= np.floor(ang*180/np.pi)#240,320
				#max magnitude
				maxer=np.argmax(mag)
				maxery=maxer/320
				maxerx=maxer%320
				#mode of direction
				uniqs,cntss=np.unique(angle,return_counts=True)
				u2=uniqs[cntss==np.amax(cntss)].min()
					
				if u2<45 or u2>315:
					u='l'
				elif 45<= u2<135:
					u='u'
				elif 135 <= u2<225:
					u='r'
				elif 225 <= u2<315:
					u='d'
				velocity=(mag[maxery,maxerx]/2)/dt
				print(u,velocity)#direction,velocity
				#Show
				cv2.imshow('frame0',im)
				cv2.imshow('frame2',rgb)
				#Save image
				cv2.imwrite('im_db.png',db)
				cv2.imwrite('im_db_1.png',db_1)
				cv2.imwrite('im_3.png',gray_3)
				cv2.imwrite('im_2.png',gray_2)
				cv2.imwrite('im_1.png',gray_1)
				cv2.imwrite('im.png',gray)
				cv2.imwrite('imrgb.png',rgb)
				
			i=i+1	
			#finish function pushing 'q'
			if cv2.waitKey(1)&0xFF==ord('q'):
				cv2.destroyAllWindows()
				break
			stream.seek(0)
			stream.truncate()


