import subprocess


with open("opcodes.html", "w") as f:
    f.write("<html>" + """
    <style>
body {
    font-family:monospace;
}
table {
    border-collapse: collapse;
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
        f.write("<td>")
        open("/tmp/bin", "wb").write(chr(i) + chr(0xee))
        out = subprocess.check_output(["./disas", "/tmp/bin"])
        out = out.splitlines()[0].split("]", 1)[1].strip()
        f.write("<b>{:02X}</b><br/>".format(i) + out)
        f.write("</td>")
        if i % 16 == 15:
            f.write("</tr>\n")
    f.write("</table></body></html>")
