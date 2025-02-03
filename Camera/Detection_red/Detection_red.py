import pyjevois
if pyjevois.pro: import libjevoispro as jevois
else: import libjevois as jevois

import cv2
import numpy as np 
import math
import sympy

## Simple test of programming JeVois modules in Python
#
# This module by default simply draws a cricle and a text message onto the grabbed video frames.
#
# Feel free to edit it and try something else. Note that this module does not import OpenCV, see the PythonOpenCV for a
# minimal JeVois module written in Python that uses OpenCV.
#
# See http://jevois.org/tutorials for tutorials on getting started with programming JeVois in Python without having
# to install any development software on your host computer.
#
# @author Lamkadem Adil - El Bertal Othmane 
# 
# @videomapping YUYV 640 480 15.0 YUYV 640 480 15.0 JeVois PythonTest
# @email itti\@usc.edu
# @address University of Southern California, HNB-07A, 3641 Watt Way, Los Angeles, CA 90089-2520, USA
# @copyright Copyright (C) 2017 by Laurent Itti, iLab and the University of Southern California
# @mainurl http://jevois.org
# @supporturl http://jevois.org/doc
# @otherurl http://iLab.usc.edu
# @license GPL v3
# @distribution Unrestricted
# @restrictions None
# @ingroup modules


def rotation_matrix(axis, angle):
    #axis = np.asarray(axis)
    axis = axis / np.linalg.norm(axis)
                  
    ux,uy,uz = axis
    cos_a = np.cos(angle)
    sin_a = np.sin(angle)
    one_minus_cos = 1 - cos_a
    return np.array([[cos_a + ux**2* one_minus_cos, ux*uy*one_minus_cos - uz*sin_a, ux*uz* one_minus_cos + uy*sin_a],
                     [uy*uz*one_minus_cos + uz*sin_a , cos_a + uy**2*one_minus_cos , uy*uz*one_minus_cos - ux*sin_a],
                     [uz*ux*one_minus_cos - uy * sin_a , uz * uy * one_minus_cos + ux*sin_a , cos_a + uz**2*one_minus_cos]])
    
             
def intersection_droite_plan(vecteur_directeur, point_droite,equation_plan):
    a,b,c = vecteur_directeur
    x0,y0,z0 = point_droite
    A,B,C,D = equation_plan
    
    #calcul du dénominateur 
    denominateur = A * a + B * b + C * c
        
    #Si le dénominateur est nul, la droite est parallèle au plan 
    if abs(denominateur) < 1e-10:
        return None # pas d'intersection ou la droite est incluse dans le plan 
          
    #Calcul du paramètre 
    t = -(A * x0 + B * y0 + C * z0 + D) / denominateur
       
    #Calcul des coordonnées du point d'intersection
    x = x0 + a * t 
    y = y0 + b * t
    z = z0 + c * t 
      
    return np.array([x, y, z])


class Detection_red:
    # ###################################################################################################
    
    ## Constructor
    def init(self):
        jevois.LINFO("DetectionRed Jevois init")
        
        pc = jevois.ParameterCategory("HSV Low Limit", "")
        self.h_min = jevois.Parameter(self, 'h_min', 'int', "h_min", 170, pc)
        self.s_min = jevois.Parameter(self, 's_min', 'int', "s_min", 200, pc)
        self.v_min = jevois.Parameter(self, 'v_min', 'int', "v_min", 100, pc)
        
        pc = jevois.ParameterCategory("HSV High Limit", "")
        self.h_max = jevois.Parameter(self, 'h_max', 'int', "h_max", 180, pc)
        self.s_max = jevois.Parameter(self, 's_max', 'int', "s_max", 255, pc)
        self.v_max = jevois.Parameter(self, 'v_max', 'int', "v_max", 255, pc)
        
        #self.h_min = 170
        #self.h_max = 180
        #self.s_min = 100
        #self.s_max = 255
        #self.v_min = 100
        #self.v_max = 255
        
        self.cpt = 0
        self.x_axis = np.array([1,0,0])
        self.y_axis = np.array([0,1,0])
        self.z_axis = np.array([0,0,1])
        
        self.v_initial = np.array([1,0,0])
        
        
        
        
     # ###################################################################################################
    ## Process function with GUI output on JeVois-Pro
    def processGUI(self, inframe, helper):
        # Start a new display frame, gets its size and also whether mouse/keyboard are idle:
        idle, winw, winh = helper.startFrame()

        # Draw full-resolution input frame from camera:
        
        
        # Get the next camera image (may block until it is captured):
        inimg = inframe.getCvRGB()
              
        lower_red = np.array([self.h_min.get(), self.s_min.get(), self.v_min.get()])
        upper_red = np.array([self.h_max.get(), self.s_max.get(), self.v_max.get()])
        hsv = cv2.cvtColor(inimg, cv2.COLOR_RGB2HSV)
        
        mask_red = cv2.inRange(hsv, lower_red, upper_red)
        
        
        mask_red = cv2.dilate(mask_red,None, iterations=2)
        
        contours, _= cv2.findContours(mask_red, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        
        height, width = inimg.shape[:2]
        
        f = 772
        Deg = 180/np.pi
        max_area = 0
        max_contour = None
        
        for contour in contours:
            area = cv2.contourArea(contour)
            if area > 500:
                if area > max_area:
                    max_area = area
                    max_contour = contour
                    
        if max_contour is not None:
            self.cpt = self.cpt + 1
            x,y,w,h = cv2.boundingRect(max_contour)
            cv2.rectangle(inimg,(x,y),(x+w,y+h),(0,255,0),2)
            center_x = x+w // 2
            center_y = y+h
            coordonnee_x = center_x - width // 2
            coordonnee_y = - center_y + height // 2
            num_alpha = np.sqrt(coordonnee_x**2 +coordonnee_y**2)
            alpha = np.arctan2(num_alpha, f)
            alpha = round(alpha * Deg,3)
            theta = np.arctan2(coordonnee_y, coordonnee_x)*Deg - 90
            if coordonnee_y < 0 and coordonnee_x < 0:
                theta += 360
            theta = round(theta, 3)
                    
            cv2.circle(inimg,(center_x,center_y),1,(0,0,255),4)     
            coords_text = f"({coordonnee_x}, {coordonnee_y}) | alpha = {alpha} | theta = {theta}"
            cv2.putText(inimg, coords_text, (20, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0,255,0),2)    
            # cv2.PutText(inimg,le texte, pntdepart du texte, police, taille, couleur, épaisseur)
            
            beta = np.radians(30)
            h = 0.25
            center = [0,0,h]
            
            rot1 = rotation_matrix(self.y_axis,beta)
            v_axe = np.dot(rot1, self.v_initial)
            
            rot2 = rotation_matrix(self.y_axis,-np.radians(alpha))
            v_alpha = np.dot(rot2, v_axe)
            
            rot3 = rotation_matrix(v_axe,np.radians(theta))
            v_director = np.dot(rot3, v_alpha)
            
            plan = [0,0,1,0] # Equation plan : 0x + 0y + z + 0 = 0 
            intersection = intersection_droite_plan(v_director, center, plan)
            x_i, y_i,z_i = intersection 
            ########################## 
            
            t = -h/v_director[2]
            
            int_x = t * v_director[0]
            int_y = t * v_director[1]
            
            
            
            ##########################
            point_text = f"cible : {round(x_i,3)},{round(y_i,3)}"
            axe_initial_text = f"axe_initial : {self.v_initial}"
            axe_optique_text = f"axe_optique : {v_axe}"
            alpha_text = f"axe_alpha : {v_alpha}"
            directeur_text = f"axe_cible : {v_director}"
            cv2.putText(inimg, point_text, (20, 60), cv2.FONT_HERSHEY_SIMPLEX, 1, (0,255,0),2)
            cv2.putText(inimg, axe_initial_text, (20, 90), cv2.FONT_HERSHEY_SIMPLEX, 1, (0,255,0),2)     
            cv2.putText(inimg, axe_optique_text, (20, 120), cv2.FONT_HERSHEY_SIMPLEX, 1, (0,255,0),2) 
            cv2.putText(inimg, alpha_text, (20, 150), cv2.FONT_HERSHEY_SIMPLEX, 1, (0,255,0),2)  
            cv2.putText(inimg, directeur_text, (20, 180), cv2.FONT_HERSHEY_SIMPLEX, 1, (0,255,0),2)   
            if self.cpt > 10:
            #    msgSend = int(int_x*1000).to_bytes(4, "big")
            #    msgSend += int(int_y*1000).to_bytes(4, "big")
            #    jevois.sendSerial(encodeUart(0x0041,msgSend,len(msgSend)))
                self.cpt = 0 
        cv2.circle(inimg,(width // 2,height // 2),5,(0,255,0),2)            
        x, y, w, h = helper.drawInputFrame("c", inframe, False, False)
        helper.drawImage("red",inimg, True,0,0, w, h, False, True)
        
        # End of frame:
        helper.endFrame()
    
    
    
def cam_checksum(msgFunction, msgPayload, lenghtPayload):
    checksum = 0
    checksum ^= 0xfe
    checksum ^= msgFunction.to_bytes(2, "big")[0]
    checksum ^= msgFunction.to_bytes(2, "big")[1]
    checksum ^= lenghtPayload.to_bytes(2, "big")[0]
    checksum ^= lenghtPayload.to_bytes(2, "big")[1]
    for i in msgPayload:
        checksum ^= i
    return checksum
    

def encodeUart(msgFunction, msgPayload, lenghtPayload):
    message = b'\xfe'
    message += msgFunction.to_bytes(2, "big")
    message += msgPayload.to_bytes(2, "big")
    message += lenghtPayload.to_bytes(2, "big")
    message += cam_checksum(msgFunction, msgPayload, lenghtPayload).to_bytes(1, "big")
    return message













































