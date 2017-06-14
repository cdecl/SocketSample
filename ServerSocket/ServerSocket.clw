; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CServerSocketDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "serversocket.h"
LastPage=0

ClassCount=3
Class1=CServerSocketApp
Class2=CAboutDlg
Class3=CServerSocketDlg

ResourceCount=3
Resource1=IDD_SERVERSOCKET_DIALOG
Resource2=IDD_ABOUTBOX
Resource3=IDD_DIALOG1

[CLS:CServerSocketApp]
Type=0
BaseClass=CWinApp
HeaderFile=ServerSocket.h
ImplementationFile=ServerSocket.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=ServerSocketDlg.cpp
ImplementationFile=ServerSocketDlg.cpp
LastObject=IDOK

[CLS:CServerSocketDlg]
Type=0
BaseClass=CDialog
HeaderFile=ServerSocketDlg.h
ImplementationFile=ServerSocketDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=CServerSocketDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_SERVERSOCKET_DIALOG]
Type=1
Class=CServerSocketDlg
ControlCount=6
Control1=IDC_BTN_LISTEN,button,1342242816
Control2=IDC_EDIT_SESSION,edit,1350633602
Control3=IDC_EDIT_LOG,edit,1353781444
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT_PORT,edit,1350631554
Control6=IDC_BTN_NOOP,button,1342242816

[DLG:IDD_DIALOG1]
Type=1
Class=?
ControlCount=2
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816

