# -- coding: utf-8 --
"""
Created on Sat Jul 25 17:45:15 2020

@author: Pc
"""

# -- coding: utf-8 --
"""
Created on Sat Apr 11 14:43:32 2020

@author: Pc
"""
import sys, math, random, time
from PyQt5.QtWidgets import QApplication, QWidget, QShortcut, QMainWindow
from PyQt5.QtGui import QPainter, QPen, QColor, QKeySequence
from PyQt5.QtCore import Qt, QTimer, QThread
from pyqtgraph import PlotWidget, plot
import pyqtgraph as pg
import serial
import time

class decoder():
    def __init__(self, veri):
        self.paket = str(veri)[2:-5]
        if self.temizmi():
            self.boslukyer()
            self.ayirici()
        
    def temizmi(self):
        if "not" in self.paket:
            return False
        else:
            return True
    
    def boslukyer(self):
        self.bosluk = [-1]
        for self.i in range(len(self.paket)):
            if self.paket[self.i] == " ":
                self.bosluk.append(self.i)
        # return self.bosluk
            
    def ayirici(self):
        self.cikti = []
        for self.j in range(len(self.bosluk)-1):
            self.sayi = self.paket[self.bosluk[self.j]+1:self.bosluk[self.j+1]]
            try:
                self.cikti.append(float(self.sayi))
            except:
                self.cikti.append(self.sayi)
        self.sayi = self.paket[self.bosluk[-1]+1::]
        try:
            self.cikti.append(float(self.sayi))
        except:
            self.cikti.append(self.sayi)

dosya = open("telemetri.csv","a")
ser = serial.Serial(port="COM4",baudrate=9600)     
class App(QMainWindow):
    def __init__(self):
        super().__init__()
        self.title = "STAL-ROBOTİK_3437"
        self.left = 100
        self.top = 100
        self.width = 800
        self.height = 600
        
        self.initUI()
        
        self.l.addLabel("Takım İsmi: STAL ROBOTİK")
        self.l.addLabel("Takım Numarası: #3437")
        self.statu = self.l.addLabel("Uydu Statüsü: İniş")
        self.l.nextRow()
        self.paketno = self.l.addLabel()
        self.zaman = self.l.addLabel()
        self.voltaj = self.l.addLabel()
        self.l.nextRow()
        self.basinct = self.l.addLabel()
        self.sicaklikt = self.l.addLabel()
        self.yukseklikt = self.l.addLabel()
        self.l.nextRow()
        self.yunuslamat = self.l.addLabel()
        self.yalpalamat = self.l.addLabel()
        self.yuvarlamat = self.l.addLabel()
        self.l.nextRow()
        self.boylamt = self.l.addLabel()
        self.enlemt = self.l.addLabel()
        self.gpsyukseklikt = self.l.addLabel()
        self.l.nextRow()



        self.p1 = self.l.addPlot(title="Basınç (Pa)")
        self.p2 = self.l.addPlot(title="Sıcaklık (°C)")
        self.p3 = self.l.addPlot(title="Yükseklik (m)")
        self.l.nextRow()
        self.px = self.l.addPlot(title="Yunuslama (°)")
        self.py = self.l.addPlot(title="Yalpalama (°)")
        self.pz = self.l.addPlot(title="Yuvarlama (°)")
        self.l.nextRow()
        self.p4 = self.l.addPlot(title="Pil Gerilimi (V)")
        self.p5 = self.l.addPlot(title="Konum (enlem(°N) - boylam(°E))",colspan=2)
            
        self.p4.setYRange(0,6, padding=0)
        self.p1.setYRange(99000,100000, padding=0)
        self.p2.setYRange(25,30, padding=0)
        self.p3.setYRange(-2,2, padding=0)
        self.px.setYRange(-180,180, padding=0)
        self.py.setYRange(-180,180, padding=0)
        self.pz.setYRange(-180,180, padding=0)


        self.basinc2 = []
        self.sicaklik2 = []
        self.yukseklik2 = []
        self.yunuslama2 = []
        self.yalpalama2 = []
        self.yuvarlama2 = []
        self.enlem2 = []
        self.boylam2 = []

        # self.basinc = self.kurucu(1006,1025)
        # self.sicaklik = self.kurucu(19,26)
        # self.yukseklik = self.kurucu(0,250)
        # self.yunuslama = self.kurucu1(-50,50)
        # self.yalpalama = self.kurucu1(-50,50)
        # self.yuvarlama = self.kurucu1(-50,50)
        self.pil = []
        # self.enlem = self.kurucu1(400,410)
        # self.boylam = self.kurucu1(280,290)
        
        self.a1 = self.p1.plot()
        self.a2 = self.p2.plot()
        self.a3 = self.p3.plot()
        self.ax = self.px.plot()
        self.ay = self.py.plot()
        self.az = self.pz.plot()
        self.a4 = self.p4.plot()
        self.a5 = self.p5.plot()

        
        
        
        self.paket = 0
        while True:
            self.update()
            QApplication.processEvents()
            self.alim = ser.readline()
            self.isleme = decoder(self.alim)
            if self.isleme.temizmi():
                print(self.isleme.cikti)
                self.paket+=1
                
                self.ekler1(self.pil,37,50)

                
                
                
                
                self.tulga(self.basinc2,self.isleme.cikti[0])
                self.tulga(self.sicaklik2,self.isleme.cikti[1])
                self.tulga(self.yukseklik2,self.isleme.cikti[2])
                self.tulga(self.yunuslama2,self.isleme.cikti[4])
                self.tulga(self.yalpalama2,self.isleme.cikti[5])
                self.tulga(self.yuvarlama2,self.isleme.cikti[3])
                self.tulga(self.enlem2,self.isleme.cikti[6])
                self.tulga(self.boylam2,self.isleme.cikti[7])
                
                # self.ekler1(self.enlem,400,410)
                # self.ekler1(self.boylam,280,290)
            
                # self.statu.setText()
                self.paketno.setText("Paket Numarası: {}".format(self.paket))
                self.zaman.setText("Gönderme Zamanı: {} - {}".format(self.isleme.cikti[9],self.isleme.cikti[10]))
                self.voltaj.setText("Pil Gerilimi: {} V".format(self.pil[-1]))
                self.basinct.setText("Basınç: {} Pa".format(int(self.isleme.cikti[0])))
                self.sicaklikt.setText("Sıcaklık: {} °C".format(self.isleme.cikti[1]))
                self.yukseklikt.setText("Yükseklik: {} m".format(self.isleme.cikti[2]))
                self.yunuslamat.setText("Yunuslama: {} °".format(self.isleme.cikti[4]))
                self.yalpalamat.setText("Yalpalama: {} °".format(self.isleme.cikti[5]))
                self.yuvarlamat.setText("Yuvarlama: {} °".format(self.isleme.cikti[3]))
                self.boylamt.setText("GPS Enlem: {} °N".format(self.isleme.cikti[6]))
                self.enlemt.setText("GPS Boylam: {} °E".format(self.isleme.cikti[7]))
                self.gpsyukseklikt.setText("GPS Yükseklik: {} m".format(self.isleme.cikti[8]))
                
                self.a1.setData(self.basinc2)
                self.a2.setData(self.sicaklik2)
                self.a3.setData(self.yukseklik2)
                self.ax.setData(self.yunuslama2)
                self.ay.setData(self.yalpalama2)
                self.az.setData(self.yuvarlama2)
                self.a4.setData(self.pil)
                self.a5.setData(self.boylam2,self.enlem2)
                
                
            else:
                print(self.alim)
                
            # self.ekler(self.basinc,990,1025)
            # self.ekler(self.sicaklik,18,39)
            # self.ekler(self.yukseklik,0,250)
            # self.ekler1(self.yunuslama,-180,180)
            # self.ekler1(self.yalpalama,-180,180)
            # self.ekler1(self.yuvarlama,-180,180)
            

            

            

    def initUI(self):
        self.view = pg.GraphicsView()
        self.l = pg.GraphicsLayout(border=(100,100,100))
        self.view.setCentralWidget(self.l)
        self.view.setWindowTitle(self.title)
        self.view.setGeometry(self.left, self.top, self.width, self.height)
        self.view.show()

            
    
    def tulga(self,liste,deger):
        liste.append(deger)
    
    def ekler(self,liste,ilk,son):
        liste.append(random.randrange(ilk,son))
        
    def ekler1(self,liste,ilk,son):
        liste.append(random.randrange(ilk,son)/10)
        
    def kurucu(self,ilk,son):
        self.retu = []
        for i in range(10):
            self.ekler(self.retu,ilk,son)
        return self.retu
    
    def kurucu1(self,ilk,son):
        self.retu = []
        for i in range(10):
            self.ekler1(self.retu,ilk,son)
        return self.retu
    

if __name__ == '__main__': #direkt calistirma. modul olarak kullanilabilir

    app = QApplication(sys.argv)
    ex = App()
    sys.exit(app.exec_())