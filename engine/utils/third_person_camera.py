import sys
from engine.graphics import Camera, vec3
from math import sin, cos, radians

class ThirdPersonCamera(Camera):

    def __init__(self, *args, **kwargs):
        Camera.__init__(self, *args, **kwargs)
        self.position = vec3(0,0,0)
        self.distance = 20.0

    def get_position(self, active_camera, look_at):
        ytheta = -active_camera.pitch
        if (ytheta < 0.0):
            ytheta = 0.0
            
        # algorithm from ThinMatrix video on third person cameras
        horizDist = self.distance * cos(radians(ytheta))
        vertDist = self.distance * sin(radians(ytheta))
        xtheta = active_camera.yaw - 90.0
        offsetx = horizDist * sin(radians(-xtheta))
        offsetz = horizDist * cos(radians(xtheta))
        return look_at + vec3(-offsetx, vertDist, -offsetz)