from pygame import *
import sys
import math
from PIL import Image, ImageDraw


def draw_it(drawable, scr = None):
    if scr is None:
        scr = screen

    xy = drawable["xy"]
    type = drawable["type"]

    if type == "point":
        draw.circle(scr, (127, 127, 127), xy, SZ/4)
    if type == "resistor":
        draw.line(scr, (100, 255, 100), xy[0], xy[1], SZ/6)
    if type == "wire":
        draw.line(scr, (255, 255, 255), xy[0], xy[1], SZ/8)
    if type == "diode":
        ct = (xy[0][0] * 3/4 + xy[1][0] * 1/4, xy[0][1] * 3/4 + xy[1][1] * 1/4)
        draw.line(scr, (255, 255, 255), xy[0], ct, SZ/4)
        draw.line(scr, (64, 64, 64), ct, xy[1], SZ/4)
    if type == "npn":
        draw.line(scr, (100, 100, 100), xy[0], xy[1], SZ/6)
        draw.line(scr, (100, 100, 100), xy[1], xy[2], SZ/6)
        ct = xy[1]
        a = math.atan2(-ct[1]+xy[0][1], -ct[0]+xy[0][0])
        b = math.atan2(-ct[1]+xy[2][1], -ct[0]+xy[2][0])
        im = Image.new("RGBA", (SZ, SZ))
        dr = ImageDraw.Draw(im)
        dr.pieslice((0, 0, SZ-1, SZ-1), b*180/math.pi, a*180/math.pi,
                fill=(100, 100, 100))
        im = image.fromstring(im.tobytes(), im.size, im.mode)

        scr.blit(im, im.get_rect(center = ct))
    if type == "cut":
        xy = xy[0]
        draw.line(scr, (64, 64, 64), 
                (xy[0]+SZ/2, xy[1]-SZ/2+1), (xy[0]+SZ/2, xy[1]+SZ/2-1), SZ/6)
    if type == "hole":
        xy = xy[0]
        draw.circle(scr, (127, 127, 127), xy, SZ/2)




components = []
points = []
preview = {"type": "point", "xy": (0, 0)}
rows = int(sys.argv[1])
cols = int(sys.argv[2])
SZ = int(sys.argv[3])
projname = sys.argv[4]


try:
    with open(projname + ".proj", "r") as f:
        lines = f.readlines()

    for line in lines:
        line = line.split()
        tp = line[0]
        line = line[1:]
        xy = []
        for i in range(len(line) / 2):
            xy.append((int(line[i*2])*SZ, int(line[i*2+1])*SZ))
        components.append({"type": tp, "xy": tuple(xy)})
except IOError:
    pass


done = False
screen = display.set_mode((SZ * (cols), SZ * (rows+1)))
show_back = False

while not done:
    for ev in event.get():
        if ev.type == QUIT:
            done = True
        if ev.type == MOUSEBUTTONDOWN:
            cp = preview.copy()
            points.append(cp)
        if ev.type == KEYDOWN:
            c = ev.unicode
            try:
                if c == "r": # Resistor
                    p1, p2 = points[-2:]
                    components.append({"type": "resistor", "xy": (p1["xy"], p2["xy"])})
                    points = points[:-2]
                elif c == "w": # Wire
                    p1, p2 = points[-2:]
                    components.append({"type": "wire", "xy": (p1["xy"], p2["xy"])})
                    points = points[:-2]
                elif c == "d": # Diode
                    p1, p2 = points[-2:]
                    components.append({"type": "diode", "xy": (p1["xy"], p2["xy"])})
                    points = points[:-2]
                elif c == "c": # Cut
                    p1 = points[-1]
                    components.append({"type": "cut", "xy": (p1["xy"], )})
                    points = points[:-1]
                elif c == "h": # Hole
                    p1 = points[-1]
                    components.append({"type": "hole", "xy": (p1["xy"], )})
                    points = points[:-1]
                elif c == "n": # NPN
                    p1, p2, p3 = points[-3:]
                    components.append({"type": "npn", "xy": (p1["xy"], p2["xy"], p3["xy"])})
                    points = points[:-3]
                elif c == "u": # Undo
                    if points:
                        points = points[:-1]
                    elif components:
                        components = components[:-1]
                elif c == "b": # Back/front view
                    show_back = not show_back
                elif c == "x": # Delete
                    todel = []
                    for co in components:
                        for pt in co["xy"]:
                            if pt == preview["xy"]:
                                todel.append(co)
                                break
                    for co in todel:
                        components.remove(co)
                elif c == "s": # Save
                    image.save(screen, projname + ".png")
                    image.save(back, projname + ".back.png")
                    with open(projname + ".proj", "w") as f:
                        for co in components:
                            f.write(co["type"] + " ")
                            for xy in co["xy"]:
                                f.write(str(xy[0]/SZ) + " " + str(xy[1]/SZ) + " ")
                            f.write("\n")
            except Exception as e:
                print(e)


    screen.fill((0, 0, 0))

    for row in range(rows):
        draw.line(screen, (200, 100, 0), (SZ-SZ/2, SZ*(row+1)-1), (SZ*cols-SZ/2, SZ*(row+1)-1), SZ*3/4)
        for col in range(cols):
            draw.circle(screen, (0, 0, 0), (SZ*(col+1), SZ*(row+1)), SZ/5)

    for dr in points:
        draw_it(dr)

    for dr in components:
        if dr["type"] in ["cut", "hole"]:
            draw_it(dr)

    back = transform.flip(screen, False, False)
    for dr in components:
        if dr["type"] not in ["cut", "hole"]:
            draw_it(dr)
            for xy in dr["xy"]:
                draw_it({"type": "point", "xy": xy}, back)

    back = transform.flip(back, True, False)

    col, row = mouse.get_pos()
    col, row = int(round(col/float(SZ))), int(round(row/float(SZ)))
    preview["xy"] = (col*SZ, row*SZ)

    if col and row and col < cols and row <= rows:
        draw_it(preview)

    if show_back:
        screen.blit(back, back.get_rect())


    display.flip()       
