import sys
sys.path.append("/home/me/3d-graphics-project/engine/bin")
from engine.graphics import *
from math import inf

def check_collisions(p1, p2):
    poly1 = p1
    poly2 = p2
    for shape in range(2):
        if (shape == 1):
            poly1 = p2
            poly2 = p1
 
        for a in range(len(poly1.translated_vertices)):
            b = (a + 1) % len(poly1.translated_vertices)
 
            axisProjx = -(poly1.translated_vertices[b][1] - poly1.translated_vertices[a][1])
            axisProjy = poly1.translated_vertices[b][0] - poly1.translated_vertices[a][0]
 
            minR1_xy = inf
            maxR1_xy = -inf
            for p in range(len(poly1.translated_vertices)):
                translated_vertex = vec2(poly1.translated_vertices[p].x, poly1.translated_vertices[p].y)
                q = dot(translated_vertex, vec2(axisProjx, axisProjy))
                minR1_xy = min(minR1_xy, q)
                maxR1_xy = max(maxR1_xy, q)
 
            minR2_xy = inf
            maxR2_xy = -inf
            for p in range(len(poly2.translated_vertices)):
                translated_vertex = vec2(poly2.translated_vertices[p].x, poly2.translated_vertices[p].y)
                q = dot(translated_vertex, vec2(axisProjx, axisProjy))
                minR2_xy = min(minR2_xy, q)
                maxR2_xy = max(maxR2_xy, q)
 
 
            axisProjx = -(poly1.translated_vertices[b][2] - poly1.translated_vertices[a][2])
            axisProjz = poly1.translated_vertices[b][0] - poly1.translated_vertices[a][0]
            minR1_xz = inf
            maxR1_xz = -inf
            for p in range(len(poly1.translated_vertices)):
                translated_vertex = vec2(poly1.translated_vertices[p].x, poly1.translated_vertices[p].z)
                q = dot(translated_vertex, vec2(axisProjx, axisProjz))
                minR1_xz = min(minR1_xz, q)
                maxR1_xz = max(maxR1_xz, q)
 
            minR2_xz = inf
            maxR2_xz = -inf
            for p in range(len(poly2.translated_vertices)):
                translated_vertex = vec2(poly2.translated_vertices[p].x, poly2.translated_vertices[p].z)
                q = dot(translated_vertex, vec2(axisProjx, axisProjz))
                minR2_xz = min(minR2_xz, q)
                maxR2_xz = max(maxR2_xz, q)
 
 
            axisProjy = -(poly1.translated_vertices[b][2] - poly1.translated_vertices[a][2])
            axisProjz = poly1.translated_vertices[b][1] - poly1.translated_vertices[a][1]
 
            minR1_yz = inf
            maxR1_yz = -inf
            for p in range(len(poly1.translated_vertices)):
                translated_vertex = vec2(poly1.translated_vertices[p].y, poly1.translated_vertices[p].z)
                q = dot(translated_vertex, vec2(axisProjy, axisProjz))
                minR1_yz = min(minR1_yz, q)
                maxR1_yz = max(maxR1_yz, q)
 
            minR2_yz = inf
            maxR2_yz = -inf
            for p in range(len(poly2.translated_vertices)):
                translated_vertex = vec2(poly2.translated_vertices[p].y, poly2.translated_vertices[p].z)
                q = dot(translated_vertex, vec2(axisProjy, axisProjz))
                minR2_yz = min(minR2_yz, q)
                maxR2_yz = max(maxR2_yz, q)
 
            # seperation between shadows of objects found => no collision possible, exit early
            if not (maxR2_xy >= minR1_xy and maxR1_xy >= minR2_xy \
                and maxR2_xz >= minR1_xz and maxR1_xz >= minR2_xz \
                and maxR2_yz >= minR1_yz and maxR1_yz >= minR2_yz):
                p1.collision_normal = vec3(axisProjx, axisProjy, axisProjz);
                p2.collision_normal = vec3(axisProjx, axisProjy, axisProjz);
                return False
    return True