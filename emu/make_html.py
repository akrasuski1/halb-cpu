import subprocess


with open("opcodes.html", "w") as f:
    f.write("<html>" + """
    <style>
body {
    background-color: #ffffff;
    font-family:monospace;
}
table {
    border-collapse: collapse;
}
.alu2 {
    background-color: #ffffdd;
}
.alu1 {
    background-color: #ddffff;
}
.load {
    background-color: #ddffdd;
}
.store {
    background-color: #ddddff;
}
.jump {
    background-color: #ffdddd;
}
.res {
    background-color: #bbbbbb;
}
.misc {
    background-color: #ffbbff;
}

table, th, td {
    border: 1px solid black;
    text-align:center;
}
    </style>
    """ + "<body><table>\n")
    for i in range(256):
        if i % 16 == 0:
            f.write("<tr>")

        cl = ""
        if i & 7 <= 4:
            cl = ' class="alu2"'
        elif i & 15 == 5:
            cl = ' class="alu1"'
        elif i in [0xaf, 0xef]:
            cl = ' class="misc"'
        elif i in [0x36, 0xa6, 0x3e, 0x3f, 0xae]:
            cl = ' class="res"'
        elif (i & 15 == 7 or i & 15 == 0xf) and i >> 4 not in [4, 5, 6, 7]:
            cl = ' class="load"'
        elif i & 15 == 6 or (i & 15 == 0xe and i >> 4 not in [4, 5, 6, 7]):
            cl = ' class="store"'
        elif i & 15 == 13:
            cl = ' class="jump"'
        elif i & 15 == 14 or (i & 15 == 15 and i >> 4 in [4, 5, 6, 7]):
            cl = ' class="res"'
        else:
            cl = ' class="misc"'

        f.write("  <td%s>" % cl)
        open("/tmp/bin", "wb").write(chr(i) + chr(0xee))
        out = subprocess.check_output(["bin/disas", "/tmp/bin"])
        out = out.splitlines()[0].split("]", 1)[1].strip()
        f.write("<b>{:02X}</b><br/>".format(i) + out)
        f.write("</td>\n")
        if i % 16 == 15:
            f.write("</tr>\n")
    f.write("</table>" + """

<br />
<b> Legend: </b> <br/>
<table>
<tr class="alu2"><td> ALU (2-op) </td></tr>
<tr class="alu1"><td> ALU (1-op) </td></tr>
<tr class="load"><td> load </td></tr>
<tr class="store"><td> store </td></tr>
<tr class="jump"><td> jump </td></tr>
<tr class="res"><td> reserved </td></tr>
<tr class="misc"><td> miscellaneous </td></tr>
</table>
    """ + "</body></html>")
