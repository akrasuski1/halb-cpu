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
    if type == "pin":
        draw.circle(scr, (255, 255, 255), xy[0], SZ/3)
    if type == "text":
        lb = arial.render(drawable["text"], 1, (127, 0, 255))
        screen.blit(lb, (xy[0][0], xy[0][1] - SZ/4))
    if type == "resistor":
        draw.line(scr, (100, 255, 100), xy[0], xy[1], SZ/6)
    if type == "wire":
        draw.line(scr, (255, 255, 255), xy[0], xy[1], SZ/8)
        draw.circle(scr, (255, 255, 255), xy[0], SZ/5)
        draw.circle(scr, (255, 255, 255), xy[1], SZ/5)
    if type == "diode":
        ct = (xy[0][0] * 3/4 + xy[1][0] * 1/4, xy[0][1] * 3/4 + xy[1][1] * 1/4)
        draw.line(scr, (255, 255, 255), xy[0], ct, SZ/4)
        draw.line(scr, (127, 64, 64), ct, xy[1], SZ/4)
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
        draw.circle(scr, (64, 64, 64), xy, SZ/2)
        draw.line(scr, (160, 0, 0), 
                (xy[0]+SZ/2, xy[1]-SZ/2), (xy[0]-SZ/2, xy[1]+SZ/2), SZ/6)
        draw.line(scr, (160, 0, 0), 
                (xy[0]-SZ/2, xy[1]-SZ/2), (xy[0]+SZ/2, xy[1]+SZ/2), SZ/6)
    if type == "jump":
        xy = xy[0]
        draw.circle(scr, (255, 255, 0), (xy[0], xy[1]+SZ/2), SZ/3)




components = []
points = []
preview = {"type": "point", "xy": (0, 0)}
rows = int(sys.argv[1])
cols = int(sys.argv[2])
SZ = int(sys.argv[3])
projname = sys.argv[4]


try:
    with open(projname + ".strip", "r") as f:
        lines = f.readlines()

    for line in lines:
        if line[0] == "#":
            continue
        line = line.split()
        comp = {"type": line[0]}
        line = line[1:]
        xy = []
        cur_xy = []
        for i, n in enumerate(line):
            try:
                cur_xy.append(int(n) * SZ)
                if len(cur_xy) == 2:
                    xy.append(tuple(cur_xy))
                    cur_xy = []
            except ValueError:
                k, v = n.split(":")
                comp[k] = v

        comp["xy"] = tuple(xy)
        components.append(comp)
except IOError:
    pass


init()
arial = font.SysFont("Arial", SZ / 2)
done = False
screen = display.set_mode((SZ * (cols+1), SZ * (rows+1)))
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
                    p1 = preview
                    components.append({"type": "cut", "xy": (p1["xy"], )})
                elif c == "j": # Solder jumper
                    p1 = preview
                    components.append({"type": "jump", "xy": (p1["xy"], )})
                elif c == "h": # Hole
                    p1 = preview
                    components.append({"type": "hole", "xy": (p1["xy"], )})
                elif c == "p": # Pin
                    p1 = preview
                    components.append({"type": "pin", "xy": (p1["xy"], )})
                elif c == "t": # Text
                    print("Write label text here:")
                    p1 = preview
                    components.append({"type": "text", "xy": (p1["xy"], ), "text": raw_input()})
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
                    fname = projname + ".strip"
                    with open(fname, "w") as f:
                        f.write("# %s\n# %d rows, %d cols\n" % (fname, rows, cols))
                        for co in components:
                            f.write(co["type"] + " ")
                            for xy in co["xy"]:
                                f.write(str(xy[0]/SZ) + " " + str(xy[1]/SZ) + " ")
                            for key in co:
                                if key in ["type", "xy"]: continue
                                f.write("%s:%s " % (key, str(co[key])))
                            f.write("\n")
                    print("Saved as %s" % fname)
            except Exception as e:
                print(e)


    screen.fill((0, 0, 0))

    for row in range(rows):
        draw.line(screen, (200, 100, 0), (SZ-SZ/2, SZ*(row+1)-1), (SZ*cols+SZ/2, SZ*(row+1)-1), SZ*3/4)
        for col in range(cols+1):
            draw.circle(screen, (0, 0, 0), (SZ*(col+1), SZ*(row+1)), SZ/5)

    for dr in points:
        draw_it(dr)

    for dr in components:
        if dr["type"] in ["cut", "hole", "jump"]:
            draw_it(dr)

    back = transform.flip(screen, False, False)
    for dr in components:
        if dr["type"] not in ["cut", "hole", "jump", "text"]:
            draw_it(dr)
            for xy in dr["xy"]:
                draw_it({"type": "point", "xy": xy}, back)

    for dr in components:
        if dr["type"] in ["text"]:
            draw_it(dr)

    back = transform.flip(back, True, False)

    col, row = mouse.get_pos()
    col, row = int(round(col/float(SZ))), int(round(row/float(SZ)))
    preview["xy"] = (col*SZ, row*SZ)

    if col and row and col <= cols and row <= rows:
        draw_it(preview)

    if show_back:
        screen.blit(back, back.get_rect())


    display.flip()       
