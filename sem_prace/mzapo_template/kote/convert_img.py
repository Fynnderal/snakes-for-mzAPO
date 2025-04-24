import cv2
import sys

nazev = sys.argv[1]
nazev_prom = nazev.replace('.','_')
poz_tecky = nazev.find('.')
if poz_tecky<0:
  nazev_c = nazev+'.c'
else:
  nazev_c = nazev[:poz_tecky]+'.c'
obr = cv2.imread(nazev)

obr2 = cv2.cvtColor(obr, cv2.COLOR_BGR2RGB)

height, width = obr2.shape[:2]

f_out = open(nazev_c, "w")
f_out.write("int "+nazev_prom+"_height = "+str(height)+";\n")
f_out.write("int "+nazev_prom+"_width = "+str(width)+";\n")
f_out.write("unsigned short int "+nazev_prom+"[] = {\n")
pix_cnt=0
for i in range(height):
  for j in range(width):
    pixel = obr2[i,j]
    lcd_pixel = ((pixel[0]>>3)<<11) | ((pixel[1]>>2)<<5) | (pixel[2]>>3)
    if i==height-1 and j == width-1:
      f_out.write(str(lcd_pixel)+'};\n')
    else:
      f_out.write(str(lcd_pixel)+',')
      pix_cnt+=1
      if j==width-1:
        f_out.write('\n\n')
      if pix_cnt>=20:
        pix_cnt=0
        f_out.write('\n')
f_out.close()
    




