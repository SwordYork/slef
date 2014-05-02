def g2h(graph_font):
    f = graph_font.replace('.','0')
    f = f.replace('*','1')
    return hex(int(f,2))
    



fontfile = open('hankaku.txt'); 
hexfont = [];

for i in range(256):
    print fontfile.readline()
    for j in range(16):
        tmp = fontfile.readline()
        print tmp
        hexfont.append(g2h(tmp))
    print fontfile.readline()

hexfontflie = open('hankaku.c','w'); 
hexfontflie.write(str(hexfont))




