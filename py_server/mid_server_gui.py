#!/usr/bin/python

import os
import sys
reload(sys)
sys.setdefaultencoding('utf-8')
import wx
import socket
import time
from  multiprocessing import Process,Queue
q=Queue(maxsize=1)
#FRAME
class Frame(wx.Frame):
    def __init__(self,parent=None,id=-1,pos=wx.DefaultPosition,title='Mid-Server'):
        pos_frm=pos          
        sz_frm=(300,100)     
        sz_bt=(80,50)       
        sz_output=(270,20)    
        pos_output=(15,10)   
        pos_bt_left=(15,40)    
        pos_bt_right=(110,40)
        pos_bt_quit=(205,40) 
        wx.Frame.__init__(self,parent,id,title,pos_frm,sz_frm,style=wx.DEFAULT_FRAME_STYLE^(wx.RESIZE_BORDER|wx.MAXIMIZE_BOX|wx.MINIMIZE_BOX))
        self.bt_left=wx.Button(self,label="Left",pos=pos_bt_left,size=sz_bt)
        self.bt_right=wx.Button(self,label="Right",pos=pos_bt_right,size=sz_bt)
        self.bt_quit=wx.Button(self,label="Quit",pos=pos_bt_quit,size=sz_bt)
        self.txt_output=wx.StaticText(self,-1,"",size=sz_output,pos=pos_output)
        self.txt_output.SetBackgroundColour("Grey")
    #Control
        self.Bind(wx.EVT_BUTTON,self.OnClickLeft,self.bt_left)
        self.Bind(wx.EVT_BUTTON,self.OnClickRight,self.bt_right)
        self.Bind(wx.EVT_BUTTON,self.OnClickQuit,self.bt_quit)
        self.skt=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
        self.skt.bind(('0.0.0.0',7777))
    #Model
    def OnClickLeft(self,event):
        self.txt_output.SetLabel("LEFT")
        data,addr=self.skt.recvfrom(1)
        for i in range(1,20):
            self.skt.sendto('l'+chr(80),addr)
        print 'left'
    def OnClickRight(self,event): 
        self.txt_output.SetLabel("RIGHT")
        data,addr=self.skt.recvfrom(1)
        for i in range(1,20):
            self.skt.sendto('r'+chr(80),addr)
        print 'right'
    def OnClickQuit(self,event):
        self.Close(True)
    def OnExit(self):
        print "Exit"

#Application
class App(wx.App):
    def __init__(self):
        wx.App.__init__(self)
    def OnInit(self):
        self.frame=Frame()
        self.frame.Show()
        self.SetTopWindow(self.frame)
        return True
    def OnExit(self):
        pass

if __name__=='__main__':
    app=App()
    app.MainLoop()
