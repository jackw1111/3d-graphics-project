import sys
sys.path.append("../engine/bin")
sys.path.append("../engine/utils")
from engine.graphics import *

def screen_to_ndc(start, WIDTH, HEIGHT):
    x1 = start.x;
    y1 = start.y;
    w = WIDTH;
    h = HEIGHT;

    # convert from screen space to NDC
    x1 = 2*x1 / w - 1;
    y1 = 2*y1 / h - 1;


    start.x = x1;
    start.y = y1;

    return start

def mouse_ray(lastX, lastY, camera, WIDTH, HEIGHT):
    x = (2.0 * lastX) / WIDTH - 1.0
    y = 1.0 - (2.0 * lastY) / HEIGHT
    z = 1.0
    ray_nds = vec3(x, y, z)
    ray_clip = vec4(ray_nds.x, ray_nds.y, -1.0, 1.0)

    # # eye space to clip we would multiply by projection so
    # # clip space to eye space is the inverse projection
    ray_eye = inverse(camera.projection_matrix) * ray_clip

    # # convert point to forwards
    ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0)
    # # world space to eye space is usually multiply by view so
    # # eye space to world space is inverse view
    #print (self.player.view)
    inv_ray_wor = (inverse(camera.view_matrix) * ray_eye)
    ray_wor = vec3(inv_ray_wor.x, inv_ray_wor.y, inv_ray_wor.z)
    ray_wor = normalize(ray_wor)
    return ray_wor