/* 
** $Id: helloworld.c,v 1.7 2003/06/13 07:15:49 weiym Exp $
**
** Listing 2.1
**
** helloworld.c: Sample program for MiniGUI Programming Guide
**      The first MiniGUI application.
**
** Copyright (C) 2003 Feynman Software.
**
** License: GPL
*/

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define BAUDRATE        B9600
#define MODEMDEVICE     "/dev/ttyS1"
#define _POSIX_SOURCE   1

#define idbtn1 110
#define idbtn2 111
#define idbtn3 112
#define idbtn4 113
#define idbtn5 114

#define R1	0
#define	B1	5
#define	G1	9
#define R2	4
#define	B2	8
#define	G2	13
#define NUM	10

int fd, res;
char buf[256];
char bufTime[5], bufPrice[5], bufTran1[5], bufTran2[5], bufTran[64];

char	s_line, d_line;
int searchHead, searchTail,destHead, destTail, i;
int ptx = 0, pty = 0, x=0,y=0;
int px[1000], py[1000],st=0,point=0;
int btn5_Press = 0;

char *station[10] = 
					{
					"Tamsui",
					"ZhongShan",					
					"TPE Main Station",
					"C.K.S. Mem. Hall",
					"XiangShan",
					"NanGang Exb.Cen.",
					"ShiMen",
					"DingPu",
					"SongShan",
					"ShinDian"
					};
char *stationNum[10] = {
						"071",	// Tamsui
						"053",	// ZhongShan
						"051",	// TPE
						"042",	// C.K.S.
						"099",	// XiangShan
						"031",	// NangGang Exb.
						"086",	// Shimen
						"076",	// DingPu
						"111",	// SongShan
						"033",	// ShinDian
						};

HWND button1,button2,button3,button4, button5;
BITMAP bkgnd, btnpic;
/*
void *uartThread(void *ptr)
{
	int i;
	while(1)
	{
		if(btn5_Press)
		{
			// transmit
			memset(buf, 0, 256);
			sprintf(buf, "1%s,1%s", stationNum[x], stationNum[y]);
			res = write(fd, buf, 256);
		
			// receive
			for (i = 0; i < 5000000; ++i);// delay, do nothing
			res = read(fd, buf, 256);
			printf("res = %d, buf = %s", res, buf);
			
			// clear the flag 
			btn5_Press = 0;
		}
	}
}
*/
void uartWR(void)
{
	int i;
	// transmit
	memset(buf, '\0', 256);
	sprintf(buf, "1%s,1%s", stationNum[x], stationNum[y]);
	res = write(fd, buf, 256);

	// receive
	memset(buf, '\0', 256);
	memset(bufTime, '\0', 5);
	memset(bufPrice, '\0', 5);
	memset(bufTran1, '\0', 5);
	memset(bufTran2, '\0', 5);
	//memset(bufTran, '\0', 64);
	for (i = 0; i < 300000000; ++i);	// delay, do nothing
	res = read(fd, buf, 256);
	printf("buf=%s",buf);
	
	// parse buf, time, price, transfer
	char *tmp, *zero="000"; 
	tmp = strtok(buf, ",");
	strcpy(bufTime, tmp);
	printf("%s\n",tmp);
	
    tmp = strtok(NULL, ",");
    strcpy(bufPrice, tmp);
    printf("%s\n",tmp);
    
    tmp = strtok(NULL, ","); 
    strcpy(bufTran1, tmp);
    printf("%s\n",tmp);
    
    tmp = strtok(NULL, "Q");
    strcpy(bufTran2, tmp);
    printf("%s\n",tmp);
    
	// parse the stationNum
	if (strcmp(bufTran1, zero)==0 && strcmp(bufTran2, zero)==0)
		strcpy(bufTran, "Directly");
	else
	{
		for(i = 0; i < 10; ++i)
		{
			if(strcmp(bufTran1, stationNum[i])==0)
				strcpy(bufTran, station[i]);
		}
		
		for(i = 0; i < 10; ++i)
		{
			if(strcmp(bufTran2, stationNum[i])==0)
			{
				strcat(bufTran, "\n");
				strcat(bufTran, station[i]);
			}
		}
	}
	
	printf("bufTime=%s\n",bufTime);
	printf("bufPrice=%s\n",bufPrice);
	printf("bufTran=%s\n",bufTran);
}

//=======================================================================================
static int HelloWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    char c[200];
    HDC hdc; 
	static RECT SStrUpdt = {65, 70, 195, 87};
	static RECT DStrUpdt = {65, 104, 195, 120};
	//static RECT StrPrice = {90, 195, 110, 210};
	//static RECT StrTime = {70, 219, 90, 234};
	//static RECT StrDis = {70, 243, 90, 258};

	static RECT transfersta = {10, 180, 140, 270};
	static RECT full = {0, 0, 195, 270};


    switch (message) {
	//----------------------------------------------------------------	
        case MSG_PAINT:
            hdc = BeginPaint (hWnd);
			//SetPenColor(hdc,RGB2Pixel(hdc,0,0,0));
			/*
			Circle (hdc, x, y, 5);
			Rectangle (hdc, 10, 10, 110, 110);
			
			MoveTo(hdc,60,10);
			LineTo(hdc,60,110);
			
			MoveTo(hdc,10,60);
			LineTo(hdc,110,60);*/
			
			//--------------Paint the background-----------------		
			LoadBitmap(HDC_SCREEN, &bkgnd, "bkgnd1.jpg");
			FillBoxWithBitmap(hdc, 0, 0, 240, 295, &bkgnd);									
			sprintf(c, "%s", station[x]); // "Start : %s"			
        	TextOut (hdc, 65, 70, c);
			sprintf(c, "%s", station[y]); // "Dest. : %s"
        	TextOut (hdc, 65, 104, c);
			sprintf(c, "%s", bufTime);	// time
			TextOut (hdc, 100, 180, c);
			sprintf(c, "%s", bufPrice); // price
			TextOut (hdc, 100, 205, c);
			sprintf(c, "%s", bufTran); // Distance 
			TextOut (hdc, 25, 250, c);
            EndPaint (hWnd, hdc);
            return 0;
			break;
	//----------------------------------------------------------------
	
	//----------------------------------------------------------------
        case MSG_CLOSE:
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
            return 0;
			break;
	//----------------------------------------------------------------
	
	//----------------------------------------------------------------
        case MSG_MOUSEMOVE:
            ptx = LOWORD(lParam);
            pty = HIWORD(lParam);
            //InvalidateRect(hWnd, NULL, TRUE);
           
            return 0;
			break;
	//----------------------------------------------------------------

	//----------------------------------------------------------------
		case MSG_COMMAND:
			switch (wParam)
		    {
				case idbtn1:   // Prev_D                  
					if(y>0){
						y--;
					}
					else y = y;					
					//InvalidateRect(hWnd, NULL, TRUE);
					InvalidateRect (hWnd, &DStrUpdt, TRUE);
		            break;

				case idbtn2:      // Next_D                  
					if(y<NUM-1){
						y++;
					}
					else y = y;
					//InvalidateRect(hWnd, NULL, TRUE);
					InvalidateRect (hWnd, &DStrUpdt, TRUE);
		            break;
				
				case idbtn3:    // Prev_S                   
					if(x>0){
						x--;
					}
					else x = x;
					//InvalidateRect(hWnd, NULL, TRUE);
					InvalidateRect (hWnd, &SStrUpdt, TRUE);
		            break;
				
				case idbtn4:  // Next_S                    
					if(x<NUM-1){
						x++;
					}
					else x = x;
					//InvalidateRect(hWnd, NULL, TRUE);
					InvalidateRect (hWnd, &SStrUpdt, TRUE);
		            break;

				case idbtn5:  // information
					//btn5_Press = 1;
					uartWR();
					InvalidateRect (hWnd, &transfersta, TRUE);
		            break;
			}
			return 0;
			break;
	//----------------------------------------------------------------
	//----------------------------------------------------------------	
		case MSG_TIMER:		
            return 0;
			break;
	//----------------------------------------------------------------
	//----------------------------------------------------------------
		case MSG_CREATE:
			//hdcbtn = BeginPaint (hWnd);
			//LoadBitmap(HDC_SCREEN, &btnpic, "btnpic.jpg");
			button1 = CreateWindow (CTRL_BUTTON,"Prev_D",WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, idbtn1,175,96, 20, 8, hWnd, 0);
			button2 = CreateWindow (CTRL_BUTTON,"Next_D",WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, idbtn2,175,120, 20, 8, hWnd, 0); 
			button3 = CreateWindow (CTRL_BUTTON,"Prev_S",WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, idbtn3,175,62, 20, 8, hWnd, 0); 
			button4 = CreateWindow (CTRL_BUTTON,"Next_S",WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, idbtn4,175,87, 20, 8, hWnd, 0);

			InvalidateRect(hWnd, NULL, TRUE); 
			button5 = CreateWindow (CTRL_BUTTON,"?",WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, idbtn5,207,55, 20, 20, hWnd, 0);			

			//-------------------------------
			InvalidateRect(hWnd, &full, TRUE);
			
			//-------------------------------				
			//EndPaint(hWnd, hdcbtn);
			return 0;
			break;
	//----------------------------------------------------------------
	
	}
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

//===========================MiniGUIMain=====================================
int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo; 

#ifdef _LITE_VERSION
    SetDesktopRect(0, 0, 768, 1024);
#endif

    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "TaipeiMetro";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = HelloWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 240;
    CreateInfo.by = 320;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    //******Set up thread and UART******
	pthread_t thread1;
	struct termios  oldtio, newtio;
	
	fd = open(MODEMDEVICE, O_RDWR|O_NOCTTY);
    if (fd < 0){
        perror(MODEMDEVICE);
        exit(1);
    }
	
	tcgetattr(fd, &oldtio);
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE|CS8|CLOCAL|CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
    // 設定為正規模式 
	//newtio.c_lflag = ICANON;
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);

	//建立執行緒
    //pthread_create(&thread1,NULL,uartThread,NULL);
	
	//***********Finish thread creation************
    
    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {       
        TranslateMessage(&Msg);
        DispatchMessage(&Msg); 
    }

    MainWindowThreadCleanup (hMainWnd);
	close(fd);
	tcsetattr(fd, TCSANOW, &oldtio);
    return 0;
}
//=======================================================================================

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

