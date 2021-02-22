import tkinter as tk
import os.path
import pickle
from os import path
import tkinter.font as tkFont
import unicodedata


def on_closing():
	outfile = open("font_raw",'wb');
	pickle.dump(font,outfile);
	outfile.close();
	window.destroy();

def togglebit(i,j):
	font[chrv.get()][i][j]=~font[chrv.get()][i][j];
	if font[chrv.get()][i][j]:
		bit[i][j].configure(bg="black",activebackground="black");
	else:
		bit[i][j].configure(bg="white",activebackground="white");


for jj in range(7):
	for ii in range(5):
		exec("def togglebit"+str(ii)+str(jj)+"(): togglebit("+str(ii)+","+str(jj)+");");
		
		
		
def update_disp():
	charshow.configure(text=chr(chrv.get()));
	for j in range(7):
		for i in range(5):
			if font[chrv.get()][i][j]:
				bit[i][j].configure(bg="black",activebackground="black");
			else:
				bit[i][j].configure(bg="white",activebackground="white");
		
def compile_c():
	cfile = open("font.h",'w');
	cfile.write("#include <avr/pgmspace.h>\n\n");
	cfile.write("static const unsigned char font[] PROGMEM = {\n");
	for ci in range(256):
		for ii in range(5):
			byt=0;
			for jj in range(7):
				if font[ci][ii][jj]:
					byt= byt + 2**jj;
			cfile.write(hex(byt) + ",");
		if unicodedata.category(chr(ci))[0]!="C":
			cfile.write(" // # " + chr(ci) + " char\n");#
		else:
			cfile.write(" // "  + "\n");
	cfile.write("};");
	cfile.close();
if path.exists("font_raw"):
	infile = open("font_raw",'rb');
	font = pickle.load(infile);
	infile.close();
else:
	font= [ [ [False] * 7 for i in range(5) ] for j in range(256)];


window = tk.Tk();

fontStyle = tkFont.Font(window,family="Courier", size=30)
window.protocol("WM_DELETE_WINDOW", on_closing)
y=0;

bit=  [ [0] * 7 for i in range(5) ] ;

fr1 = tk.Frame(window);
for jj in range(7):
	for ii in range(5):
		exec("bit["+str(ii)+"]["+str(jj)+"]=tk.Button(fr1,text=' ',bg='white',activebackground='white', font=fontStyle);");
		exec("bit["+str(ii)+"]["+str(jj)+"].configure(state='normal',relief='raised',command=togglebit"+str(ii)+str(jj)+ ");");
		exec("bit["+str(ii)+"]["+str(jj)+"].grid(row="+str(jj)+", column="+str(ii)+");");
fr1.pack();
chrv =tk.IntVar();
chrv.set(0);
charsel = tk.Spinbox(window,from_=0,to=255,textvariable=chrv,command=update_disp, font=fontStyle);
charsel.pack();
charshow = tk.Label(window,text=chr(chrv.get()), font=fontStyle);
charshow.pack();
comp_btn = tk.Button(window,text="compile",command=compile_c);
comp_btn.pack();
#bit00 = tk.Button(window, text=" ", bg="white",activebackground="white");
#bit00.configure(state = "normal", relief="raised",command= togglebit00);
#bit00.pack(side=tk.LEFT);
update_disp();
window.mainloop();
