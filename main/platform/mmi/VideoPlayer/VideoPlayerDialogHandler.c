/*=================================================================================================================
文件：
      VideoPlayerDialogHandler.c
      版权所有(c) 2009 Anylook Tech. CO.,LTD.

 文件说明：
 作者:  
 创建日期：2008-10-06
 当前版本：Draft
=================================================================================================================*/

/*=================================================================================================================
修改历史记录：
时       间      修 改 人    问题单号  修改内容、位置及原因
-----------      ----------  --------  -----------------------------------------
=================================================================================================================*/


/*=================================================================================================================
                          本文件包含的外部文件
=================================================================================================================*/
#include "VideoPlayer_priv.h"
#ifndef WIN32
#ifdef FEATURE_MEDIAPLAYER_DECODER_INTERFACE
#include "mmd_ply_driver.h"
#endif
#else
#include "oemhelperfunctype.h"
#endif//WIN32
#include "appscommon_color.brh"
#include "appscommon.brh"

boolean        FullScreen = FALSE;
/*=================================================================================================================
                             宏定义和常数
=================================================================================================================*/
// 关闭对话框
#define CLOSE_DIALOG(DlgRet)  {    \
                                     pMe->m_eDlgRet = DlgRet; \
                                     (void) ISHELL_EndDialog(pMe->m_pShell);  \
                                 }


/*=================================================================================================================
                             类型定义
=================================================================================================================*/


/*=================================================================================================================
                              函数声明
=================================================================================================================*/

/*=================================================================================================================
                对话框IDD_VIDEOPLAYER事件处理函数
=================================================================================================================*/

static boolean VPDVideoPlayer_HandleEvent(CVideoPlayer *pMe,AEEEvent eCode, uint16 wParam,uint32 dwParam); 

/*=================================================================================================================
                   对话框IDD_OPTION事件处理函数
=================================================================================================================*/

//static boolean VPDOption_HandleEvent(CVideoPlayer *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam); 

/*=================================================================================================================
                   对话框IDD_MSGBOX事件处理函数
=================================================================================================================*/

static boolean VPDMSGBOX_HandleEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam);

/*=================================================================================================================
                             视频播放相关处理函
=================================================================================================================*/

//对话框IDD_VIDEOPLAYER key事件处理函数
static boolean VPDVideoPlayer_HandleKeyEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam);

//对话框IDD_OPTION key事件处理函数
//static boolean VPDOption_HandleKeyEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam);

//对话框IDD_MSGBOX key事件处理函数
//static boolean VPDMSGBOX_HandleKeyEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam);

//播放视频 
void VideoPlayer_PlayVideo(CVideoPlayer *pMe);

//画视频播放器相关图片
static void VideoPlayer_DrawImage(CVideoPlayer *pMe, char *resFile, int16 resId, int x, int y);

//播放下一个文件
static void VideoPlayer_PlayNext(CVideoPlayer *pMe, boolean bDirection); 

//重画player屏的上半部分的文件名
static void VideoPlayer_RefreshPlayerFileName(CVideoPlayer *pMe);
static void VideoPlayer_ShowName(CVideoPlayer *pMe);

//重画player屏的下半部分的按钮部分
static void VideoPlayer_UpdateButton(CVideoPlayer *pMe);
static void VideoPlayer_UpdateFRButton(CVideoPlayer *pMe);

//刷新播放时间
static void VideoPlayer_RefreshPlayingTick(CVideoPlayer *pMe);

//刷新音量bar
static void VideoPlayer_RefreshVolBar(CVideoPlayer *pMe);

//刷新进度条
static void VideoPlayer_RefreshScheduleBar(CVideoPlayer *pMe);

// 删除当前文件
//static boolean VideoPlayer_DeleteVideo(CVideoPlayer *pMe);

// 接受CMX底层回调
static void VideoPlayer_VideoNotify(void * pUser, AEEMediaCmdNotify * pCmdNotify);

// 定时函数
static void DialogTimeoutCallback(void *pUser);

// 获取指定视频的名字
static int  VideoPlayer_GetFileID(CVideoPlayer *pMe);


/*=================================================================================================================
                              全局数据
=================================================================================================================*/

/*=================================================================================================================
                             本地（静态）数据
=================================================================================================================*/


/*=================================================================================================================
                                函数定义
=================================================================================================================*/

/*=================================================================================================================
函数：VideoPlayer_ShowDialog

说明：
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数：
       pMe [in]：指向VideoPlayer Applet对象结构的指针。该结构包含小程序的特定信息。
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值：
        无

备注：:

=================================================================================================================*/
int VideoPlayer_ShowDialog(CVideoPlayer *pMe , uint16 dlgResId)
{
    int nRet;
    
       // At most one dialog open at once
       if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
       {
           // Looks like there is one dialog already opened.
           // Flag an error an return without doing anything.
   #if defined(AEE_STATIC)
           ERR("Trying to create a dialog without closing the previous one",0,0,0);
   #endif
           return  EFAILED;
       } 
       
       nRet = ISHELL_CreateDialog(pMe->m_pShell,AEE_APPSVIDEOPLAYER_RES_FILE,dlgResId,NULL);    
    
   #if defined(AEE_STATIC)
           if (nRet != SUCCESS)
           {
               ERR("Failed to create the dialog in the videoplayer applet",0,0,0);
           }
   #endif
           return  nRet;

}

/*=================================================================================================================
函数:
    VideoPlayer_ShowMsgBox

说明:
    函数由状态处理函数用来唤起消息对话框，显示消息提示用户。

参数:
    pMe [in]: 指向 VIDEOPLAYER Applet 对象结构的指针。该结构包含小程序的特定信息。
    wTextResId [in]: 消息文本资源 ID。

返回值:
    none

备注:

=================================================================================================================*/
int  VideoPlayer_ShowMsgBox(CVideoPlayer *pMe, uint16 wTextResId)
{
    pMe->m_nMsgResID = wTextResId;
     
    // 创建消息对话框.
    return  VideoPlayer_ShowDialog(pMe, IDD_MSGBOX);
}

/*=================================================================================================================
函数：VideoPlayer_RouteDialogEvent

说明：
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数：
       pMe [in]：指向VideoPlayer Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       dwParam [in]：与事件eCode关联的数据。。

返回值：
        TRUE：传入事件被处理。
        FALSE：传入事件没被处理。

备注：:

=================================================================================================================*/
boolean VideoPlayer_RouteDialogEvent( CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam)
{
    #if defined(AEE_STATIC)
        if (NULL == pMe)
        {
            return FALSE;
        }
    #endif
    
    if (NULL == pMe->m_pActiveDlg)//当前活动对话框IDialog接口指针
    {
        return FALSE;
    }
    switch (pMe->m_pActiveDlgID)
    {     
        case IDD_VIDEOPLAYER:
            return VPDVideoPlayer_HandleEvent(pMe,eCode,wParam,dwParam);

        case IDD_MSGBOX:
            return VPDMSGBOX_HandleEvent(pMe,eCode,wParam,dwParam);
#if 0
        case IDD_OPTION:
            return VPDOption_HandleEvent(pMe,eCode,wParam,dwParam);
#endif  
        default:
            return FALSE;
     }
}
/*=================================================================================================================
函数：VPDVideoPlayer_HandleEvent

说明：
       VideoPlayer Applet 主界面对话框IDD_VIDEOPLAYER事件处理函数。

参数：
       pMe [in]：指向VideoPlayer Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件参数
       dwParam [in]：与wParam关联的数据。。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注：
=================================================================================================================*/
static  boolean VPDVideoPlayer_HandleEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam)
{   
    #if defined(AEE_STATIC)
        ASSERT(pMe != NULL);
    #endif

    switch (eCode)
    {
        case EVT_DIALOG_INIT:        
            return TRUE;
        
        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER_REDRAW,0,0);                                   
            return TRUE;
        
        case EVT_USER_REDRAW:
            if(!pMe->IsPlay)
            {
                VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_PLAYERPICTURE_PLAY, 0, 0);                 
            }
            else
            {
                VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_PLAYERPICTURE_PAUSE, 0, 0);                
            } 
            VideoPlayer_RefreshPlayingTick(pMe);
            VideoPlayer_RefreshVolBar(pMe); 
            if(pMe->m_FileToPlay != NULL) // 如果不加此判断条件，则在pMe->m_FileToPlay为NULL时，屏幕上会显示乱码
            {
                VideoPlayer_RefreshPlayerFileName(pMe); //刷新文件名
            }
            if(!pMe->IsPause && !pMe->IsPlay)                 
            {
                if(!pMe->UserStop)
                {
                    DRAW_BOTTOMBAR(BTBAR_GALLERY_BACK);
                }
                else
                {
                    if(pMe->m_IsPlaynext)
                    {
                        DRAW_BOTTOMBAR(BTBAR_GALLERY_PLAY_BACK);
                    }
                    else
                    {
                        DRAW_BOTTOMBAR(BTBAR_PLAY_BACK);
                    }
                }
            }
            if(pMe->IsPause)
            {
                DRAW_BOTTOMBAR(BTBAR_PLAY_STOP);
            }
            if(pMe->IsPlay)
            {
                DRAW_BOTTOMBAR(BTBAR_FULLSCREEN_PAUSE_STOP);
            } 
            IDISPLAY_Update(pMe->m_pDisplay); 
            if(pMe->Is848Busy)
            {
                CLOSE_DIALOG(DLGRET_MSGBOX);
            }          
            if(pMe->IsGallery)
            {
                if(pMe->m_InitFailed != SUCCESS )
                {                    
                    CLOSE_DIALOG(DLGRET_MSGBOX);
                }
                else if(pMe->IsPlay)
                {                    
                    VideoPlayer_PlayVideo(pMe);
                }
            } 
            return TRUE;
  
        case EVT_DIALOG_END:            
            return TRUE;
        
        case EVT_KEY: 
            if(pMe->IsGallery)
            {
                return  VPDVideoPlayer_HandleKeyEvent(pMe,eCode,wParam,dwParam); 
            }
            else 
            {
                switch(wParam)
                {
                    case AVK_SELECT:
                        #if 0
                        //启动Mdeida Gallery applet                   
                        if (!ISHELL_CanStartApplet(pMe->m_pShell, AEECLSID_MEDIAGALLERY)) 
                        {
                            //当前不能启动指定 Applet
                            DBGPRINTF("ERR!!!!!");
                            return FALSE;
                        }
                        if (SUCCESS != ISHELL_StartApplet(pMe->m_pShell, AEECLSID_MEDIAGALLERY)) 
                        {
                            DBGPRINTF("RIGHT~~~~~~");
                            return FALSE;
                        }
                        #endif
                        CMediaGallery_FileExplorer(GALLERY_VIDEO_ADDLIST,NULL);
                        return TRUE;
                
                    case AVK_CLR:
                        CLOSE_DIALOG(DLGRET_CANCELED);
                        break;
                        
                    default:
                        return TRUE;
                }
                     
            }

        case EVT_APPISREADY: 
            pMe->m_bAppIsReady  = TRUE;
            return TRUE;
           
        case EVT_DISPLAYDIALOGTIMEOUT:
            CLOSE_DIALOG(DLGRET_CANCELED);
            return TRUE;
           
        case EVT_COMMAND:
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
		case EVT_PEN_UP:
			{
				int i;
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
				int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
				
				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, devinfo.cyScreen);
				if ((wXPos>0)&&(wXPos<devinfo.cxScreen/3)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
				{
					boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_SELECT,1);
					return rt;
				}
				else if ((wXPos>devinfo.cxScreen/3)&&(wXPos<(devinfo.cxScreen/3)*2)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
				{
					boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_INFO,0);
					 return rt;
				}
				else if ((wXPos>wXPos<(devinfo.cxScreen/3)*2)&&(wXPos<devinfo.cxScreen)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
				{
					boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_CLR,0);
					return rt;
				}
			}
			return TRUE;
#endif//FEATURE_LCD_TOUCH_ENABLE       
        default:
               break;
    }//switch (eCode)
        return FALSE;
}

/*=================================================================================================================

函数：VPDOption_HandleEvent

说明：
       VideoPlayer Applet 主界面对话框IDD_OPTION事件处理函数。

参数：
       pMe [in]：指向VideoPlayer Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件参数
       dwParam [in]：与wParam关联的数据。。

返回值：
        TRUE：传入事件被处理。
        FALSE：传入事件没被处理。

备注：:

================================================================================================================*/
#if 0
static boolean VPDOption_HandleEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam)
{   
    
    IMenuCtl* pMenuOption = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                          IDC_OPTION_MENU);
    if( pMenuOption == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {     
        case EVT_DIALOG_INIT:  
            if(pMe->IsPlay)
            {
                IMENUCTL_AddItem(pMenuOption,AEE_APPSCOMMONRES_LANGFILE,IDS_PAUSE, IDS_PAUSE,0,0);
                IMENUCTL_AddItem(pMenuOption,AEE_APPSCOMMONRES_LANGFILE,IDS_FULLSCREEN,IDS_FULLSCREEN,0,0);
            }
            if(pMe->IsPause)
            {
                IMENUCTL_AddItem(pMenuOption,AEE_APPSCOMMONRES_LANGFILE,IDS_PLAY,IDS_PLAY,0,0); 
            }
            IMENUCTL_AddItem(pMenuOption,AEE_APPSVIDEOPLAYER_RES_FILE,IDS_DELETE, IDS_DELETE,0,0);
            
            return TRUE;
         
        case EVT_DIALOG_START:        
            IMENUCTL_SetPopMenuRect(pMenuOption);
            IMENUCTL_SetBottomBarType(pMenuOption, BTBAR_SELECT_BACK);
            (void) ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER_REDRAW,0,0);
            return TRUE;

        case EVT_USER_REDRAW:
            (void)IMENUCTL_Redraw( pMenuOption);       
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:            
            return VPDOption_HandleKeyEvent(pMe,eCode,wParam,dwParam); 

        case EVT_DISPLAYDIALOGTIMEOUT:
            CLOSE_DIALOG(DLGRET_CANCELED);
            return TRUE;

        //自定义控件事件 ,菜单按下时发生
        case EVT_COMMAND:        
            pMe->m_nItemID=wParam;
            switch(wParam)
            {      
                case IDS_PLAY:
                    if(SUCCESS==pMe->m_InitFailed)
                    {    
                        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PAUSE, 82, 179); //"||" 
                        //刷新屏幕，包括时间和进度条
                        //VideoPlayer_RefreshPlayingTick(pMe);//刷新时间
                        //VideoPlayer_RefreshScheduleBar(pMe);//刷新进度条                                       
                        CLOSE_DIALOG(DLGRET_CANCELED);
                        if(pMe->IsPause)
                        {
                            ISHELL_SetTimer(pMe->m_pShell,150, (PFNNOTIFY)(IMEDIA_Resume),pMe->m_pMedia); 
                        }
                        else
                        {
                            ISHELL_SetTimer(pMe->m_pShell,150, (PFNNOTIFY)VideoPlayer_PlayVideo,pMe);                            
                        }
                        pMe->IsPlay  = TRUE;
                        pMe->IsPause=FALSE;
                        pMe->UserStop=FALSE; 
 
                    } 
                    else CLOSE_DIALOG(DLGRET_MSGBOX);
                    break; 
                    
                case IDS_PAUSE:
                    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PLAY, 82,179); //"|>"
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    
                    //if(!pMe->PauseLock)//pMe->bTotalTime == pMe->bCurrentTime || pMe->bCurrentTime == 0时,pMe->PauseLock=TRUE，
                                       //所以该判断条件暂不能用
                    //{                        
                        ISHELL_SetTimer(pMe->m_pShell,150, (PFNNOTIFY)(IMEDIA_Pause),pMe->m_pMedia);
                        pMe->IsPause = TRUE;
                        pMe->IsPlay = FALSE;
                        pMe->UserStop = FALSE;                            
                        
                    //} 
                    break;
                  
                case IDS_DELETE:                    
                    if(pMe->IsPlay)
                    {   
                        CLOSE_DIALOG(DLGRET_MSGBOX);
                        
                        ISHELL_SetTimer(pMe->m_pShell,1000, (PFNNOTIFY)(IMEDIA_Pause),pMe->m_pMedia);                                              
                        pMe->IsPause = TRUE;
                        pMe->IsPlay = FALSE;                        
                    }
                    //CLOSE_DIALOG(DLGRET_MSGBOX);
                    break;
                    
                case IDS_FULLSCREEN:
                    CLOSE_DIALOG(DLGRET_CANCELED);                    
                    VideoPlayer_PlayMod(pMe, 0);
                    break;
                     
                default:
                    break;
            }
            return TRUE;
      
        default:
            break;
    }
        return FALSE;
}
#endif
/*=================================================================================================================
函数：VPDMSGBOX_HandleEvent

说明：
       VideoPlayer Applet 主界面对话框IDD_DELETE事件处理函数。

参数：
       pMe [in]：指向VideoPlayer Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件参数
       dwParam [in]：与wParam关联的数据。。

返回值：
        TRUE：传入事件被处理。
        FALSE：传入事件没被处理。

备注：:

=================================================================================================================*/

static boolean  VPDMSGBOX_HandleEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam)
{   
    static  IStatic * pStatic = NULL;
    AECHAR  wstrText[128];
    int     set_timer=0;
    
    if (NULL == pMe)
    {
        return FALSE;
    }

    switch(eCode)
    {
        case EVT_DIALOG_INIT:
            if (NULL == pStatic)
            {
                AEERect rect = {0};
                if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_STATIC,(void **)&pStatic))
                {
                    return FALSE;
                }
                ISTATIC_SetRect(pStatic, &rect);
            }
            return TRUE;
        

        case EVT_DIALOG_START: 
            ISHELL_PostEvent(pMe->m_pShell, AEECLSID_VIDEOPLAYER,EVT_USER_REDRAW,  0, 0);
            return TRUE;        

        case EVT_USER_REDRAW:            
            {
                PromptMsg_Param_type PromptMsg={0}; 
    
                if(pMe->m_nMsgResID==IDS_PLAYFAILED)
                {
                    DBGPRINTF("message");
                    PromptMsg.ePMsgType = MESSAGE_ERR;
                    set_timer=3000;//SETTIME_MIDDLE; // 1000
                    (void)ISHELL_LoadResString(pMe->m_pShell, AEE_APPSVIDEOPLAYER_RES_FILE, pMe->m_nMsgResID, wstrText, sizeof(wstrText));
                    PromptMsg.pwszMsg = wstrText;
                    DrawPromptMessage(pMe->m_pDisplay, pStatic, &PromptMsg);
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
                    (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
                     //启动发送关闭对话框事件的定时器               
                    ISHELL_SetTimer(pMe->m_pShell,set_timer,DialogTimeoutCallback,pMe);
                }
                else 
                {  
                    PromptMsg.ePMsgType =MESSAGE_CONFIRM;
                    PromptMsg.eBBarType = BTBAR_OK_BACK;
                    (void)ISHELL_LoadResString(pMe->m_pShell, AEE_APPSCOMMONRES_LANGFILE, pMe->m_nMsgResID, wstrText, sizeof(wstrText));                    
                    PromptMsg.pwszMsg = wstrText;
                    DrawPromptMessage(pMe->m_pDisplay, pStatic, &PromptMsg);
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
                    (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
                } 
            }        
            return TRUE; 

        case EVT_DIALOG_END:        
            ISHELL_CancelTimer(pMe->m_pShell, DialogTimeoutCallback,pMe);
            ISTATIC_Release(pStatic);
            pStatic = NULL;
            FREE(wstrText);
            return TRUE;
        
        case EVT_KEY:           
            if(pMe->m_nMsgResID != IDS_PLAYFAILED)
            {
                switch(wParam)
                {
                    case AVK_SELECT:
                        pMe->Is848Busy = FALSE; 
                        if(app_media_scheduler()==APP_MEDIA_IMPACT_BY_FM)
                        {                        
                            ISHELL_SendEvent(pMe->m_pShell, AEECLSID_APP_FMRADIO, EVT_CLOSEAPP, 0, 0);
                        }
                        if(app_media_scheduler()==APP_MEDIA_IMPACT_BY_MP3)
                        {                           
                            ISHELL_SendEvent(pMe->m_pShell, AEECLSID_APP_MUSICPLAYER, EVT_CLOSEAPP, 0, 0);
                        } 
                        //初始化Media指针并注册回调
                        //VideoPlayer_InitVideo(pMe); 
                        CLOSE_DIALOG(DLGRET_VIDEOPLAYER);
                        break;
                        
                    case AVK_CLR:
                        CLOSE_DIALOG(DLGRET_CANCELED);
                        break;
                        
                    default:
                        break;
                }
            }          
            //return VPDMSGBOX_HandleKeyEvent(pMe,eCode,wParam,dwParam);            

        case EVT_DISPLAYDIALOGTIMEOUT:            
            if(pMe->m_nMsgResID==IDS_PLAYFAILED)
            {
                CLOSE_DIALOG(DLGRET_CANCELED);
            }
#if 0
            if(pMe->m_nMsgResID==IDS_DEL || pMe->m_nMsgResID==IDS_DONE)
            {                
                CLOSE_DIALOG(DLGRET_VIDEOPLAYER);
            }  
#endif 
            return TRUE; 

        default:
            break;
    }
    return FALSE;
}
/*=================================================================================================================  
  处理视频播放的按钮事件
=================================================================================================================*/
#ifndef WIN32
#ifdef FEATURE_MEDIAPLAYER_DECODER_INTERFACE
extern MMD_PLYFILE pFileHandle;
#endif
#endif//WIN32
static boolean VPDVideoPlayer_HandleKeyEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam ,uint32 dwParam)
{   
    switch(wParam)
    {       
        //播放或暂停
        case AVK_INFO:
            (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
            if(pMe->m_InitFailed != SUCCESS)
            {
                CLOSE_DIALOG(DLGRET_MSGBOX);
            }
            else // 初始化成功
            {
                if(!pMe->IsFullScreen)// 正常模式
                {                   
                    if(!pMe->IsPlay) // 非播放状态
                    {                          
                        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PAUSE, VIDEOPLAYER_PLAY_X, VIDEOPLAYER_PLAY_Y); //"||"
						pMe->m_rtype = TYPE_PLAYER;
                        DRAW_BOTTOMBAR(BTBAR_FULLSCREEN_PAUSE_STOP);
                        IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);

                        if(pMe->IsPause)// 当前状态为暂停时
                        {                             
                            IMEDIA_Resume(pMe->m_pMedia);
                            //ISHELL_SetTimer(pMe->m_pShell,50,(PFNNOTIFY)(IMEDIA_Resume),pMe->m_pMedia);
                            SetDeviceState(DEVICE_TYPE_MP4,DEVICE_MP4_STATE_ON);                              
                        }
                        else  
                        {
                            VideoPlayer_PlayVideo(pMe);
                            //ISHELL_SetTimer(pMe->m_pShell,50,(PFNNOTIFY)VideoPlayer_PlayVideo,pMe);
                        }                        
                        
                        pMe->IsPlay=TRUE;
                        pMe->IsPause=FALSE;
                        pMe->UserStop=FALSE; 
                        videoplayer_play_flag = TRUE;
                    }
                    else// 当前状态为播放时
                    {                        
                        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PLAY, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y); //"|>"
						pMe->m_rtype = TYPE_PLAYER;
                        DRAW_BOTTOMBAR(BTBAR_PLAY_STOP); 
                        IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
                        //IMEDIA_Pause(pMe->m_pMedia); // 不设时间的话，底条刷不出来
                        ISHELL_SetTimer(pMe->m_pShell,50,(PFNNOTIFY)(IMEDIA_Pause),pMe->m_pMedia); 
                        SetDeviceState(DEVICE_TYPE_MP4,DEVICE_MP4_STATE_OFF);  
                        pMe->IsPlay=FALSE;
                        pMe->IsPause=TRUE;
                        pMe->UserStop=FALSE;                       
                    }               
                }
                else // 全屏模式
                {                  
                    SetDeviceState(DEVICE_TYPE_MP4,DEVICE_MP4_STATE_ON);
                    if(!pMe->IsPlay) // 非播放状态
                    { 
                        if(pMe->IsPause)// 当前状态为暂停时
                        {
                            IMEDIA_Resume(pMe->m_pMedia);
                            //ISHELL_SetTimer(pMe->m_pShell,50,(PFNNOTIFY)(IMEDIA_Resume),pMe->m_pMedia);
                            
                        }
                        else// 当前状态为停止时
                        {                            
                            VideoPlayer_PlayVideo(pMe);
                            //ISHELL_SetTimer(pMe->m_pShell,50,(PFNNOTIFY)VideoPlayer_PlayVideo,pMe); 
                        }
                        pMe->IsPlay=TRUE;;
                        pMe->IsPause=FALSE;
                        pMe->UserStop=FALSE;
                        videoplayer_play_flag = TRUE;
                    }
                    else // 当前状态为播放时
                    {   
                        IMEDIA_Pause(pMe->m_pMedia);
                        //ISHELL_SetTimer(pMe->m_pShell,50,(PFNNOTIFY)(IMEDIA_Pause),pMe->m_pMedia); 
                        
                        pMe->IsPlay=FALSE;
                        pMe->IsPause=TRUE;
                        pMe->UserStop=FALSE;
                    }
                }                
            }
            return TRUE; 
       
        case AVK_CLR:            
            (void)ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
            /*在暂停或播放状态按stop键后，oem层都会传上来一个MM_STATUS_DONE回调事件，
               此时的UI已在回调函数内重绘，此处不做处理*/
            if(!pMe->IsFullScreen)//非全屏状态
            {
                
                if(pMe->IsPause || pMe->IsPlay)
                { 
                    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_PLAY, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y); //将中间图标刷新为三角形的小图标      
					pMe->m_rtype = TYPE_PLAYER;
                    if(pMe->m_IsPlaynext)
                    {
                        DRAW_BOTTOMBAR(BTBAR_GALLERY_PLAY_BACK);
                    }
                    else
                    {
                        DRAW_BOTTOMBAR(BTBAR_PLAY_BACK);
                    }                
                    pMe->bCurrentTime=0; //将当前播放时间设为0，总时间不变 
                    VideoPlayer_RefreshPlayingTick(pMe);//刷新时间
                    VideoPlayer_RefreshScheduleBar(pMe);//进度条置在初始位置，不刷新文件名和其他设置
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
                    
                    pMe->UserStop=TRUE;
                    pMe->IsPlay=FALSE;
                    pMe->IsPause=FALSE;
                    
                    if(pMe->m_pMedia)
					{
						IMEDIA_Stop(pMe->m_pMedia);
					}
                    SetDeviceState(DEVICE_TYPE_MP4,DEVICE_MP4_STATE_OFF);
                    //ISHELL_SetTimer(pMe->m_pShell,50,(PFNNOTIFY)(IMEDIA_Stop),pMe->m_pMedia);                                    
                }
                else
                {
                    VideoPlayer_ReleaseVideo(pMe);
                    CLOSE_DIALOG(DLGRET_CANCELED);
                } 
            }            
            else //全屏状态下
            {
                {  
#ifdef FEATURE_MEDIAPLAYER_DECODER_INTERFACE					
                    MMD_LCDRotate(0);
#endif
                    pMe->IsFullScreen = FALSE;
		            FullScreen = FALSE;
                    
                    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_PLAYERPICTURE_PLAY, 0, 0);//背景图
                    if( pMe->IsPlay)
                    {   
                        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PAUSE, VIDEOPLAYER_PLAY_X, VIDEOPLAYER_PLAY_Y); // "||"   
						pMe->m_rtype = TYPE_PLAYER;
                        DRAW_BOTTOMBAR(BTBAR_FULLSCREEN_PAUSE_STOP);
                        
                    }
                    else if(pMe->IsPause)
                    {

#ifdef FEATURE_MEDIAPLAYER_DECODER_INTERFACE
					    MMD_Player_File_Pause(pFileHandle);     //add by wangjian 2009.2.5
#endif//WIN32
                        SetDeviceState(DEVICE_TYPE_MP4,DEVICE_MP4_STATE_OFF);
                        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PLAY, VIDEOPLAYER_PLAY_X, VIDEOPLAYER_PLAY_Y); //"|>"
						pMe->m_rtype = TYPE_PLAYER;
                        DRAW_BOTTOMBAR(BTBAR_PLAY_STOP);
                        
                    }
                    else
                    {
                        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PLAY, VIDEOPLAYER_PLAY_X, VIDEOPLAYER_PLAY_Y); //"|>"
						pMe->m_rtype = TYPE_PLAYER;
                        if(pMe->m_IsPlaynext)
                        {
                            DRAW_BOTTOMBAR(BTBAR_GALLERY_PLAY_BACK);
                        }
                        else
                        {
                            DRAW_BOTTOMBAR(BTBAR_PLAY_BACK);
                        }
                    }
                    
                    if(pMe->m_FileToPlay!=NULL )
                    {                       
                        VideoPlayer_RefreshPlayerFileName(pMe); //刷新文件名                        
                    }                    
                
                    VideoPlayer_RefreshPlayingTick(pMe);//刷新时间tick
                    VideoPlayer_RefreshVolBar(pMe);//刷新音量
                    VideoPlayer_RefreshScheduleBar(pMe);//刷新进度条            
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);                                   
                }
            }            
            return TRUE;
                              
        //前进
        case AVK_POUND:
            if(pMe->IsPlay)
            {             
                if((pMe->bTotalTime - pMe->bCurrentTime) < 5)
                {
                    IMEDIA_FastForward(pMe->m_pMedia,pMe->bTotalTime - pMe->bCurrentTime);
                    pMe->bCurrentTime=pMe->bTotalTime;
                }
                else 
                {
                    IMEDIA_FastForward(pMe->m_pMedia,5000); 
                    pMe->bCurrentTime=pMe->bCurrentTime+5;
                }
                if(!pMe->IsFullScreen)
                {
                    VideoPlayer_RefreshPlayingTick(pMe);//刷新时间
                    VideoPlayer_RefreshScheduleBar(pMe);//刷新播放进度                    
                    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_FORWARD_SELECT, VIDEOPLAYER_FORWARD_X,VIDEOPLAYER_FORWARD_Y); //画按键按下去的小图标
					pMe->m_rtype = TYPE_KUAIJIN;
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
                    (void) ISHELL_SetTimer(pMe->m_pShell, 100, (PFNNOTIFY)VideoPlayer_UpdateFRButton, pMe);            
                }
            }
            return TRUE;
                
        //后退
        case AVK_STAR:
            if(pMe->IsPlay)
            {               
                if( pMe->bCurrentTime < 5)
                {                  
                    IMEDIA_Rewind(pMe->m_pMedia,pMe->bCurrentTime);
                    pMe->bCurrentTime=0;
                }
                else
                {
                    IMEDIA_Rewind(pMe->m_pMedia,5000);   
                    pMe->bCurrentTime=pMe->bCurrentTime-5;
                }
                if(!pMe->IsFullScreen)
                {
                    VideoPlayer_RefreshPlayingTick(pMe);
                    VideoPlayer_RefreshScheduleBar(pMe);//刷新播放进度条
                    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_REWIND_SELECT, VIDEOPLAYER_REWIND_X, VIDEOPLAYER_REWIND_Y); //画按键按下去的小图标   
					pMe->m_rtype = TYPE_KUAITUI;
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
                    (void) ISHELL_SetTimer(pMe->m_pShell, 100, (PFNNOTIFY)VideoPlayer_UpdateFRButton, pMe);            
                }
            }
            return TRUE;     
               
        //播放上一首   
        case AVK_LEFT:           
            if(! pMe->m_IsPlaynext)
            {
                return TRUE;
            }
            ISHELL_SetTimer(pMe->m_pShell,APPISREADY_TIMER,VideoPlayer_APPIsReadyTimer,pMe);
            if(! pMe->m_bAppIsReady)
            {
                return TRUE;
            }
            if(!pMe->IsFullScreen)
            {
    
                VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_BEFORE_SELECT, VIDEOPLAYER_PREVIOUS_X, VIDEOPLAYER_PREVIOUS_Y); //画按键按下去的小图标
                pMe->m_rtype = TYPE_PREVIOUS;
                IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
                (void) ISHELL_SetTimer(pMe->m_pShell, 100, (PFNNOTIFY)VideoPlayer_UpdateButton, pMe);//100ms后画按钮弹起的图
                VideoPlayer_PlayNext(pMe, FALSE); 
            }
            else if(!pMe->IsPause)
            {
                VideoPlayer_PlayNext(pMe, FALSE); 
            }
            pMe->m_bAppIsReady=FALSE;
            return TRUE;
       
        //播放下一首   
        case AVK_RIGHT:  
            if(! pMe->m_IsPlaynext)
            {
                return TRUE;
            }
            ISHELL_SetTimer(pMe->m_pShell,APPISREADY_TIMER,VideoPlayer_APPIsReadyTimer,pMe);
            if(! pMe->m_bAppIsReady)
            {
                return TRUE;
            }
            if(!pMe->IsFullScreen)
            { 
                VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_NEXT_SELECT, VIDEOPLAYER_NEXT_X, VIDEOPLAYER_NEXT_Y);//画按键按下去的小图标  
				pMe->m_rtype = TYPE_NEXT;
                IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
                (void) ISHELL_SetTimer(pMe->m_pShell, 100, (PFNNOTIFY)VideoPlayer_UpdateButton, pMe);//100ms后画按钮弹起的图
                VideoPlayer_PlayNext(pMe, TRUE);
            }            
            else if(!pMe->IsPause)//全屏播放状态
            {
                DBGPRINTF("play next");
                VideoPlayer_PlayNext(pMe, TRUE); 
            }
            pMe->m_bAppIsReady=FALSE;
            return TRUE;          
          
        //增大音量   
        case AVK_UP: 
            if(pMe->m_bVolumeLevel < VOLUME_FIVE)
            {
                pMe->m_bVolumeLevel++;
                if(pMe->m_bVolumeLevel == VOLUME_FIVE)
                {
                    pMe->totalvolume = 100;//最大音量值
                }
                else
                {
                    pMe->totalvolume = pMe->m_bVolumeLevel * VP_VOLUME_STEP;
                }
                    
                if(pMe->m_pMedia)
                {
                    (void)IMEDIA_SetVolume(pMe->m_pMedia,pMe->totalvolume); 
                } 
                if(!pMe->IsFullScreen)
                {
                    VideoPlayer_RefreshVolBar(pMe); // 重新刷新音量bar
                    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_ADD_SELECT, VIDEOPLAYER_VOLUME_ADD_X,VIDEOPLAYER_VOLUME_ADD_Y);// 画按键按下去的小图标
					pMe->m_rtype = TYPE_ADDVOLUME;
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);                    
                    (void) ISHELL_SetTimer(pMe->m_pShell, 100, (PFNNOTIFY)VideoPlayer_UpdateButton, pMe);// 50ms后画按钮弹起的图                
                }
            }             
            return TRUE;          
          
        //减小音量  
        case AVK_DOWN: 
            if(pMe->m_bVolumeLevel > VOLUME_OFF)
            {
                pMe->m_bVolumeLevel--;
                pMe->totalvolume = pMe->m_bVolumeLevel * VP_VOLUME_STEP;
                if(pMe->m_pMedia)                                                                                                                                                                   
                {
                    (void)IMEDIA_SetVolume(pMe->m_pMedia,pMe->totalvolume);
                } 
                if(!pMe->IsFullScreen)
                {
                    VideoPlayer_RefreshVolBar(pMe);// 重新刷新音量bar 
                    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_LOW_SELECT, VIDEOPLAYER_VOLUME_LOW_X, VIDEOPLAYER_VOLUME_LOW_Y);// 画按键按下去的小图标
					pMe->m_rtype = TYPE_DECVOLUME;
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
                    (void) ISHELL_SetTimer(pMe->m_pShell, 100, (PFNNOTIFY)VideoPlayer_UpdateButton, pMe);// 50ms后画按钮弹起的图   
                }
            }            
            return TRUE;
       
#if 0
        //全屏播放
        case AVK_0:
            return VideoPlayer_PlayMod(pMe);            
            break; 
            
        case AVK_SELECT:
            if(pMe->IsFullScreen)
            {                
                return TRUE;
            }
            else
            {
                CLOSE_DIALOG(DLGRET_OPTION);
                return TRUE;
            }               
#endif
        
        case AVK_GSENSOR_STAND:
        case AVK_GSENSOR_UPEND: 
        case AVK_GSENSOR_LEFT:
        case AVK_GSENSOR_RIGHT: 
 
        case AVK_SELECT:
            if(pMe->IsPlay)
            {
                FullScreen = TRUE;
                return VideoPlayer_PlayMod(pMe,wParam);
            }
            if(pMe->UserStop && pMe->m_IsPlaynext)
            {
                CMediaGallery_FileExplorer(GALLERY_VIDEO_ADDLIST,NULL);
                return TRUE;
            }
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
		case EVT_PEN_UP:
			{
				int i;
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
				int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
				
				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, devinfo.cyScreen);
				if ((wXPos>0)&&(wXPos<devinfo.cxScreen/3)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
				{
					boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_SELECT,1);
					return rt;
				}
				else if ((wXPos>devinfo.cxScreen/3)&&(wXPos<(devinfo.cxScreen/3)*2)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
				{
					boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_INFO,0);
					 return rt;
				}
				else if ((wXPos>wXPos<(devinfo.cxScreen/3)*2)&&(wXPos<devinfo.cxScreen)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
				{
					boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_USER,AVK_CLR,0);
					return rt;
				}
			}
			return TRUE;
#endif//FEATURE_LCD_TOUCH_ENABLE         
        default:
            break;
    }
    return TRUE;
}
/*=================================================================================================================
  处理OPTION的按钮事件
=================================================================================================================*/
#if 0
static boolean VPDOption_HandleKeyEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam)
{
    switch(wParam)
    {       
        case AVK_UP:
        case AVK_DOWN:
            {                    
                if(pMe->m_nItemID == IDS_PLAY || IDS_PAUSE)     
                {
                    pMe->m_nItemID = IDS_FULLSCREEN;
                }
                if(pMe->m_nItemID== IDS_FULLSCREEN) 
                {
                    pMe->m_nItemID = IDS_DELETE || IDS_GALLERY;
                }
                if(pMe->m_nItemID== IDS_GALLERY) 
                {
                    pMe->m_nItemID = IDS_PLAY || IDS_PAUSE;
                }
                else if(pMe->m_nItemID== IDS_DELETE) 
                {
                    pMe->m_nItemID = IDS_GALLERY;  
                }
            } 
            return TRUE;
  
        case AVK_INFO:
        case AVK_SELECT:
            CLOSE_DIALOG(DLGRET_MSGBOX);
            return TRUE;

        case AVK_CLR:
            CLOSE_DIALOG(DLGRET_CANCELED);
            return TRUE;   

        default:
            break;         
    }
    return TRUE;
}
#endif
/*=================================================================================================================
      处理MSGBOX的按钮事件
=================================================================================================================*/
#if 0
static boolean VPDMSGBOX_HandleKeyEvent(CVideoPlayer *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam)
{ 
    if(pMe->m_nMsgResID==IDS_DONE)
    {
        CLOSE_DIALOG(DLGRET_VIDEOPLAYER);
    }
    switch(wParam)
    {
        case AVK_SELECT:
            (void)ISHELL_CancelTimer(pMe->m_pShell, DialogTimeoutCallback,pMe);            
            /*
            if(pMe->m_nMsgResID==IDS_PLAYFAILED)
            {
                CLOSE_DIALOG(DLGRET_CANCELED);
            }
            */
            if(pMe->m_nMsgResID==IDS_DEL)
            {    
                //删除正在播放的视频文件
                //VideoPlayer_DeleteVideo(pMe);
                //如果删除成功，则显示Done对话框
                CLOSE_DIALOG(DLGRET_MSGBOX);//Done弹框
            }
            return TRUE;
                    
        case AVK_CLR:  
            (void)ISHELL_CancelTimer(pMe->m_pShell, DialogTimeoutCallback,pMe);
            if(pMe->m_nMsgResID==IDS_DEL || pMe->m_nMsgResID==IDS_DONE)
            {                
                CLOSE_DIALOG(DLGRET_VIDEOPLAYER);
            }
            return TRUE;
                   
        default:
            break;
    }

    return TRUE;
}   
#endif
/*=================================================================================================================
  在指定位置画图
=================================================================================================================*/
static void VideoPlayer_DrawImage(CVideoPlayer *pMe, char *resFile, int16 resId, int x, int y)
{
    IImage *image = ISHELL_LoadResImage( pMe->m_pShell, resFile, resId);
    if( image != NULL)
    {
        IIMAGE_Draw(image, x, y);
        IIMAGE_Release(image);
    }
    //IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
}
/*=================================================================================================================
   重画player下半屏的按钮部分
=================================================================================================================*/                
static  void VideoPlayer_UpdateButton(CVideoPlayer *pMe)
{
    if(!pMe->IsPlay)
    {
        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PLAY_PART, VIDEOPLAYER_PLAY_PART_X,VIDEOPLAYER_PLAY_PART_Y);//重画三角形按键盘,弹起的大图   
    }
    else VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PAUSE_PART, VIDEOPLAYER_PAUSE_PART_X,VIDEOPLAYER_PAUSE_PART_Y);//重画"||"按键盘

	if(pMe->m_rtype == TYPE_ADDVOLUME)
	{
		VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_ADD, VIDEOPLAYER_VOLUME_ADD_X,VIDEOPLAYER_VOLUME_ADD_Y);//重画三角形按键盘,弹起的大图   
	}
	else if(pMe->m_rtype == TYPE_DECVOLUME)
	{
		VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_LOW, VIDEOPLAYER_VOLUME_LOW_X,VIDEOPLAYER_VOLUME_LOW_Y);//重画三角形按键盘,弹起的大图   
	}
	else if(pMe->m_rtype == TYPE_PREVIOUS)
	{
		VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_BEFORE, VIDEOPLAYER_PREVIOUS_X,VIDEOPLAYER_PREVIOUS_Y);//重画三角形按键盘,弹起的大图   
	}
	else if(pMe->m_rtype == TYPE_NEXT)
	{
		VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_NEXT, VIDEOPLAYER_NEXT_X,VIDEOPLAYER_NEXT_Y);//重画三角形按键盘,弹起的大图   
	}
	else if(pMe->m_rtype == TYPE_KUAIJIN)
	{
		VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_FORWARD, VIDEOPLAYER_FORWARD_X,VIDEOPLAYER_FORWARD_Y);//重画三角形按键盘,弹起的大图   
	}
	else if(pMe->m_rtype == TYPE_KUAITUI)
	{
		VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_REWIND, VIDEOPLAYER_REWIND_X,VIDEOPLAYER_REWIND_Y);//重画三角形按键盘,弹起的大图   
	}
    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
}
static void VideoPlayer_UpdateFRButton(CVideoPlayer *pMe)
{    
    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_FR_PART, VIDEOPLAYER_FR_PART_X,VIDEOPLAYER_FR_PART_Y);
    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
}
/*=================================================================================================================
初始化video
=================================================================================================================*/
void  VideoPlayer_InitVideo(CVideoPlayer  *pMe)                  
{      
    //填写m_md stucture
    pMe->m_md.clsData = MMD_FILE_NAME;
    pMe->m_md.pData = (void*)pMe->m_FileToPlay;
    pMe->m_md.dwSize = 0;
    DBGPRINTF("pMe->m_FileToPlay=%s",pMe->m_FileToPlay);
    pMe->m_InitFailed = AEEMediaUtil_CreateMedia(pMe->m_pShell, &pMe->m_md, &pMe->m_pMedia);
    DBGPRINTF("pMe->m_InitFailed=%d",pMe->m_InitFailed);

    if(pMe->m_InitFailed == SUCCESS)
    {     
        (void)IMEDIA_RegisterNotify(pMe->m_pMedia, VideoPlayer_VideoNotify, pMe);//注册底层回调 
        pMe->bCurrentTime = 0;
        pMe->bTotalTime = 0;
        (void)IMEDIA_GetTotalTime(pMe->m_pMedia); 
        (void)IMEDIA_SetVolume(pMe->m_pMedia, pMe->totalvolume); //设置当前音量大小
    }    
}
/*=================================================================================================================
  播放视频
=================================================================================================================*/      
void VideoPlayer_PlayVideo(CVideoPlayer *pMe)
{     
    if(pMe->m_pMedia)
    {  
        pMe->m_PlayFailed = IMEDIA_Play(pMe->m_pMedia);//播放  
        SetDeviceState(DEVICE_TYPE_MP4,DEVICE_MP4_STATE_ON);
    }      
} 
                    
/*=================================================================================================================
   播放下一个文件, 
   bDirection为TRUE, 下一个文件
   bDirection为FALSE, 前一个文件
   pMe->m_MenuSelected
=================================================================================================================*/
static  void VideoPlayer_PlayNext(CVideoPlayer *pMe, boolean bDirection)
{  
    uint16    videoID = 0;
    MGFileInfo  pInfo;
    
    videoID=VideoPlayer_GetFileID(pMe);
    DBGPRINTF("pMe->m_RecordCount=%d",pMe->m_RecordCount);
    //video放置在指定文件夹外
    if(videoID >= pMe->m_RecordCount)
    {  
        DBGPRINTF("videoID >= pMe->m_RecordCount");
        return ;
    }
    else
    {
        switch(pMe->m_RecordCount)
        {
            //文件数为0，什么都不做
            case 0:
                return;
        
            //文件数为1，重新播放此文件(直接播放)   
            case 1:    
                pMe->bCurrentTime = 0; 
                if(! pMe->UserStop)
                {
                    (void)IMEDIA_Stop(pMe->m_pMedia); 
                }
                VideoPlayer_PlayVideo(pMe);   
                videoplayer_play_flag = TRUE;
                pMe->IsPlay   = TRUE;
                pMe->IsPause  = FALSE;
                pMe->UserStop = FALSE; 
                if( ! pMe->IsFullScreen && !pMe->IsPlay)
                {
                    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_PAUSE, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y);   
#ifdef FEATURE_SPORTS_APP					
                    DRAW_BOTTOMBAR(BTBAR_PAUSE_STOP); 
#endif                    
                    VideoPlayer_RefreshPlayingTick(pMe);//刷新时间
                    VideoPlayer_RefreshScheduleBar(pMe);//进度条置在初始位置
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
                }
                break;
        
            //文件数大于1，播放下一首   
            default:  
                if(! pMe->UserStop)
                {
                    (void)IMEDIA_Stop(pMe->m_pMedia); 
                }
                //释放本m_pMedia指针
                VideoPlayer_ReleaseVideo(pMe);
                FREEIF(pMe->m_FileToPlay);
                if(bDirection)//下一首
                {   
                    //当前播放的文件为记录中的最后一个,则播放记录中的第一个文件     
                    if(videoID +1 == pMe->m_RecordCount)
                    {                  
                        videoID=0;
                    }                    
                    else  videoID+=1;                    
                }                
                else//上一首
                {
                    //当前播放的文件为记录中的第一个,则播放记录中的最后一个文件     
                    if(videoID == 0)
                    {     
                        videoID=pMe->m_RecordCount-1;
                    }                    
                    else  videoID-=1;
                }    
                CMediaGallery_GetFileInfoRecord(pMe->m_pFileDB,videoID, &pInfo);
                
                pMe->m_FileToPlay= STRDUP(pInfo.szName);
                if(pMe->m_pFileDB!=NULL)
                {
                    IDATABASE_Release(pMe->m_pFileDB);
                    pMe->m_pFileDB = NULL;
                }
           
                VideoPlayer_InitVideo(pMe);
                // 如果当前视频为播放状态,则下一首视频也直接置为播放状态;不是,则手动播放                
                VideoPlayer_PlayVideo(pMe); 
                if(pMe->IsPause)
                {
                    IMedia_Pause(pMe->m_pMedia);                    
                }
                if(pMe->UserStop)
                {
                    IMedia_Stop(pMe->m_pMedia);                   
                }
                if(pMe->IsFullScreen)
                {
#ifdef FEATURE_MEDIAPLAYER_DECODER_INTERFACE                
                    MMD_LCDRotate(0);
#endif
                    //如果下一首播放成功，则转为全屏，不成功则在正常模式弹出播放失败的弹框
                    if(pMe->m_PlayFailed == SUCCESS)
                    {
#ifdef FEATURE_MEDIAPLAYER_DECODER_INTERFACE                
                    	MMD_LCDRotate(1);
#endif                      
                    }
                    else
                    {
                        VideoPlayer_RefreshPlayerFileName(pMe);// 刷新文件名 
                        IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);    
                    }
                    /*全屏暂停时不playnext
                    if(pMe->IsPause)
                    {
                        MMD_Player_File_Pause(pFileHandle); 
                    }
                    */
                }  
                
                if(!pMe->IsFullScreen)
                {
                    if(pMe->m_PlayFailed == SUCCESS)
                    {
                        VideoPlayer_RefreshPlayingTick(pMe);// 刷新播放时间             
                        VideoPlayer_RefreshScheduleBar(pMe);// 刷新进度条 
                    }
                    VideoPlayer_RefreshPlayerFileName(pMe);// 刷新文件名 
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);    
                }
            
                break;    
        }      
    }
    //非用户手动停止
    //pMe->UserStop = FALSE;
}

/*=================================================================================================================
   刷player屏的上半部分的文件名
=================================================================================================================*/                                                                        
static  void VideoPlayer_RefreshPlayerFileName(CVideoPlayer *pMe)
{   
    AEERect   rc_name;
    char      title_copy[MAX_STR_LEN];  
    AECHAR    player_title[MAX_STR_LEN];
    char      *filename=NULL; 
    int       len;
        
    pMe->m_np=0;
    MEMSET(title_copy, 0, sizeof(title_copy));
    MEMSET(player_title, 0, sizeof(player_title));
    MEMSET(pMe->m_playname, 0, sizeof(pMe->m_playname));
    //截取文件路径名中的文件名以显示在播放器上 
    filename= STRRCHR(pMe->m_FileToPlay, (int)DIRECTORY_CHAR);
    if(filename!=NULL)
    {
        filename=filename+1;
        STRCPY(title_copy,filename);
    } 
    else STRCPY(title_copy,pMe->m_FileToPlay);    
   
    //UTF8转AECHAR
    (void)UTF8TOWSTR((const byte *)title_copy,(int)STRLEN(title_copy),player_title,sizeof(player_title)); 
    (void)UTF8TOWSTR((const byte *)title_copy,(int)STRLEN(title_copy),pMe->m_playname,sizeof(pMe->m_playname));

    len=IDISPLAY_MeasureText(pMe->m_pDisplay,AEE_FONT_NORMAL,player_title); // 绘制字符串所需要的像素    
    if(len > MAX_NAME_LEN )
    {        
        //滚动显示名字
        VideoPlayer_ShowName(pMe);  
    }
    else 
    {        
        VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_NAME_PART, 0, 0);  
        SETAEERECT(&rc_name, 2,1, 172, 17);
        //写title
        DrawTextWithProfile(pMe->m_pShell, 
                            pMe->m_pDisplay, 
                            RGB_WHITE, //文本轮廓的RGBVAL颜色值,RGB_BLACK
                            AEE_FONT_BOLD,
                            player_title, 
                            -1, 
                            22, 
                            22, 
                            &rc_name, 
                            IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE|IDF_TEXT_TRANSPARENT); 
    }
}
//滚动显示文件名
static void VideoPlayer_ShowName(CVideoPlayer *pMe) 
{ 
    AEERect  rc_title;
    int      str;
    AECHAR   *pp;   
    int      settime;
    
    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_NAME_PART, 0, 0); 
    SETAEERECT(&rc_title, 2,1, 172, 17);
    
    pp=&pMe->m_playname[pMe->m_np]; 
    pMe->m_np += 1;
    str=IDISPLAY_MeasureText(pMe->m_pDisplay,AEE_FONT_NORMAL,pp);   
     
    //写title
    DrawTextWithProfile(pMe->m_pShell, 
                        pMe->m_pDisplay, 
                        RGB_WHITE, //文本轮廓的RGBVAL颜色值,RGB_BLACK
                        AEE_FONT_BOLD,
                        pp, 
                        -1, 
                        22, 
                        22, 
                        &rc_title, 
                        IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE|IDF_TEXT_TRANSPARENT);
    IDISPLAY_UpdateEx(pMe->m_pDisplay,TRUE); 
    if(str <= MAX_NAME_LEN)
    {
        pMe->m_np=0;
    }
    if(pMe->m_np==1)
    {
        settime=SETTIME_LONG; // 3000 
    }
    else settime=SETTIME_SHORT; // 300
    ISHELL_SetTimer(pMe->m_pShell,settime,(PFNNOTIFY) VideoPlayer_ShowName,pMe);
  
}
/*=================================================================================================================
   刷新播放时间
=================================================================================================================*/   
static void VideoPlayer_RefreshPlayingTick(CVideoPlayer *pMe)
{
    AEERect rc_tick;    
    char    tick_time[MAX_STR_LEN];
    AECHAR  Wtick_time[MAX_STR_LEN]; 
    
    //在临近结束和开始的时候不允许暂停，否则可能出错
    if(pMe->bTotalTime == pMe->bCurrentTime || pMe->bCurrentTime == 0)
    {
        pMe->PauseLock = TRUE;
    }
    else
    {
        pMe->PauseLock = FALSE; 
    }      
   
    //画时间显示区域
    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_TIME_PART, VIDEOPLAYER_TIME_X, VIDEOPLAYER_TIME_Y); 
    //tick time
    SETAEERECT(&rc_tick, VIDEOPLAYER_TIME_X, VIDEOPLAYER_TIME_Y, VIDEOPLAYER_TIME_W, VIDEOPLAYER_TIME_H);
    
    MEMSET(tick_time,0,MAX_STR_LEN);

    SPRINTF(tick_time,"%02d:%02d",pMe->bCurrentTime/60,pMe->bCurrentTime%60);
    
    STRTOWSTR(tick_time, Wtick_time, sizeof(Wtick_time));
    DrawTextWithProfile(pMe->m_pShell, 
                        pMe->m_pDisplay, 
                        RGB_WHITE, 
                        AEE_FONT_BOLD,
                        Wtick_time, 
                        -1, 
                        21, 
                        57, 
                        &rc_tick, 
                        IDF_ALIGN_LEFT|IDF_ALIGN_MIDDLE|IDF_TEXT_TRANSPARENT);
}    

/*=================================================================================================================
  刷新进度条
=================================================================================================================*/
static void VideoPlayer_RefreshScheduleBar(CVideoPlayer *pMe)
{
    AEERect  rc;
    AEERect  Rc;
    AEERect  OldClip;
    AEERect  Clip;
    int16    ma;
    
    //取小图标图片
    IImage *image =ISHELL_LoadResImage(pMe->m_pShell, VIDEOPLAYER_IMAGES_RES_FILE, IDI_GLIDER); 
    
    //画进度条
    VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_SCHEDULE_EMPTY, VIDEOPLAYER_SCHEDULE_X, VIDEOPLAYER_SCHEDULE_Y); 
    SETAEERECT(&rc,VIDEOPLAYER_SCHEDULE_X,VIDEOPLAYER_SCHEDULE_Y,5,5);//滑块起始位置 
    SETAEERECT(&Rc,VIDEOPLAYER_SCHEDULE_X + VIDEOPLAYER_SCHEDULE_W,VIDEOPLAYER_SCHEDULE_Y,5,5);//滑块最终位置    
    ma=Rc.x-rc.x;//滑块可以移动的长度，26个像素  
    if(pMe->bTotalTime == 0)
    {
        Clip.x=rc.x;        
    }   
    else 
    {
        Clip.x=rc.x + pMe->bCurrentTime * ma / pMe->bTotalTime;
    }
    
    if(pMe->bCurrentTime > pMe->bTotalTime)
    {
        Clip.x=Rc.x;        
    }  
    //VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_GLIDER, Clip.x, 174);
    
    IDISPLAY_GetClipRect(pMe->m_pDisplay, &OldClip);//restore clip rect
    SETAEERECT(&Clip,Clip.x,VIDEOPLAYER_SCHEDULE_Y,5,5);//滑块移动时的位置    
    IDISPLAY_SetClipRect(pMe->m_pDisplay, &Clip);
    IIMAGE_Draw(image,Clip.x,VIDEOPLAYER_SCHEDULE_Y);    
    IIMAGE_Release(image);
    IDISPLAY_SetClipRect(pMe->m_pDisplay, &OldClip);//recover clip rect
}
/*=================================================================================================================
  刷新音量bar
 ================================================================================================================*/
static void VideoPlayer_RefreshVolBar(CVideoPlayer *pMe)
{
    uint16 ResID;   
    switch ((int)pMe->m_bVolumeLevel)
    {
        case 0:
            ResID = IDI_VOLUME2_0;//图片资源ID 
            break;
            
        case 1:
            ResID = IDI_VOLUME2_1;
            break;
            
        case 2:
            ResID = IDI_VOLUME2_2;
            break;
            
        case 3:
            ResID = IDI_VOLUME2_3;
            break;
            
        case 4:
            ResID = IDI_VOLUME2_4;
            break;
            
        case 5:
            ResID = IDI_VOLUME2_5;
            break;
        
        default :
            ResID = IDI_VOLUME2_0;            
    }
    VideoPlayer_DrawImage( pMe, VIDEOPLAYER_IMAGES_RES_FILE,ResID, VIDEOPLAYER_VOLUME_X, VIDEOPLAYER_VOLUME_Y);  
}

/*=================================================================================================================
   视频播放的模式:部分 or 全屏
=================================================================================================================*/
boolean VideoPlayer_PlayMod(CVideoPlayer *pMe, uint16 wParam)
{  
  
    if(pMe->IsPlay)
    {
        if(!pMe->IsFullScreen)
        {
            if((AVK_GSENSOR_STAND != wParam) && (AVK_GSENSOR_UPEND != wParam))
            {    
                ISHELL_CancelTimer(pMe->m_pShell,NULL,pMe); 
                pMe->IsFullScreen = TRUE;
#ifdef FEATURE_MEDIAPLAYER_DECODER_INTERFACE 			
                MMD_LCDRotate(1);
#endif
            }
        }
        else
        {   
            if((AVK_GSENSOR_LEFT != wParam) && (AVK_GSENSOR_RIGHT != wParam))
            {   
                pMe->IsFullScreen = FALSE;
                FullScreen = FALSE;
                
                VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE,IDI_PLAYERPICTURE_PAUSE, 0, 0);//背景图
                DRAW_BOTTOMBAR(BTBAR_FULLSCREEN_PAUSE_STOP);               
                VideoPlayer_RefreshPlayerFileName(pMe); //刷新文件名               
                VideoPlayer_RefreshPlayingTick(pMe);
                VideoPlayer_RefreshVolBar(pMe);//刷新音量
                VideoPlayer_RefreshScheduleBar(pMe);//刷新进度条
                IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE); 
#ifdef FEATURE_MEDIAPLAYER_DECODER_INTERFACE 			                
                MMD_LCDRotate(0);
#endif
            }
        return TRUE;
        }
    }    
   
    return TRUE;
}
#if 0
/*=================================================================================================================
   删除当前播放的文件
==================================================================================================================*/
static boolean VideoPlayer_DeleteVideo(CVideoPlayer *pMe)
{
    uint32  NumOfFile;//总文件数

    NumOfFile=1;//从GALLERY里获取

    if(!pMe->UserStop)
    {
        pMe->UserStop=TRUE;        
        //IMEDIA_Stop(pMe->m_pMedia);
    }
    VideoPlayer_ReleaseVideo(pMe);
    //if(成功删除文件)
    //{
    //    pMe->m_MenuSelected++;
          NumOfFile= NumOfFile-1; 
    //}
    switch(NumOfFile)
    {
        case 0:
            return FALSE;         
            
        default:
            //因为底层使用的是事件传递机制，需要给底层stop并释放Video 的时间，
            //否则会出错，此处等1秒钟后初始化下一首
            (void) ISHELL_SetTimer(pMe->m_pShell,1000,(PFNNOTIFY) VideoPlayer_InitVideo,pMe);
            break;
            
    }
    return TRUE;
}
#endif

/*=================================================================================================================
   接受CMX底层回调
=================================================================================================================*/ 
static void VideoPlayer_VideoNotify(void * pUser, AEEMediaCmdNotify * pCmdNotify)
{
    CVideoPlayer *   pMe = (CVideoPlayer *)pUser; 

    switch (pCmdNotify->nStatus)
    {   
        case MM_STATUS_SEEK_FAIL:     
        case MM_STATUS_PAUSE_FAIL:
        case MM_STATUS_RESUME_FAIL:
        default: 
            break;
    }
    
    //播放视频
    if (pCmdNotify->nCmd == MM_CMD_PLAY)
    {
        switch (pCmdNotify->nStatus)
        {   
            // playback done
            case MM_STATUS_DONE: 
                IMEDIA_Stop(pMe->m_pMedia);
                SetDeviceState(DEVICE_TYPE_MP4,DEVICE_MP4_STATE_OFF); 
                pMe->IsPlay=FALSE;
                pMe->IsPause=FALSE;
                pMe->UserStop=TRUE; 
                videoplayer_play_flag = FALSE;
        
                pMe->bCurrentTime=0;
                pMe->IsFullScreen=FALSE;                   
                VideoPlayer_RefreshPlayingTick(pMe);
                VideoPlayer_RefreshScheduleBar(pMe);//刷新进度条
                VideoPlayer_RefreshVolBar(pMe);// 刷新音量
                VideoPlayer_RefreshPlayerFileName(pMe);
                VideoPlayer_DrawImage(pMe,VIDEOPLAYER_IMAGES_RES_FILE, IDI_PLAY, VIDEOPLAYER_PLAY_X,VIDEOPLAYER_PLAY_Y); //"|>"
				pMe->m_rtype = TYPE_PLAYER;
                
                if(pMe->m_IsPlaynext)
                {
                    DRAW_BOTTOMBAR(BTBAR_GALLERY_PLAY_BACK);
                }
                else
                {
                    DRAW_BOTTOMBAR(BTBAR_PLAY_BACK);
                }
                
                IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);                                    
                break;
                 
            // playback aborted,异常中断
            case MM_STATUS_ABORT:
                CLOSE_DIALOG(DLGRET_MSGBOX); // 提示播放失败后退出  
                break;  
            
            case MM_STATUS_PAUSE:
                //取暂停时间,以便挂起时恢复到当前位置
                pMe->m_bPauseTime= (uint32)pCmdNotify->pCmdData;        
                break;

            case MM_STATUS_RESUME:
                //取恢复时的时间,以便挂起时恢复到当前位置
                //pMe->m_bResumeTime= (uint32)pCmdNotify->pCmdData;
                               
            //播放的时候每秒会发上来一次
            case MM_STATUS_TICK_UPDATE: 
                pMe->bCurrentTime=((uint32)pCmdNotify->pCmdData) / 1000;   
                if(!pMe->IsFullScreen)
                {
                    VideoPlayer_RefreshScheduleBar(pMe);//刷新进度条
                    VideoPlayer_RefreshPlayingTick(pMe);  
                    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
                }                        
                break; 
                        
            default:
                break;        
        }
    } 
    
    //return from IMEDIA_GetTotalTime
    if(pCmdNotify->nCmd == MM_CMD_GETTOTALTIME) 
    {
        pMe->bTotalTime = ((uint32)pCmdNotify->pCmdData) / 1000;
    }         
}
/*=================================================================================================================
函数:
    DialogTimeoutCallback

说明:
    传递给BREW层的回调函数，以便设定的定时器超时发出超时事件通知对话框。

参数:
    pUser [in]: 必须是指向 videoplayer Applet对象结构的指针。

返回值:
    none

备注:

=================================================================================================================*/
static void DialogTimeoutCallback(void *pUser)
{
    CVideoPlayer *pMe = (CVideoPlayer*)pUser;  

    if (NULL == pMe)
    {
        return;
    }
    
    if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_VIDEOPLAYER)
    {
        return;
    }
    (void) ISHELL_PostEvent(pMe->m_pShell,AEECLSID_VIDEOPLAYER,EVT_DISPLAYDIALOGTIMEOUT,0,0);
                         
}
#if 1
// 获取指定视频的名字
static int VideoPlayer_GetFileID(CVideoPlayer *pMe)
{   
    MGFileInfo  pInfo;
    uint16    fileID=0;//ufileID;
    
    if(pMe->m_pFileDB!=NULL)
    {
        IDATABASE_Release(pMe->m_pFileDB);
        pMe->m_pFileDB = NULL;
     }
 
    //打开video数据库
    CMediaGallery_OpenFileInfoDatabase(pMe->m_pShell,MG_VIDEOFILE_DB_NAME,TRUE,&pMe->m_pFileDB);
    if(pMe->m_pFileDB !=NULL)
    {
        // 获取数据库中记录的个数
        pMe->m_RecordCount=IDATABASE_GetRecordCount(pMe->m_pFileDB);  
        DBGPRINTF("pMe->m_RecordCount=%s",pMe->m_RecordCount);
    }
    for(fileID=0;fileID < pMe->m_RecordCount; fileID++)
    {
        CMediaGallery_GetFileInfoRecord(pMe->m_pFileDB,fileID, &pInfo);  
            
        if(STRCMP(pInfo.szName,pMe->m_FileToPlay)== 0)
        {
            break;
        }
    }   

    return fileID;

    #if 0
    if(SUCCESS != IFILEMGR_EnumInit(pMe->pIFileMgr,const char * pszDir,FALSE))
    {
        return EFAILED;
    }  
    for(;;)
    {           
        ret= IFILEMGR_EnumNext(pMe->pIFileMgr,&pInfo);
        
        if(ret!=SUCCESS)
        {
            break;
        }
        //将文件的名字放在FileName中,以便在播放上下首时可以取得文件名进行初始化
       
    }   
    #endif

}

#endif

