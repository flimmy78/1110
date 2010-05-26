/*==============================================================================
//           U S E R   I N T E R F A C E   M A I N  M O D U L E
// 文件:
//        CoreAppDialogHandler.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明:
//        本应用是 UI 的起始点，在开机时由 BREW 自动启动。         
//
// 作者:2007-09-18
// 当前版本:Draft
==============================================================================*/

/*==============================================================================
修改历史记录:
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------

==============================================================================*/



/*==============================================================================
                             
                             本文件包含的外部文件
                             
==============================================================================*/
#include "OEMFeatures.h"
#include "CoreApp_priv.h"
#include "AEEMenu.h"
#ifndef WIN32
#include "uim.h"
#include "mobile.h"             // for ver_modelname
#include "mccdma.h"  //for use of cdma_bs_type
#endif
#include "Appscommon.h"
#include "OEMRTC.h"
//#include "AEECallList.h"
#include "clockapps_images.brh"
#include "Appscommon_color.brh"
#include "AEEGraphics.h "

#include "clockapps.brh"
#define  CLOCK_RES_PATH ("fs:/mod/clockapps/" AEE_RES_LANGDIR CLOCKAPPS_RES_FILE)
#if defined( FEATURE_POWERDOWN_ALARM)
extern void registerPowerdownAlarmclock( void);
#endif
extern void OEMPriv_DrawMessageCB(void *pUnused);
#ifdef FEATRUE_SUPPORT_G_SENSOR
#include "g_sensor.h"
#include "DisplayMenu.h"
#endif

/*==============================================================================

                               宏定义和常数
                                 
==============================================================================*/
#define CoreDrawBottomBar(x)                        \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = x;                         \
    DrawBottomBar(pMe->m_pDisplay, &BarParam);      \
}

#ifndef CUST_EDITION
extern boolean g_b_no_batt;
#endif

// 开关机动画播放时间
#define PWRON_ANI_TIME    ((PWRON_ANI_RATE)*(PWRON_ANI_FRAME_COUNT))
#define PWROFF_ANI_TIME  ((PWROFF_ANI_RATE)*(PWROFF_ANI_FRAME_COUNT))

/*==============================================================================

                                 类型定义

==============================================================================*/
//typedef struct {
//   char *num_buf;
//   byte  num_len;
//} EmergencyNumber;
static IImage * pWallPaper = NULL;
//int charging_mark = 0;
//int charging_mark2 = 0;

static boolean bHideText = FALSE;
static boolean bImageDecoded = FALSE;
/*==============================================================================

                                 函数声明
                                 
==============================================================================*/
// 对话框 IDD_ALARM 事件处理函数
#if defined( FEATURE_POWERDOWN_ALARM)
static boolean  IDD_ALARM_Handler(void *pUser,
                                AEEEvent   eCode,
                                uint16     wParam,
                                uint32     dwParam);
#endif//#if FEATURE_POWERDOWN_ALARM
// 对话框 IDD_LPM 事件处理函数
static boolean  IDD_LPM_Handler(void *pUser,
                                AEEEvent   eCode,
                                uint16     wParam,
                                uint32     dwParam);
                                
// 对话框 IDD_MSGBOX 事件处理函数
static boolean  IDD_MSGBOX_Handler(void *pUser,
                                   AEEEvent   eCode,
                                   uint16     wParam,
                                   uint32     dwParam);
                                
// 对话框 IDD_PWDINPUT 事件处理函数
static boolean  IDD_PWDINPUT_Handler(void *pUser,
                                     AEEEvent   eCode,
                                     uint16     wParam,
                                     uint32     dwParam);
                                     
// 对话框 IDD_UIMERR 事件处理函数
static boolean  IDD_UIMERR_Handler(void *pUser,
                                   AEEEvent   eCode,
                                   uint16     wParam,
                                   uint32     dwParam);

// 对话框 IDD_UIMSECCODE 事件处理函数
static boolean  IDD_UIMSECCODE_Handler(void *pUser,
                                       AEEEvent   eCode,
                                       uint16     wParam,
                                       uint32     dwParam);
                                       
// 对话框 IDD_EMERGENCYNUMLIST 事件处理函数
static boolean  IDD_EMERGENCYNUMLIST_Handler(void *pUser,
                                 AEEEvent   eCode,
                                 uint16     wParam,
                                 uint32     dwParam);
                                 
// 对话框 IDD_STARTUPANI 事件处理函数
static boolean  IDD_STARTUPANI_Handler(void *pUser,
                                       AEEEvent   eCode,
                                       uint16     wParam,
                                       uint32     dwParam);
                                       
// 对话框 IDD_LOADING 事件处理函数
static boolean  IDD_LOADING_Handler(void *pUser,
                                 AEEEvent   eCode,
                                 uint16     wParam,
                                 uint32     dwParam);
                                       
// 对话框 IDD_IDLE 事件处理函数
static boolean  IDD_IDLE_Handler(void *pUser,
                                 AEEEvent   eCode,
                                 uint16     wParam,
                                 uint32     dwParam);
                                 
#if defined(FEATURE_WMS_APP)
// 对话框 IDD_WMSTIPS 事件处理函数
static boolean  IDD_WMSTIPS_Handler(void *pUser,
                                 AEEEvent   eCode,
                                 uint16     wParam,
                                 uint32     dwParam);
#endif

// 对话框 IDD_POWERDOWN 事件处理函数
static boolean  IDD_POWERDOWN_Handler(void  *pMe,
                                      AEEEvent    eCode,
                                      uint16      wParam,
                                      uint32      dwParam);

#ifdef FEATURE_UTK2
static boolean  IDD_UTKREFRESH_Handler(void *pUser,
                                   AEEEvent   eCode,
                                   uint16     wParam,
                                   uint32     dwParam);
#endif //FEATURE_UTK2
// 对话框 IDD_PWDIMSIMCC 事件处理函数
#ifdef FATRUE_LOCK_IMSI_MCCMNC
static boolean  IDD_PWDIMSIMCC_Handler(void       *pUser,
                                     AEEEvent   eCode,
                                     uint16     wParam,
                                     uint32     dwParam);
#endif
                                       
// 更新待机界面的定时器函数。程序运行稳定后，每分钟执行一次
static void CoreApp_UpdateIdleTimer(void *pUser);                                       

static void CoreApp_DrawBannerMessage(CCoreApp    *pMe);

#ifdef FEATURE_APP_MUSICPLAYER
static void CoreApp_DrawMusicName(CCoreApp    *pMe,uint16 nIdx);
static void CoreApp_MusicNameAutoScroll(CCoreApp *pMe);
static boolean Core_bMusicNameRequiredScroll(CCoreApp *pMe, int nIdx);
static void Core_DrawNameResetScroll(CCoreApp *pMe);
#endif

static void CoreApp_SearchingTimer(void *pUser);

static void CoreApp_UpdateDateTime(CCoreApp    *pMe);

// 开机动画播放函数
static void CoreApp_PlayPwrOnAni(CCoreApp *pMe);

//关机动画播放函数
static void CoreApp_PlayPwrOffAni(CCoreApp *pMe);

//绘制墙纸
static void CoreApp_DrawWallPaper(CCoreApp *pMe);

static boolean CoreApp_LaunchApplet(CCoreApp *pMe,  AEECLSID   classID);

//static void CoreApp_UpdateAnnunciator(CCoreApp *pMe);
static void CoreApp_Process_Rtc_Event(CCoreApp *pMe);

#ifdef FEATURE_KEYGUARD
static void CoreApp_TimeKeyguard(void *pUser);
#endif

static void CoreApp_UpdateBottomBar(CCoreApp    *pMe); 

//static void CoreApp_GetRecordCount(CCoreApp *pMe);

//static boolean CoreApp_PowerAlarm_Event(CCoreApp *pMe);
//static void CoreApp_DrawStartAlarm(CCoreApp *pMe);
//boolean CoreApp_Start_Alarm(CCoreApp *pMe);
static void CoreApp_GetSPN(CCoreApp *pMe);

static void CoreApp_ImageNotify(void *po, IImage *pIImage, AEEImageInfo *pii, int nErr);

//add by ydc 20090523
static void CoreApp_SetMintueImageID(CCoreApp *pMe,uint16 MINUTE_angle);
static double CoreApp_sin(uint16 angle);
static void CoreApp_DrawMinuteHand(CCoreApp *pMe,uint16 Minute_angle);
/*==============================================================================

                                 函数定义
                                 
==============================================================================*/

/*==============================================================================
函数:
    CoreApp_ShowDialog
       
说明:
    函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
    pMe [In]: 指向 Core Applet对象结构的指针,该结构包含小程序的特定信息.
    dlgResId [in]:对话框资源ID，函数根据该ID创建对话框。

返回值:
    无

备注:

==============================================================================*/
void CoreApp_ShowDialog(CCoreApp *pMe,uint16  dlgResId)
{
    int nRet;

    // 一次只允许打开一个对话框
    if (ISHELL_GetActiveDialog(pMe->a.m_pIShell) != NULL)
    {
        // Looks like there is one dialog already opened.
        // Flag an error an return without doing anything.
        ERR("Trying to create a dialog without closing the previous one",0,0,0);
        return;
    }

    nRet = ISHELL_CreateDialog(pMe->a.m_pIShell,
                               AEE_COREAPPRES_LANGFILE,
                               dlgResId,NULL);

    if (nRet != SUCCESS)
    {
        ERR("Failed to create the dialog in the Core applet",0,0,0);
    }
}

/*==============================================================================
函数:
    CoreApp_ShowMsgDialog
       
说明:
    函数用来显示指定消息的对话框。

参数:
    pMe [In]: 指向 Core Applet对象结构的指针,该结构包含小程序的特定信息.
    nResId [in]:消息字符串在资源文件中的 ID 号。

返回值:
    none

备注:

==============================================================================*/
void CoreApp_ShowMsgDialog(CCoreApp *pMe,uint16  nResId)
{
    pMe->m_nMsgID = nResId;

    (void)CoreApp_ShowDialog(pMe,IDD_MSGBOX);
}

/*==============================================================================
函数:
    CoreApp_SetDialogHandler
       
说明:
    函数根据 pMe->m_wActiveDlgID 设定对话事件处理函数指针 m_pDialogHandler 相应值。

参数:
    pMe [In]: 指向 Core Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
    none

备注:
    若找不到对应的对话框处理函数, m_pDialogHandler 被置为 NULL
==============================================================================*/
void CoreApp_SetDialogHandler(CCoreApp *pMe)
{
    switch (pMe->m_wActiveDlgID)
    {
#if defined( FEATURE_POWERDOWN_ALARM)
        case IDD_ALARM:
            pMe->m_pDialogHandler = IDD_ALARM_Handler;
            break;
#endif //#if FEATURE_POWERDOWN_ALARM
        case IDD_LPM:
            pMe->m_pDialogHandler = IDD_LPM_Handler;
            break;
            
        case IDD_PWDINPUT:
            pMe->m_pDialogHandler = IDD_PWDINPUT_Handler;
            break;
        #ifdef FATRUE_LOCK_IMSI_MCCMNC
	  case IDD_PWDIMSIMCC:
            pMe->m_pDialogHandler = IDD_PWDIMSIMCC_Handler;
            break;
        #endif     	
        case IDD_UIMSECCODE:
            pMe->m_pDialogHandler = IDD_UIMSECCODE_Handler;
            break;
            
        case IDD_UIMERR:
            pMe->m_pDialogHandler = IDD_UIMERR_Handler;
            break;
            
        case IDD_MSGBOX:
            pMe->m_pDialogHandler = IDD_MSGBOX_Handler;
            break;
            
        case IDD_EMERGENCYNUMLIST:
            pMe->m_pDialogHandler = IDD_EMERGENCYNUMLIST_Handler;
            break;
            
        case IDD_STARTUPANI:
            pMe->m_pDialogHandler = IDD_STARTUPANI_Handler;
            break;
            
        case IDD_LOADING:
            pMe->m_pDialogHandler = IDD_LOADING_Handler;
            break;
            
        case IDD_IDLE:
            pMe->m_pDialogHandler = IDD_IDLE_Handler;
            break;

#if defined(FEATURE_WMS_APP)
        case IDD_WMSTIPS:
            pMe->m_pDialogHandler = IDD_WMSTIPS_Handler;
            break;
#endif            
        
        case IDD_POWERDOWN:
            pMe->m_pDialogHandler = IDD_POWERDOWN_Handler;
            break;
            
#ifdef FEATURE_UTK2
        case IDD_UTKREFRESH:
            pMe->m_pDialogHandler = IDD_UTKREFRESH_Handler;
            break;            
#endif //FEATURE_UTK2

        default:
            pMe->m_pDialogHandler = NULL;
            break;
    }
}

/*==============================================================================
函数:
    CoreApp_RouteDialogEvent
       
说明:
    函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
    pMe [In]: 指向 Core Applet对象结构的指针,该结构包含小程序的特定信息.
    eCode [in]:事件代码。
    wParam [in]:与事件eCode关联的数据。
    dwParam [in]:与事件eCode关联的数据。

返回值:
    TRUE:传入事件被处理。
    FALSE:传入事件没被处理。

备注:

==============================================================================*/
boolean CoreApp_RouteDialogEvent(CCoreApp *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam)
{
    if (NULL == pMe->m_pDialogHandler)
    {
        return FALSE;
    }
    
    return pMe->m_pDialogHandler((void *)pMe, eCode, wParam, dwParam);
}

/*==============================================================================
函数:
    DialogTimeoutCallback
    
说明:
    定时回调函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。

       
返回值:
    none

       
备注:
    函数用来向 Core Applet 发送当前对话框提示超时的事件。
       
==============================================================================*/
static void DialogTimeoutCallback(void *pUser)
{
    CCoreApp *pMe = (CCoreApp*)pUser;

    if (NULL == pMe)
    {
        return;
    }
    
    if (ISHELL_ActiveApplet(pMe->a.m_pIShell) != AEECLSID_CORE_APP)
    {
        return;
    }

    // Post a message to the dialog telling it to close
    (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                            AEECLSID_CORE_APP,
                            EVT_DISPLAYDIALOGTIMEOUT,
                            0,
                            0);
}


/*==============================================================================
函数:
    IDD_MSGBOX_Handler
    
说明:
    IDD_MSGBOX 对话框事件处理函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。
    eCode [in]: 事件代码。
    wParam: 事件相关数据。
    dwParam: 事件相关数据。
       
返回值:
    TRUE:  传入事件被处理。
    FALSE: 传入事件被忽略。
       
备注:
    函数显示提示消息给用户，并自动关闭返回。
       
==============================================================================*/
static boolean  IDD_MSGBOX_Handler(void       *pUser,
                                   AEEEvent   eCode,
                                   uint16     wParam,
                                   uint32     dwParam)
{
    CCoreApp *pMe = (CCoreApp *)pUser;
    static IStatic * pStatic = NULL;
    CORE_ERR("%x %x %x IDD_MSGBOX_Handler",eCode,wParam,dwParam);
    if (NULL == pMe)
    {
        return FALSE;
    }

    switch(eCode)
    {
        case EVT_DIALOG_INIT:
        {
            if (NULL == pStatic)
            {
                AEERect rect = {0};
                if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_STATIC,(void **)&pStatic))
                {
                    return FALSE;
                }
                ISTATIC_SetRect(pStatic, &rect);
            }
            return TRUE;
        }

        case EVT_DIALOG_START:
        {
            uint32 set_time= 1500;
            switch (pMe->m_nMsgID)
            {
                case IDS_SERVICEREQ:
                case IDS_EXIT_EMERGENCY_MODE:
                    set_time = 0;
                    break;

#ifdef FEATRUE_AUTO_POWER
                case IDS_AUTO_POWER_OFF:
                    //in this state,we need to disable the key guard. 
#ifdef FEATURE_KEYGUARD
                    if(OEMKeyguard_IsEnabled())
                    {
                        OEMKeyguard_SetState(FALSE);
                    }
#endif
                    IBACKLIGHT_Enable(pMe->m_pBacklight);
                    set_time = 20000;
                    break;
#endif /* FEATRUE_AUTO_POWER */

#ifdef FEATURE_PLANEMODE
                case IDS_PLANEMODE_QUERY:
                    set_time = 5000;
                    break;
#endif
                case 0xFFFF:
                    set_time = 3000;
                    break;
                    
                default:
                    break;
            }

            if (set_time != 0)
            {
                // 启动发送关闭对话框事件的定时器
                ISHELL_SetTimer(pMe->a.m_pIShell, set_time,DialogTimeoutCallback,pMe);
            }
            ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_CORE_APP,EVT_USER_REDRAW,  0, 0);
            return TRUE;
        }

        case EVT_USER_REDRAW:
        {
            if(pMe->m_nMsgID !=IDS_EXIT_EMERGENCY_MODE)
            {
                PromptMsg_Param_type m_PromptMsg;
                MEMSET(&m_PromptMsg,0,sizeof(PromptMsg_Param_type));
                m_PromptMsg.nMsgResID= pMe->m_nMsgID;
                if(pMe->m_nMsgID == IDS_AUTO_POWER_OFF)
                {
                    m_PromptMsg.ePMsgType = MESSAGE_CONFIRM;
                }
                else if(pMe->m_nMsgID == 0xFFFF)
                {
                    m_PromptMsg.nMsgResID = 0;
                    m_PromptMsg.ePMsgType = MESSAGE_WARNNING;
                    m_PromptMsg.pwszMsg = pMe->m_cdg_msgptr;
                }
                else
                {
                    m_PromptMsg.ePMsgType = MESSAGE_WARNNING;
                }

                if(pMe->m_nMsgID != 0xFFFF)
                {
                    STRLCPY(m_PromptMsg.strMsgResFile, AEE_COREAPPRES_LANGFILE,MAX_FILE_NAME);
                    if((pMe->m_nMsgID == IDS_FULLY_CHARGED) || (pMe->m_nMsgID == IDS_CHARGER_ON))
                    {
                        m_PromptMsg.eBBarType = BTBAR_NONE;
                    }
                    else
                    {
                        m_PromptMsg.eBBarType = BTBAR_OK_BACK;
                    }
                }
                DrawPromptMessage(pMe->m_pDisplay,pStatic,&m_PromptMsg);
            }
            else
            {
                AECHAR  *wstrText = NULL;
                int32  nSize=0;
                nSize = 128 * sizeof(AECHAR);
                wstrText = (AECHAR *) MALLOC(nSize);
                if (NULL != wstrText)
                {
                    ISHELL_LoadResString(pMe->a.m_pIShell, AEE_COREAPPRES_LANGFILE,  pMe->m_nMsgID,wstrText,nSize);
                    ISTATIC_SetProperties(pStatic, ST_CENTERTEXT | ST_MIDDLETEXT);
                    ISTATIC_SetText(pStatic,NULL,wstrText,AEE_FONT_NORMAL,AEE_FONT_NORMAL);
                    FREE(wstrText);
                }                
                ISTATIC_Redraw(pStatic);
                COREAPP_DRAW_BOTTOMBAR(BTBAR_OK_BACK)
            }
            IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
            return TRUE;            
        }

        case EVT_DIALOG_END:
        {
            ISHELL_CancelTimer(pMe->a.m_pIShell, DialogTimeoutCallback,pMe);
            ISTATIC_Release(pStatic);
            pStatic = NULL;
#ifdef FEATURE_PLANEMODE
            if(IDS_PLANEMODE_QUERY == pMe->m_nMsgID)
            {
                FREEIF(pMe->m_cdg_msgptr);
                return TRUE;
            }
#endif
            // 此对话框返回值仅为 DLGRET_MSGOK ，为防止挂起 Applet 
            // 关闭对话框回到错误状态，显示给对话框返回值赋值
#ifdef FEATRUE_AUTO_POWER
            pMe->m_b_autopower_off = FALSE;
            /*if(pMe->m_nMsgID == IDS_AUTO_POWER_OFF)
            {
                pMe->m_eDlgRet = DLGRET_CREATE;
            }
            else
            {
                pMe->m_eDlgRet = DLGRET_MSGOK;
            }*/
            pMe->m_eDlgRet = DLGRET_MSGOK;

            if(pMe->m_b_needclose_core)
            {
                ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
            }
#else
            pMe->m_eDlgRet = DLGRET_MSGOK;
#endif //#ifdef FEATRUE_AUTO_POWER
            FREEIF(pMe->m_cdg_msgptr);
            return TRUE;
        }

        case EVT_KEY:
        {
            switch (wParam)
            {
                case AVK_SELECT:
                {
                    if(pMe->m_nMsgID ==IDS_EXIT_EMERGENCY_MODE)
                    {
                        //exit the emergency mode
                        ICM_SetSystemPreference(pMe->m_pCM,
                                                        AEECM_MODE_PREF_PERSISTENT, AEECM_PREF_TERM_PERMANENT, 0,
                                                        AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE, AEECM_BAND_PREF_NO_CHANGE,
                                                        AEECM_ROAM_PREF_NO_CHANGE, AEECM_HYBR_PREF_NO_CHANGE,
                                                        AEECM_SRV_DOMAIN_PREF_NO_CHANGE, AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE,
                                                        NULL, NULL, NULL);
                        pMe->m_bemergencymode = FALSE;
#ifdef FEATURE_SID_LOCK
                        {
                            nv_item_type nvi;
                            nvi.enabled_sid_lock.nam = cm_get_curr_nam();
                            if(NV_DONE_S == OEMNV_Get(NV_SID_LOCK_I,&nvi))
                            {
                                db_items_value_type sid_lock;
                                db_get(DB_SID_LOCK, &sid_lock);
                                if((TRUE == sid_lock.b_sid_lock) && (FALSE == nvi.enabled_sid_lock.b_sid_lock))
                                {
                                    sid_lock.b_sid_lock = FALSE;
                                    db_put(DB_SID_LOCK, &sid_lock);
                                }
                            }
                        }
#endif
                    }
#ifdef FEATRUE_AUTO_POWER
                    else if(pMe->m_nMsgID == IDS_AUTO_POWER_OFF)
                    {
                        ISHELL_CancelTimer(pMe->a.m_pIShell,DialogTimeoutCallback,pMe);                        
                        pMe->m_ePowerDownType = POWERDOWN_NORMAL;
                        if (pMe->m_eCurState != COREST_POWEROFF)
                        {
                            //ISHELL_CloseApplet(pMe->a.m_pIShell, TRUE);
                            pMe->m_b_needclose_core = FALSE;
                            pMe->m_wStartupAniTime = 0;
                            MOVE_TO_STATE(COREST_POWEROFF)
                            CLOSE_DIALOG(DLGRET_CREATE)
                            return TRUE;
                        }
                    }
#endif//#ifdef FEATRUE_AUTO_POWER
#ifdef FEATURE_PLANEMODE
                    else if(pMe->m_nMsgID == IDS_PLANEMODE_QUERY)
                    {
                        ISHELL_CancelTimer(pMe->a.m_pIShell,DialogTimeoutCallback,pMe);
                        pMe->m_SYS_MODE_NO_SRV = TRUE;  // 这里付值可以在idle界面骗过用户，planemode可以马上显示出来
                        CLOSE_DIALOG(DLGRET_YES)
                        return TRUE;
                    }
#endif

                }
                case AVK_CLR:    
                    (void) ISHELL_CancelTimer(pMe->a.m_pIShell,DialogTimeoutCallback,pMe);
#ifdef FEATURE_PLANEMODE
                    if(pMe->m_nMsgID == IDS_PLANEMODE_QUERY)
                    {
                        CLOSE_DIALOG(DLGRET_NO);
                    }
                    else
#endif
                    {
                        CLOSE_DIALOG(DLGRET_MSGOK);
                    }
                    break;

                default:
                    break;
            }
            return TRUE;
        }

        case EVT_DISPLAYDIALOGTIMEOUT:
        {
#ifdef FEATRUE_AUTO_POWER
            if(pMe->m_nMsgID == IDS_AUTO_POWER_OFF)
            {
                pMe->m_ePowerDownType = POWERDOWN_NORMAL;
                if (pMe->m_eCurState != COREST_POWEROFF)
                {
                    //ISHELL_CloseApplet(pMe->a.m_pIShell, TRUE);
                    pMe->m_b_needclose_core = FALSE;
                    pMe->m_wStartupAniTime = 0;
                    MOVE_TO_STATE(COREST_POWEROFF)
                    CLOSE_DIALOG(DLGRET_CREATE)
                    return TRUE;
                }
            }
#endif//#ifdef FEATRUE_AUTO_POWER
#ifdef FEATURE_PLANEMODE
            if(pMe->m_nMsgID == IDS_PLANEMODE_QUERY)
            {
                CLOSE_DIALOG(DLGRET_NO);
                return TRUE;
            }
#endif
            CLOSE_DIALOG(DLGRET_MSGOK);
            return TRUE;
        }

        default:
            break;
    }
    return FALSE;
}
/*==============================================================================
函数:
    IDD_ALARM_Handler
    
说明:
    IDD_ALARM 对话框事件处理函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。
    eCode [in]: 事件代码。
    wParam: 事件相关数据。
    dwParam: 事件相关数据。
       
返回值:
    TRUE:  传入事件被处理。
    FALSE: 传入事件被忽略。
==============================================================================*/
#if defined( FEATURE_POWERDOWN_ALARM)
static void powerdown( CCoreApp* pMe)
{
    pMe->m_ePowerDownType = POWERDOWN_NORMAL;
    if (pMe->m_eCurState != COREST_POWEROFF)
    {
        pMe->m_wStartupAniTime = 0;
        MOVE_TO_STATE(COREST_POWEROFF)
        CLOSE_DIALOG(DLGRET_CREATE)
    }
}
static void defaultProcessAlarm( void* pUser)
{
#ifndef WIN32
    extern void snoozePowerdownAlarmclock( void);
    snoozePowerdownAlarmclock();
#endif
    powerdown( (CCoreApp *)pUser);
}

static boolean  IDD_ALARM_Handler(void       *pUser,
                                AEEEvent   eCode,
                                uint16     wParam,
                                uint32     dwParam)
{
    //static IImage * pIImage = NULL;
    static byte keyBeepVolumeSetting = 0;
    static PowerDown_Alarm_Cfg time = {0};

    CCoreApp *pMe = (CCoreApp *)pUser;
    CORE_ERR("%x %x %x IDD_ALARM_Handler",eCode,wParam,dwParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            OEM_GetConfig( CFGI_POWERDOWN_ALARM, (void *)&time, sizeof(time));

            ISHELL_SetTimer( pMe->a.m_pIShell, 60000, defaultProcessAlarm, pMe);
            return TRUE;

        case EVT_DIALOG_START:
        {
            byte mute = OEMSOUND_MUTE_VOL;
            ISHELL_PostEvent(pMe->a.m_pIShell,AEECLSID_CORE_APP,EVT_USER_REDRAW,0,0);
            ICONFIG_GetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &keyBeepVolumeSetting, sizeof(byte));
            ICONFIG_SetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &mute, sizeof(byte));
            return TRUE;
        }

        case EVT_USER_REDRAW:
        {
            uint32  dwAlarmTime;
            AECHAR  wszTime[16];
            IImage* pResImg = NULL;
            uint16  ring_id = 1;
            byte    profilenum;
            ringID  ringid[PROFILENUMBER];
            AEERect rc = {0};
            RGBVAL nOldFontColor;
            
            MEMSET(wszTime,0,sizeof(wszTime));
            dwAlarmTime = time.dwWATime;
            Appscommon_FormatTimeString(dwAlarmTime, wszTime, sizeof(wszTime));
            pResImg = ISHELL_LoadResImage( pMe->a.m_pIShell, "fs:/mod/clockapps/clockapps_images.bar", IDI_ALARMCLOCK);
            CORE_ERR("ALARM %x %S",pResImg,wszTime);
            Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
            if( pResImg != NULL)
            {
                AEEImageInfo ImageInfo;
                
                IIMAGE_GetInfo(pResImg, &ImageInfo);
                IIMAGE_Draw(pResImg, (pMe->m_rc.dx - ImageInfo.cx)/2, 2*TITLEBAR_HEIGHT + (pMe->m_rc.dy - BOTTOMBAR_HEIGHT - TITLEBAR_HEIGHT- ImageInfo.cy)/2);
                IIMAGE_Release( pResImg);
                pResImg = NULL;
            }

            SETAEERECT(&rc, 0, TITLEBAR_HEIGHT, pMe->m_rc.dx, TITLEBAR_HEIGHT);
            nOldFontColor = IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
            (void)IDISPLAY_DrawText(pMe->m_pDisplay, AEE_FONT_BOLD, wszTime, -1, 0, 0, &rc,
                                  IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_TEXT_TRANSPARENT);
            (void)IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, nOldFontColor);
#if 1
            OEM_GetConfig( CFGI_ALARM_RINGER, &ring_id, sizeof(ring_id));

            ICONFIG_GetItem(pMe->m_pConfig, CFGI_PROFILE_CUR_NUMBER, &profilenum, sizeof(profilenum));
            ICONFIG_GetItem(pMe->m_pConfig, CFGI_PROFILE_ALARM_RINGER, (void*)ringid, sizeof(ringid));
            if(ringid[profilenum].ringType == OEMNV_MID_RINGER)
            {
                IALERT_StartRingerAlert(pMe->m_pAlert, ring_id);
            }
            else
            {
                if ((IALERT_StartMp3Alert(pMe->m_pAlert, ringid[profilenum].szMusicname, ALERT_NORMAL_SND) != SUCCESS))
                {
                    IALERT_StartRingerAlert(pMe->m_pAlert, (uint32)ring_id);
                }
            }
#else
            ICONFIG_GetItem( pMe->m_pConfig,CFGI_ALARM_RINGER,&ring_id,sizeof(ring_id));
            IALERT_StartRingerAlert(pMe->m_pAlert, ring_id);
#endif
            {
                TitleBar_Param_type title = {0};
                AECHAR wszTitle[16] = {0};
                int len = 0;

                ISHELL_LoadResString( pMe->a.m_pIShell,
                                      CLOCK_RES_PATH,
                                      IDS_ALARMCLOCK_TITLE,
                                      wszTitle,
                                      sizeof(wszTitle)
                                  );
                len = WSTRLEN( wszTitle);
                WSPRINTF( wszTitle + len, sizeof( wszTitle) - len*2, L"%d", time.alarm_id + 1);
                MEMSET(&title,0,sizeof(TitleBar_Param_type));
                //title.dwAlignFlags = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER;
                title.dwAlignFlags = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
                title.pwszTitle = wszTitle;
                DrawTitleBar(pMe->a.m_pIDisplay,&title);
                CoreDrawBottomBar( BTBAR_SNOOZE_CONTINUE_STOP);
            }
            IDISPLAY_Update( pMe->a.m_pIDisplay);
            return TRUE;
        }

        case EVT_DIALOG_END:
            IBACKLIGHT_Enable(pMe->m_pBacklight);
#ifdef FEATURE_LED_CONTROL
            IBACKLIGHT_SigLedDisable( pMe->m_pBacklight);
#endif
#if !defined( AEE_SIMULATOR)
            IALERT_StopRingerAlert(pMe->m_pAlert);
            IALERT_StopMp3Alert(pMe->m_pAlert);
#endif
            ICONFIG_SetItem( pMe->m_pConfig, CFGI_BEEP_VOL, &keyBeepVolumeSetting, sizeof(byte));
			ISHELL_CancelTimer( pMe->a.m_pIShell, defaultProcessAlarm, pMe);
            return TRUE;

        case EVT_KEY:
        {
            if(wParam == AVK_INFO)
            {
#if !defined( AEE_SIMULATOR)
                IALERT_StopRingerAlert(pMe->m_pAlert);
#endif

                CLOSE_DIALOG(DLGRET_OK)
            }
            else if( wParam == AVK_SELECT)
            {
                defaultProcessAlarm( pMe);
            }
            else if( wParam == AVK_CLR)
            {
                powerdown( pMe);
            }
        }
        return TRUE;


        case EVT_COMMAND:
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // IDD_ALARM_Handler

#endif

/*==============================================================================
函数:
    IDD_LPM_Handler
    
说明:
    IDD_LPM 对话框事件处理函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。
    eCode [in]: 事件代码。
    wParam: 事件相关数据。
    dwParam: 事件相关数据。
       
返回值:
    TRUE:  传入事件被处理。
    FALSE: 传入事件被忽略。
       
备注:
    函数作卡的 PIN 码、 PUK 码的输入界面处理。
       
==============================================================================*/
static boolean  IDD_LPM_Handler(void       *pUser,
                                AEEEvent   eCode,
                                uint16     wParam,
                                uint32     dwParam)
{
    CCoreApp *pMe = (CCoreApp *)pUser;
    CORE_ERR("%x %x %x IDD_LPM_Handler",eCode,wParam,dwParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            pMe->m_battery_count = 0;
            pMe->m_battery_Image = ISHELL_LoadImage(pMe->a.m_pIShell, CHARGING_ANIFILE "charge.png");
            IIMAGE_SetParm(pMe->m_battery_Image, IPARM_NFRAMES, CHARGING_FRAME_COUNT, 0);
            /*pMe->m_battery_Image[0] = ISHELL_LoadImage(pMe->a.m_pIShell, CHARGING_ANIFILE "charge011.png");
            pMe->m_battery_Image[1] = ISHELL_LoadImage(pMe->a.m_pIShell, CHARGING_ANIFILE "charge021.png");
            pMe->m_battery_Image[2] = ISHELL_LoadImage(pMe->a.m_pIShell, CHARGING_ANIFILE "charge031.png");
            pMe->m_battery_Image[3] = ISHELL_LoadImage(pMe->a.m_pIShell, CHARGING_ANIFILE "charge041.png");
            pMe->m_battery_Image[4] = ISHELL_LoadImage(pMe->a.m_pIShell, CHARGING_ANIFILE "charge051.png");*/
            //CORE_ERR("%x %x %x ",pMe->m_battery_Image[0],pMe->m_battery_Image[1],pMe->m_battery_Image[2]);
            return TRUE;
            
        case EVT_DIALOG_START:
            //IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,FALSE);
            ISHELL_PostEvent(pMe->a.m_pIShell,AEECLSID_CORE_APP,EVT_USER_REDRAW,0,0);
            return TRUE;
            
        case EVT_USER_REDRAW:
        case EVT_UPDATEIDLE:
        {
            AEEChargerStatus status;
            AECHAR  *wszText=NULL;
            int32   nSize;

            nSize = 32 * sizeof(AECHAR);
            wszText =  (AECHAR *) MALLOC(nSize);
            wszText[0] = 0;
            status = IBATT_GetChargerStatus(pMe->m_pBatt);
            CORE_ERR("ChargerStatus %d ",status);
            IDISPLAY_ClearScreen(pMe->m_pDisplay);

            if (AEECHG_STATUS_CHARGING == status) 
            {
                CoreApp_Draw_Charger_image(pMe);
            } 
            else if (AEECHG_STATUS_FULLY_CHARGE == status)
            {
                ISHELL_CancelTimer(pMe->a.m_pIShell, CoreApp_Draw_Charger_image, pMe);
                if (NULL != pMe->m_battery_Image)
                {
                    IIMAGE_DrawFrame(pMe->m_battery_Image, CHARGING_FRAME_COUNT - 1, 0, 0);
                }
                /*if (NULL != pMe->m_battery_Image[4] )
                {
                    IIMAGE_Draw(pMe->m_battery_Image[4] , 0, 0);
                }*/
            } 
            else if (AEECHG_STATUS_OVERVOLTAGE == status) 
            {
                ISHELL_CancelTimer(pMe->a.m_pIShell, CoreApp_Draw_Charger_image, pMe);
                if (NULL != wszText)
                {
                    (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                                AEE_COREAPPRES_LANGFILE,
                                                IDS_OVERVOLTAGE,
                                                wszText,
                                                nSize);
                }
            }

            // 绘制提示文本
            if (NULL != wszText && wszText[0] != 0)
            {
                (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                    AEE_FONT_BOLD,
                    wszText, -1,
                    0, 0, &pMe->m_rc, 
                    IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_TOP);
                
                //FREE ((void*) wszText);
            }
            FREEIF ((void*) wszText);
            IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
            return TRUE;
        }
            
            
        case EVT_DIALOG_END:
        {
            //int i = 0;
            ISHELL_CancelTimer(pMe->a.m_pIShell, CoreApp_Draw_Charger_image, pMe);
            if (NULL != pMe->m_battery_Image)
            {
                IIMAGE_Release(pMe->m_battery_Image);
                pMe->m_battery_Image = NULL;
            }
            //IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,TRUE);
            /*for (i = 0;i <5; i++)
            {
                if (NULL != pMe->m_battery_Image[i])
                {
                    IIMAGE_Release(pMe->m_battery_Image[i]);
                    pMe->m_battery_Image[i] = NULL;
                }
            }*/
            return TRUE;
        }
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_END:
#ifndef CUST_EDITION                    
                    if(!g_b_no_batt)
                    {
                        CLOSE_DIALOG(DLGRET_OK)
                    }
#endif                    
                    return TRUE;
                    
                default:
                    break;
            }
            
            return TRUE;
            
        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
            return TRUE;
            
        default:
            break;
    }
    
    return FALSE;
} // IDD_LPM_Handler

/*==============================================================================
函数:
    IDD_EMERGENCYNUMLIST_Handler
    
说明:
    IDD_EMERGENCYNUMLIST 对话框事件处理函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。
    eCode [in]：事件代码。
    wParam：事件相关数据。
    dwParam：事件相关数据。
       
返回值:
    TRUE: 传入事件被处理。
    FALSE: 传入事件被忽略。
       
备注:
       
==============================================================================*/
static boolean  IDD_EMERGENCYNUMLIST_Handler(void  *pUser,
                                 AEEEvent   eCode,
                                 uint16     wParam,
                                 uint32     dwParam)
{
    CCoreApp *pMe = (CCoreApp *)pUser;
    IMenuCtl *pMenu = NULL;
  
    PARAM_NOT_REF(dwParam)
    CORE_ERR("%x %x %x IDD_EMERGENCYNUMLIST_Handler",eCode,wParam,dwParam);
    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_EMERGENCYNUM);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:            
            {
                //AEERect rc;
                AECHAR  wstrNum[MAX_EMERGENCY_NUM_LEN+1];
                EmergencyNum_Table emerg_tab;
                //EmergencyNumber pList[3];
                //EmergencyNumber pTepItem ;
                //EmergencyNumber pItem;
                int   i = 0;
                ICONFIG_GetItem(pMe->m_pConfig, CFGI_EMERGENCYNUM_TABLE, (void*)&emerg_tab, sizeof(EmergencyNum_Table));
                
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
                IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SECURITY_BACKGROUND); //added by chengxiao 2009.03.20
#endif
                for(i=0; i<emerg_tab.emert_size; i++)
                {
                    wstrNum[0] = 0;
                    STRTOWSTR(emerg_tab.emerg_num[i].num_buf, wstrNum, sizeof(wstrNum));
                    IMENUCTL_AddItem(pMenu, NULL, NULL, 500+i, wstrNum, NULL);
                }
                //SETAEERECT(&rc,  0, 0, pMe->m_rc.dx, pMe->m_rc.dy - (BOTTOMBAR_HEIGHT + IDLE_ICON_HEIGHT));
                //IMENUCTL_SetRect(pMenu, &rc);

                //pList = AEEOEM_GetEmergencyNumList(pMe->m_pOEM_TSGBridge);
                
                //pList[0].num_buf = "10010";
                //pList[0].num_len = 5;

                //pList[1].num_buf = "10011";
                //pList[1].num_len = 5;

                //pList[2].num_buf = "10012";
                //pList[2].num_len = 5;

                //for (j=0; j<3; j++)
                //{
                //    pTepItem = pList[j];
                //    pItem = pTepItem;

                 //   if ((pItem.num_len == 0) || (pItem.num_buf == NULL))
                 //   {
                 //       break;
                 //   }
                           
                 //   wstrNum[0] = 0;
                 //   (void)STRTOWSTR(pItem.num_buf, wstrNum, sizeof(wstrNum));
                 //   (void)IMENUCTL_AddItem(pMenu, NULL, NULL, 500+i, wstrNum, NULL);
                 //   i++;                   
                //}                 
            }
            ISHELL_PostEvent(pMe->a.m_pIShell,AEECLSID_CORE_APP,EVT_USER_REDRAW, 0,0);
            return TRUE;

        case EVT_USER_REDRAW:
            // 绘制底条提示
            // Select       Back
            //CoreDrawBottomBar(BTBAR_SELECT_BACK)
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            (void)IMENUCTL_Redraw(pMenu);
            return TRUE;

        case EVT_DIALOG_END:
            if (pMe->m_eDlgRet != DLGRET_CANCELED)
            {
                pMe->m_eDlgRet = DLGRET_CANCELED;
            }
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_END:
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                case AVK_SEND:
                    {
                        CtlAddItem ai;
                        uint16   wID;
                        
                        wID = IMENUCTL_GetSel(pMenu);
                        if (IMENUCTL_GetItem(pMenu, wID, &ai))
                        {
                           (void)MakeVoiceCall(pMe->a.m_pIShell, FALSE, (AECHAR *)ai.pText);
                        }
                    }
                    return TRUE;
                    
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {
                CtlAddItem ai;
                
                if (IMENUCTL_GetItem(pMenu, wParam, &ai))
                {
                     (void)MakeVoiceCall(pMe->a.m_pIShell, FALSE, (AECHAR *)ai.pText);
                }
            }
            return TRUE;

        default:
            break;
    }

    return FALSE;
} // IDD_EMERGENCYNUMLIST_Handler

   #ifdef FATRUE_LOCK_IMSI_MCCMNC
/*==============================================================================
函数:
    IDD_PWDINPUT_Handler
    
说明:
    IDD_PWDINPUT 对话框事件处理函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。
    eCode [in]: 事件代码。
    wParam: 事件相关数据。
    dwParam: 事件相关数据。
       
返回值:
    TRUE:  传入事件被处理。
    FALSE: 传入事件被忽略。
       
备注:
    函数作卡的 PIN 码、 PUK 码的输入界面处理。
       
==============================================================================*/
static boolean  IDD_PWDIMSIMCC_Handler(void       *pUser,
                                     AEEEvent   eCode,
                                     uint16     wParam,
                                     uint32     dwParam)
{
    //IMenuCtl *pMenu = NULL;
    CCoreApp *pMe = (CCoreApp *)pUser;
    
    if (NULL == pMe)
    {
        return FALSE;
    }
    CORE_ERR("%x %x %x IDD_PWDINPUT_Handler",eCode,wParam,dwParam);
    //pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_PHONEPWD_MENU);
    //if (NULL == pMenu)
    //{
    //    return FALSE;
    //}
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            MEMSET(pMe->m_strPhonePWD, 0, sizeof(pMe->m_strPhonePWD));
            return TRUE;
            
        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                                    AEECLSID_CORE_APP,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            //if (NULL != pMenu)
            //{
            //     (void)IMENUCTL_SetTitle(pMenu, AEE_COREAPPRES_LANGFILE, IDS_ENTER_PHONELOCK, NULL);
            //}

            return TRUE;
            
        case EVT_USER_REDRAW:
            // 绘制相关信息
            {
                AECHAR  wstrDisplay[PHONEPASSWORDLENTH+1] = {0};
                char    strDisplay[PHONEPASSWORDLENTH+1] = {0};
                int xOffset = 5, nLen = 0;
                AECHAR  text[32] = {0};
                RGBVAL nOldFontColor;
                TitleBar_Param_type  TitleBar_Param = {0};
                
                // 先清屏
#ifdef FEATURE_CARRIER_CHINA_VERTU
                {
                    IImage *pImageBg = ISHELL_LoadResImage(pMe->a.m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SECURITY_BACKGROUND);
                    
                    Appscommon_ResetBackground(pMe->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &pMe->m_rc, 0, 0);
                    if(pImageBg != NULL)
                    {
                        IImage_Release(pImageBg);
                    }
                }
#else
                Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
#endif
                //IDISPLAY_FillRect  (pMe->m_pDisplay,&pMe->m_rc, RGB_BLACK);
                
                // 画标题条
                (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                                                            AEE_COREAPPRES_LANGFILE,
                                                            IDS_MCC_LOCK, 
                                                            text,
                                                            sizeof(text));
                TitleBar_Param.pwszTitle = text;
                TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
                DrawTitleBar(pMe->m_pDisplay, &TitleBar_Param);
                
                (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                                                AEE_COREAPPRES_LANGFILE,
                                                IDS_ENTER_PASSWORD, 
                                                text,
                                                sizeof(text));
                nOldFontColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                    AEE_FONT_BOLD, 
                                    text,
                                    -1, 
                                    xOffset, 
                                    TITLEBAR_HEIGHT + MENUITEM_HEIGHT*1/2,
                                    NULL, 
                                    IDF_TEXT_TRANSPARENT);
                
                // 绘制输入
                nLen = STRLEN(pMe->m_strPhonePWD);
                MEMSET(strDisplay, '*', nLen);
                strDisplay[nLen] = '|';
                strDisplay[nLen + 1] = '\0';
                (void) STRTOWSTR(strDisplay, wstrDisplay, sizeof(wstrDisplay));
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                AEE_FONT_BOLD, 
                                wstrDisplay,
                                -1, 
                                2*xOffset, 
                                TITLEBAR_HEIGHT + MENUITEM_HEIGHT*3/2,
                                NULL, 
                                IDF_TEXT_TRANSPARENT);
                (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, nOldFontColor);
                
                // 绘制底条提示
                if (nLen > 3)
                {// 确定-----删除
                    CoreDrawBottomBar(BTBAR_OK_DELETE)
                }
                else if(nLen > 0)
                {// 删除
                    CoreDrawBottomBar(BTBAR_DELETE)
                }
                else
                {// SOS
                    CoreDrawBottomBar(BTBAR_SOS)
                }
            }
            // 更新显示
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);  
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            {
                char  chEnter = 0;
                int   nLen = 0;
                boolean bRedraw = FALSE;
                
                switch (wParam)
                {
                    case AVK_0:
                    case AVK_1:
                    case AVK_2:
                    case AVK_3:
                    case AVK_4:
                    case AVK_5:
                    case AVK_6:
                    case AVK_7:
                    case AVK_8:
                    case AVK_9:
                        chEnter = '0' + (wParam - AVK_0);
                        break;

                    case AVK_STAR:
                        chEnter = '*';
                        break;
 
                    case AVK_POUND:
                        chEnter = '#';
                        break;
                        
                    case AVK_SEND:
                        CLOSE_DIALOG(DLGRET_EMGCALL)
                        return TRUE;
                        
                    case AVK_CLR:
                        chEnter = 0;
                        if (STRLEN(pMe->m_strPhonePWD) == 0)
                        {
                            CLOSE_DIALOG(DLGRET_EMGCALL)
                            return TRUE;
                        }
                        break;
                        
                    case AVK_SELECT:
                        if (STRLEN(pMe->m_strPhonePWD) < 4)
                        {
                            return TRUE;
                        }
                        else
                        {
                            CLOSE_DIALOG(DLGRET_ENTEROK)
                        }
                        return TRUE;
                        
                    default:
                        return TRUE;
                }
                
                nLen = STRLEN(pMe->m_strPhonePWD);
                if (chEnter == 0)
                {// 删除字符
                    if (nLen > 0)
                    {
                        bRedraw = TRUE;
                        pMe->m_strPhonePWD[nLen-1] = chEnter;
                    }
                }
                else if (nLen < PHONEPASSWORDLENTH)
                {
                    pMe->m_strPhonePWD[nLen] = chEnter;
                    nLen++;
                    pMe->m_strPhonePWD[nLen] = 0;
                    bRedraw = TRUE;
                }
                
                if (bRedraw)
                {
                    (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                                            AEECLSID_CORE_APP,
                                            EVT_USER_REDRAW,
                                            NULL,
                                            NULL);
                }
            }
            return TRUE;
            
        default:
            break;
    }
    
    return FALSE;
} // IDD_PWDINPUT_Handler

#endif

/*==============================================================================
函数:
    IDD_PWDINPUT_Handler
    
说明:
    IDD_PWDINPUT 对话框事件处理函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。
    eCode [in]: 事件代码。
    wParam: 事件相关数据。
    dwParam: 事件相关数据。
       
返回值:
    TRUE:  传入事件被处理。
    FALSE: 传入事件被忽略。
       
备注:
    函数作卡的 PIN 码、 PUK 码的输入界面处理。
       
==============================================================================*/
static boolean  IDD_PWDINPUT_Handler(void       *pUser,
                                     AEEEvent   eCode,
                                     uint16     wParam,
                                     uint32     dwParam)
{
    //IMenuCtl *pMenu = NULL;
    CCoreApp *pMe = (CCoreApp *)pUser;
    
    if (NULL == pMe)
    {
        return FALSE;
    }
    CORE_ERR("%x %x %x IDD_PWDINPUT_Handler",eCode,wParam,dwParam);
    //pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_PHONEPWD_MENU);
    //if (NULL == pMenu)
    //{
    //    return FALSE;
    //}
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            MEMSET(pMe->m_strPhonePWD, 0, sizeof(pMe->m_strPhonePWD));
            return TRUE;
            
        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                                    AEECLSID_CORE_APP,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            //if (NULL != pMenu)
            //{
            //     (void)IMENUCTL_SetTitle(pMenu, AEE_COREAPPRES_LANGFILE, IDS_ENTER_PHONELOCK, NULL);
            //}

            return TRUE;
            
        case EVT_USER_REDRAW:
            // 绘制相关信息
            {
                AECHAR  wstrDisplay[PHONEPASSWORDLENTH+1] = {0};
                char    strDisplay[PHONEPASSWORDLENTH+1] = {0};
                int xOffset = 5, nLen = 0;
                AECHAR  text[32] = {0};
                RGBVAL nOldFontColor;
                TitleBar_Param_type  TitleBar_Param = {0};
                
                // 先清屏
#ifdef FEATURE_CARRIER_CHINA_VERTU
                {
                    IImage *pImageBg = ISHELL_LoadResImage(pMe->a.m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SECURITY_BACKGROUND);
                    
                    Appscommon_ResetBackground(pMe->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &pMe->m_rc, 0, 0);
                    if(pImageBg != NULL)
                    {
                        IImage_Release(pImageBg);
                    }
                }
#else
                Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
#endif
                //IDISPLAY_FillRect  (pMe->m_pDisplay,&pMe->m_rc, RGB_BLACK);
                
                // 画标题条
                (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                                                            AEE_COREAPPRES_LANGFILE,
                                                            IDS_ENTER_PHONELOCK, 
                                                            text,
                                                            sizeof(text));
                TitleBar_Param.pwszTitle = text;
                TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
                DrawTitleBar(pMe->m_pDisplay, &TitleBar_Param);
                
                (void)ISHELL_LoadResString(pMe->a.m_pIShell, 
                                                AEE_COREAPPRES_LANGFILE,
                                                IDS_ENTER_PASSWORD, 
                                                text,
                                                sizeof(text));
                nOldFontColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                    AEE_FONT_BOLD, 
                                    text,
                                    -1, 
                                    xOffset, 
                                    TITLEBAR_HEIGHT + MENUITEM_HEIGHT*1/2,
                                    NULL, 
                                    IDF_TEXT_TRANSPARENT);
                
                // 绘制输入
                nLen = STRLEN(pMe->m_strPhonePWD);
                MEMSET(strDisplay, '*', nLen);
                strDisplay[nLen] = '|';
                strDisplay[nLen + 1] = '\0';
                (void) STRTOWSTR(strDisplay, wstrDisplay, sizeof(wstrDisplay));
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                AEE_FONT_BOLD, 
                                wstrDisplay,
                                -1, 
                                2*xOffset, 
                                TITLEBAR_HEIGHT + MENUITEM_HEIGHT*3/2,
                                NULL, 
                                IDF_TEXT_TRANSPARENT);
                (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, nOldFontColor);
                
                // 绘制底条提示
                if (nLen > 3)
                {// 确定-----删除
                    CoreDrawBottomBar(BTBAR_OK_DELETE)
                }
                else if(nLen > 0)
                {// 删除
                    CoreDrawBottomBar(BTBAR_DELETE)
                }
                else
                {// SOS
                    CoreDrawBottomBar(BTBAR_SOS)
                }
            }
            // 更新显示
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);  
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            {
                char  chEnter = 0;
                int   nLen = 0;
                boolean bRedraw = FALSE;
                
                switch (wParam)
                {
                    case AVK_0:
                    case AVK_1:
                    case AVK_2:
                    case AVK_3:
                    case AVK_4:
                    case AVK_5:
                    case AVK_6:
                    case AVK_7:
                    case AVK_8:
                    case AVK_9:
                        chEnter = '0' + (wParam - AVK_0);
                        break;

                    case AVK_STAR:
                        chEnter = '*';
                        break;
 
                    case AVK_POUND:
                        chEnter = '#';
                        break;
                        
                    case AVK_SEND:
                        CLOSE_DIALOG(DLGRET_EMGCALL)
                        return TRUE;
                        
                    case AVK_CLR:
                        chEnter = 0;
                        if (STRLEN(pMe->m_strPhonePWD) == 0)
                        {
                            CLOSE_DIALOG(DLGRET_EMGCALL)
                            return TRUE;
                        }
                        break;
                        
                    case AVK_SELECT:
                        if (STRLEN(pMe->m_strPhonePWD) < 4)
                        {
                            return TRUE;
                        }
                        else
                        {
                            CLOSE_DIALOG(DLGRET_ENTEROK)
                        }
                        return TRUE;
                        
                    default:
                        return TRUE;
                }
                
                nLen = STRLEN(pMe->m_strPhonePWD);
                if (chEnter == 0)
                {// 删除字符
                    if (nLen > 0)
                    {
                        bRedraw = TRUE;
                        pMe->m_strPhonePWD[nLen-1] = chEnter;
                    }
                }
                else if (nLen < PHONEPASSWORDLENTH)
                {
                    pMe->m_strPhonePWD[nLen] = chEnter;
                    nLen++;
                    pMe->m_strPhonePWD[nLen] = 0;
                    bRedraw = TRUE;
                }
                
                if (bRedraw)
                {
                    (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                                            AEECLSID_CORE_APP,
                                            EVT_USER_REDRAW,
                                            NULL,
                                            NULL);
                }
            }
            return TRUE;
            
        default:
            break;
    }
    
    return FALSE;
} // IDD_PWDINPUT_Handler

/*==============================================================================
函数:
    IDD_UIMSECCODE_Handler
    
说明:
    IDD_UIMSECCODE 对话框事件处理函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。
    eCode [in]: 事件代码。
    wParam: 事件相关数据。
    dwParam: 事件相关数据。
       
返回值:
    TRUE:  传入事件被处理。
    FALSE: 传入事件被忽略。
       
备注:
    函数作卡的 PIN 码、 PUK 码的输入界面处理。
       
==============================================================================*/
static boolean  IDD_UIMSECCODE_Handler(void       *pUser,
                                       AEEEvent   eCode,
                                       uint16     wParam,
                                       uint32     dwParam)
{
    static char *szCode = NULL;
    static int nMaxLen = 0;
    static uint16 nResID = 0;
    //IMenuCtl *pMenu = NULL;
    CCoreApp *pMe = (CCoreApp *)pUser;
    
    if (NULL == pMe)
    {
        return FALSE;
    }
    CORE_ERR("%x %x %x IDD_UIMSECCODE_Handler",eCode,wParam,dwParam);
    //pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_UIMSECCODE_MENU);
    //if (NULL == pMenu)
    //{
    //    return FALSE;
    //}
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            szCode = NULL;
            nMaxLen = 0;
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            //uint16 nResID = 0;            
            // 初始化初始数据
            switch (pMe->m_eRUIMSCode)
            {
                case ENTERPIN_ONLY:
                    MEMSET(pMe->m_strPIN1, 0, sizeof(pMe->m_strPIN1));
                    szCode = pMe->m_strPIN1;
                    nMaxLen = PINCODE_LENGTH;
                    nResID = IDS_ENTER_PIN;
                    break;
                    
                case ENTERPUK_STEP1:
                    MEMSET(pMe->m_strPIN1, 0, sizeof(pMe->m_strPIN1));
                    szCode = pMe->m_strPIN1;
                    nMaxLen = PINCODE_LENGTH;
                    nResID = IDS_NEWPIN;
                    break;
                    
                case ENTERPUK_STEP2:
                    MEMSET(pMe->m_strPIN2, 0, sizeof(pMe->m_strPIN2));
                    szCode = pMe->m_strPIN2;
                    nMaxLen = PINCODE_LENGTH;
                    nResID = IDS_NEWPIN_AGAIN;
                    break;
                    
                case ENTERPUK_STEP0:
                    MEMSET(pMe->m_strPUK, 0, sizeof(pMe->m_strPUK));
                    szCode = pMe->m_strPUK;
                    nMaxLen = PUKCODE_LENGTH;
                    nResID = IDS_ENTER_PUK;
                    break;
                
                default:
                    szCode = NULL;
                    break;
            }           
            
            ISHELL_PostEvent(pMe->a.m_pIShell,AEECLSID_CORE_APP,EVT_USER_REDRAW, 0,0);            
            return TRUE;
        }
            
            
        case EVT_USER_REDRAW:
            if (NULL == szCode)
            {
                return TRUE;
            }
    
            // 绘制相关信息
            {
                AECHAR  wstrDisplay[12] = {0};
                AEECHVStatus  chvst;
                int xOffset = 5, nLen = 0;
                AECHAR  wstrCnt[32] = {0};
                char    strDisplay[12] = {0};
                AECHAR  text[32] = {0};
                RGBVAL nOldFontColor;
                TitleBar_Param_type  TitleBar_Param = {0};
                
                // 先清屏
#ifdef FEATURE_CARRIER_CHINA_VERTU
                {
                    IImage *pImageBg = ISHELL_LoadResImage(pMe->a.m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SECURITY_BACKGROUND);
                    
                    Appscommon_ResetBackground(pMe->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &pMe->m_rc, 0, 0);
                    if(pImageBg != NULL)
                    {
                        IImage_Release(pImageBg);
                    }
                }
#else
                Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
#endif
                //IDISPLAY_FillRect  (pMe->m_pDisplay,&pMe->m_rc,RGB_BLACK);  
                // 画标题条
                if (nResID != 0)
                {
                    ISHELL_LoadResString(pMe->a.m_pIShell, 
                                                        AEE_COREAPPRES_LANGFILE,
                                                        nResID,
                                                        text, 
                                                        sizeof(text));
                    TitleBar_Param.pwszTitle = text;
                    TitleBar_Param.dwAlignFlags = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
                    DrawTitleBar(pMe->m_pDisplay,&TitleBar_Param);
                }
                
                nOldFontColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
                if ((pMe->m_eRUIMSCode == ENTERPIN_ONLY) ||
                    (pMe->m_eRUIMSCode == ENTERPUK_STEP0))
                {// 绘制次数提示字符串
                    // 读取UIM卡的状态
                    (void)IRUIM_GetCHVStatus(pMe->m_pIRUIM, &chvst);
                    
                    (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                                AEE_COREAPPRES_LANGFILE,
                                                IDS_AVAILCNT,
                                                wstrCnt,
                                                sizeof(wstrCnt));
                                                
                    if (pMe->m_eRUIMSCode == ENTERPIN_ONLY)
                    {
                        WSPRINTF(text, sizeof(text),
                                 wstrCnt,  chvst.chv1CountRemain);
                    }
                    else
                    {
                        WSPRINTF(text, sizeof(text),
                                 wstrCnt,  chvst.chv1UnblockRemain);
                    }
                }
                else
                {
                    (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                                AEE_COREAPPRES_LANGFILE,
                                                IDS_ENTER_PASSWORD,
                                                text,
                                                sizeof(text));
                }
                    
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                                    AEE_FONT_BOLD, 
                                                    text,
                                                    -1, 
                                                    xOffset, 
                                                    TITLEBAR_HEIGHT + MENUITEM_HEIGHT*1/2, 
                                                    NULL, 
                                                    IDF_TEXT_TRANSPARENT);
                
                // 绘制输入
                nLen = STRLEN(szCode);
                MEMSET(strDisplay, '*', nLen);
                strDisplay[nLen] = '|';
                strDisplay[nLen + 1] = '\0';
                (void) STRTOWSTR(strDisplay, wstrDisplay, sizeof(wstrDisplay));
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                                AEE_FONT_BOLD, 
                                                wstrDisplay,
                                                -1, 
                                                2*xOffset,
                                                TITLEBAR_HEIGHT + MENUITEM_HEIGHT*3/2,
                                                NULL, 
                                                IDF_TEXT_TRANSPARENT);
                (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, nOldFontColor);

                
                // 绘制底条提示
                if (nLen > 3)
                {// 确定-----删除
                    CoreDrawBottomBar(BTBAR_OK_DELETE)
                }
                else if(nLen > 0)
                {// 删除
                    CoreDrawBottomBar(BTBAR_DELETE)
                }
                else
                {//SOS
                    CoreDrawBottomBar(BTBAR_SOS)
                }
            }
            
            // 更新显示
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);  
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            {
                char  chEnter = 0;
                int   nLen = 0;
                boolean bRedraw = FALSE;
                
                switch (wParam)
                {
                    case AVK_0:
                    case AVK_1:
                    case AVK_2:
                    case AVK_3:
                    case AVK_4:
                    case AVK_5:
                    case AVK_6:
                    case AVK_7:
                    case AVK_8:
                    case AVK_9:
                        chEnter = '0' + (wParam - AVK_0);
                        break;

                    case AVK_STAR:
                        chEnter = '*';
                        break;

                    case AVK_POUND:
                        chEnter = '#';
                        break;
                        
                    case AVK_SEND:
                        CLOSE_DIALOG(DLGRET_EMGCALL)
                        return TRUE;
                        
                    case AVK_CLR:
                        chEnter = 0;
                        if (szCode == NULL)
                        {
                            return TRUE;
                        }
                        if (STRLEN(szCode) == 0)
                        {
                            CLOSE_DIALOG(DLGRET_EMGCALL)
                            return TRUE;
                        }
                        break;
                        
                    case AVK_SELECT:
                        if (szCode == NULL)
                        {
                            return TRUE;
                        }
                        if (STRLEN(szCode) < 4)
                        {
                            return TRUE;
                        }
//                        else if ((nMaxLen == PINCODE_LENGTH) && (STRLEN(szCode) < nMaxLen))
//                        {
//                            return TRUE;
//                        }
                        else
                        {
                            CLOSE_DIALOG(DLGRET_OK)
                        }
                        return TRUE;
                        
                    default:
                        return TRUE;
                }
                
                if (szCode == NULL)
                {
                    return TRUE;
                }
                
                nLen = STRLEN(szCode);
                if (chEnter == 0)
                {// 删除字符
                    if (nLen > 0)
                    {
                        bRedraw = TRUE;
                        szCode[nLen-1] = chEnter;
                    }
                }
                else if (nLen < nMaxLen)
                {
                    szCode[nLen] = chEnter;
                    nLen++;
                    szCode[nLen] = 0;
                    bRedraw = TRUE;
                }
                
                if (bRedraw)
                {
                    (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                                            AEECLSID_CORE_APP,
                                            EVT_USER_REDRAW,
                                            NULL,
                                            NULL);
                }
            }
            return TRUE;
            
        default:
            break;
    }
    
    return FALSE;
} // IDD_UIMSECCODE_Handler

/*==============================================================================
函数:
    IDD_UIMERR_Handler
       
说明:
    IDD_UIMERR 对话框事件处理函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。
    eCode [in]: 事件代码。
    wParam: 事件相关数据。
    dwParam: 事件相关数据。
       
返回值:
    TRUE:  传入事件被处理。
    FALSE: 传入事件被忽略。
       
备注:
    函数处理 UIM 错误界面提示及相关事件。
       
==============================================================================*/
static boolean  IDD_UIMERR_Handler(void       *pUser,
                                   AEEEvent   eCode,
                                   uint16     wParam,
                                   uint32     dwParam)
{
    IStatic * pStatic = NULL;
    CCoreApp *pMe = (CCoreApp *)pUser;
    
    if (NULL == pMe)
    {
        return FALSE;
    }
    CORE_ERR("%x %x %x IDD_UIMERR_Handler",eCode,wParam,dwParam);
    pStatic = (IStatic*)IDIALOG_GetControl(pMe->m_pActiveDlg, 
                            IDC_UIMERR_STATIC);

    if (NULL == pStatic)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
	        MEMSET(pMe->m_strLockuimPWD, 0, sizeof(pMe->m_strLockuimPWD));
            return TRUE;
            
        case EVT_DIALOG_START:
            //设置控件区域
            {
                AEERect rc;
                SETAEERECT(&rc, 0, 0,pMe->m_rc.dx, pMe->m_rc.dy - BOTTOMBAR_HEIGHT);
                
                // 先清屏
                IDISPLAY_EraseRect(pMe->m_pDisplay,&rc);
                rc.x = 1;
                rc.dx -= 2;
                ISTATIC_SetRect(pStatic, &rc);
                
                // 设置静态文本控件属性
                ISTATIC_SetProperties(pStatic, ST_UNDERLINE | 
                        ST_CENTERTEXT  | ST_NOSCROLL|ST_MIDDLETEXT);
            }

            (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                      AEECLSID_CORE_APP,
                      EVT_USER_REDRAW,
                      0,
                      0);
            return TRUE;
            
        case EVT_USER_REDRAW:
            {
                uint16    nResID = 0;
                AECHAR    *wTipsMsg = NULL;
                
                switch (pMe->m_eUIMErrCode)
                {
                    // 没插入卡
                    case UIMERR_NOUIM:
                        nResID = IDS_NORUIM;
                        break;
                        
                    // UIM 卡无效(原因输PUK码超过规定次数导致卡失效)
                    case UIMERR_BLOCKED:
                        nResID = IDS_UIMBLOCKED;
                        break;
                        
                    // UIM 卡无效(原因是卡不符合运营商要求而要求锁定)
                    case UIMERR_LOCKED:
                        nResID = IDS_LOCKEDRUIM;
                        break;
                        
                    // UIM 卡正常无误, 程序不应执行到此
                    default:
                        break;
                }
                
                if (nResID != 0)
                {
                    int32  nSize = 128 * sizeof(AECHAR);
                    
                    wTipsMsg = (AECHAR *) MALLOC(nSize);
                    
                    if (NULL != wTipsMsg)
                    {
                        // 取提示文本
                        (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                                    AEE_COREAPPRES_LANGFILE,
                                                    nResID,
                                                    wTipsMsg,
                                                    nSize);
                                                    
                        // 设置静态控件文本
                        (void)ISTATIC_SetText(pStatic,
                                NULL,
                                wTipsMsg,
                                AEE_FONT_NORMAL,
                                AEE_FONT_NORMAL);
                                
                        // 释放不再使用的临时 buffer
                        FREE(wTipsMsg);
                        wTipsMsg = NULL;
                    }
                }
                
                ISTATIC_SetActive(pStatic, TRUE);
                (void) ISTATIC_Redraw(pStatic);
                
                // 绘制底部提示菜单
                // -----SOS
                CoreDrawBottomBar(BTBAR_SOS)

                // 立即更新屏幕
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            }          
            
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;
        case EVT_KEY:
		{
		char  chEnter = 0; 
		static int   nLen = 0; 
            switch (wParam)
            {
                    case AVK_0:
                    case AVK_1:
                    case AVK_2:
                    case AVK_3:
                    case AVK_4:
                    case AVK_5:
                    case AVK_6:
                    case AVK_7:
                    case AVK_8:
                    case AVK_9:
                      chEnter = '0' + (wParam - AVK_0);
                        break;

                    case AVK_STAR:
                        chEnter = '*';
                        break;
 
                    case AVK_POUND:
                        chEnter = '#';
                        break;
                        
                    case AVK_SEND:
                        CLOSE_DIALOG(DLGRET_EMGCALL)
                        return TRUE;
                        
                    case AVK_CLR:
                        chEnter = 0;
                        CLOSE_DIALOG(DLGRET_EMGCALL)
                        return TRUE;
                        
                    case AVK_SELECT:
                        CLOSE_DIALOG(DLGRET_ENTER)  
                        return TRUE;
                        
                    default:
                        return TRUE;
                }

                nLen = STRLEN(pMe->m_strLockuimPWD);
                if (chEnter == 0)
                {// 删除字符
                    if (nLen > 0)
                    {
                        pMe->m_strLockuimPWD[nLen-1] = chEnter; 
                    }
                }
                else if (nLen < LOCKUIMPASSWORDLENTH)
                {
                    pMe->m_strLockuimPWD[nLen] = chEnter;
                    nLen++;
                    pMe->m_strLockuimPWD[nLen] = 0;
                }
            break;
        }
        default:
            break;
    }
    
    return FALSE;
} // IDD_UIMERR_Handler
/*==============================================================================
函数:
    IDD_STARTUPANI_Handler
    
说明:
    IDD_STARTUPANI 对话框事件处理函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。
    eCode [in]: 事件代码。
    wParam: 事件相关数据。
    dwParam: 事件相关数据。
       
返回值:
    TRUE:  传入事件被处理。
    FALSE: 传入事件被忽略。
       
备注：WHEN                WHO      WHAT          WHY
      2008-2-16           fj       V1.6          New develop, Add power down animation  
==============================================================================*/
static boolean  IDD_STARTUPANI_Handler(void       *pUser,
                                       AEEEvent   eCode,
                                       uint16     wParam,
                                       uint32     dwParam)
{
    CCoreApp *pMe = (CCoreApp *)pUser;

    PARAM_NOT_REF(wParam)
    PARAM_NOT_REF(dwParam)
    CORE_ERR("%x %x %x IDD_STARTUPANI_Handler",eCode,wParam,dwParam);
    switch (eCode) 
    {
        case EVT_DIALOG_INIT:
            if(pMe->m_bemergencymode)
            {
                //exit the emergency mode
                ICM_SetSystemPreference(pMe->m_pCM,
                                                AEECM_MODE_PREF_PERSISTENT, AEECM_PREF_TERM_PERMANENT, 0,
                                                AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE, AEECM_BAND_PREF_NO_CHANGE,
                                                AEECM_ROAM_PREF_NO_CHANGE, AEECM_HYBR_PREF_NO_CHANGE,
                                                AEECM_SRV_DOMAIN_PREF_NO_CHANGE, AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE,
                                                NULL, NULL, NULL);
                pMe->m_bemergencymode = FALSE;
            }
            return TRUE;

        case EVT_DIALOG_START: 
            if(pMe->m_wStartupAniTime == 0)
            {
                if ( NULL == pMe->m_pStartupAniImg )
                {
                    pMe->m_pStartupAniImg = ISHELL_LoadImage( pMe->a.m_pIShell, PWRON_ANI_FILE);
                }
                (void) ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_CORE_APP, EVT_USER_REDRAW, 0, 0);
            }
            else
            {
                //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            }
            return TRUE;

        case EVT_USER_REDRAW:
            if(pMe->m_wStartupAniTime == 0)
            {
                // 在此播放开机音乐
                uint16    aRing_type[PROFILENUMBER]  = {0} ;                                          // CFGI_PROFILE_STARTUP_MUSIC
                byte      Ring_Cur_Music                      =  OEMNV_PROFILE_NORMALMODE;
                byte      poweronoff_alert[PROFILENUMBER] =  {0};

                ICONFIG_GetItem( pMe->m_pConfig, CFGI_PROFILE_CUR_NUMBER, &Ring_Cur_Music, sizeof(Ring_Cur_Music) ); 
                ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_POWERONOFF_ALERT,poweronoff_alert,sizeof(poweronoff_alert));  
                if(poweronoff_alert[Ring_Cur_Music]  == OEMNV_POWERONOFF_ENABLE)
                {
                    ICONFIG_GetItem( pMe->m_pConfig, CFGI_PROFILE_STARTUP_MUSIC, aRing_type, sizeof(aRing_type) );
                    //DBGPRINTF("fj music_num = %d , Ring_Cur_Music = %d, IDD_STARTUPANI_Handler",(uint32)aRing_type[Ring_Cur_Music], Ring_Cur_Music);
                    IALERT_StartRingerAlert_Ex( pMe->m_pAlert, (uint32)aRing_type[Ring_Cur_Music] );
                }

                // 播放开机动画
                //pMe->m_wStartupAniTime = 0;

                if ( NULL != pMe->m_pStartupAniImg )
                {     
                    CoreApp_PlayPwrOnAni(pMe);
                }
                else
                {
                    CLOSE_DIALOG( DLGRET_OK ); 
                }
                //ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_DIALER, "S");
            }
            return TRUE; 

        case EVT_DIALOG_END:
            //IALERT_StopRingerAlert(pMe->m_pAlert);
            if (pMe->m_eDlgRet != DLGRET_OK)
            {// 开机动画播放过程中被其他应用启动时中断
                if (NULL != pMe->m_pStartupAniImg)
                {     
                    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)CoreApp_PlayPwrOnAni, pMe);
                    IIMAGE_Stop(pMe->m_pStartupAniImg);
                    IIMAGE_Release(pMe->m_pStartupAniImg);
                    pMe->m_pStartupAniImg = NULL;
                    
                    pMe->m_eDlgRet = DLGRET_OK;
                }
                IALERT_StopRingerAlert(pMe->m_pAlert);
            }
            return TRUE;

        case EVT_DISPLAYDIALOGTIMEOUT:
#if 0
            IALERT_StopRingerAlert(pMe->m_pAlert);
            if ( NULL != pMe->m_pStartupAniImg )
            {     
                IIMAGE_Stop(pMe->m_pStartupAniImg);
                IIMAGE_Release(pMe->m_pStartupAniImg);
                pMe->m_pStartupAniImg = NULL;
            }

            CLOSE_DIALOG(DLGRET_OK)
#endif
            return TRUE;

        case EVT_KEY:
            return TRUE;

        default:
            break;
    }

    return FALSE;
}

/*==============================================================================
函数:
    IDD_LOADING_Handler
    
说明:
    IDD_LOADING 对话框事件处理函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。
    eCode [in]: 事件代码。
    wParam: 事件相关数据。
    dwParam: 事件相关数据。
       
返回值:
    TRUE:  传入事件被处理。
    FALSE: 传入事件被忽略。
       
备注:
    函数显示提示消息给用户，并自动关闭返回。
       
==============================================================================*/
static boolean  IDD_LOADING_Handler(void       *pUser,
                                   AEEEvent   eCode,
                                   uint16     wParam,
                                   uint32     dwParam)
{
    CCoreApp *pMe = (CCoreApp *)pUser;
    IStatic * pStatic = NULL;
    
    PARAM_NOT_REF(wParam)
    CORE_ERR("%x %x %x IDD_LOADING_Handler",eCode,wParam,dwParam);
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    pStatic = (IStatic*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_LOADING);

    if (NULL == pStatic)
    {
        return FALSE;
    }
    
    switch(eCode)
    {
        case EVT_DIALOG_INIT:
            ISTATIC_SetProperties(pStatic, (ST_CENTERTEXT | ST_MIDDLETEXT));
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent(pMe->a.m_pIShell, 
                                    AEECLSID_CORE_APP,
                                    EVT_USER_REDRAW, 
                                    0, 0);

            return TRUE;

        case EVT_USER_REDRAW:
            {
                AECHAR  *wstrText=NULL;
                int32  nSize=0;

                nSize = 32 * sizeof(AECHAR);
                wstrText = (AECHAR *) MALLOC(nSize);
                
                if (NULL != wstrText)
                {
                    // 从资源文件取消息内容
                    (void)ISHELL_LoadResString(pMe->a.m_pIShell,
                                    AEE_COREAPPRES_LANGFILE,                                
                                    IDS_LOADING,
                                    wstrText,
                                    nSize);
                                    
                    (void) ISTATIC_SetText(pStatic,
                                    NULL,
                                    wstrText,
                                    AEE_FONT_NORMAL,
                                    AEE_FONT_NORMAL);
                                    
                    FREE(wstrText);
                }
            }
            
            (void) ISTATIC_Redraw(pStatic);

            // 设置自动关闭对话框的定时器
            (void)ISHELL_SetTimer(pMe->a.m_pIShell, 
                                  1000,
                                  DialogTimeoutCallback, 
                                  pMe);
            if(IsRunAsUIMVersion())
            {
                if(IRUIM_IsCardConnected(pMe->m_pIRUIM))
                {
                    if (NULL == pMe->m_pAddrRUIM)
                    {
                        (void) ISHELL_CreateInstance( pMe->a.m_pIShell,
                                                  AEECLSID_ADDRBOOK_RUIM,
                                                  (void **)&pMe->m_pAddrRUIM);
                    }
                }
            }
            
            if (NULL == pMe->m_pAddrPhone)
            {            
                (void) ISHELL_CreateInstance( pMe->a.m_pIShell,
                                      AEECLSID_ADDRBOOK,
                                      (void **)&pMe->m_pAddrPhone);
            }	    
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            return TRUE;
        
        case EVT_DISPLAYDIALOGTIMEOUT:            
            CLOSE_DIALOG(DLGRET_INITED);
            return TRUE;

        default:
            break;
    }

    return FALSE;
}

static void CoreApp_ImageNotify(void *po, IImage *pIImage, AEEImageInfo *pii, int nErr)
{   
    CCoreApp *pMe = (CCoreApp *)po;
    
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
    db_items_value_type  need_capture;
#endif

    MSG_ERROR("CoreApp_ImageNotify, nErr:%d",nErr, 0, 0);
    if(SUCCESS == nErr) 
    {
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
        if (pMe->m_capture == DB_CAPTURE_NEED)
        {
            extern IDIB ** OEMANNUNCIATOR_GetWallpaperBarDDB(void);
            IDIB **ppDIB=NULL;
            AEERect rc;
            IBitmap *pDevBmp=NULL;
            IBitmap *pBmp =NULL;
            
            pMe->m_capture = DB_CAPTURE_NONE;
            ppDIB = OEMANNUNCIATOR_GetWallpaperBarDDB();
            if ((NULL != ppDIB) &&
                (SUCCESS == IANNUNCIATOR_GetAnnunciatorBarSize(pMe->m_pIAnn,AEECLSID_DISPLAY1, &rc)))
            {
                if (IDISPLAY_GetDeviceBitmap(pMe->m_pDisplay, &pDevBmp) == SUCCESS) 
                {
                    IBITMAP_CreateCompatibleBitmap(pDevBmp, &pBmp, rc.dx, rc.dy);
                    IBITMAP_Release(pDevBmp);
                    
                    IDISPLAY_SetDestination(pMe->m_pDisplay, pBmp);
                    if ( NULL != pWallPaper )
                    {
                        IIMAGE_SetDrawSize(pWallPaper, rc.dx, rc.dy);
                        IIMAGE_Draw(pWallPaper, 0, 0);
                    }
                    else
                    {
                        IDISPLAY_EraseRect(pMe->m_pDisplay, &rc);
                    }
                    IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE); // debug for update issue
                    
                    IDISPLAY_SetDestination(pMe->m_pDisplay, NULL);
                    (void)IBITMAP_QueryInterface(pBmp, AEECLSID_DIB, (void **) ppDIB);
                    IBITMAP_Release (pBmp);
                }
            }
        }
#endif                
        if ( NULL != pWallPaper )
        {
            AEEImageInfo info;
            MSG_ERROR("pMe->m_rc.dx:%d, pMe->m_rc.dy:%d", pMe->m_rc.dx, pMe->m_rc.dy, 0);
            IImage_GetInfo(pWallPaper, &info);
            IDisplay_ClearScreen(pMe->m_pDisplay);
            IIMAGE_Draw(pWallPaper, (pMe->m_rc.dx - info.cx)/2, (pMe->m_rc.dy - info.cy)/2);
        }
        else
        {
            IDISPLAY_EraseRect(pMe->m_pDisplay, &pMe->m_rc);
        }

#ifdef FEATRUE_SET_ANN_FULL_SCREEN
        need_capture.b_capture = DB_CAPTURE_INIDLE;
        db_put(DB_CAPTURE_WALLPER,&need_capture);
        IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,TRUE);//added by chengxiao 2008.11.26
#endif

        if (bHideText)
        {
            CoreApp_UpdateAnnunciator(pMe);//解决背景播放换歌或按AVK_END键状态栏图表闪烁问题
                                         // 故把该句提到IDISPLAY_UpdateEx之前
#ifdef FEATURE_APP_MUSICPLAYER                                         
            ISHELL_CancelTimer(pMe->a.m_pIShell,(PFNNOTIFY)CoreApp_MusicNameAutoScroll,pMe);
#endif
            IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE); // debug for update issue
            bImageDecoded = TRUE;
            return;
        }  

#ifdef FEATURE_APP_MUSICPLAYER
        //CoreApp_DrawMusicName(pMe);
        Core_DrawNameResetScroll(pMe);
#endif

        // 绘制服务提供商名和待机问候语
        CoreApp_DrawBannerMessage(pMe);
    
        // 绘制当前日期、时间信息
        CoreApp_UpdateDateTime(pMe);
        CoreApp_UpdateBottomBar(pMe);//didn't display the sos and key lock icon at the same time
        CoreApp_UpdateAnnunciator(pMe);  //解决背景播放换歌或按AVK_END键状态栏图标闪烁问题
                                         // 故把该句提到IDISPLAY_UpdateEx之前
    }   
    bImageDecoded = TRUE;
    IDISPLAY_UpdateEx(pMe->m_pDisplay,TRUE);
}


//wlh 20090522 add start
/*==============================================================================
函数:
    CoreApp_UpdateIcon

说明:
    用于更新icon显示。

参数:
    pMe [In]: 指向 Core Applet 对象结构的指针。

返回值:
    无。

备注:
    刷新显示操作由调用者根据需要执行相关操作。
==============================================================================*/
static void CoreApp_UpdateIcon(CCoreApp    *pMe)
{
	int i,j,num,num1;
	if ( pMe == NULL ) 
	{
		return;
	}    
	if(pMe->m_bSuspended)
	{
		return;
	}
	if(pMe->m_Idle_Icon->m_b_show)
	{
		if(pMe->m_Idle_Icon->m_icon_rc.x > 0) 
		{
			pMe->m_Idle_Icon->m_icon_rc.x = pMe->m_Idle_Icon->m_icon_rc.x - 20;
			if(pMe->m_Idle_Icon->m_icon_rc.x < 0)
				pMe->m_Idle_Icon->m_icon_rc.x = 0;
		}
	}
	else
	{
		if(pMe->m_Idle_Icon->m_icon_rc.x < 162) 
		{
			pMe->m_Idle_Icon->m_icon_rc.x = pMe->m_Idle_Icon->m_icon_rc.x + 20;
			if(pMe->m_Idle_Icon->m_icon_rc.x > 162)
				pMe->m_Idle_Icon->m_icon_rc.x = 162;
		}
	}
	//第一步，画背景条
	if( pMe->m_Idle_Icon->m_iconback_Image == NULL)
	{
		DBGPRINTF( "Draw m_iconback_Image, load  failed");
	}
	else
	{
		IIMAGE_Draw( pMe->m_Idle_Icon->m_iconback_Image, pMe->m_Idle_Icon->m_icon_rc.x, pMe->m_Idle_Icon->m_icon_rc.y);
	}	

	if((pMe->m_Idle_Icon->m_b_show) && (pMe->m_Idle_Icon->m_icon_rc.x == 0))
	{
		IIMAGE_Draw( pMe->m_Idle_Icon->m_arrow_check, pMe->m_Idle_Icon->m_icon_rc.x + 1, pMe->m_Idle_Icon->m_icon_rc.y );
	}
	else
	{
		IIMAGE_Draw( pMe->m_Idle_Icon->m_arrow_uncheck, pMe->m_Idle_Icon->m_icon_rc.x + 1, pMe->m_Idle_Icon->m_icon_rc.y);
	}

	pMe->m_Idle_Icon->m_icon_rc.x = pMe->m_Idle_Icon->m_icon_rc.x + 14;

	//第二步，画大小图标,注意小图表的坐标取相对于背景的坐标pMe->m_Idle_Icon->m_icon_rc,大图标用的是自己的坐标
	num = 0;
	num1 = 0;
	j = 0;

	//画大icon
	for(i=11;i>0;i--)
	{
		for(j=0;j<11;j++)
		{
			if((pMe->m_Idle_Icon->m_IconList[j]->m_b_Max) && (pMe->m_Idle_Icon->m_IconList[j]->xIconAppNum == i) && (j == pMe->m_now_Num - 1))
			{
				IIMAGE_Draw( pMe->m_Idle_Icon->m_IconList[j]->m_ImageBig ,pMe->m_move_xstation - 30, pMe->m_move_ystation - 30);
			}
			else if((pMe->m_Idle_Icon->m_IconList[j]->m_b_Max) && (pMe->m_Idle_Icon->m_IconList[j]->xIconAppNum == i))
			{
				IIMAGE_Draw( pMe->m_Idle_Icon->m_IconList[j]->m_ImageBig ,pMe->m_Idle_Icon->m_IconList[j]->m_rc.x  , pMe->m_Idle_Icon->m_IconList[j]->m_rc.y);
			}
		}
	}
	//画小icon
	j = 0;
	for(i=0;i<11;i++)
	{
		if(!pMe->m_Idle_Icon->m_IconList[i]->m_b_Max)
			j++;
	}
	if(j < 6)
	{
		for(i=0;i<11;i++)
		{	
			if(!pMe->m_Idle_Icon->m_IconList[i]->m_b_Max)
			{
				pMe->m_Idle_Icon->m_fisticon = i +1;
				pMe->m_Idle_Icon->m_checkicon = pMe->m_Idle_Icon->m_fisticon;
				break;
			}
		}
	}
	for(i=0;i<11;i++)
	{
		if((i >= pMe->m_Idle_Icon->m_fisticon - 1) && (!pMe->m_Idle_Icon->m_IconList[i]->m_b_Max ))
		{
			if((i == pMe->m_now_Num - 1) && (!TOUCH_PT_IN_RECT(pMe->m_move_xstation,pMe->m_move_ystation,pMe->m_Idle_Icon->m_icon_rc))) 
				IIMAGE_Draw( pMe->m_Idle_Icon->m_IconList[i]->m_ImageSmallCheck     , pMe->m_move_xstation - 15, pMe->m_move_ystation - 15);
			else if(i == pMe->m_Idle_Icon->m_checkicon - 1)
				IIMAGE_Draw( pMe->m_Idle_Icon->m_IconList[i]->m_ImageSmallCheck     , pMe->m_Idle_Icon->m_icon_rc.x + 2 + 30*num, pMe->m_Idle_Icon->m_icon_rc.y + 2);
			else
				IIMAGE_Draw( pMe->m_Idle_Icon->m_IconList[i]->m_ImageSmall     , pMe->m_Idle_Icon->m_icon_rc.x + 2 + 30*num, pMe->m_Idle_Icon->m_icon_rc.y + 2);
			
			num++;
		}
	}
#ifndef CUST_EDITION
#ifndef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
	IIMAGE_Draw( pMe->m_Idle_Icon->m_ImageSmallCheck[pMe->m_Idle_Icon->m_checkicon - 1]     , pMe->m_Idle_Icon->m_icon_rc.x + 2 + 30*(pMe->m_Idle_Icon->m_fisticon - num1 -1) , pMe->m_Idle_Icon->m_icon_rc.y + 2);
#endif			
#endif /*CUST_EDITION*/
	pMe->m_Idle_Icon->m_icon_rc.x = pMe->m_Idle_Icon->m_icon_rc.x - 14;

	//第三步，画左右箭头
	if((pMe->m_Idle_Icon->m_b_show) && (pMe->m_Idle_Icon->m_icon_rc.x == 0))
	{
		IIMAGE_Draw( pMe->m_Idle_Icon->m_arrowleft_Image, pMe->m_Idle_Icon->m_icon_rc.x +164, pMe->m_Idle_Icon->m_icon_rc.y + 2);
		IIMAGE_Draw( pMe->m_Idle_Icon->m_arrowright_Image, pMe->m_Idle_Icon->m_icon_rc.x + 164, pMe->m_Idle_Icon->m_icon_rc.y + 22);
	}

	if(pMe->m_Idle_Icon->m_b_show)
	{
		if(pMe->m_Idle_Icon->m_icon_rc.x > 0)
		{
			ISHELL_PostEventEx(pMe->a.m_pIShell, 
                       EVTFLG_ASYNC, 
                       AEECLSID_CORE_APP,
                       EVT_UPDATEIDLE,
                       0,0L);
		}
	}
	else
	{
		if(pMe->m_Idle_Icon->m_icon_rc.x < 162)
		{
			ISHELL_PostEventEx(pMe->a.m_pIShell, 
                       EVTFLG_ASYNC, 
                       AEECLSID_CORE_APP,
                       EVT_UPDATEIDLE,
                       0,0L);
		}
	}
} // CoreApp_UpdateIcon
//wlh 20090522 add end
//wlh 20090427 add start
/*==============================================================================
函数:
    CoreApp_UpdateTimeDial

说明:
    用于更新时间表盘显示。

参数:
    pMe [In]: 指向 Core Applet 对象结构的指针。

返回值:
    无。

备注:
    刷新显示操作由调用者根据需要执行相关操作。
==============================================================================*/
static void CoreApp_UpdateTimeDial(CCoreApp    *pMe)
{
	JulianType  jDate;
	AECHAR      wszDate[64]= {0};
	AECHAR      wFormat[64]= {0};
	byte        bTFmt;
	
	//时针、分钟的角度
	uint16      HUUR_angle;
	uint16      MINUTE_angle;

	//图片
	int err;
	IGraphics * m_pIGraphics;
	IShell           *pShell = AEE_GetShell();
	err = ISHELL_CreateInstance( pShell,
		AEECLSID_GRAPHICS,
		(void **)&m_pIGraphics);
	if (!m_pIGraphics)
	{
		return;
	}
	
	if ( pMe == NULL ) 
	{
		return;
	}    
	// 符合下列条件之一，函数立即返回
	if(pMe->m_bSuspended)
	{
		return;
	}

	CoreApp_Process_Rtc_Event(pMe);

	GetJulianDate(GETTIMESECONDS(), &jDate);

	if(pMe->m_b_dialflag)//画表盘
	{
		if(pMe->m_dial_Image)
		{
			AEERect rc;
			uint16 temp;
			//AEELine hour_Z;
			//uint16 hx1,hx2,mx1,mx2;
			//uint16 hy1,hy2,my1,my2;
			uint16 wHour = jDate.wHour > 12 ? (jDate.wHour - 12) : jDate.wHour;
			if(jDate.wHour == 0)
			{
				wHour = 12;
			}
			IIMAGE_Draw( pMe->m_dial_Image, pMe->m_dial_rc.x , pMe->m_dial_rc.y );
			pMe->m_dial_rc.x = pMe->m_dial_rc.x +19;
			pMe->m_dial_rc.y = pMe->m_dial_rc.y +19;

			MINUTE_angle = jDate.wMinute*6;
			temp = jDate.wMinute/12;
			HUUR_angle = (wHour*30)+(temp*6);
			if(HUUR_angle>360)
			HUUR_angle = abs(HUUR_angle - 360);
			if ((HUUR_angle>=0) && (HUUR_angle<90))
			{
	
				//int temp_sinx =(int)((CoreApp_sin(HUUR_angle)));
				uint16 i;
				/*
				uint16 j;
				uint16 w;
				uint16 h_dx,h_dy; 
				double temp_x1 = CoreApp_sin(HUUR_angle)*16.00;
				w = 0;
				hx1 = pMe->m_dial_rc.x + pMe->m_dial_rc.dx/2 - 1 + temp_x1;
				hy1 = pMe->m_dial_rc.y + pMe->m_dial_rc.dy/2-(16.00*(CoreApp_sin(90-HUUR_angle))) ;
				
				hx2 = pMe->m_dial_rc.x + pMe->m_dial_rc.dx/2 - 1 - (4.00*(CoreApp_sin(HUUR_angle))); 
				hy2 = pMe->m_dial_rc.y + pMe->m_dial_rc.dy/2 + (4.00*(CoreApp_sin(90-HUUR_angle)));
				h_dx = abs(hx1-hx2);
				h_dy = abs(hy1-hy2);
				hour_Z.sx = hx1;
				hour_Z.sy = hy1+1 ;
				hour_Z.ex = hx2;
				hour_Z.ey = hy2+1 ;
				IGRAPHICS_DrawLine(m_pIGraphics,&hour_Z);
				*/
				i = HUUR_angle/6;
				switch (i)
				{
				case 0:
					pMe->m_dial_hour_image[0] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_0);
					if (pMe->m_dial_hour_image[0] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[0], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[0]);
					break;
				case 1:
					pMe->m_dial_hour_image[1] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_6);
					if (pMe->m_dial_hour_image[1] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[1], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[1]);
					break;
				case 2:
					pMe->m_dial_hour_image[2] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_12);
					if (pMe->m_dial_hour_image[1] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[2], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[2]);
					break;
				case 3:
					pMe->m_dial_hour_image[3] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_18);
					if (pMe->m_dial_hour_image[3] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[3], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[3]);
					break;
				case 4:
					pMe->m_dial_hour_image[4] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_24);
					if (pMe->m_dial_hour_image[4] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[4], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[4]);
					break;
				case 5:
					pMe->m_dial_hour_image[5] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_30);
					if (pMe->m_dial_hour_image[5] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[5], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[5]);
					break;
				case 6:
					pMe->m_dial_hour_image[6] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_36);
					if (pMe->m_dial_hour_image[6] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[6], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[6]);
					break;
				case 7:
					pMe->m_dial_hour_image[7] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_42);
					if (pMe->m_dial_hour_image[7] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[7], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[7]);
					break;
				case 8:
					pMe->m_dial_hour_image[8] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_48);
					if (pMe->m_dial_hour_image[8] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[8], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[8]);
					break;
				case 9:
					pMe->m_dial_hour_image[9] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_54);
					if (pMe->m_dial_hour_image[9] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[9], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[9]);
					break;
				case 10:
					pMe->m_dial_hour_image[10] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_60);
					if (pMe->m_dial_hour_image[10] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[10], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[10]);
					break;
				case 11:
					pMe->m_dial_hour_image[11] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_66);
					if (pMe->m_dial_hour_image[11] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[11], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[11]);
					break;
				case 12:
					pMe->m_dial_hour_image[12] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_72);
					if (pMe->m_dial_hour_image[12] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[12], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[12]);
					break;
				case 13:
					pMe->m_dial_hour_image[13] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_78);
					if (pMe->m_dial_hour_image[13] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[13], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[13]);
					break;
				case 14:
					pMe->m_dial_hour_image[14] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_84);
					if (pMe->m_dial_hour_image[14] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[14], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[14]);
					break;
				default:
					break;
				}
				CoreApp_SetMintueImageID(pMe,MINUTE_angle);
				
			}
			else if ((HUUR_angle>=90)&&(HUUR_angle<180))
			{
				
				uint16 i;
				/*
				uint16 j;
				uint16 w;
				uint16 h_dx,h_dy; 
				double temp_x1 = CoreApp_sin((180-HUUR_angle))*16.00;
				w = 0;
				hx1 = pMe->m_dial_rc.x + pMe->m_dial_rc.dx/2 - 1 + temp_x1;
				hy1 = pMe->m_dial_rc.y + pMe->m_dial_rc.dy/2+(16.00*(CoreApp_sin(HUUR_angle-90))) ;

				hx2 = pMe->m_dial_rc.x + pMe->m_dial_rc.dx/2 - 1 - (4.00*(CoreApp_sin((180-HUUR_angle)))); 
				hy2 = pMe->m_dial_rc.y + pMe->m_dial_rc.dy/2 - (4.00*(CoreApp_sin(HUUR_angle-90)));
				hour_Z.sx = hx1+1;
				hour_Z.sy = hy1;
				hour_Z.ex = hx2;
				hour_Z.ey = hy2+1;
				IGRAPHICS_DrawLine(m_pIGraphics,&hour_Z);
				*/

				i = HUUR_angle/6;
				switch (i)
				{
				case 15:
					pMe->m_dial_hour_image[15] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_90);
					if (pMe->m_dial_hour_image[15] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[15], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[15]);
					break;
				case 16:
					pMe->m_dial_hour_image[16] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_96);
					if (pMe->m_dial_hour_image[16] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[16], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[16]);
					break;
				case 17:
					pMe->m_dial_hour_image[17] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_102);
					if (pMe->m_dial_hour_image[17] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[17], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[17]);
					break;
				case 18:
					pMe->m_dial_hour_image[18] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_108);
					if (pMe->m_dial_hour_image[18] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[18], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[18]);
					break;
				case 19:
					pMe->m_dial_hour_image[19] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_114);
					if (pMe->m_dial_hour_image[19] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[19], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[19]);
					break;
				case 20:
					pMe->m_dial_hour_image[20] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_120);
					if (pMe->m_dial_hour_image[20] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[20], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[20]);
					break;
				case 21:
					pMe->m_dial_hour_image[21] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_126);
					if (pMe->m_dial_hour_image[21] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[21], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[21]);
					break;
				case 22:
					pMe->m_dial_hour_image[22] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_132);
					if (pMe->m_dial_hour_image[22] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[22], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[22]);
					break;
				case 23:
					pMe->m_dial_hour_image[23] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_138);
					if (pMe->m_dial_hour_image[23] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[23], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[23]);
					break;
				case 24:
					pMe->m_dial_hour_image[24] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_144);
					if (pMe->m_dial_hour_image[24] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[24], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[24]);
					break;
				case 25:
					pMe->m_dial_hour_image[25] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_150);
					if (pMe->m_dial_hour_image[25] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[25], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[25]);
					break;
				case 26:
					pMe->m_dial_hour_image[26] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_156);
					if (pMe->m_dial_hour_image[26] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[26], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[26]);
					break;
				case 27:
					pMe->m_dial_hour_image[27] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_162);
					if (pMe->m_dial_hour_image[27] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[27], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[27]);
					break;
				case 28:
					pMe->m_dial_hour_image[28] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_168);
					if (pMe->m_dial_hour_image[28] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[28], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[28]);
					break;
				case 29:
					pMe->m_dial_hour_image[29] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_174);
					if (pMe->m_dial_hour_image[29] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[29], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[29]);
					break;
				default:
					break;
				}

				CoreApp_SetMintueImageID(pMe,MINUTE_angle);

			}
			else if ((HUUR_angle>=180)&&(HUUR_angle<270))
			{
				int i;
				/*
				double temp_x1 = CoreApp_sin((270-HUUR_angle))*16.00;
				hx1 = pMe->m_dial_rc.x + pMe->m_dial_rc.dx/2 - 1 - (16.00*(CoreApp_sin(HUUR_angle-180)));
				hy1 = pMe->m_dial_rc.y + pMe->m_dial_rc.dy/2 - (temp_x1);

				hx2 = pMe->m_dial_rc.x + pMe->m_dial_rc.dx/2 - 1 + (4.00*(CoreApp_sin((HUUR_angle-180)))); 
				hy2 = pMe->m_dial_rc.y + pMe->m_dial_rc.dy/2 - (4.00*(CoreApp_sin(270-HUUR_angle)));
				hour_Z.sx = hx1;
				hour_Z.sy = hy1;
				hour_Z.ex = hx2;
				hour_Z.ey = hy2;
				IGRAPHICS_DrawLine(m_pIGraphics,&hour_Z);
				hour_Z.sx = hx1;
				hour_Z.sy = hy1-1;
				hour_Z.ex = hx2;
				hour_Z.ey = hy2-1;
				IGRAPHICS_DrawLine(m_pIGraphics,&hour_Z);

*/
				i = HUUR_angle/6;
				switch (i)
				{
				case 30:
					pMe->m_dial_hour_image[30] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_180);
					if (pMe->m_dial_hour_image[30] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[30], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[30]);
					break;
				case 31:
					pMe->m_dial_hour_image[31] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_186);
					if (pMe->m_dial_hour_image[31] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[31], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[31]);
					break;
				case 32:
					pMe->m_dial_hour_image[32] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_192);
					if (pMe->m_dial_hour_image[32] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[32], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[32]);
					break;
				case 33:
					pMe->m_dial_hour_image[33] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_198);
					if (pMe->m_dial_hour_image[33] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[33], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[33]);
					break;
				case 34:
					pMe->m_dial_hour_image[34] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_204);
					if (pMe->m_dial_hour_image[34] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[34], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[34]);
					break;
				case 35:
					pMe->m_dial_hour_image[35] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_210);
					if (pMe->m_dial_hour_image[35] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[35], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[35]);
					break;
				case 36:
					pMe->m_dial_hour_image[36] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_216);
					if (pMe->m_dial_hour_image[36] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[36], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[36]);
					break;
				case 37:
					pMe->m_dial_hour_image[37] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_222);
					if (pMe->m_dial_hour_image[37] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[37], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[37]);
					break;
				case 38:
					pMe->m_dial_hour_image[38] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_228);
					if (pMe->m_dial_hour_image[38] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[38], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[38]);
					break;
				case 39:
					pMe->m_dial_hour_image[39] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_234);
					if (pMe->m_dial_hour_image[39] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[39], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[39]);
					break;
				case 40:
					pMe->m_dial_hour_image[40] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_240);
					if (pMe->m_dial_hour_image[40] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[40], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[40]);
					break;
				case 41:
					pMe->m_dial_hour_image[41] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_246);
					if (pMe->m_dial_hour_image[41] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[41], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[41]);
					break;
				case 42:
					pMe->m_dial_hour_image[42] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_252);
					if (pMe->m_dial_hour_image[42] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[42], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[42]);
					break;
				case 43:
					pMe->m_dial_hour_image[43] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_258);
					if (pMe->m_dial_hour_image[43] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[43], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[43]);
					break;
				case 44:
					pMe->m_dial_hour_image[44] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_264);
					if (pMe->m_dial_hour_image[44] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[44], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[44]);
					break;
				default:
					break;
				}
				CoreApp_SetMintueImageID(pMe,MINUTE_angle);

			}
			else if ((HUUR_angle>=270)&&(HUUR_angle<360))
			{
				int i = HUUR_angle/6;
				/*
				double temp_x1 = CoreApp_sin((360-HUUR_angle))*16.00;
				hx1 = pMe->m_dial_rc.x + pMe->m_dial_rc.dx/2 - 1 - temp_x1;
				hy1 = pMe->m_dial_rc.y + pMe->m_dial_rc.dy/2 - (16.00*(CoreApp_sin(HUUR_angle-270)));

				hx2 = pMe->m_dial_rc.x + pMe->m_dial_rc.dx/2 - 1 + (4.00*(CoreApp_sin((360-HUUR_angle)))); 
				hy2 = pMe->m_dial_rc.y + pMe->m_dial_rc.dy/2 + (4.00*(CoreApp_sin(HUUR_angle-270)));
				hour_Z.sx = hx1;
				hour_Z.sy = hy1-1;
				hour_Z.ex = hx2;
				hour_Z.ey = hy2-1;
				IGRAPHICS_DrawLine(m_pIGraphics,&hour_Z);
*/
				switch (i)
				{
				case 45:
					pMe->m_dial_hour_image[45] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_270);
					if (pMe->m_dial_hour_image[45] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[45], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[45]);
					break;
				case 46:
					pMe->m_dial_hour_image[46] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_276);
					if (pMe->m_dial_hour_image[46] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[46], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[46]);
					break;
				case 47:
					pMe->m_dial_hour_image[47] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_282);
					if (pMe->m_dial_hour_image[47] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[47], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[47]);
					break;
				case 48:
					pMe->m_dial_hour_image[48] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_288);
					if (pMe->m_dial_hour_image[48] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[48], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[48]);
					break;
				case 49:
					pMe->m_dial_hour_image[49] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_294);
					if (pMe->m_dial_hour_image[49] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[49], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[49]);
					break;
				case 50:
					pMe->m_dial_hour_image[50] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_300);
					if (pMe->m_dial_hour_image[50] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[50], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[50]);
					break;
				case 51:
					pMe->m_dial_hour_image[51] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_306);
					if (pMe->m_dial_hour_image[51] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[51], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[51]);
					break;
				case 52:
					pMe->m_dial_hour_image[52] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_312);
					if (pMe->m_dial_hour_image[52] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[52], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[52]);
					break;
				case 53:
					pMe->m_dial_hour_image[53] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_318);
					if (pMe->m_dial_hour_image[53] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[53], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[53]);
					break;
				case 54:
					pMe->m_dial_hour_image[54] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_324);
					if (pMe->m_dial_hour_image[54] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[54], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[54]);
					break;
				case 55:
					pMe->m_dial_hour_image[55] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_330);
					if (pMe->m_dial_hour_image[55] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[55], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[55]);
					break;
				case 56:
					pMe->m_dial_hour_image[56] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_336);
					if (pMe->m_dial_hour_image[56] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[56], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[56]);
					break;
				case 57:
					pMe->m_dial_hour_image[57] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_342);
					if (pMe->m_dial_hour_image[57] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[57], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[57]);
					break;
				case 58:
					pMe->m_dial_hour_image[58] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_348);
					if (pMe->m_dial_hour_image[58] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[58], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[58]);
					break;
				case 59:
					pMe->m_dial_hour_image[59] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_HOUR_354);
					if (pMe->m_dial_hour_image[59] == NULL)
					{
						break;
					}
					IIMAGE_Draw( pMe->m_dial_hour_image[59], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
					IIMAGE_Release(pMe->m_dial_hour_image[59]);
					break;
				default:
					break;
				}
				CoreApp_SetMintueImageID(pMe,MINUTE_angle);
			}
			//CoreApp_DrawMinuteHand(pMe,MINUTE_angle);
			pMe->m_dial_rc.x = pMe->m_dial_rc.x - 12;
			pMe->m_dial_rc.y = pMe->m_dial_rc.y - 12;
			
			rc.x = pMe->m_dial_rc.x + pMe->m_dial_rc.dx/2 - 2;
			rc.y = pMe->m_dial_rc.y + pMe->m_dial_rc.dy/2 - 14;
			rc.dx = 4;
			rc.dy = 20;
			
			rc.x = pMe->m_dial_rc.x + pMe->m_dial_rc.dx/2 - 7;
			rc.y = pMe->m_dial_rc.y + pMe->m_dial_rc.dy/2 ;
			IDisplay_Update(pMe->m_pDisplay);
		}

	}
	else//画时间条
	{
		AEERect m_dial_rc;
		if( pMe->m_time_Image == NULL)
		{
			DBGPRINTF( ";DrawBackground, load wall paper failed");
		}
		else
		{
		//IMAGE_Draw( pMe->m_time_Image, pMe->m_dial_rc.x, pMe->m_dial_rc.y);
		}
		    // 格式化时间字符串并绘制
		wFormat[0] = 0;
		wszDate[0] = 0;
		(void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                AEE_COREAPPRES_LANGFILE,
                                IDS_TIME_TILE,
                                wFormat,
                                sizeof(wFormat));
		if (bTFmt == OEMNV_TIMEFORM_AMPM)
		{        
			uint16    wHour,len;
                                     
			wHour = jDate.wHour > 12 ? (jDate.wHour - 12) : jDate.wHour;
			if(jDate.wHour == 0)
			{
				wHour = 12;
			}
			WSPRINTF(wszDate, sizeof(wszDate), wFormat, wHour, jDate.wMinute);
			len = WSTRLEN(wszDate);
			if(jDate.wHour >= 12)
			{
				wszDate[len] = (AECHAR)'P';
				wszDate[len+1] = (AECHAR)'M';
				wszDate[len+2] = 0;
			}
			else
			{
				wszDate[len] = (AECHAR)'A';
				wszDate[len+1] = (AECHAR)'M';
				wszDate[len+2] = 0;
			}
		}
		else
		{
			WSPRINTF(wszDate, sizeof(wszDate), wFormat, jDate.wHour, jDate.wMinute);
		}  
		(void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_NORMAL,
                              wszDate, -1,
                              0, 0, &pMe->m_dial_rc,
                              IDF_ALIGN_MIDDLE
                              | IDF_ALIGN_LEFT 
                              | IDF_TEXT_TRANSPARENT);
							 
	}
	if (m_pIGraphics != NULL)
	{
		IGRAPHICS_Release(m_pIGraphics);
		m_pIGraphics = NULL;
	}
       
} // CoreApp_UpdateTimeDial
//wlh 20090427 add end
/*==============================================================================
函数:
    IDD_IDLE_Handler
    
说明:
    IDD_IDLE 对话框事件处理函数
       
参数:
    pUser [in]: 这里必须是指向 Core Applet 对象结构的指针。
    eCode [in]: 事件代码。
    wParam: 事件相关数据。
    dwParam: 事件相关数据。
       
返回值:
    TRUE:  传入事件被处理。
    FALSE: 传入事件被忽略。
       

备注     :  WHEN                    WHO                WHAT                     WHY
           2008-02-20              fj                 V1.6                      New Dev, for Wallpaper       
==============================================================================*/
#ifdef FEATURE_SPORTS_APP
extern boolean is_g_sportsapp_pedometer_bground_flag();
#endif
//extern boolean is_g_bMp3PlayerRun();

//wlh add start
static void CoreApp_IconLeft(CCoreApp *pMe)//小图标向左移
{
	int i,j;
	boolean flag;
	if(pMe->m_Idle_Icon->m_checkicon > 1)
	{
		j = 0;
		flag = FALSE;
		for(i=pMe->m_Idle_Icon->m_checkicon - 2;i>=0;i--)
		{
			if(!pMe->m_Idle_Icon->m_IconList[i]->m_b_Max)
			{
				j++;
				flag = TRUE;
				break;
			}
			else
			{
				j++; 
			}
		}
		if(flag)
			pMe->m_Idle_Icon->m_checkicon = pMe->m_Idle_Icon->m_checkicon -j;

		if(pMe->m_Idle_Icon->m_checkicon < 1)
				pMe->m_Idle_Icon->m_checkicon = 1;
		if(pMe->m_Idle_Icon->m_checkicon < pMe->m_Idle_Icon->m_fisticon)
		{
			j = 0;
			for(i=pMe->m_Idle_Icon->m_checkicon - 1;i>=0;i--)
			{	
				if(!pMe->m_Idle_Icon->m_IconList[i]->m_b_Max)
				{
					pMe->m_Idle_Icon->m_fisticon = i + 1; 
					j++;
					if(j == 5)
						break;
				}
			}
		}
						
		ISHELL_PostEventEx(pMe->a.m_pIShell, 
											EVTFLG_ASYNC, 
											AEECLSID_CORE_APP,
											EVT_UPDATEIDLE,
											0,0L);
	}
}
static void CoreApp_IconRight(CCoreApp *pMe)//小图标向右移
{
	int i,j;
	boolean flag;
	j = 0;
	flag = FALSE;
	for(i=pMe->m_Idle_Icon->m_checkicon;i<11;i++)
	{
		if(!pMe->m_Idle_Icon->m_IconList[i]->m_b_Max)
		{
			j++; 
			flag = TRUE;
			break;
		}
		else
		{
			j++; 
		}
	}	
	if(flag)
		pMe->m_Idle_Icon->m_checkicon = pMe->m_Idle_Icon->m_checkicon +j;
	if(pMe->m_Idle_Icon->m_checkicon == 12)
		pMe->m_Idle_Icon->m_checkicon = 1;

	j = 0;
	flag = FALSE;
	for(i=pMe->m_Idle_Icon->m_fisticon - 1;i<11;i++)
	{	
		if(!pMe->m_Idle_Icon->m_IconList[i]->m_b_Max)
		{
			j++;
			if(j == 5)
			{
				j = i + 1;
				flag = TRUE;
				break;
			}
		}
	}
	if(flag)
	{
		if(j < pMe->m_Idle_Icon->m_checkicon)
			pMe->m_Idle_Icon->m_fisticon = j + 1;
	}
						
	ISHELL_PostEventEx(pMe->a.m_pIShell, 
											EVTFLG_ASYNC, 
											AEECLSID_CORE_APP,
											EVT_UPDATEIDLE,
											0,0L);
}
//wlh add end
static boolean  IDD_IDLE_Handler(void       *pUser,
                                 AEEEvent   eCode,
                                 uint16     wParam,
                                 uint32     dwParam)
{
	int i,j,l;
	boolean flag;
    PARAM_NOT_REF(dwParam)
    static boolean bHideText = FALSE;
    
    CCoreApp *pMe = (CCoreApp *)pUser;
#ifdef FEATURE_KEYGUARD	 
    boolean  bData;
#endif
    CORE_ERR("%x %x %x IDD_IDLE_Handler",eCode,wParam,dwParam);
    if (NULL == pMe)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            MEMSET(pMe->m_wstrEnterNum, 0, sizeof(pMe->m_wstrEnterNum));
            //CoreApp_GetRecordCount(pMe);
            CoreApp_DrawWallPaper(pMe);
#ifdef FEATURE_KEYGUARD
            if(pMe->m_b_set_lock)
            {
                OEMKeyguard_SetState(TRUE);
                pMe->m_b_set_lock = FALSE;
            }
#endif

            return TRUE;
            
        case EVT_DIALOG_START:
        {
            dword shake;
#ifndef CUST_EDITION
            if(!((MMI_GSENSOR_SHAKE_OPEN == mmi_g_sensor_state) 
                ||(MMI_GSENSOR_SHAKE_OPEN_IN_IDLE == mmi_g_sensor_state)))   //ignore if sensor has been open.
            {
                ICONFIG_GetItem(pMe->m_pConfig,CFGI_GSENSOR,&shake,sizeof(shake));
                if(shake & OEMNV_SHAKE_WALLPAPER_MASK)
                {
#ifdef FEATRUE_SUPPORT_G_SENSOR
#ifndef WIN32//wlh 临时修改
                    if(is_g_sportsapp_pedometer_bground_flag() == FALSE)
                    {
                        mmi_g_sensor_process(G_SENSOR_ENABLE | G_SENSOR_SHAKE);
                        mmi_g_sensor_state = MMI_GSENSOR_SHAKE_OPEN;
                    }
#endif//WIN32
#endif
                } 
            }
#endif            
            // 绘制墙纸、软件提示语、运营商名称、待机问候语
            // 以及时间、日期、星期等。由下列定时器函数启动
#if(! defined FEATURE_CARRIER_ISRAEL_PELEPHONE) && (! defined FEATURE_LANG_HEBREW)
            if(pMe->svc_p_name[0] == 0)
#endif
            {
                //DBGPRINTF("IRUIM_Read_Svc_P_Name");
                if(IsRunAsUIMVersion())
                {
                    if(EFAILED == IRUIM_Read_Svc_P_Name(pMe->m_pIRUIM,pMe->svc_p_name))
                    {
                        MEMSET(pMe->svc_p_name,0,(UIM_CDMA_HOME_SERVICE_SIZE+1)*sizeof(AECHAR));
                        ICONFIG_GetItem(pMe->m_pConfig, CFGI_BANNER, pMe->svc_p_name, (NV_MAX_LTRS+1)*sizeof(AECHAR));
                    }
                }
                else
                {
                    MEMSET(pMe->svc_p_name,0,(UIM_CDMA_HOME_SERVICE_SIZE+1)*sizeof(AECHAR));
                    {
#if(defined FEATURE_CARRIER_ISRAEL_PELEPHONE) && (defined FEATURE_LANG_HEBREW)
                        nv_language_enum_type sel = 0;
                        ICONFIG_GetItem(pMe->m_pConfig, CFGI_LANGUAGE_SELECTION, &sel, sizeof(sel));
                        if(sel ==  NV_LANGUAGE_HEBREW)
                        {
                            ISHELL_LoadResString(pMe->a.m_pIShell,AEE_COREAPPRES_LANGFILE, IDS_ISRAEL_BANNER, pMe->svc_p_name, sizeof(pMe->svc_p_name));
                        }
                        else
#endif
                        {
                            ICONFIG_GetItem(pMe->m_pConfig, CFGI_BANNER, pMe->svc_p_name, (NV_MAX_LTRS+1)*sizeof(AECHAR));
                        }
                    }
                }
            }
            CoreApp_UpdateIdleTimer(pMe);
			
#ifdef FEATURE_KEYGUARD
            if(!OEMKeyguard_IsEnabled())
            {
                (void)ISHELL_CancelTimer(pMe->a.m_pIShell,
                                CoreApp_TimeKeyguard,
                                pMe);
            (void) ICONFIG_GetItem(pMe->m_pConfig,
                                    CFGI_KEY_LOCK_CHECK/*CFGI_PHONE_KEY_LOCK_CHECK*/,
                                    &bData,
                                    sizeof(bData));
            if(bData)
            {
                if ((ISHELL_ActiveApplet(pMe->a.m_pIShell) == AEECLSID_CORE_APP)&&
                                    (pMe->m_wActiveDlgID == IDD_IDLE))
                {
                    (void)ISHELL_SetTimer(pMe->a.m_pIShell,
                                    30*1000,
                                    CoreApp_TimeKeyguard,
                                    pMe);
                    }
                }
            }
#endif		  
            return TRUE;
        }
            
        case EVT_UPDATEIDLE:
        {
            
            CoreApp_DrawWallPaper(pMe); // debug for wallpaper update issue
            if(pWallPaper)
            {
                bImageDecoded = FALSE;
                IImage_Notify(pWallPaper, (PFNIMAGEINFO)CoreApp_ImageNotify, pMe);
            }

#if defined(FEATURE_WMS_APP)
            if (pMe->m_bsmstipscheck)
            {
                uint16  nNewsVmail=0, nNewsSMS=0;
                
                // 获取消息数
                wms_cacheinfolist_getcounts(WMS_MB_VOICEMAIL, &nNewsVmail, NULL, NULL);
                wms_cacheinfolist_getcounts(WMS_MB_INBOX, &nNewsSMS, NULL, NULL);
                
                if (nNewsVmail>0 || nNewsSMS>0)
                {
                    CLOSE_DIALOG(DLGRET_SMSTIPS)
                }
            }
#endif            
            
            return TRUE;            
        }

        case EVT_DIALOG_END:
            // 取消相关定时器
            (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
                                      CoreApp_UpdateIdleTimer,
                                      pMe);
            (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
                                      CoreApp_SearchingTimer,
                                      pMe);
#ifdef FEATURE_KEYGUARD	    
	     (void)ISHELL_CancelTimer(pMe->a.m_pIShell, 
                                    CoreApp_TimeKeyguard,
                                    pMe);
#endif
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
            {
                db_items_value_type  need_capture;
                need_capture.b_capture = DB_CAPTURE_NONE;
                db_put(DB_CAPTURE_WALLPER,&need_capture);
                IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,FALSE);
            }
#endif
            return TRUE;
       
        case EVT_KEY_PRESS: 
            if(pMe->m_bemergencymode)
            {
                return TRUE;
            }
            //if set jpeg as wallpaper, it needs more time to finish decode. in this case
            // press other key in idle quickly while exitting from setting wallpaper, topline
            // will be overlapped with signal bar and other app's title.
            if(bImageDecoded == FALSE && pWallPaper)
            {
                return TRUE;
            }
            switch (wParam)
            {
				
                case AVK_UP:
#ifdef FEATURE_CARRIER_THAILAND_HUTCH
                    return CoreApp_LaunchApplet(pMe, AEECLSID_ALARMCLOCK);
#else //#ifdef FEATURE_CARRIER_THAILAND_HUTCH
#if defined(FEATURE_WMS_APP)
                    return CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
#endif
#endif //#ifdef FEATURE_CARRIER_THAILAND_HUTCH
                case AVK_DOWN:
#ifdef FEATURE_CARRIER_THAILAND_HUTCH
                {
#if defined( FEATURE_FM_RADIO)
                    return CoreApp_LaunchApplet(pMe, AEECLSID_APP_FMRADIO);
#else
                    return CoreApp_LaunchApplet(pMe, AEECLSID_ALARMCLOCK);
#endif
                }
//                    return CoreApp_LaunchApplet(pMe, AEECLSID_APPMANAGER);
#else //#ifdef FEATURE_CARRIER_THAILAND_HUTCH
                {
                    IMainMenu *pIMainMenu = NULL;
                    if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,
                                                            AEECLSID_MAIN_MENU,
                                                            (void**)&pIMainMenu))
                    {
                        IMainMenu_StartPlayer(pIMainMenu);     
                    }
                    if(NULL != pIMainMenu )
                    {
                        (void)IMainMenu_Release(pIMainMenu);
                        pIMainMenu = NULL;
                    }                        
                }
                return TRUE;  
#endif  //#ifdef FEATURE_CARRIER_THAILAND_HUTCH
                case AVK_LEFT:
#ifdef FEATURE_CARRIER_THAILAND_HUTCH
                    {
                        IWmsApp *pIWmsApp = NULL;
                        if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,
                                                                                        AEECLSID_WMSAPP,
                                                                                        (void**)&pIWmsApp))
                        {
                            IWmsApp_WriteNewMessage(pIWmsApp);     
                        }
                        if(NULL !=pIWmsApp )
                        {
                            (void)IWmsApp_Release(pIWmsApp);
                            pIWmsApp = NULL;
                        }
                    }
                      return TRUE;  
#else //#ifdef FEATURE_CARRIER_THAILAND_HUTCH
                {
//#if defined( FEATURE_FM_RADIO)
//                    return CoreApp_LaunchApplet(pMe, AEECLSID_APP_FMRADIO);
//#else
//                    return CoreApp_LaunchApplet(pMe, AEECLSID_ALARMCLOCK);
//#endif
                    return CoreApp_LaunchApplet(pMe, AEECLSID_APP_CAMERA);
                }
#endif //#ifdef FEATURE_CARRIER_THAILAND_HUTCH
				
                case AVK_RIGHT:
					//wlh 20090522 add start
					if(pMe->m_Idle_Icon->m_b_show)
					{
						CoreApp_IconRight(pMe);
						return TRUE;
					}
					else
					//wlh 20090522 add end
//#if defined FEATURE_SUPPORT_BT_APP
//                    return CoreApp_LaunchApplet(pMe, AEECLSID_BTUIAPP);
//#else
					{
						return CoreApp_LaunchApplet(pMe, AEECLSID_APP_SOUNDMENU);//
					}
//#endif
                case AVK_SELECT:
                    return TRUE;//CoreApp_LaunchApplet(pMe, AEECLSID_MAIN_MENU);

                case AVK_INFO:
					/*
#if defined(FEATURE_CARRIER_TFL)                
                    return CoreApp_LaunchApplet(pMe, AEECLSID_MAIN_MENU);               
#else //FEATURE_CARRIER_TFL     
                    {
                        IMainMenu *pIMainMenu = NULL;
                        if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,
                                                            AEECLSID_MAIN_MENU,
                                                            (void**)&pIMainMenu))
                        {
                            IMainMenu_StartData(pIMainMenu);     
                        }
                        if(NULL != pIMainMenu )
                        {
                            (void)IMainMenu_Release(pIMainMenu);
                            pIMainMenu = NULL;
                        }                        
                    }
                    return TRUE;
#endif //FEATURE_CARRIER_TFL                    
*/
					if(pMe->m_Idle_Icon->m_b_show)
					{
						pMe->m_now_Num = -1;
						switch (pMe->m_Idle_Icon->m_checkicon)
						{
							case 1:
							return CoreApp_LaunchApplet(pMe, AEECLSID_BTUIAPP);
							case 2:
							return CoreApp_LaunchApplet(pMe, AEECLSID_WORLDTIME);
							case 3:
							return CoreApp_LaunchApplet(pMe, AEECLSID_CALCAPP);
							case 4:
							return CoreApp_LaunchApplet(pMe, AEECLSID_SCHEDULEAPP);
							case 5:
							return CoreApp_LaunchApplet(pMe, AEECLSID_MEDIAGALLERY);
							case 6:
							return CoreApp_LaunchApplet(pMe, AEECLSID_GAME_TETRIS);
							case 7:
							return CoreApp_LaunchApplet(pMe, AEECLSID_VIDEOPLAYER);
							case 8:
							return CoreApp_LaunchApplet(pMe, AEECLSID_APP_MUSICPLAYER);
							case 9:
							return CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
							case 10:
							return CoreApp_LaunchApplet(pMe, AEECLSID_SPORTSAPP);
							case 11:
							return CoreApp_LaunchApplet(pMe, AEECLSID_UIALARM);
							default:
								break;
						}
						return TRUE;
					}
					else
					{
						return CoreApp_LaunchApplet(pMe, AEECLSID_MAIN_MENU);
					}
                case AVK_CLR:
#ifdef FEATURE_KEYGUARD
                    if(OEMKeyguard_IsEnabled())
                    {
                        //OEMKeyguard_SetState(FALSE);
                        //pMe->m_b_set_lock = TRUE;
                        //CLOSE_DIALOG(DLGRET_EMGCALL)
                        return TRUE;
                    }
#endif /* FEATURE_KEYGUARD*/
                    return CoreApp_LaunchApplet(pMe, AEECLSID_APP_CONTACT);

                default:
                    break;
            }
            return FALSE;

        case EVT_GSENSOR_SHAKE:
        case EVT_KEY:
            {
                boolean b_FMBackground = FALSE;
                dword shake;
#ifndef CUST_EDITION
                if((wParam != AVK_END)&&(wParam != AVK_GSENSOR_FORWARD)&&(wParam != AVK_GSENSOR_BACKWARD))
                {
                   if(!((MMI_GSENSOR_SHAKE_CLOSE == mmi_g_sensor_state) 
                    ||(MMI_GSENSOR_SHAKE_CLOSE_IN_IDLE == mmi_g_sensor_state)))  //ignore if sensor has been closed.
                    {
                        OEM_GetConfig(CFGI_GSENSOR,&shake,sizeof(shake));
                        OEM_GetConfig(CFGI_FM_BACKGROUND,&b_FMBackground, sizeof(b_FMBackground));
                		//do close shake if FM or MP3 is playing.
                        if(!(((shake & OEMNV_SHAKE_FM_RADIO_MASK) && (TRUE == b_FMBackground))
                            ||((shake & OEMNV_SHAKE_MUSIC_MASK) && (MP3STATUS_NONE != GetMp3PlayerStatus()))))
						{
                    		if(is_g_sportsapp_pedometer_bground_flag() == FALSE)
                            {
#ifdef FEATRUE_SUPPORT_G_SENSOR	
#ifndef WIN32//wlh 临时修改
                                mmi_g_sensor_process(G_SENSOR_SHAKE_DISABLE);
#endif//WIN32
#endif             
                                mmi_g_sensor_state = MMI_GSENSOR_SHAKE_CLOSE;                    
                            }      
                		}
                    }
                }
#endif                
            }
            
            if(pMe->m_bemergencymode)
            {
                switch (wParam)
                {
                    case AVK_CLR:
                    case AVK_END:
                        CLOSE_DIALOG(DLGRET_MSG)
                        //CoreApp_ShowMsgDialog(pMe,IDS_EXIT_EMERGENCY_MODE);
                        return TRUE;

                    case AVK_SEND:
                    case AVK_LEFT:   
                        return TRUE;
                        
                    default:
                        break;
                }
            }

            //if set jpeg as wallpaper, it needs more time to finish decode. in this case
            // press select key in idle quickly while exitting from setting wallpaper, topline
            // will be overlapped with signal bar and other app's title.
            if(bImageDecoded == FALSE && pWallPaper)
            {
                return TRUE;
            }
            
            switch (wParam)
            {
                case AVK_CLR:
#ifdef FEATURE_KEYGUARD
                    if(OEMKeyguard_IsEnabled())
                    {
                        OEMKeyguard_SetState(FALSE);
                        pMe->m_b_set_lock = TRUE;
                        CLOSE_DIALOG(DLGRET_EMGCALL)
                    }
#endif
                    return TRUE;

                case AVK_SELECT:
                    return   CoreApp_LaunchApplet(pMe, AEECLSID_MAIN_MENU);
                 // 重拨
                case AVK_SEND:
                    if (pMe->m_bAcquiredTime && !pMe->m_bemergencymode)
                    {
                        IRecentCalls  *pRecentCall = NULL;
                        if (AEE_SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,
                                                                 AEECLSID_APP_RECENTCALL,
                                                                 (void **)&pRecentCall))
                        {
                            IRecentCalls_SendList(pRecentCall);
                            IRecentCalls_Release(pRecentCall);
                        }
                        return TRUE;                        
                    }
                    break;

                case AVK_0:
                case AVK_1:
                case AVK_2:
                case AVK_3:
                case AVK_4:
                case AVK_5:
                case AVK_6:
                case AVK_7:
                case AVK_8:
                case AVK_9:
                case AVK_STAR:
                case AVK_POUND:
					if((pMe->m_Idle_Icon->m_b_show)  && (wParam == AVK_POUND))
					{
						pMe->m_Idle_Icon->m_b_show = FALSE;
						ISHELL_PostEventEx(pMe->a.m_pIShell, 
											EVTFLG_ASYNC, 
											AEECLSID_CORE_APP,
											EVT_UPDATEIDLE,
											0,0L);
						return TRUE;
					}
					if((!pMe->m_Idle_Icon->m_b_show)  && (wParam == AVK_POUND))
					{
						pMe->m_Idle_Icon->m_b_show = TRUE;
						ISHELL_PostEventEx(pMe->a.m_pIShell, 
											EVTFLG_ASYNC, 
											AEECLSID_CORE_APP,
											EVT_UPDATEIDLE,
											0,0L);
						return TRUE;
					}
					if(pMe->m_Idle_Icon->m_b_show)
					{
						;
					}
					else
                    {
                        ICallApp         *pCallApp = NULL;
                        if ( SUCCESS != ISHELL_CreateInstance( pMe->a.m_pIShell,
                                                        AEECLSID_DIALER,
                                                        (void **)&pCallApp))
                        {
                            return FALSE;
                        }
                        MEMSET(pMe->m_wstrEnterNum, 0, sizeof(pMe->m_wstrEnterNum));
                        if ((AVKType)wParam == AVK_STAR)
                        {
                            pMe->m_wstrEnterNum[0] = (AECHAR)'*';
                        }
                        else if ((AVKType)wParam == AVK_POUND)
                        {
                            pMe->m_wstrEnterNum[0] = (AECHAR)'#';
                        }
                        else
                        {
                            // 48 为'0'的 ASCII 码值
                            pMe->m_wstrEnterNum[0] = (AECHAR)
                                (48 + ((AVKType)wParam - AVK_0));/*lint !e656*/
                        }
                        ICallApp_VoiceCallDial_F(pCallApp,pMe->m_wstrEnterNum);
                        if (pCallApp) 
                        {
                            ICallApp_Release(pCallApp);
                            pCallApp = NULL;
                        }
                        //MakeVoiceCallDial(pMe->a.m_pIShell , pMe->m_wstrEnterNum);
                        return TRUE;
                    }
                    
// 侯春峰, 2008.03.19, 上面已经有启动收音机的代码了,如果还用这个,会造成启动两次
#if 0
                case AVK_LEFT:
                {
                    CoreApp_LaunchApplet(pMe, AEECLSID_APP_FMRADIO);
                }
#endif
#ifndef CUST_EDITION
                case AVK_GSENSOR_FORWARD: 
                 {
                   boolean isShake0n = FALSE; 
                   (void) ICONFIG_GetItem(pMe->m_pConfig,
                           CFGI_SHAKE_WALLPAPER_CHECK,
                           &isShake0n, 
                           sizeof(boolean));
                    if(isShake0n)
                    {
                         IDisplayMenu * pDisplayMenu = NULL;
                         if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,
                             AEECLSID_APP_DISPLAYMENU,
                             (void**)&pDisplayMenu))
                         {
                             if (pDisplayMenu == NULL)
                             {
                                 return FALSE;
                             }
                             IDisplayMenu_ChangePicture(pDisplayMenu, TRUE);
                         }
                         IDisplayMenu_Release(pDisplayMenu);
                         CLOSE_DIALOG(DLGRET_GS_SHAKE);
                         return TRUE;
                    }
                    else
                    {
                         return TRUE;
                    }
                     break;
                 }
                
                 case AVK_GSENSOR_BACKWARD:
                 {
                    boolean isShake0n = FALSE; 
                     
                     (void) ICONFIG_GetItem(pMe->m_pConfig,
                            CFGI_SHAKE_WALLPAPER_CHECK,
                            &isShake0n, 
                            sizeof(boolean));
                     if(isShake0n)
                     {
                         IDisplayMenu * pDisplayMenu = NULL;
                         if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,
                             AEECLSID_APP_DISPLAYMENU,
                             (void**)&pDisplayMenu))
                         {
                             if (pDisplayMenu == NULL)
                             {
                                 return FALSE;
                             }
                             IDisplayMenu_ChangePicture(pDisplayMenu, FALSE);
                         }
                         IDisplayMenu_Release(pDisplayMenu);
                         CLOSE_DIALOG(DLGRET_GS_SHAKE);
                         return TRUE;
                     }
                     else
                     {
                         return TRUE;
                     }
                     break;
                 }
#endif                 
/*
                 case AVK_END:
                     bHideText = !bHideText;
                     ISHELL_PostEventEx(pMe->a.m_pIShell, 
                                        EVTFLG_ASYNC, 
                                        AEECLSID_CORE_APP,
                                        EVT_UPDATEIDLE,
                                        0,0L);
                    return TRUE; 
      */          
                default:
                    break;
            }
            break;

#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);

				//保护
				if(pMe->m_now_Num > 11)
				{
					pMe->m_now_Num = -1;
				}
				//判断是否时间，时间始终在最上面
				SETAEERECT(&rc, pMe->m_dial_rc.x, pMe->m_dial_rc.y,DIAL_W, DIAL_H);
				//如果是时间状态的变换，数字变表盘
				if((TOUCH_PT_IN_RECT(wXPos,wYPos,rc)) && (abs(pMe->m_down_xstation - wXPos) < 10) && (abs(pMe->m_down_ystation - wYPos) < 10) )
				{
					pMe->m_now_Num = 0;
					if(!pMe->m_b_dialflag) 
					{
						pMe->m_b_dialflag = TRUE;
						pMe->m_dial_rc.dx = DIAL_W;
						pMe->m_dial_rc.dy = DIAL_H;
					}
					else
					{
						pMe->m_b_dialflag = FALSE;
						pMe->m_dial_rc.dx = DIAL_W;
						pMe->m_dial_rc.dy = DIAL_H/2;
					}

					ISHELL_PostEventEx(pMe->a.m_pIShell, 
							EVTFLG_ASYNC, 
							AEECLSID_CORE_APP,
							EVT_UPDATEIDLE,
							0,0L);
					return TRUE;			
				}
				//时间状态下的移动
				if((pMe->m_now_Num == 0)&& ((abs(pMe->m_down_xstation - wXPos) > 10) || (abs(pMe->m_down_ystation - wYPos) > 10)))
				{
					if(pMe->m_Idle_Icon->m_b_show)
					{
						SETAEERECT(&rc, pMe->m_Idle_Icon->m_icon_rc.x, pMe->m_Idle_Icon->m_icon_rc.y, pMe->m_Idle_Icon->m_icon_rc.dx, pMe->m_Idle_Icon->m_icon_rc.dy);
						if(!TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
						{
							pMe->m_dial_rc.x = wXPos - DIAL_W/2;
							pMe->m_dial_rc.y = wYPos - DIAL_H/2;
							
						}
						else
						{
							pMe->m_dial_rc.x = pMe->m_down_xstation - DIAL_W/2;
							pMe->m_dial_rc.y = pMe->m_down_ystation - DIAL_H/2;
						}
					}
					else
					{
						pMe->m_dial_rc.x = wXPos - DIAL_W/2;
						pMe->m_dial_rc.y = wYPos - DIAL_H/2;
					}
					ISHELL_PostEventEx(pMe->a.m_pIShell, 
								EVTFLG_ASYNC, 
								AEECLSID_CORE_APP,
								EVT_UPDATEIDLE,
								0,0L);
					return TRUE;
				}
				//底部选项，这个在大图标之前响应
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
				if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						if (pMe->m_bAcquiredTime && !pMe->m_bemergencymode)
						{
							IRecentCalls  *pRecentCall = NULL;
							if (AEE_SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,
                                                                 AEECLSID_APP_RECENTCALL,
                                                                 (void **)&pRecentCall))
							{
								IRecentCalls_SendList(pRecentCall);
								IRecentCalls_Release(pRecentCall);
							}
							return TRUE;                        
						}
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//中
					{
						 return CoreApp_LaunchApplet(pMe, AEECLSID_MAIN_MENU);
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//右
					{						
#ifdef FEATURE_KEYGUARD
						if(OEMKeyguard_IsEnabled())
						{
                        //OEMKeyguard_SetState(FALSE);
                        //pMe->m_b_set_lock = TRUE;
                        //CLOSE_DIALOG(DLGRET_EMGCALL)
							return TRUE;
						}
#endif /* FEATURE_KEYGUARD*/
						return CoreApp_LaunchApplet(pMe, AEECLSID_APP_CONTACT);
					}
				}
				//小图标区域
				if(TOUCH_PT_IN_RECT(wXPos,wYPos,pMe->m_Idle_Icon->m_icon_rc)) 
				{
					if(pMe->m_Idle_Icon->m_b_show == FALSE)
					{
						pMe->m_Idle_Icon->m_b_show = TRUE;
						pMe->m_now_Num = -1;
						ISHELL_PostEventEx(pMe->a.m_pIShell, 
											EVTFLG_ASYNC, 
											AEECLSID_CORE_APP,
											EVT_UPDATEIDLE,
											0,0L);
					}
					else
					{
						//第一种情况点中隐藏按纽
						SETAEERECT(&rc, 0, 15, 14, 34);
						if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
						{
							pMe->m_Idle_Icon->m_b_show = FALSE;
							pMe->m_now_Num = -1;
							ISHELL_PostEventEx(pMe->a.m_pIShell, 
											EVTFLG_ASYNC, 
											AEECLSID_CORE_APP,
											EVT_UPDATEIDLE,
											0,0L);
							return TRUE;
						}
						//第二种情况点中右按纽
						SETAEERECT(&rc, 164, 15, 12, 17);
						if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
						{
							pMe->m_now_Num = -1;
							CoreApp_IconRight(pMe);
							return TRUE;				
						}
						//第三种情况点中左按纽
						SETAEERECT(&rc, 164, 32, 12, 17);
						if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
						{
							CoreApp_IconLeft(pMe);
							pMe->m_now_Num = -1;
							return TRUE;
						}
						//第四种情况，大还原成小
						if(pMe->m_now_Num > 0)
						{
							if(pMe->m_Idle_Icon->m_IconList[pMe->m_now_Num - 1]->m_b_Max)
							{
								pMe->m_Idle_Icon->m_IconList[pMe->m_now_Num - 1]->m_b_Max = FALSE;
								pMe->m_Idle_Icon->m_IconList[pMe->m_now_Num - 1]->xIconAppNum = 0;
								pMe->m_now_Num = -1;
								ISHELL_PostEventEx(pMe->a.m_pIShell, 
											EVTFLG_ASYNC, 
											AEECLSID_CORE_APP,
											EVT_UPDATEIDLE,
											0,0L);
								return TRUE;
							}
						
						//第5钟情况，启动小程序
							flag = FALSE;
							for(i=0;i<11;i++)
							{
								if(!pMe->m_Idle_Icon->m_IconList[i]->m_b_Max)
								{
									flag =TRUE;
								}
							}
							if(flag)
							{
								switch (pMe->m_Idle_Icon->m_checkicon)
								{
									case 1:
										if(!pMe->m_Idle_Icon->m_IconList[0]->m_b_Max)
										{
											pMe->m_now_Num = -1;
											return CoreApp_LaunchApplet(pMe, AEECLSID_BTUIAPP);
										}
										break;
									case 2:
										if(!pMe->m_Idle_Icon->m_IconList[1]->m_b_Max)
										{
											pMe->m_now_Num = -1;
											return CoreApp_LaunchApplet(pMe, AEECLSID_WORLDTIME);
										}
										break;
									case 3:
										if(!pMe->m_Idle_Icon->m_IconList[2]->m_b_Max)
										{
											pMe->m_now_Num = -1;
											return CoreApp_LaunchApplet(pMe, AEECLSID_CALCAPP);
										}
									case 4:
										if(!pMe->m_Idle_Icon->m_IconList[3]->m_b_Max)
										{
											pMe->m_now_Num = -1;
											return CoreApp_LaunchApplet(pMe, AEECLSID_SCHEDULEAPP);
										}
									case 5:
										if(!pMe->m_Idle_Icon->m_IconList[4]->m_b_Max)
										{
											pMe->m_now_Num = -1;
											return CoreApp_LaunchApplet(pMe, AEECLSID_MEDIAGALLERY);
										}
									case 6:
										if(!pMe->m_Idle_Icon->m_IconList[5]->m_b_Max)
										{
											pMe->m_now_Num = -1;
											return CoreApp_LaunchApplet(pMe, AEECLSID_GAME_TETRIS);
										}
									case 7:
										if(!pMe->m_Idle_Icon->m_IconList[6]->m_b_Max)
										{
											pMe->m_now_Num = -1;
											return CoreApp_LaunchApplet(pMe, AEECLSID_VIDEOPLAYER);
										}
									case 8:
										if(!pMe->m_Idle_Icon->m_IconList[7]->m_b_Max)
										{
											pMe->m_now_Num = -1;
											return CoreApp_LaunchApplet(pMe, AEECLSID_APP_MUSICPLAYER);
										}
									case 9:
										if(!pMe->m_Idle_Icon->m_IconList[8]->m_b_Max)
										{
											pMe->m_now_Num = -1;
											return CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
										}
									case 10:
										if(!pMe->m_Idle_Icon->m_IconList[9]->m_b_Max)
										{
											pMe->m_now_Num = -1;
											return CoreApp_LaunchApplet(pMe, AEECLSID_SPORTSAPP);
										}
									case 11:
										if(!pMe->m_Idle_Icon->m_IconList[10]->m_b_Max)
										{
											pMe->m_now_Num = -1;
											return CoreApp_LaunchApplet(pMe, AEECLSID_UIALARM);
										}
									default:
										break;
								}
							}
						}
					}
				}
				else
				{
					if((pMe->m_now_Num > 0) && (pMe->m_now_Num < 12))
					{
						for(i=0;i<11;i++)
						{
							if(pMe->m_Idle_Icon->m_IconList[i]->m_b_Max)
							{
								if((pMe->m_Idle_Icon->m_IconList[i]->xIconAppNum > 0) &&(pMe->m_Idle_Icon->m_IconList[i]->xIconAppNum < pMe->m_now_Num))
									pMe->m_Idle_Icon->m_IconList[i]->xIconAppNum++; 
							}
						}
						pMe->m_Idle_Icon->m_IconList[pMe->m_now_Num - 1]->m_b_Max = TRUE;
						pMe->m_Idle_Icon->m_IconList[pMe->m_now_Num - 1]->m_b_Move = FALSE;
						pMe->m_Idle_Icon->m_IconList[pMe->m_now_Num - 1]->xIconAppNum = 1;
						pMe->m_Idle_Icon->m_IconList[pMe->m_now_Num - 1]->m_rc.x = wXPos - 30;
						pMe->m_Idle_Icon->m_IconList[pMe->m_now_Num - 1]->m_rc.y = wYPos - 30;

						if(pMe->m_now_Num == pMe->m_Idle_Icon->m_fisticon)
						{
							for(i=pMe->m_Idle_Icon->m_fisticon;i<11;i++)
							{
								if(!pMe->m_Idle_Icon->m_IconList[i]->m_b_Max)
								{
									pMe->m_Idle_Icon->m_fisticon++; 
									pMe->m_Idle_Icon->m_checkicon = pMe->m_Idle_Icon->m_fisticon;
									break;
								}
								else
								{
									pMe->m_Idle_Icon->m_fisticon++; 
								}
							}	
						}
						else
						{
							pMe->m_Idle_Icon->m_checkicon = pMe->m_Idle_Icon->m_fisticon;
						}

						pMe->m_up_xstation = (int16)AEE_GET_X(dwParam);
						pMe->m_up_ystation = (int16)AEE_GET_Y(dwParam);
						if((abs(pMe->m_up_xstation - pMe->m_down_xstation) < 10) && (abs(pMe->m_up_ystation - pMe->m_down_ystation) < 10))
						{
							switch (pMe->m_now_Num)
							{
							case 1:
								pMe->m_now_Num = -1;
							return CoreApp_LaunchApplet(pMe, AEECLSID_BTUIAPP);
							case 2:
								pMe->m_now_Num = -1;
							return CoreApp_LaunchApplet(pMe, AEECLSID_WORLDTIME);
							case 3:
								pMe->m_now_Num = -1;
							return CoreApp_LaunchApplet(pMe, AEECLSID_CALCAPP);
							case 4:
								pMe->m_now_Num = -1;
							return CoreApp_LaunchApplet(pMe, AEECLSID_SCHEDULEAPP);
							case 5:
								pMe->m_now_Num = -1;
							return CoreApp_LaunchApplet(pMe, AEECLSID_MEDIAGALLERY);
							case 6:
								pMe->m_now_Num = -1;
							return CoreApp_LaunchApplet(pMe, AEECLSID_GAME_TETRIS);
							case 7:
								pMe->m_now_Num = -1;
							return CoreApp_LaunchApplet(pMe, AEECLSID_VIDEOPLAYER);
							case 8:
								pMe->m_now_Num = -1;
							return CoreApp_LaunchApplet(pMe, AEECLSID_APP_MUSICPLAYER);
							case 9:
								pMe->m_now_Num = -1;
							return CoreApp_LaunchApplet(pMe, AEECLSID_WMSAPP);
							case 10:
								pMe->m_now_Num = -1;
							return CoreApp_LaunchApplet(pMe, AEECLSID_SPORTSAPP);
							case 11:
								pMe->m_now_Num = -1;
							return CoreApp_LaunchApplet(pMe, AEECLSID_UIALARM);
							}
						}
					}
					pMe->m_now_Num = -1;
					ISHELL_PostEventEx(pMe->a.m_pIShell, 
											EVTFLG_ASYNC, 
											AEECLSID_CORE_APP,
											EVT_UPDATEIDLE,
											0,0L);
					return TRUE;
				}
				//pMe->m_b_dialmove = FALSE;
				
			}
			break;
		case EVT_PEN_MOVE:
			{
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				//保护
				if(pMe->m_now_Num > 11)
					pMe->m_now_Num = -1;

				if(pMe->m_now_Num > -1)
				{
					SETAEERECT(&rc, pMe->m_dial_rc.x, pMe->m_dial_rc.y, pMe->m_dial_rc.dx, pMe->m_dial_rc.dy);
					if((pMe->m_now_Num == 0) && (TOUCH_PT_IN_RECT(wXPos,wYPos,rc)))
					{
						pMe->m_dial_rc.x = wXPos - pMe->m_dial_rc.dx/2 ;
						pMe->m_dial_rc.y = wYPos - pMe->m_dial_rc.dy/2 ;
						
						ISHELL_PostEventEx(pMe->a.m_pIShell, 
								EVTFLG_ASYNC, 
								AEECLSID_CORE_APP,
								EVT_UPDATEIDLE,
								0,0L);
						return TRUE;						
					}
					else
					{
						pMe->m_move_xstation = wXPos ;
						pMe->m_move_ystation = wYPos;
						ISHELL_PostEventEx(pMe->a.m_pIShell, 
								EVTFLG_ASYNC, 
								AEECLSID_CORE_APP,
								EVT_UPDATEIDLE,
								0,0L);
						return TRUE;
					}
				}
			}
			break;
		
		case EVT_PEN_DOWN:
			{
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);
				//判断点击的对象要按规定的顺序来
				pMe->m_now_Num = -1;
				//2。判断是否表盘，表盘始终在大ICON上面
				SETAEERECT(&rc, pMe->m_dial_rc.x, pMe->m_dial_rc.y, 55, 55);
				if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
				{	
					pMe->m_now_Num = 0;
					pMe->m_down_xstation = (int16)AEE_GET_X(dwParam);
					pMe->m_down_ystation = (int16)AEE_GET_Y(dwParam);
					pMe->m_move_xstation = (int16)AEE_GET_X(dwParam);
					pMe->m_move_ystation = (int16)AEE_GET_Y(dwParam);

					ISHELL_PostEventEx(pMe->a.m_pIShell, 
							EVTFLG_ASYNC, 
							AEECLSID_CORE_APP,
							EVT_UPDATEIDLE,
							0,0L);
					return TRUE;
				}
				//1。判断是否小ICON
				if(pMe->m_Idle_Icon->m_b_show)
				{
					SETAEERECT(&rc, 15, 15, 150, 34);
					if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
					{
						SETAEERECT(&rc, 15, 15, 30, 34);
						if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
						{
							pMe->m_now_Num = pMe->m_Idle_Icon->m_fisticon;
							j = 0;
							for(i=pMe->m_Idle_Icon->m_fisticon - 1;i<11;i++)
							{
								if(pMe->m_Idle_Icon->m_IconList[i]->m_b_Max)
										pMe->m_now_Num++;
								else
								{
									j++;
									if(j == 1)
										break;
								}					
							}				
						}
						SETAEERECT(&rc, 15+30, 15, 30, 34);
						if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
						{
							pMe->m_now_Num = pMe->m_Idle_Icon->m_fisticon + 1;
							j = 0;
							for(i=pMe->m_Idle_Icon->m_fisticon - 1;i<11;i++)
							{
								if(pMe->m_Idle_Icon->m_IconList[i]->m_b_Max)
										pMe->m_now_Num++;
								else
								{
									j++;
									if(j == 2)
										break;
								}					
							}	
						}
						SETAEERECT(&rc, 15+30+30, 15, 30, 34);
						if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
						{
							pMe->m_now_Num = pMe->m_Idle_Icon->m_fisticon + 2;
							j = 0;
							for(i=pMe->m_Idle_Icon->m_fisticon - 1;i<11;i++)
							{
								if(pMe->m_Idle_Icon->m_IconList[i]->m_b_Max)
										pMe->m_now_Num++;
								else
								{
									j++;
									if(j == 3)
										break;
								}					
							}	
						}
						SETAEERECT(&rc, 15+30+30+30, 15, 30, 34);
						if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
						{
							pMe->m_now_Num = pMe->m_Idle_Icon->m_fisticon + 3;
							j = 0;
							for(i=pMe->m_Idle_Icon->m_fisticon - 1;i<11;i++)
							{
								if(pMe->m_Idle_Icon->m_IconList[i]->m_b_Max)
										pMe->m_now_Num++;
								else
								{
									j++;
									if(j == 4)
										break;
								}					
							}	
						}
						SETAEERECT(&rc, 15+30+30+30+30, 15, 30, 34);
						if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
						{
							pMe->m_now_Num = pMe->m_Idle_Icon->m_fisticon + 4;
							j = 0;
							for(i=pMe->m_Idle_Icon->m_fisticon - 1;i<11;i++)
							{
								if(pMe->m_Idle_Icon->m_IconList[i]->m_b_Max)
										pMe->m_now_Num++;
								else
								{
									j++;
									if(j == 5)
										break;
								}					
							}	
						}
						if(pMe->m_now_Num > 11)
							pMe->m_now_Num = -1;
						if(pMe->m_now_Num > 0)
						{
							pMe->m_Idle_Icon->m_checkicon = pMe->m_now_Num;
							pMe->m_down_xstation = (int16)AEE_GET_X(dwParam);
							pMe->m_down_ystation = (int16)AEE_GET_Y(dwParam);
							pMe->m_move_xstation = (int16)AEE_GET_X(dwParam);
							pMe->m_move_ystation = (int16)AEE_GET_Y(dwParam);
							ISHELL_PostEventEx(pMe->a.m_pIShell, 
								EVTFLG_ASYNC, 
								AEECLSID_CORE_APP,
								EVT_UPDATEIDLE,
								0,0L);
						}
						return TRUE;
					}
				}
					
				//3。判断是否大ICON
				flag = FALSE;
				for(j = 1; j< 12;j++)
				{
					for(i = 0; i< 11;i++)
					{
						if(pMe->m_Idle_Icon->m_IconList[i]->xIconAppNum == j)
						{
							SETAEERECT(&rc, pMe->m_Idle_Icon->m_IconList[i]->m_rc.x, pMe->m_Idle_Icon->m_IconList[i]->m_rc.y, pMe->m_Idle_Icon->m_IconList[i]->m_rc.dx, pMe->m_Idle_Icon->m_IconList[i]->m_rc.dy);
							if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
							{
								for(l = 0; l< 11;l++)
								{
									if((pMe->m_Idle_Icon->m_IconList[l]->xIconAppNum < j) && (pMe->m_Idle_Icon->m_IconList[l]->xIconAppNum > 0 ))
									{
										pMe->m_Idle_Icon->m_IconList[l]->xIconAppNum++;
									}
								}
								pMe->m_Idle_Icon->m_IconList[i]->xIconAppNum = 1;
								pMe->m_now_Num = i+1;
								flag = TRUE;
								break;
							}
						}
					}
					if(flag)
						break;
				}
			

				if(pMe->m_now_Num > -1)
				{
					pMe->m_down_xstation = (int16)AEE_GET_X(dwParam);
					pMe->m_down_ystation = (int16)AEE_GET_Y(dwParam);
					pMe->m_move_xstation = (int16)AEE_GET_X(dwParam);
					pMe->m_move_ystation = (int16)AEE_GET_Y(dwParam);
					ISHELL_PostEventEx(pMe->a.m_pIShell, 
							EVTFLG_ASYNC, 
							AEECLSID_CORE_APP,
							EVT_UPDATEIDLE,
							0,0L);
				}
				return TRUE;
			}
			break;
#endif//FEATURE_LCD_TOUCH_ENABLE    
        default:
            break;
    }
    
    return FALSE;
} // IDD_IDLE_Handler

#if defined(FEATURE_WMS_APP)
/*==============================================================================
函数:
    IDD_WMSTIPS_Handler

说明:
    CoreApp 对话框 IDD_WMSTIPS 事件处理函数。

参数:
    pMe [in]:       指向Core Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]:     事件代码。
    wParam:         事件参数
    dwParam [in]:   与wParam关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_WMSTIPS_Handler(void        *pUser, 
                                   AEEEvent    eCode, 
                                   uint16      wParam, 
                                   uint32      dwParam)
{
    static IStatic * pStatic = NULL;
    static IBitmap * pDevBmp = NULL;
    CCoreApp *pMe = (CCoreApp *)pUser;
    CORE_ERR("%x %x %x IDD_WMSTIPS_Handler",eCode,wParam,dwParam);
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    if (NULL == pStatic)
    {
        AEERect rc = {0};

        if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_STATIC,
                (void**)&pStatic) == SUCCESS)
        {
            ISTATIC_SetRect(pStatic, &rc);
        }
        else
        {
            return FALSE;
        }
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,TRUE);
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->a.m_pIShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_CORE_APP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;

        case EVT_USER_REDRAW:
            {
                AECHAR  wstrText[64]={0};
                AECHAR  wstrFMT[64]={0};
                uint16  nNewsVmail=0, nNewsSMS=0;
                PromptMsg_Param_type  Msg_Param={0};
                db_items_value_type  need_capture;
                
                need_capture.b_capture = DB_CAPTURE_NONE;
                db_put(DB_CAPTURE_WALLPER,&need_capture);
                // 获取消息数
                wms_cacheinfolist_getcounts(WMS_MB_VOICEMAIL, &nNewsVmail, NULL, NULL);
                wms_cacheinfolist_getcounts(WMS_MB_INBOX, &nNewsSMS, NULL, NULL);
                
                
                if (nNewsVmail > 0)
                {
                    // 从资源文件取消息内容
                    (void)ISHELL_LoadResString(pMe->a.m_pIShell,
                                    AEE_COREAPPRES_LANGFILE,                                
                                    IDS_VMAILTIPS_FMT,
                                    wstrFMT,
                                    sizeof(wstrFMT));
                                    
                    WSPRINTF(wstrText, sizeof(wstrText), wstrFMT, nNewsVmail);
                }
                else
                {
                    // 从资源文件取消息内容
                    (void)ISHELL_LoadResString(pMe->a.m_pIShell,
                                    AEE_COREAPPRES_LANGFILE,                                
                                    IDS_NEWWMSTIPS_FMT,
                                    wstrFMT,
                                    sizeof(wstrFMT));
                                    
                    WSPRINTF(wstrText, sizeof(wstrText), wstrFMT, nNewsSMS);
                }
  
                Msg_Param.ePMsgType = MESSAGE_INFORMATION;
                Msg_Param.pwszMsg = wstrText;
                Msg_Param.eBBarType = BTBAR_READ_BACK;

                if(pDevBmp != NULL)
                {
                    IDISPLAY_BitBlt(pMe->m_pDisplay, 0, 0, pMe->m_rc.dx, pMe->m_rc.dy, 
                            pDevBmp, 0, 0, AEE_RO_COPY);
                }
                DrawPromptMessage(pMe->m_pDisplay, pStatic, &Msg_Param);
                IANNUNCIATOR_Redraw(pMe->m_pIAnn);
            }
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            
            return TRUE;

        case EVT_DIALOG_END:
            if(pMe->m_bActive)
            {
                if(pDevBmp != NULL)
                {
                    IBITMAP_Release(pDevBmp);
                    pDevBmp = NULL;
                }
            }
            else if(NULL == pDevBmp)
            {
                IBitmap *pTempBmp = NULL;

                IDISPLAY_GetDeviceBitmap(pMe->m_pDisplay, &pTempBmp);
                IBITMAP_CreateCompatibleBitmap(pTempBmp, &pDevBmp, pMe->m_rc.dx, pMe->m_rc.dy);
                if(NULL != pTempBmp && NULL != pDevBmp)
                {
                    IBITMAP_BltIn(pDevBmp, 0, 0, pMe->m_rc.dx, pMe->m_rc.dy, pTempBmp, 0, 0, AEE_RO_COPY);
                }
            }
            ISTATIC_Release(pStatic);
            pStatic = NULL;
            IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,FALSE);
            return TRUE;
  
        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                case AVK_END:
#ifdef FEATURE_SMSTONETYPE_MID 
                // 通知收音机关闭声音
                    IALERT_StopSMSAlert(pMe->m_pAlert);
#endif //#if defined FEATURE_SMSTONETYPE_MID		    
                    IALERT_StopMp3Alert(pMe->m_pAlert);
                    pMe->m_bsmstipscheck = FALSE;
                    CLOSE_DIALOG(DLGRET_SMSTIPS_OK)
                    return TRUE;

                case AVK_SELECT:
#ifdef FEATURE_SMSTONETYPE_MID                        
                    IALERT_StopSMSAlert(pMe->m_pAlert);
#endif //#if defined FEATURE_SMSTONETYPE_MID		    
                    IALERT_StopMp3Alert(pMe->m_pAlert);
                    pMe->m_bsmstipscheck = FALSE;
                    CLOSE_DIALOG(DLGRET_VIEWSMS)
                    return TRUE;
  
                default:
                    break;
            }
            break;

        default:
            break;
    }

    return FALSE;
} // IDD_WMSTIPS_Handler
#endif

/*==============================================================================
函数：
       IDD_POWERDOWN_Handler
说明：
       IDD_POWERDOWN对话框事件处理函数
       
参数：
       pMe [in]：指向Core Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：WHEN                WHO      WHAT          WHY
     2008-2-16           fj       V1.6          New develop, Add power down animation      
==============================================================================*/
static boolean  IDD_POWERDOWN_Handler(void *pUser,
                                      AEEEvent   eCode,
                                      uint16     wParam,
                                      uint32     dwParam)
{
    PARAM_NOT_REF(dwParam)    
    CCoreApp *pMe = (CCoreApp *)pUser;    
    CORE_ERR("%x %x %x IDD_POWERDOWN_Handler",eCode,wParam,dwParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
            
#ifdef FEATURE_UIALARM
            // 挂起警报器
            CoreApp_EnableShellAlarms(pMe, FALSE);
#endif
            
#ifdef FEATRUE_AUTO_POWER
        OEMRTC_Process_Auto_Power_On();
#endif

#if defined( FEATURE_POWERDOWN_ALARM)
            {
                extern void registerAgainPowerdownAlarmclock( void);
                registerAgainPowerdownAlarmclock();
            }
#endif
#ifdef FEATURE_APP_MUSICPLAYER
          ISHELL_PostEvent(pMe->a.m_pIShell,
                            AEECLSID_APP_MUSICPLAYER,
                            EVT_CLOSEAPP,
                            0,
                            0);
#endif
            // 不再关心 IBatt 通知消息
            //(void) ISHELL_RegisterNotify(pMe->a.m_pIShell,
            //            AEECLSID_CORE_APP,
            //            0);

            // Turn off RSSI indicator
            //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RSSI, ANNUN_STATE_OFF);
           
            // 关闭电池图标
            //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_BATT, ANNUN_STATE_OFF);
            if(pWallPaper)
            {
                IIMAGE_Release(pWallPaper);
                pWallPaper = NULL;
            }
            (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                              AEECLSID_CORE_APP,
                              EVT_USER_REDRAW,
                              0,
                              0);
            
            return TRUE;
            
        case EVT_USER_REDRAW:
 
            // 提示文字相关代码
            {
                AECHAR    wszMsgText[128];
                AECHAR    wszNewLine[ ] = {(AECHAR)'\n', (AECHAR)'\0'};
                int nLen;
                wszMsgText[0] = 0;
                switch (pMe->m_ePowerDownType)
                {
#ifdef FEATURE_UIM
                    case POWERDOWN_RUIM_DOOR_RESET:
                       {    
                            // 在此播放提示音
                            
                            //IALERT_StartRingerAlert( pMe->m_pAlert, (uint32)aRing_type[Ring_Cur_Music] );
                
                            // 取"R-UIM door removed" 字符串
                            (void) ISHELL_LoadResString( pMe->a.m_pIShell,
                                                         AEE_COREAPPRES_LANGFILE,
                                                         IDS_RUIM_DOOR_REMOVED,
                                                         wszMsgText,
                                                         sizeof(wszMsgText));
                                
                            // 追加两个换行符实现一个空行
                            (void)WSTRCAT(wszMsgText, wszNewLine);
                            (void)WSTRCAT(wszMsgText, wszNewLine);
                
                            nLen = WSTRLEN(wszMsgText);
                
                            // 取"电话正在" 字符串
                            (void) ISHELL_LoadResString( pMe->a.m_pIShell,
                                                         AEE_COREAPPRES_LANGFILE,
                                                         IDS_RESET_1,
                                                         &wszMsgText[nLen],
                                                         (128 - nLen) * sizeof(AECHAR));
                                
                            // 加换行符
                            (void)WSTRCAT(wszMsgText, wszNewLine);
                        
                            nLen = WSTRLEN(wszMsgText);
                        
                            // 取"重新设置..." 字符串
                            (void) ISHELL_LoadResString( pMe->a.m_pIShell,
                                                         AEE_COREAPPRES_LANGFILE,
                                                         IDS_RESET_2,
                                                         &wszMsgText[nLen],
                                                         (128 - nLen) * sizeof(AECHAR));
                        }    
                        break;
                       
#endif  /* FEATURE_UIM */                              
                    case POWERDOWN_NORMAL:
                        if(pMe->m_wStartupAniTime == 0)
                        {  
                            // 在此播放关机音乐
                            uint16    aRing_type[PROFILENUMBER]  = {0} ;                                          // CFGI_PROFILE_STARTUP_MUSIC
                            byte      Ring_Cur_Music                      =  OEMNV_PROFILE_NORMALMODE;
                            byte      poweronoff_alert[PROFILENUMBER] =  {0};			

                            ICONFIG_GetItem( pMe->m_pConfig, CFGI_PROFILE_CUR_NUMBER, &Ring_Cur_Music, sizeof(Ring_Cur_Music) ); 
                            ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_POWERONOFF_ALERT,poweronoff_alert,sizeof(poweronoff_alert));
                            if(poweronoff_alert[Ring_Cur_Music]  == OEMNV_POWERONOFF_ENABLE)
                            {
                                ICONFIG_GetItem( pMe->m_pConfig, CFGI_PROFILE_SHUTDOWN_MUSIC, aRing_type, sizeof(aRing_type) );
                                //DBGPRINTF("fj music_num = %d , Ring_Cur_Music = %d, IDD_POWERDOWN_Handler",(uint32)aRing_type[Ring_Cur_Music], Ring_Cur_Music);
                                IALERT_StartRingerAlert_Ex( pMe->m_pAlert, (uint32)aRing_type[Ring_Cur_Music] );
                            }
                            
                            //pMe->m_wStartupAniTime = 0; //  控制播放次数
                             
                            if ( NULL != pMe->m_pStartupAniImg )
                            {     
                                CoreApp_PlayPwrOffAni(pMe);
                            }
                            //else
                            //{ 
                            //    (void)ISHELL_PostEvent( pMe->a.m_pIShell, 
                            //                            AEECLSID_CORE_APP, 
                            //                            EVT_DISPLAYDIALOGTIMEOUT, 
                            //                            0, 0);
                            //}
                        }
                        else
                        {
                            if ( NULL != pMe->m_pStartupAniImg )
                            {     
                                IIMAGE_Stop(pMe->m_pStartupAniImg);
                                IIMAGE_Release(pMe->m_pStartupAniImg);
                                pMe->m_pStartupAniImg = NULL;
                            }
                            (void)ISHELL_PostEvent( pMe->a.m_pIShell,  AEECLSID_CORE_APP, EVT_DISPLAYDIALOGTIMEOUT,  0, 0);
                        }
                    
                        return TRUE;
                        
                    case POWERDOWN_RESET:
                        // 取"电话正在" 字符串
                        (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                AEE_COREAPPRES_LANGFILE,
                                IDS_RESET_1,
                                wszMsgText,
                                sizeof(wszMsgText));
                                
                        // 加换行符
                        (void)WSTRCAT(wszMsgText, wszNewLine);
                        
                        nLen = WSTRLEN(wszMsgText);
                        
                        // 取"重新设置..." 字符串
                        (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                AEE_COREAPPRES_LANGFILE,
                                IDS_RESET_2,
                                &wszMsgText[nLen],
                                (128 - nLen) * sizeof(AECHAR));
                        break;
                        
                    default:
                        break;
                }
                if(POWERDOWN_NORMAL != pMe->m_ePowerDownType)
                {
                    (void) IDISPLAY_DrawText(pMe->m_pDisplay,
                                    AEE_FONT_NORMAL,
                                    wszMsgText,  -1,  0,  0,  &pMe->m_rc,
                                    IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
                }
                // 立即更新屏幕                        
                //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            }            

            // 设置自动关闭对话框的定时器
//            (void)ISHELL_SetTimer(pMe->a.m_pIShell, 
//                                  RUIM_DOOR_OPEN_TIMER,
//                                  pMe);
            return TRUE;
                    
        case EVT_DIALOG_END:
//            (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
//                                      pMe);
            //IALERT_StopRingerAlert(pMe->m_pAlert);
            return TRUE;

        case EVT_DISPLAYDIALOGTIMEOUT:
        {
#if 0
            IBacklight   *pBacklight = NULL;
            ISHELL_CreateInstance(pMe->a.m_pIShell,
                                 AEECLSID_BACKLIGHT,
                                 (void**)&pBacklight);
            if(pBacklight != NULL)
            {
                IBACKLIGHT_TurnOff(pBacklight);
                IBACKLIGHT_Release(pBacklight);
            }
            CLOSE_DIALOG(DLGRET_OK);
            // 不再关心 IBatt 通知消息
            (void) ISHELL_RegisterNotify(pMe->a.m_pIShell,
                        AEECLSID_CORE_APP,
                        AEECLSID_BATT_NOTIFIER,
                        0);

            // Turn off RSSI indicator
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RSSI, ANNUN_STATE_OFF);
           
            // 关闭电池图标
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_BATT, ANNUN_STATE_OFF);
            //IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,FALSE);            
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            ICM_SetOperatingMode(pMe->m_pCM, AEECM_OPRT_MODE_PWROFF); 
                           
            return TRUE;
#else
            IALERT_StopRingerAlert(pMe->m_pAlert);
            CoreApp_Poweroff_Phone(pMe);
            CLOSE_DIALOG(DLGRET_OK);
            return TRUE;
#endif

        }
          
        default:
            break;
    }
    
    return FALSE;
} // IDD_POWERDOWN_Handler

/*==============================================================================
函数:
    CoreApp_UpdateIdleTimer

说明:
    用于每分钟更新日期时间显示的定时器函数。

参数:
    pUser [in]:这里必须是指向 Core Applet 对象结构的指针。

返回值:
    none

备注:
       
==============================================================================*/
static void CoreApp_UpdateIdleTimer(void *pUser)
{
    CCoreApp    *pMe = (CCoreApp *)pUser;
    uint32         dwSeconds;

    if (pMe == NULL) 
    {
        return;
    }
    
    // 更新日期和时间显示
    ISHELL_PostEventEx(pMe->a.m_pIShell, 
                       EVTFLG_ASYNC, 
                       AEECLSID_CORE_APP,
                       EVT_UPDATEIDLE,
                       0,0L);
    
    // 计算下次定时器定时时间，时间尽可能靠近下一分钟，做到显示时间精确
    dwSeconds = GETTIMESECONDS();
    
    if (FALSE == pMe->m_bAcquiredTime) 
    {
        // 20x365x24x60x60 = 630720000
        if (dwSeconds > 630720000) 
        {
            pMe->m_bAcquiredTime = TRUE;
        }
        else
        {
            // 重设分钟定时器, 系统没获到有效时间时，间隔 5 秒
            (void) ISHELL_SetTimer(pMe->a.m_pIShell,
                    (int32)(5 * 1000),
                    CoreApp_UpdateIdleTimer,
                    pMe);
                    
            return;
        }
    }
    
    if (dwSeconds > 0) 
    {
        dwSeconds %= 60;
        dwSeconds = (60 - dwSeconds);
    }
     
    if (dwSeconds == 0) 
    {
        dwSeconds = 60;
    }
    
    // 重设分钟定时器
    (void) ISHELL_SetTimer(pMe->a.m_pIShell,
                           (int32)(dwSeconds * 1000),
                           CoreApp_UpdateIdleTimer,
                           pMe);
}
 /*==============================================================================
函数:
    CoreApp_TimeKeyguard

说明:
    用于 定时锁键盘的函数

参数:
    pUser [in]:这里必须是指向 Core Applet 对象结构的指针。

返回值:
    none

备注:
       
==============================================================================*/
#ifdef FEATURE_KEYGUARD
static void CoreApp_TimeKeyguard(void *pUser)
{
    CCoreApp    *pMe = (CCoreApp *)pUser;
    pMe->m_b_set_lock = TRUE;
    if(pMe->m_b_set_lock)
    {
        OEMKeyguard_SetState(TRUE);
        CoreApp_UpdateBottomBar(pMe);
        //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
        pMe->m_b_set_lock = FALSE;
    }
}
#endif
/*==============================================================================
函数:
    CoreApp_SearchingTimer

说明:
    用于每分钟更新日期时间显示的定时器函数。

参数:
    pUser [in]:这里必须是指向 Core Applet 对象结构的指针。

返回值:
    none

备注:
       
==============================================================================*/
static void CoreApp_SearchingTimer(void *pUser)
{
    CCoreApp  *pMe = (CCoreApp *)pUser;

    if (( pMe == NULL ) || (IDD_IDLE != pMe->m_wActiveDlgID))
    {
        return;
    }

    ISHELL_PostEventEx(pMe->a.m_pIShell, 
                       EVTFLG_ASYNC, 
                       AEECLSID_CORE_APP,
                       EVT_UPDATEIDLE,
                       0,0L);

    (void)ISHELL_SetTimer(pMe->a.m_pIShell,
                          IDLESEARCHINTIME,
                          CoreApp_SearchingTimer,
                          pMe);
}

/*==============================================================================
函数:
    CoreApp_DrawBannerMessage

说明:
    函数用于SPN、待机问候语栏的文本绘制处理。

参数:
    pMe [In]: 指向 Core Applet 对象结构的指针。
       
返回值:
    无。

备注:
    刷新显示操作由调用者根据需要执行相关操作。
    
==============================================================================*/
static void CoreApp_DrawBannerMessage(CCoreApp    *pMe)
{
    AEERect   rc;
    AECHAR    *wszBuf = NULL;
    int32     nSize;
    int32     y;
    //AEECMSSInfo  *pssinfo = NULL;
    boolean   bSetsearchingTimer = FALSE;
    
    nSize = 32 * sizeof(AECHAR);
    wszBuf = (AECHAR *)MALLOC(nSize);
    if (NULL == wszBuf)
    {
        return;
    }
    
    // 先取消相关定时器
    (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
                              CoreApp_SearchingTimer,
                              pMe);

    // Determine displaying rectangle     
    y = 130; 
    SETAEERECT(&rc, 0, y, pMe->m_rc.dx, pMe->m_nLargeFontHeight);
    wszBuf[0] = 0;
    
    if(pMe->m_SYS_MODE_NO_SRV || pMe->bPlaneModeOn == TRUE)
    {// 搜网提示优先
#ifdef FEATURE_PLANEMODE
        byte planeMode_cfg;
        (void) ICONFIG_GetItem(pMe->m_pConfig,
                               CFGI_PLANEMODE,
                               &planeMode_cfg,
                               sizeof(planeMode_cfg));
        //OEMNV_PLANEMODE_QUERY == bytData 维持当前bPlaneModeOn的值
        if(OEMNV_PLANEMODE_OFF== planeMode_cfg)
        {
            pMe->bPlaneModeOn = FALSE;
        }
        else if(OEMNV_PLANEMODE_ON == planeMode_cfg)
        {
            pMe->bPlaneModeOn = TRUE;
        }
        
        if(TRUE == pMe->bPlaneModeOn)
        {
            (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                        AEE_COREAPPRES_LANGFILE,
                                        IDS_PLANEMODE,
                                        wszBuf,
                                        nSize);
        }
        else
#endif //FEATURE_PLANEMODE            
        {
            // 搜网提示优先
            bSetsearchingTimer = TRUE;
            (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                        AEE_COREAPPRES_LANGFILE,
                                        IDS_SEARCHING,
                                        wszBuf,
                                        nSize);
        }
    }
    else if(pMe->m_bemergencymode)
    {
        ISHELL_LoadResString(pMe->a.m_pIShell,
                                    AEE_COREAPPRES_LANGFILE,
                                    IDS_EMERGENCY_MODE,
                                    wszBuf,
                                    nSize);
    }
    else if (IsRunAsFactoryTestMode())
    {// 其次是厂测模式提示
        (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                    AEE_COREAPPRES_LANGFILE,
                                    IDS_FACTORYTESTMODE,
                                    wszBuf,
                                    nSize);
    }
    else
    {// 最后是正常情况下的提示
        // 获取待机问候语 
        CoreApp_GetSPN(pMe);
        
        if(pMe->svc_p_name[0] != 0)
        {
            WSTRLCPY(wszBuf,pMe->svc_p_name,32);
        }
        else
        {
#ifdef WIN32
            STRTOWSTR("WIN32BUILD", wszBuf, nSize);
#else
            STRTOWSTR(ver_modelname, wszBuf, nSize);
#endif
        }
    }
    // Display the string
    (void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_NORMAL,
                              wszBuf, -1,
                              0, 0, &rc, 
                              IDF_ALIGN_CENTER 
                              | IDF_ALIGN_MIDDLE 
                              | IDF_TEXT_TRANSPARENT);
    
    if (bSetsearchingTimer
#ifdef FEATURE_SID_LOCK
            && !pMe->m_bAcquiredTime
#endif
            )
    {
        (void)ISHELL_SetTimer(pMe->a.m_pIShell,
                              IDLESEARCHINTIME,
                              CoreApp_SearchingTimer,
                              pMe);
    }
    
    // 释放动态分配的内存
    FREE(wszBuf);
    //FREE(pssinfo);
}

/*==============================================================================
函数:
    CoreApp_UpdateDateTime

说明:
    用于更新日期时间和星期几的显示。

参数:
    pMe [In]: 指向 Core Applet 对象结构的指针。

返回值:
    无。

备注:
    刷新显示操作由调用者根据需要执行相关操作。
==============================================================================*/
static void CoreApp_UpdateDateTime(CCoreApp    *pMe)
{
    AECHAR      wszDate[64]= {0};
    AECHAR      wFormat[64]= {0};
    JulianType  jDate;
    AEERect     rc;
    AEERect     rc_week;
    //AECHAR      wszBuf[16]= {(AECHAR)'\0'}; //fj remark ,Define but no used
#ifndef FEATURE_CARRIER_SUDAN_SUDATEL
    uint16   nWeekResID;
    AEEDeviceInfo di;
#endif /*FEATURE_CARRIER_SUDAN_SUDATEL*/
    byte      bTFmt;
    
    if ( pMe == NULL ) 
    {
        return;
    }
    
    // 符合下列条件之一，函数立即返回
    if(pMe->m_bSuspended)
    {
        return;
    }

    CoreApp_Process_Rtc_Event(pMe);
    (void) ICONFIG_GetItem( pMe->m_pConfig,
                           CFGI_TIME_FORMAT,
                           &bTFmt,
                           sizeof(bTFmt));

    // 取当前日期
    if(pMe->m_bAcquiredTime)
    {
        GetJulianDate(GETTIMESECONDS(), &jDate);
    }
    else
    {

#if (defined(FEATURE_PM1000_RTC) || defined(FEATURE_PMIC_RTC))
        pm_rtc_julian_type time = {0};
        pm_rtc_rw_cmd(PM_RTC_GET_CMD, &time);
        jDate.wSecond = (uint16)time.second&0xFFFF;
        jDate.wMinute = (uint16)time.minute&0xFFFF;
        jDate.wHour = (uint16)time.hour&0xFFFF;
        jDate.wDay = (uint16)time.day&0xFFF;
        jDate.wMonth = (uint16)time.month&0xFFFF;
        jDate.wYear = (uint16)time.year&0xFFFF;
        jDate.wWeekDay = (uint16)time.day_of_week&0xFFFF;
#else
        jDate.wYear = 1980;
        jDate.wMonth = 1;
        jDate.wDay =6;
        jDate.wHour = 0;
        jDate.wMinute =0;
        jDate.wSecond = 0;
        jDate.wWeekDay = 6;
#endif
    }
#ifndef FEATURE_CARRIER_SUDAN_SUDATEL
    // 确定星期所用资源字符串
    switch (jDate.wWeekDay)
    {
        case 0: // 星期一
            nWeekResID = AEE_IDS_MONDAY;
            break;
            
        case 1: // 星期二
            nWeekResID = AEE_IDS_TUESDAY;
            break;

        case 2: // 星期三
            nWeekResID = AEE_IDS_WEDNESDAY;
            break;

        case 3: // 星期四
            nWeekResID = AEE_IDS_THURSDAY;
            break;

        case 4: // 星期五
            nWeekResID = AEE_IDS_FRIDAY;
            break;

        case 5: // 星期六
            nWeekResID = AEE_IDS_SATURDAY;
            break;

        default:  // 6:星期日
            nWeekResID = AEE_IDS_SUNDAY;
            break;
    }
#endif /*FEATURE_CARRIER_SUDAN_SUDATEL*/
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
#ifdef FEATURE_DISP_176X220
    SETAEERECT(&rc, 
               1,
               20,
               pMe->m_rc.dx-2, 
               pMe->m_nNormalFontHeight);
    SETAEERECT(&rc_week, 
               26,//1,
               160,//40, 
               130, 
               pMe->m_nNormalFontHeight);
#else
SETAEERECT(&rc, 
           1,
           15, 
           pMe->m_rc.dx-2, 
           pMe->m_nNormalFontHeight);
SETAEERECT(&rc_week, 
           (pMe->m_rc.dx-10)/2 ,
           15, 
           pMe->m_rc.dx-(pMe->m_rc.dx-10)/2, 
           pMe->m_nNormalFontHeight);
#endif //FEATURE_DISP_176X220
#else
    SETAEERECT(&rc, 
               1,
               1, 
               pMe->m_rc.dx-2, 
               pMe->m_nNormalFontHeight);
    SETAEERECT(&rc_week, 
               (pMe->m_rc.dx-10)/2 ,
               1, 
               pMe->m_rc.dx-(pMe->m_rc.dx-10)/2, 
               pMe->m_nNormalFontHeight);
#endif
    // 格式化时间字符串并绘制
    wFormat[0] = 0;
    wszDate[0] = 0;
    (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                AEE_COREAPPRES_LANGFILE,
                                IDS_TIME_TILE,
                                wFormat,
                                sizeof(wFormat));
    if (bTFmt == OEMNV_TIMEFORM_AMPM)
    {        
        uint16    wHour,len;
                                     
        wHour = jDate.wHour > 12 ? (jDate.wHour - 12) : jDate.wHour;
        if(jDate.wHour == 0)
        {
            wHour = 12;
        }
        WSPRINTF(wszDate, sizeof(wszDate), wFormat, wHour, jDate.wMinute);
        len = WSTRLEN(wszDate);
        if(jDate.wHour >= 12)
        {
            wszDate[len] = (AECHAR)'P';
            wszDate[len+1] = (AECHAR)'M';
            wszDate[len+2] = 0;
        }
        else
        {
            wszDate[len] = (AECHAR)'A';
            wszDate[len+1] = (AECHAR)'M';
            wszDate[len+2] = 0;
        }
    }
    else
    {
        WSPRINTF(wszDate, sizeof(wszDate), wFormat, jDate.wHour, jDate.wMinute);
    }
//    IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);//wlh test               
    // Display the string of date or time or weekday
	/*wlh 测试临时屏蔽
    (void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_NORMAL,
                              wszDate, -1,
                              0, 0, &rc, 
                              IDF_ALIGN_MIDDLE
                              | IDF_ALIGN_LEFT 
                              | IDF_TEXT_TRANSPARENT);
							  */
        
 //   IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);//wlh test  
    // 格式化日期字符串并绘制
    wszDate[0] = (AECHAR)'\0';
    wFormat[0] = (AECHAR)'\0';
#ifdef FEATURE_TIME_DATA_SETTING
    {
        //uint16 m_year = 0;
        ICONFIG_GetItem( pMe->m_pConfig, CFGI_DATE_FORMAT,&bTFmt, sizeof(bTFmt));

        //m_year = jDate.wYear -2000;
        switch(bTFmt)
        {
            case OEMNV_DATEFORM_DMY:
#ifdef FEATURE_DISP_176X220
                WSTRLCPY(wFormat,L"%02d/%02d/%04d",63);
#else
                WSTRLCPY(wFormat,L"%02d/%02d",63);
#endif //FEATURE_DISP_176X220
                WSPRINTF(wszDate, sizeof(wszDate), wFormat, jDate.wDay, jDate.wMonth, jDate.wYear);
                break;
                
            case OEMNV_DATEFORM_MDY:
#ifdef FEATURE_DISP_176X220                
                WSTRLCPY(wFormat,L"%02d/%02d/%04d",63);
#else
                WSTRLCPY(wFormat,L"%02d/%02d",63);
#endif //FEATURE_DISP_176X220
                WSPRINTF(wszDate, sizeof(wszDate), wFormat, jDate.wMonth, jDate.wDay, jDate.wYear);
                break;
                
            default:
            case OEMNV_DATEFORM_YMD:
#ifdef FEATURE_DISP_176X220                
                WSTRLCPY(wFormat,L"%04d/%02d/%02d",63);
#else
                WSTRLCPY(wFormat,L"%02d/%02d",63);
#endif //FEATURE_DISP_176X220
                WSPRINTF(wszDate, sizeof(wszDate), wFormat,  jDate.wYear, jDate.wMonth,jDate.wDay);
                break;
        }
#ifdef FEATURE_DISP_176X220          
        DrawTextWithProfile(pMe->a.m_pIShell,
                                  pMe->m_pDisplay,
                                  RGB_WHITE_NO_TRANS,
                                  AEE_FONT_NORMAL,
                                  &wszDate[0], -1,
                                  0, 0, &rc_week, 
                                  IDF_ALIGN_MIDDLE
                                  | IDF_ALIGN_LEFT
                                  | IDF_TEXT_TRANSPARENT); 
#else
        DrawTextWithProfile(pMe->a.m_pIShell,
                                  pMe->m_pDisplay,
                                  RGB_WHITE_NO_TRANS,
                                  AEE_FONT_NORMAL,
                                  &wszDate[0], -1,
                                  0, 0, &rc, 
                                  IDF_ALIGN_MIDDLE
                                  | IDF_ALIGN_RIGHT
                                  | IDF_TEXT_TRANSPARENT); 
#endif //FEATURE_DISP_176X220
    }
#else /*FEATURE_TIME_DATA_SETTING*/
    //Get date format     
    (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                AEE_COREAPPRES_LANGFILE,
                                IDS_MONTH_DATE,
                                wFormat,
                                sizeof(wFormat));

    WSPRINTF(wszDate, sizeof(wszDate), wFormat,jDate.wYear, jDate.wDay, jDate.wMonth);
#ifdef FEATURE_DISP_176X220
    (void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_NORMAL,
                              &wszDate[5], -1,
                              0, 0, &rc_week, 
                              IDF_ALIGN_MIDDLE
                              | IDF_ALIGN_LEFT
                              | IDF_TEXT_TRANSPARENT);  
#else
    (void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_NORMAL,
                              &wszDate[5], -1,
                              0, 0, &rc, 
                              IDF_ALIGN_MIDDLE
                              | IDF_ALIGN_RIGHT
                              | IDF_TEXT_TRANSPARENT);  
#endif //FEATURE_DISP_176X220

#endif /*FEATURE_TIME_DATA_SETTING*/

#ifndef FEATURE_CARRIER_SUDAN_SUDATEL
    // 绘制星期字符串
    wszDate[0] = 0;
    wFormat[0] = 0;
    
    (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                OEMAEECONTROLS_LNGRES_FILE,
                                nWeekResID,
                                wszDate,
                                sizeof(wszDate));
    
    wszDate[3] = (AECHAR)'\0';
    (void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_NORMAL,
                              wszDate, -1,
                              0, 0, &rc_week, 
                              IDF_ALIGN_MIDDLE
#ifdef FEATURE_DISP_176X220                              
                              | IDF_ALIGN_RIGHT
#else
                              | IDF_ALIGN_LEFT
#endif //FEATURE_DISP_176X220
                              | IDF_TEXT_TRANSPARENT);
#endif /*FEATURE_CARRIER_SUDAN_SUDATEL*/
            
} // CoreApp_UpdateDateTime

// 绘制待机界面的 "Menu         Contacts"
static void CoreApp_UpdateBottomBar(CCoreApp    *pMe)
{
    BottomBar_e_Type    eBBarType = BTBAR_NONE;
    IImage *pBottomMask;
	IImage *pBottomMaskleft;
	IImage *pBottomMaskcenter;
	IImage *pBottomMaskright;
    
    if(pMe->m_bemergencymode)
    {
        eBBarType = BTBAR_BACK;
    }
#ifdef FEATURE_KEYGUARD
    else if(OEMKeyguard_IsEnabled())
    {
        eBBarType = BTBAR_UNLOCK_SOS;
    }
#endif
    else
    {
        eBBarType = BTBAR_MENU_CONTACTS;
    }

    //draw idle bottom zone background
    pBottomMask       = ISHELL_LoadResImage(pMe->a.m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_BOTTOMMASK);
	pBottomMaskleft   = ISHELL_LoadResImage(pMe->a.m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_BOHAO);
	pBottomMaskcenter = ISHELL_LoadResImage(pMe->a.m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CAIDAN);
	pBottomMaskright  = ISHELL_LoadResImage(pMe->a.m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_DIANHUABEN);
    if((NULL != pBottomMask) || (NULL != pBottomMaskleft) || (NULL != pBottomMaskcenter) || (NULL != pBottomMaskright))
    {
        IIMAGE_Draw(pBottomMask, pMe->m_rc.x, pMe->m_rc.dy - 40);//BOTTOMBAR_HEIGHT);
        IIMAGE_Release(pBottomMask);
        pBottomMask = NULL;

		IIMAGE_Draw(pBottomMaskleft, pMe->m_rc.x + 11, pMe->m_rc.dy - 38);
        IIMAGE_Release(pBottomMaskleft);
        pBottomMaskleft = NULL;

		IIMAGE_Draw(pBottomMaskcenter, pMe->m_rc.x + 11+36+11+1+11, pMe->m_rc.dy - 38);
        IIMAGE_Release(pBottomMaskcenter);
        pBottomMaskcenter = NULL;

		IIMAGE_Draw(pBottomMaskright, pMe->m_rc.x + 11+36+11+1+11+36+11+1+11, pMe->m_rc.dy - 38);
        IIMAGE_Release(pBottomMaskright);
        pBottomMaskright = NULL;
    }
    //DrawBottomBar_Ex(pMe->a.m_pIShell, pMe->m_pDisplay,eBBarType);
}

/*==============================================================================
函数     : CoreApp_PlayPwrOnAni

说明     :  开关机动画有可能不一样，考虑的函数的耦合，
                   开关机动画播放分别用不同的函数

参数     :  pMe [in]: 指向 Core Applet 对象结构的指针。

返回     :  无 

备注     :  WHEN                    WHO                WHAT                     WHY
           2008-02-17              fj                   V1.6                       New Dev, for add play poweroff animation
==============================================================================*/
static void CoreApp_PlayPwrOnAni(CCoreApp *pMe) 
{
    AEEImageInfo  ImgInfo;  //Gets the information about an image

    ASSERT(pMe != NULL);
    CORE_ERR("%x %x  CoreApp_PlayPwrOnAni",pMe->m_pStartupAniImg,pMe->m_wStartupAniTime);
    if ( (NULL != pMe->m_pStartupAniImg) && (pMe->m_wStartupAniTime < 1)  )
    {
        IIMAGE_GetInfo( pMe->m_pStartupAniImg, &ImgInfo );

        // 设置动画速度(毫秒)
        IIMAGE_SetAnimationRate(pMe->m_pStartupAniImg, PWRON_ANI_RATE);

        // 设置图像中的帧数
        IIMAGE_SetFrameCount(pMe->m_pStartupAniImg, PWRON_ANI_FRAME_COUNT);

        // 设置要显示的图像的实际大小
        IIMAGE_SetDrawSize( pMe->m_pStartupAniImg, ImgInfo.cx/PWRON_ANI_FRAME_COUNT, ImgInfo.cy );

        // 开始播放开机动画
        IIMAGE_Start( pMe->m_pStartupAniImg,
                            (pMe->m_rc.dx - ImgInfo.cx/PWRON_ANI_FRAME_COUNT)/2,
                            (pMe->m_rc.dy - ImgInfo.cy)/2 );

        pMe->m_wStartupAniTime++; // 滚动播放次数
        //AEE_SetSysTimer( PWRON_ANI_TIME,  (PFNNOTIFY)CoreApp_PlayPwrOnAni,  (void*)pMe);
       (void) ISHELL_SetTimer(pMe->a.m_pIShell,
                             PWRON_ANI_TIME,
                             (PFNNOTIFY)CoreApp_PlayPwrOnAni,
                             (void*)pMe);

    }
    else
    {
#if 0
        if ( NULL != pMe->m_pStartupAniImg )
        {
            IIMAGE_Stop(pMe->m_pStartupAniImg);
            IIMAGE_Release(pMe->m_pStartupAniImg);
            pMe->m_pStartupAniImg = NULL;
        }
        // 发送事件关闭开机动画播放对话
        (void)ISHELL_PostEvent( pMe->a.m_pIShell,  AEECLSID_CORE_APP,  EVT_DISPLAYDIALOGTIMEOUT,  0, 0);
#else
        IALERT_StopRingerAlert(pMe->m_pAlert);
        if ( NULL != pMe->m_pStartupAniImg )
        {     
            IIMAGE_Stop(pMe->m_pStartupAniImg);
            IIMAGE_Release(pMe->m_pStartupAniImg);
            pMe->m_pStartupAniImg = NULL;
        }
        CLOSE_DIALOG(DLGRET_OK)
#endif
    }
    
}

/*==============================================================================
函数     : CoreApp_PlayPwrOffAni

说明     :  用于播放关机动画，开关机动画有可能不一样，考虑的函数的耦合，
                   开关机动画播放分别用不同的函数

参数     :  pMe [in]: 指向 Core Applet 对象结构的指针。

返回     :  无 

备注     :  WHEN                    WHO                WHAT                     WHY
           2008-02-17              fj                  V1.6                     New Dev, for add play poweroff animation
==============================================================================*/
static void CoreApp_PlayPwrOffAni(CCoreApp *pMe) 
{
    AEEImageInfo  ImgInfo;  //Gets the information about an image

    ASSERT(pMe != NULL);

    if ( (NULL != pMe->m_pStartupAniImg) && (pMe->m_wStartupAniTime < 1)  ) 
    {      
        IIMAGE_GetInfo( pMe->m_pStartupAniImg, &ImgInfo );

        // 设置动画速度(毫秒)
        IIMAGE_SetAnimationRate(pMe->m_pStartupAniImg, PWROFF_ANI_RATE);

        // 设置图像中的帧数
        IIMAGE_SetFrameCount(pMe->m_pStartupAniImg, PWROFF_ANI_FRAME_COUNT);

        // 设置要显示的图像的实际大小
        IIMAGE_SetDrawSize( pMe->m_pStartupAniImg, 
                                ImgInfo.cx/PWROFF_ANI_FRAME_COUNT, ImgInfo.cy );

        // 开始播放开机动画
        IIMAGE_Start( pMe->m_pStartupAniImg, 
                                (pMe->m_rc.dx - ImgInfo.cx/PWROFF_ANI_FRAME_COUNT)/2, 
                                (pMe->m_rc.dy - ImgInfo.cy)/2 );

        pMe->m_wStartupAniTime++; // 滚动播放次数
        AEE_SetSysTimer( PWRON_ANI_TIME,  (PFNNOTIFY)CoreApp_PlayPwrOffAni,  (void*)pMe);
        //(void) ISHELL_SetTimer(pMe->a.m_pIShell,
        //                      PWROFF_ANI_TIME,
        //                      (PFNNOTIFY)CoreApp_PlayPwrOffAni,
        //                      (void*)pMe);
    }
    else
    {
        if ( NULL != pMe->m_pStartupAniImg )
        {     
            IIMAGE_Stop(pMe->m_pStartupAniImg);
            IIMAGE_Release(pMe->m_pStartupAniImg);
            pMe->m_pStartupAniImg = NULL;
        }    
        // 发送事件关闭开机动画播放对话
        (void)ISHELL_SendEvent( pMe->a.m_pIShell,  AEECLSID_CORE_APP, 
                                EVT_DISPLAYDIALOGTIMEOUT,  0, 0);
    }

}

/*==============================================================================
函数      :  CoreApp_DrawWallPaper

说明      :  函数用于绘制墙纸, 绘制墙纸后不会进行LCD的刷新,
                  LCD的刷新由调用者管理。

参数      :  pMe [In]: 指向Idle Applet对象结构的指针,该结构包含小程序的特定信息.
       
返回值 : 无。

备注     :  WHEN                    WHO                WHAT                     WHY
           2008-02-20              fj                  V1.6                    New Dev, for Wallpaper
==============================================================================*/
static void CoreApp_DrawWallPaper(CCoreApp *pMe) 
{
    AEEImageInfo   ImgInfo;
    static char        szWallPaperName[AEE_MAX_FILE_NAME/*FILESPECLEN*/];
    char                 szNewWallPaperName[AEE_MAX_FILE_NAME/*FILESPECLEN*/];
    //int                    nX = 0,  nY = 0;
    
    if ( (NULL == pMe) || (IDD_IDLE != pMe->m_wActiveDlgID) )
    {
        return;
    }

    MEMSET( &ImgInfo, 0x00, sizeof(ImgInfo) );
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
    pMe->m_capture = DB_CAPTURE_NONE;
#endif
    if ( NULL == pWallPaper )
    {  
        // 初始化墙纸或上次取墙纸不成功
        MEMSET(szWallPaperName, 0x00, sizeof(szWallPaperName));
        
        // 取设定的墙纸文件名
        (void) ICONFIG_GetItem(pMe->m_pConfig,
                                CFGI_WALLPAPER,
                                szWallPaperName,
                                sizeof(szWallPaperName));

        // test the file existed or not, if not existed, load the default wallpaper.
        {
            IFileMgr *pFileMgr = NULL;
            ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pFileMgr);
            if(pFileMgr)
            {
                if(IFILEMGR_Test(pFileMgr, szWallPaperName) == SUCCESS)
                {
                    pWallPaper = ISHELL_LoadImage(pMe->a.m_pIShell, szWallPaperName);
                }
                else // if specified wallpaper not existed, load the default wallpaper
                {
                    pWallPaper = ISHELL_LoadImage(pMe->a.m_pIShell, OEMNV_WALLPAPER);
                }
                IFILEMGR_Release(pFileMgr);
                pFileMgr = NULL;
            }
        }
        
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
        pMe->m_capture = DB_CAPTURE_NEED;
#endif
    }
    else
    {   
        // 已经成功加载墙纸图片，但须检查墙纸图片设置是否被变更
        MEMSET(szNewWallPaperName, 0x00, sizeof(szNewWallPaperName));
        
        // 取设定的墙纸文件名
        (void) ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_WALLPAPER,
                          szNewWallPaperName,
                          sizeof(szNewWallPaperName));
                          
        if ( 0 != STRCMP(szNewWallPaperName, szWallPaperName) )
        {   
            // 墙纸设置已变
            MEMSET( szWallPaperName, 0x00, sizeof(szWallPaperName) );
            (void)STRCPY( szWallPaperName, szNewWallPaperName );
            
            // 释放先前图片占用的资源
            IIMAGE_Release(pWallPaper);
            
            // test the file existed or not, if not existed, load the default wallpaper.
            {
                IFileMgr *pFileMgr = NULL;
                ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pFileMgr);
                if(pFileMgr)
                {
                    if(IFILEMGR_Test(pFileMgr, szWallPaperName) == SUCCESS)
                    {
                        pWallPaper = ISHELL_LoadImage(pMe->a.m_pIShell, szWallPaperName);
                    }
                    else // if specified wallpaper not existed, load the default wallpaper
                    {
                        pWallPaper = ISHELL_LoadImage(pMe->a.m_pIShell, OEMNV_WALLPAPER);
                    }
                    IFILEMGR_Release(pFileMgr);
                    pFileMgr = NULL;
                }
            }
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
            pMe->m_capture = DB_CAPTURE_NEED;
#endif
        }
    }
#if 0
    if ( NULL != pWallPaper )
    {
        IIMAGE_GetInfo(pWallPaper, &ImgInfo);
        
        nX = (ImgInfo.cx >= pMe->m_rc.dx)?  0 : ( (pMe->m_rc.dx - ImgInfo.cx)/2 );
        //#if !defined(LCD_OPIMIZE_ICON)
        //nY = (ImgInfo.cy >= pMe->m_rc.dy)?  12 : ( (pMe->m_rc.dy - ImgInfo.cy - 12)/2 + 12 );
        //#else   
        nY = (ImgInfo.cy >= pMe->m_rc.dy)?  0 : ( (pMe->m_rc.dy - ImgInfo.cy )/2 );
        //#endif 
        
        IIMAGE_Draw(pWallPaper, nX, nY);
    }
    else
    {
        IDISPLAY_EraseRect(pMe->m_pDisplay, &pMe->m_rc);
    }
#endif
}



/*==============================================================================
函数：
      CoreApp_LaunchApplet

说明：
       用于启动指定 Applet。

参数：
       pMe [in]：指向Core Applet对象结构的指针。该结构包含小程序的
                          特定信息。
       classID [in]：指定 Applet 的 类 ID 号。
返回值：
        TRUE：启动成功
        FALSE：当前不能启动或启动失败

备注：
       
==============================================================================*/
static boolean CoreApp_LaunchApplet(CCoreApp *pMe,  AEECLSID classID)
{

    if (!ISHELL_CanStartApplet(pMe->a.m_pIShell, classID)) 
    {
        // 当前不能启动 指定 Applet
        ERR("Can't start Applet 0x%X",classID,0,0);
        return FALSE;
    }

    if (SUCCESS != ISHELL_StartApplet(pMe->a.m_pIShell, classID)) 
    {
        ERR("Failed to start Applet 0x%X",classID,0,0);
        return FALSE;
    } 

    return TRUE;
}

/*==============================================================================
函数:
    CoreApp_UpdateAnnunciator

说明:
    用于更新Annunciator的显示。

参数:
    pMe [In]: 指向 Core Applet 对象结构的指针。

返回值:
    无。

备注:
    刷新显示操作由调用者根据需要执行相关操作。
==============================================================================*/
void CoreApp_UpdateAnnunciator(CCoreApp *pMe)
{
    /*Set head set icon*/
    uint32 iann    = 0;
    byte alertType;
    boolean b_FMBackground = FALSE;
    boolean b_headset = FALSE;
    boolean missed_call_icon;
    //static boolean b_flag = TRUE;

    ICONFIG_GetItem(pMe->m_pConfig, CFGI_HEADSET_PRESENT, &b_headset, sizeof(b_headset));
    ICONFIG_GetItem(pMe->m_pConfig, CFGI_FM_BACKGROUND, &b_FMBackground, sizeof(b_FMBackground));
    ICONFIG_GetItem(pMe->m_pConfig, CFGI_PROFILE_CUR_NUMBER,&alertType, sizeof(alertType));//CFGI_ALERT_TYPE
    ICONFIG_GetItem(pMe->m_pConfig, CFGI_MISSED_CALL_ICON,&missed_call_icon, sizeof(missed_call_icon));

    if (b_headset)
    {
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_HEADSET*/, ANNUN_STATE_HEADSET_ON/*ANNUN_STATE_ON*/);
    if (b_FMBackground)
    {
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO, ANNUN_STATE_FMRADIO_ON/*ANNUN_STATE_ON*/);
    }
    }
    else
    {
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO, ANNUN_STATE_FMRADIO_OFF/*ANNUN_STATE_OFF*/);
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_FMRADIO/*ANNUN_FIELD_HEADSET*/, ANNUN_STATE_HEADSET_OFF/*ANNUN_STATE_OFF*/);
    }
    
    switch(alertType)
    {
        case OEMNV_PROFILE_QUIETMODE:           //OEMNV_ALERTTYPE_OFF :
            iann = ANNUN_STATE_RINGTONE_SILENT;//ANNUN_FIELD_SILENT;
            break;

        case OEMNV_PROFILE_NORMALMODE:         //OEMNV_ALERTTYPE_RINGER :
            iann = ANNUN_STATE_RINGTONE_ALERT;//ANNUN_FIELD_RING;
            break;

        case OEMNV_PROFILE_MEETING:              //OEMNV_ALERTTYPE_VIB :
            iann = ANNUN_STATE_RINGTONE_VIBRATOR;//ANNUN_FIELD_VIBRATE;
            break;

        case OEMNV_PROFILE_NOISEMODE:           //OEMNV_ALERTTYPE_VIBRINGER :
            iann = ANNUN_STATE_RINGTONE_VIBRING;//ANNUN_FIELD_ALERT;
            break;

        case OEMNV_PROFILE_CARMODE:             //OEMNV_ALERTTYPE_VIBANDRINGER :
            iann = ANNUN_STATE_RINGTONE_VIBRING;//ANNUN_FIELD_ALERT;
            break;

        default :
            break;            
    }
    IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, iann);
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
    {
        uint32 state = ANNUN_STATE_OFF;
        uint16 i = 0;
        
        /*
        IANNUNCIATOR_GetField (pMe->m_pIAnn,ANNUN_FIELD_RSSI,&state);
        if(state != ANNUN_STATE_OFF)
        {
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RSSI, state);
        }
        */
        // 此处已经设置了全部field的状态，不需要再单独为某个field进行设置
        for (i = ANNUN_FIELD_RSSI; i <=ANNUN_FIELD_BATT; i++) 
        {
            state = ANNUN_STATE_OFF;
            IANNUNCIATOR_GetField (pMe->m_pIAnn,i,&state);
            //CORE_ERR("GetField i= %d state= %d",i,state);
            if(state != ANNUN_STATE_OFF)
            {
                IANNUNCIATOR_SetField (pMe->m_pIAnn, i, state);
            }
        }
        /*
        IANNUNCIATOR_GetField (pMe->m_pIAnn,ANNUN_FIELD_BLUETOOTH,&state);
        if(state != ANNUN_STATE_OFF)
        {
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_BLUETOOTH, state);
        }
        */
    }
#endif
    if (missed_call_icon)
    {
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_CALL, ANNUN_STATE_CALL_MISSEDCALL_ON);
    }
}

static void CoreApp_Process_Rtc_Event(CCoreApp *pMe)
{
    static boolean have_run = FALSE;
    if(!have_run)
    {
        uint32 current_time = 0;
        JulianType current_time_ex;
        current_time = GETTIMESECONDS();
        GETJULIANDATE(current_time, &current_time_ex);
        //DBGPRINTF("%d %d:%d current_time_ex",current_time_ex.wYear,current_time_ex.wMonth,current_time_ex.wDay);
        //DBGPRINTF("%d %d:%d current_time_ex",current_time_ex.wHour,current_time_ex.wMinute,current_time_ex.wSecond);
        if(current_time_ex.wYear < 2008)
        {
            JulianType *get_rtc_time = NULL;
#ifndef WIN32
            get_rtc_time = (JulianType*)OEMRTC_hs_bsp_test_rtc_get();
#endif//WIN32
            if(get_rtc_time->wYear < 2008)
            {
                return ;
            }
        }
        
#ifdef FEATRUE_AUTO_POWER
#ifndef WIN32
        OEMRTC_Process_Auto_Power_Down(pMe->a.m_pIShell);
        OEMRTC_Process_Auto_Power_On();
#endif//WIN32
#endif //#ifdef FEATRUE_AUTO_POWER        
        //jignag 20080213 add
        //OEMRTC_Process_PowerDown_Alarm();

#if defined( FEATURE_POWERDOWN_ALARM)
        registerPowerdownAlarmclock();
#endif
        have_run = TRUE;
    }
    return ;
}

//static  boolean CoreApp_PowerAlarm_Event(CCoreApp *pMe)
//{
//
//        uint32 current_time = 0;
//        JulianType current_time_ex;
//        current_time = GETTIMESECONDS();
//        GETJULIANDATE(current_time, &current_time_ex);
//        if(current_time_ex.wYear < 2008)
//        {
//            JulianType *get_rtc_time = NULL;
//            get_rtc_time = (JulianType*)OEMRTC_hs_bsp_test_rtc_get();
//            if(get_rtc_time->wYear < 2008)
//            {
//                return FALSE;
//            }
//        }
//        OEMRTC_hs_bsp_test_rtc_set((void *)&current_time_ex);
//
//        OEMRTC_Process_PowerDown_Alarm();
//}
#if 0
/*static void CoreApp_GetRecordCount(CCoreApp *pMe)
{
    OEMCallHistoryEntry  ent;
    ICallList      *m_pCallList = NULL;
    IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_MISSEDCALL,ANNUN_STATE_OFF);
    //CORE_ERR("Count1");
    if (AEE_SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CALLLIST, (void **)&m_pCallList))
    {
        //CORE_ERR("Count2");
        if( ICALLLIST_GetEntry(m_pCallList, CALLHISTORY_MISSED_CATEGORY,0,&ent))
        {
            //CORE_ERR("Count3");
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_MISSEDCALL,ANNUN_STATE_ON);
        }
        ICALLLIST_Release(m_pCallList);
        m_pCallList = NULL;
    }
}*/
#endif
void CoreApp_Poweroff_Phone(void *pp)
{
    CCoreApp *pMe = (CCoreApp *)pp;
    IBacklight   *pBacklight = NULL;
    boolean b_FMBackground = FALSE;
    
    ISHELL_CreateInstance(pMe->a.m_pIShell,
                         AEECLSID_BACKLIGHT,
                         (void**)&pBacklight);
    if(pBacklight != NULL)
    {
        IBACKLIGHT_TurnOff(pBacklight);
        IBACKLIGHT_Release(pBacklight);
    }
    // 不再关心 IBatt 通知消息
    (void) ISHELL_RegisterNotify(pMe->a.m_pIShell,
                AEECLSID_CORE_APP,
                AEECLSID_BATT_NOTIFIER,
                0);
    
    ICONFIG_SetItem(pMe->m_pConfig, CFGI_FM_BACKGROUND, &b_FMBackground, sizeof(b_FMBackground));

    // Turn off RSSI indicator
    IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RSSI, ANNUN_STATE_OFF);
   
    // 关闭电池图标
    IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_BATT, ANNUN_STATE_OFF);
    //IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,FALSE);
    //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
    ICM_SetOperatingMode(pMe->m_pCM, AEECM_OPRT_MODE_PWROFF);                    
}

void CoreApp_Draw_Charger_image(void *pp)
{
    CCoreApp *pMe = (CCoreApp *)pp;
    AEEChargerStatus status;
    status = IBATT_GetChargerStatus(pMe->m_pBatt);
    if(status  != AEECHG_STATUS_CHARGING)
    {
        ISHELL_PostEvent(pMe->a.m_pIShell,AEECLSID_CORE_APP,EVT_USER_REDRAW,0,0);
        return;
    }
    //CORE_ERR("%d  %d Charger_image",pMe->m_battery_count,pMe->m_bExtPwrState);
    IDISPLAY_ClearScreen(pMe->m_pDisplay);
    if(pMe->m_battery_count >= CHARGING_FRAME_COUNT)
    {
        pMe->m_battery_count = 0;
    }
    if(NULL != pMe->m_battery_Image)
    {
        IIMAGE_DrawFrame(pMe->m_battery_Image, pMe->m_battery_count, 0, 0);
    }
    /*if (NULL != pMe->m_battery_Image[pMe->m_battery_count])
    {
        IIMAGE_Draw(pMe->m_battery_Image[pMe->m_battery_count], 0, 0);
    }*/
    pMe->m_battery_count ++; 
#ifndef CUST_EDITION    
    if(g_b_no_batt)
    {
           ISHELL_CancelTimer(pMe->a.m_pIShell, CoreApp_Draw_Charger_image, pMe);
           IDISPLAY_DrawText(pMe->m_pDisplay,
                                                     AEE_FONT_BOLD,
                                                     L"NO BATTERY", -1,
                                                     0, 0, &pMe->m_rc, 
                                                     IDF_ALIGN_MIDDLE|IDF_TEXT_TRANSPARENT| 
                                                     IDF_ALIGN_CENTER);
           //IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);

           return;
    }
#endif    
    //IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
    ISHELL_SetTimer(pMe->a.m_pIShell, CHARGING_ANIMATE_RATE,CoreApp_Draw_Charger_image, pMe);
}
#ifdef FEATURE_UTK2
/*==============================================================================
函数：
    CoreApp_CloseRefreshDlgTimer

说明：
    用于发送 EVT_DISPLAYDIALOGTIMEOUT 给 AEECLSID_CORE_APP 通知程序关闭
    当前对话框。

参数：
    pme [in]：指向CoreApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
    无。

备注：
    若 Applet 被挂起，将不做任何操作直接返回。
       
==============================================================================*/
static void CoreApp_CloseRefreshDlgTimer(void *pme)
{
    CCoreApp *pMe = (CCoreApp *)pme;
    db_items_value_type  db_item;
    boolean  bIninted = FALSE;
    
    if (NULL == pMe)
    {
        return;
    }
    
    if (pMe->m_bSuspended)
    {
        return;
    }
#ifndef WIN32    
    db_get(DB_UIMSMSINIT, &db_item);
    bIninted = db_item.db_uimsmsinited;
    db_get(DB_UIMADDINIT, &db_item);
    bIninted = (bIninted && db_item.db_uimaddinited);
    if(!bIninted)
    {
        (void)ISHELL_SetTimer(pMe->a.m_pIShell, 
                          500,
                          CoreApp_CloseRefreshDlgTimer, 
                          pMe);
        return;
    }
#endif
	// 发送EVT_DISPLAYDIALOGTIMEOUT事件
    (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                      AEECLSID_CORE_APP,
                      EVT_DISPLAYDIALOGTIMEOUT,
                      0,
                      0);
}

/*==============================================================================
函数：
    IDD_UTKREFRESH_Handler
       
说明：
    IDD_UTKREFRESH 对话框事件处理函数
       
参数：
    pMe [in]：指向CoreApp Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]：事件代码。
    wParam：事件相关数据。
    dwParam：事件相关数据。
       
返回值：
    TRUE：传入事件被处理。
    FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  IDD_UTKREFRESH_Handler(void *pUser,
                                   AEEEvent   eCode,
                                   uint16     wParam,
                                   uint32     dwParam)
{
    PARAM_NOT_REF(dwParam)    
    IStatic * pStatic;    
    CCoreApp *pMe = (CCoreApp *)pUser;    
    
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    pStatic = (IStatic*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_UTKREFRESH_STATIC);

    if (NULL == pStatic)
    {
        return FALSE;
    }
    
    if (NULL == pStatic)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            // 设置静态文本控件矩形尺寸
            ISTATIC_SetRect(pStatic, &pMe->m_rc);
            
            // 设置静态文本控件属性
            ISTATIC_SetProperties(pStatic, ST_CENTERTEXT | ST_MIDDLETEXT);
            return TRUE;
            
        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent(pMe->a.m_pIShell,
                                    AEECLSID_CORE_APP,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;
            
        case EVT_USER_REDRAW:
            {
                AECHAR    wszMsgText[32];
                db_items_value_type db_item;
                
                (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                            AEE_COREAPPRES_LANGFILE,
                                            IDS_UTKREFRESH,
                                            wszMsgText,
                                            sizeof(wszMsgText));
                
                // 设置静态文本控件文本
                (void)ISTATIC_SetText(pStatic, 
                                      NULL, 
                                      wszMsgText, 
                                      AEE_FONT_NORMAL, 
                                      AEE_FONT_NORMAL);

                (void)ISTATIC_Redraw(pStatic);
                
#ifdef FEATURE_CDSMS_RUIM
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
#ifndef WIN32
                // 通知短信刷新卡上短信
                if ((pMe->wRefreshMask == REFRESHMASK_UIMALL) ||
                    (pMe->wRefreshMask & REFRESHMASK_UIMSMS))
                {
                    IWmsApp *pWmsApp=NULL;
                    
                    db_item.db_uimsmsinited = FALSE;
                    db_put(DB_UIMSMSINIT, &db_item);
                    
                    if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WMSAPP,
                            (void**)&pWmsApp) == SUCCESS)
                    {
                        IWmsApp_RefreshRUIMSMS(pWmsApp);
                        
                        IWmsApp_Release(pWmsApp);
                    }
                }
#endif
#endif
#endif
#ifdef FEATURE_RUIM_PHONEBOOK
                // 先通过释放接口来释放初始化的 Buffer
                if ((pMe->wRefreshMask == REFRESHMASK_UIMALL) ||
                    (pMe->wRefreshMask & REFRESHMASK_UIMADD))
                {
                    extern void OEMRUIMAddr_Refresh(void);
#ifndef WIN32                    
                    db_item.db_uimaddinited = FALSE;
                    db_put(DB_UIMADDINIT, &db_item);
                    
                    OEMRUIMAddr_Refresh();
#endif
                }
#endif

                // 设置自动关闭对话框的定时器
                (void)ISHELL_SetTimer(pMe->a.m_pIShell, 
                                      500,
                                      CoreApp_CloseRefreshDlgTimer, 
                                      pMe);
            }
            
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;
            
        case EVT_DISPLAYDIALOGTIMEOUT:
            CLOSE_DIALOG(DLGRET_OK)
            return TRUE;
        
        case EVT_KEY:
            return TRUE;
        
        default:
            break;
    }
    
    return FALSE;
} // IDD_UTKREFRESH_Handler
#endif /* FEATURE_UTK2 */

#ifdef FEATURE_APP_MUSICPLAYER
/*==============================================================================
函数:
    CoreApp_DrawMusicName

说明:
    绘制背景播放的音乐名字

参数:
    pMe [In]: 指向 Core Applet 对象结构的指针。
       
返回值:
    无。

备注:
    刷新显示操作由调用者根据需要执行相关操作。
    
==============================================================================*/
#if 0
static void CoreApp_DrawMusicName(CCoreApp    *pMe)
{
    AEERect rect;
    AECHAR musicName[DISP_MUSICNAME_MAX + 5] = {0};
    int pos = 0;
    int rnt = 0;

    if(NULL == pMe || MP3STATUS_RUNONBACKGROUND != GetMp3PlayerStatus() || NULL ==pMe->m_pMusicName)
    {
        return ;
    }
    musicName[pos++] = (AECHAR)'[';
    musicName[pos++] = (AECHAR)' ';
    //only display first 20 AECHAR
    if(DISP_MUSICNAME_MAX < WSTRLEN(g_pMp3FileToPlay))
    {
        //'[' take place musicName[0] and "..."take three
        rnt = WSTRLCPY(&musicName[pos], g_pMp3FileToPlay, DISP_MUSICNAME_MAX - 3 + 1);
        pos += (DISP_MUSICNAME_MAX - 3);
        musicName[pos++] = (AECHAR)'.';
        musicName[pos++] = (AECHAR)'.';
        musicName[pos++] = (AECHAR)'.';
    }
    else
    {
        rnt = WSTRLCPY(&musicName[pos], g_pMp3FileToPlay, WSTRLEN(g_pMp3FileToPlay) + 1);
        pos += WSTRLEN(g_pMp3FileToPlay);
    }
    musicName[pos++] = (AECHAR)' ';
    musicName[pos++] = (AECHAR)']';
    musicName[pos] = (AECHAR)'\0';
    
    SETAEERECT(&rect, 0, pMe->m_rc.dy *2/5, pMe->m_rc.dx, pMe->m_nLargeFontHeight); 

    // Display the string
    (void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_NORMAL,
                              musicName, -1,
                              0, 0, &rect, 
                              IDF_ALIGN_CENTER 
                              | IDF_ALIGN_MIDDLE 
                              | IDF_TEXT_TRANSPARENT);
}
#else
static void CoreApp_DrawMusicName(CCoreApp    *pMe,uint16 nIdx)
{
    AEERect rect;
    AEERect oldClip;
    AEERect clip;
    AECHAR bracket[2] = {(AECHAR)'[',(AECHAR)'\0'};

    if(NULL == pMe || MP3STATUS_RUNONBACKGROUND != GetMp3PlayerStatus() || NULL == pMe->m_pMusicName)
    {
        return ;
    }
    if(pWallPaper != NULL)
    {
        SETAEERECT(&clip, 0, 58, pMe->m_rc.dx, pMe->m_nLargeFontHeight + 4); 
        IDISPLAY_GetClipRect( pMe->m_pDisplay, &oldClip);
        IDISPLAY_SetClipRect( pMe->m_pDisplay, &clip);

        IIMAGE_SetOffset( pWallPaper, clip.x,clip.y);
        IIMAGE_SetDrawSize( pWallPaper, clip.dx,clip.dy);
        IIMAGE_Draw( pWallPaper, clip.x,clip.y);
        IDISPLAY_SetClipRect( pMe->m_pDisplay,&oldClip);
        IIMAGE_SetOffset( pWallPaper, 0,0);
        IIMAGE_SetDrawSize( pWallPaper, pMe->m_rc.dx,pMe->m_rc.dy);
    }
    SETAEERECT(&rect, pMe->m_rc.dx/8, 60, pMe->m_nLargeFontHeight, pMe->m_nLargeFontHeight);
     IDISPLAY_SetColor(pMe->m_pDisplay,CLR_USER_TEXT,MAKE_RGB(60, 128, 196));
     // Display the string
    (void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_BOLD,
                              bracket, -1,
                              0, 0, &rect, 
                              IDF_ALIGN_LEFT
                              | IDF_ALIGN_MIDDLE 
                              | IDF_TEXT_TRANSPARENT);
   bracket[0]=(AECHAR)']';
   SETAEERECT(&rect, pMe->m_rc.dx*7/8, 60, pMe->m_nLargeFontHeight, pMe->m_nLargeFontHeight);
    // Display the string
   (void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_BOLD,
                              bracket, -1,
                              0, 0, &rect, 
                              IDF_ALIGN_LEFT
                              | IDF_ALIGN_MIDDLE 
                              | IDF_TEXT_TRANSPARENT);
  SETAEERECT(&rect, (pMe->m_rc.dx/8 + pMe->m_nLargeFontHeight), 60,(pMe->m_rc.dx*3/4 - 2*DISP_BLANK_WIDTH), pMe->m_nLargeFontHeight);
  (void)DrawTextWithProfile(pMe->a.m_pIShell,
                              pMe->m_pDisplay,
                              RGB_WHITE_NO_TRANS,
                              AEE_FONT_BOLD,
                              pMe->m_pMusicName+nIdx, -1,
                              0, 0, &rect, 
                              IDF_ALIGN_CENTER
                              | IDF_ALIGN_MIDDLE 
                              | IDF_TEXT_TRANSPARENT);
  IDISPLAY_SetColor(pMe->m_pDisplay,CLR_USER_TEXT,CLR_SYS_TITLE_TEXT);
}
#endif
/*滚动显示当前正在播放的歌曲名称*/
static void CoreApp_MusicNameAutoScroll(CCoreApp *pMe)
{
    int nIdx,nIdxNew,n;
    nIdx  = nIdxNew = pMe->m_nAutoScrollIdx;
    n = WSTRLEN(pMe->m_pMusicName);

    if ((ISHELL_ActiveApplet(pMe->a.m_pIShell) != AEECLSID_CORE_APP) ||
                        (pMe->m_wActiveDlgID != IDD_IDLE))
    {
        ISHELL_CancelTimer(pMe->a.m_pIShell,(PFNNOTIFY)CoreApp_MusicNameAutoScroll,pMe);
        return; 
    }
    if(nIdx < n)
    {
        // Measure the text starting from the auto-scroll offset
        if(Core_bMusicNameRequiredScroll(pMe, nIdx))
        {
            nIdxNew++;
        }
        else
        {
            nIdxNew = 0;
        }
        CoreApp_DrawMusicName(pMe,nIdx);
        IDISPLAY_Update(pMe->m_pDisplay);
        //(void)ISHELL_PostEvent(pMe->a.m_pIShell,
        //                       AEECLSID_CORE_APP,
        //                       EVT_UPDATEIDLE,
        //                       0,
        //                       0);
    }
    else
    {
        nIdxNew = 0;
    }

    pMe->m_nAutoScrollIdx = nIdxNew;

    ISHELL_SetTimer( pMe->a.m_pIShell,
                     MUSICNAME_AUTO_SCROLL_TIME,
                    (PFNNOTIFY)CoreApp_MusicNameAutoScroll, 
                     pMe);
}
/*判断是否需要滚动显示*/
static boolean Core_bMusicNameRequiredScroll(CCoreApp *pMe, int nIdx)
{
    int   cx,cxMax;

    cx = IDISPLAY_MeasureText(pMe->m_pDisplay, AEE_FONT_BOLD, pMe->m_pMusicName + nIdx);
   
    cxMax = pMe->m_rc.dx*3/4 - 2*DISP_BLANK_WIDTH; //显示名字矩形筐宽度   
    
    return(cx > cxMax);
}
/*设置是否需要滚动*/
static void Core_DrawNameResetScroll(CCoreApp *pMe)
{
    if(NULL == pMe || MP3STATUS_RUNONBACKGROUND != GetMp3PlayerStatus() || NULL == pMe->m_pMusicName)
    {
        return ;
    }
    if(pMe==NULL)
    {
        return;
    }
    pMe->m_nAutoScrollIdx = 0;
    if(Core_bMusicNameRequiredScroll(pMe,0))
    {
       CoreApp_MusicNameAutoScroll(pMe); 
    }
    else
    {
        ISHELL_CancelTimer(pMe->a.m_pIShell,(PFNNOTIFY)CoreApp_MusicNameAutoScroll,pMe);
        CoreApp_DrawMusicName(pMe,0);  
    }
}
#endif
static void CoreApp_GetSPN(CCoreApp *pMe)
{

#ifndef WIN32
#if defined FEATURE_CARRIER_THAILAND_CAT
    MEMSET(pMe->svc_p_name,0,sizeof(pMe->svc_p_name));
    return;
#elif ((defined(FEATURE_CARRIER_THAILAND_HUTCH))&&(!(defined(FEATURE_CARRIER_THAILAND_CAT_FACE))))
    MEMSET(pMe->svc_p_name,0,(UIM_CDMA_HOME_SERVICE_SIZE+1)*sizeof(AECHAR));
    ICONFIG_GetItem(pMe->m_pConfig, CFGI_BANNER, pMe->svc_p_name, (NV_MAX_LTRS+1)*sizeof(AECHAR));
    return;
#endif

#ifdef FEATURE_SPN_FROM_BSMCCMNC
    /*1. Represent the 3-digit Mobile Country Code as D1 D2 D3 with the digit equal to zero
        being given the value of ten.
     2. Compute 100 × D1 + 10 × D2 + D3 - 111.
    */
    //Get MCC&MNC codeS
    extern cdma_bs_type *cur_bs_ptr;
    boolean   ServNameinNv = FALSE;
    word curr_mcc;
    byte curr_mnc;

    if(NULL == cur_bs_ptr)
    {
        (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                        AEE_COREAPPRES_LANGFILE,
                        IDS_UNKNOWN_NETWORK,
                        pMe->svc_p_name,
                        sizeof(pMe->svc_p_name));   
        return;
    }
    curr_mcc = cur_bs_ptr->csp.esp.mcc + 111;
    curr_mnc = cur_bs_ptr->csp.esp.imsi_11_12 + 11; 

    //ERR("BS mcc(%d), BS mnc(%d)",cur_bs_ptr->esp.mcc,cur_bs_ptr->esp.imsi_11_12,0);

    if ( curr_mcc % 10 == 0 )
    {
       curr_mcc -= 10;
    }
    if ( (curr_mcc/10) % 10 == 0 )
    {
       curr_mcc -= 100;
    }
    if ( curr_mcc >= 1000 )
    {
       curr_mcc -= 1000; 
    } 

    if ( curr_mnc % 10 == 0 )
    {
       curr_mnc -= 10;
    }
    if ( curr_mnc >= 100 )
    {
       curr_mnc -= 100;
    } 

    //Load name string of service provider
    if ( 460 == curr_mcc && 3 == curr_mnc ) 
    {
       (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                       AEE_COREAPPRES_LANGFILE,
                       IDS_CHINA_TELECOM,
                       pMe->svc_p_name,
                       sizeof(pMe->svc_p_name));   
    }
    else
    {
       // RUIM's mcc,mnc are different from the ones from the BS
       // look for the SP list
       int i;
       int len = sizeof(List_SP)/sizeof(ServiceProviderList);
       for (i = 0; i < len; i++)
       {
          if ((List_SP[i].mcc == curr_mcc) && (List_SP[i].mnc == curr_mnc))
          {
             STRTOWSTR ( List_SP[i].ids_name, pMe->svc_p_name, sizeof(pMe->svc_p_name) );
             ServNameinNv = TRUE;
             //ERR("RUIM mcc == BS mcc ",0,0,0);
             break;
          }
       }
       if(!ServNameinNv)
        {
            AECHAR szFormat[16]; 
            STR_TO_WSTR("%d/%d  ", szFormat, sizeof(szFormat));
            WSPRINTF(pMe->svc_p_name, sizeof(pMe->svc_p_name), szFormat, curr_mcc, curr_mnc);
        }
    }
#else //FEATURE_SPN_FROM_BSMCCMNC    

#if(! defined FEATURE_CARRIER_ISRAEL_PELEPHONE) && (! defined FEATURE_LANG_HEBREW)
    if(pMe->svc_p_name[0] == 0)
#endif
    {
        //DBGPRINTF("IRUIM_Read_Svc_P_Name");
        if(IsRunAsUIMVersion())
        {
       
            if(EFAILED == IRUIM_Read_Svc_P_Name(pMe->m_pIRUIM,pMe->svc_p_name))
            {
                MEMSET(pMe->svc_p_name,0,(UIM_CDMA_HOME_SERVICE_SIZE+1)*sizeof(AECHAR));
                ICONFIG_GetItem(pMe->m_pConfig, CFGI_BANNER, pMe->svc_p_name, (NV_MAX_LTRS+1)*sizeof(AECHAR));
            }        
        }
        else
        {
            MEMSET(pMe->svc_p_name,0,(UIM_CDMA_HOME_SERVICE_SIZE+1)*sizeof(AECHAR));
            {
#if(defined FEATURE_CARRIER_ISRAEL_PELEPHONE) && (defined FEATURE_LANG_HEBREW)
                nv_language_enum_type sel = 0;
                ICONFIG_GetItem(pMe->m_pConfig, CFGI_LANGUAGE_SELECTION, &sel, sizeof(sel));
                if(sel ==  NV_LANGUAGE_HEBREW)
                {
                    ISHELL_LoadResString(pMe->a.m_pIShell,AEE_COREAPPRES_LANGFILE, IDS_ISRAEL_BANNER, pMe->svc_p_name, sizeof(pMe->svc_p_name));
                }
                else
#endif
                {
                    ICONFIG_GetItem(pMe->m_pConfig, CFGI_BANNER, pMe->svc_p_name, (NV_MAX_LTRS+1)*sizeof(AECHAR));
                }
            }
        }
    }
#endif //FEATURE_SPN_FROM_BSMCCMNC   
#endif//WIN32
}

//根据角度取得sin値
static void CoreApp_SetMintueImageID(CCoreApp *pMe,uint16 MINUTE_angle)
{
	int value_ofsin = MINUTE_angle/6;
	pMe->m_dial_rc.x = pMe->m_dial_rc.x - 7;
	pMe->m_dial_rc.y = pMe->m_dial_rc.y - 7;
	switch (value_ofsin)
	{
	case 0:
		pMe->m_dial_minue_image[0] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_0);
		if (pMe->m_dial_minue_image[0] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[0], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[0]);
		break;
	case 1:
		pMe->m_dial_minue_image[1] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_6);
		if (pMe->m_dial_minue_image[1] = NULL)
		{
			break;
		}
        IIMAGE_Draw( pMe->m_dial_minue_image[1], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[1]);
		break;
	case 2:
		pMe->m_dial_minue_image[2] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_12);
		if (pMe->m_dial_minue_image[2] = NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[2], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[2]);



		break;
	case 3:
		pMe->m_dial_minue_image[3] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_18);
		if (pMe->m_dial_minue_image[3] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[3], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[3]);

		break;
	case 4:
		pMe->m_dial_minue_image[4] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_24);
		if (pMe->m_dial_minue_image[4] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[4], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[4]);
		break;
	case 5:
		pMe->m_dial_minue_image[5] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_30);
		if (pMe->m_dial_minue_image[5] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[5], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[5]);
		break;
	case 6:
		pMe->m_dial_minue_image[6] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_36);
		if (pMe->m_dial_minue_image[6] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[6], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[6]);
		break;
	case 7:
		pMe->m_dial_minue_image[7] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_42);
		if (pMe->m_dial_minue_image[0] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[7], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[7]);
		break;
	case 8:
		pMe->m_dial_minue_image[8] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_48);
		if (pMe->m_dial_minue_image[8] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[8], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[8]);
		break;
	case 9:
		pMe->m_dial_minue_image[9] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_54);
		if (pMe->m_dial_minue_image[9] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[9], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[9]);
		break;
	case 10:
		pMe->m_dial_minue_image[10] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_60);
		if (pMe->m_dial_minue_image[10] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[10], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[10]);
		break;
	case 11:
		pMe->m_dial_minue_image[11] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_66);
		if (pMe->m_dial_minue_image[11] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[11], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[11]);
		break;
	case 12:
		pMe->m_dial_minue_image[12] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_72);
		if (pMe->m_dial_minue_image[12] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[12], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[12]);
		break;
	case 13:
		pMe->m_dial_minue_image[13] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_78);
		if (pMe->m_dial_minue_image[13] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[13], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[13]);
		break;
	case 14:
		pMe->m_dial_minue_image[14] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_84);
		if (pMe->m_dial_minue_image[14] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[14], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[14]);
		break;
	case 15 :
		pMe->m_dial_minue_image[15] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_90);
		if (pMe->m_dial_minue_image[15] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[15], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[15]);
		break;
	case 16:
		pMe->m_dial_minue_image[16] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_96);
		if (pMe->m_dial_minue_image[16] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[16], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[16]);
		break;

	case 17:
		pMe->m_dial_minue_image[17] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_102);
		if (pMe->m_dial_minue_image[17] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[17], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[17]);
		break;
	case 18:
		pMe->m_dial_minue_image[18] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_108);
		if (pMe->m_dial_minue_image[18] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[18], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[18]);
		break;
	case 19:
		pMe->m_dial_minue_image[19] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_114);
		if (pMe->m_dial_minue_image[19] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[19], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[19]);
		break;
	case 20:
		pMe->m_dial_minue_image[20] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_120);
		if (pMe->m_dial_minue_image[20] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[20], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[20]);
		break;
	case 21:
		pMe->m_dial_minue_image[21] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_126);
		if (pMe->m_dial_minue_image[21] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[21], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[21]);
		break;
	case 22:
		pMe->m_dial_minue_image[22] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_132);
		if (pMe->m_dial_minue_image[22] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[22], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[22]);
		break;
	case 23:
		pMe->m_dial_minue_image[23] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_138);
		if (pMe->m_dial_minue_image[23] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[23], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[23]);
		break;
	case 24:
		pMe->m_dial_minue_image[24] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_144);
		if (pMe->m_dial_minue_image[24] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[24], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[24]);
		break;
	case 25:
		pMe->m_dial_minue_image[25] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_150);
		if (pMe->m_dial_minue_image[25] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[25], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[25]);
		break;
	case 26:
		pMe->m_dial_minue_image[26] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_156);
		if (pMe->m_dial_minue_image[26] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[26], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[26]);
		break;
	case 27:
		pMe->m_dial_minue_image[27] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_162);
		if (pMe->m_dial_minue_image[27] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[27], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[27]);
		break;
	case 28:
		pMe->m_dial_minue_image[28] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_168);
		if (pMe->m_dial_minue_image[28] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[28], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[28]);
		break;
	case 29:
		pMe->m_dial_minue_image[29] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_174);
		if (pMe->m_dial_minue_image[29] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[29], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[29]);
		break;
	case 30:
		pMe->m_dial_minue_image[30] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_180);
		if (pMe->m_dial_minue_image[30] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[30], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[30]);
		break;
	case 31 :
		pMe->m_dial_minue_image[31] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_186);
		if (pMe->m_dial_minue_image[31] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[31], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[31]);

		break;
	case 32:
		pMe->m_dial_minue_image[32] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_192);
		if (pMe->m_dial_minue_image[32] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[32], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[32]);
		break;
	case 33:
		pMe->m_dial_minue_image[33] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_198);
		if (pMe->m_dial_minue_image[33] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[33], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[33]);
		break;
	case 34:
		pMe->m_dial_minue_image[34] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_204);
		if (pMe->m_dial_minue_image[34] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[34], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[34]);
		break;
	case 35:
		pMe->m_dial_minue_image[35] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_210);
		if (pMe->m_dial_minue_image[35] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[35], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[35]);
		break;
	case 36:
		pMe->m_dial_minue_image[36] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_216);
		if (pMe->m_dial_minue_image[36] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[36], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[36]);
		break;
	case 37:
		pMe->m_dial_minue_image[37] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_222);
		if (pMe->m_dial_minue_image[37] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[37], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[37]);
		break;
	case 38:
		pMe->m_dial_minue_image[38] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_228);
		if (pMe->m_dial_minue_image[38] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[38], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[38]);
		break;
	case 39:
		pMe->m_dial_minue_image[39] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_234);
		if (pMe->m_dial_minue_image[39] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[39], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[39]);
		break;
	case 40:
		pMe->m_dial_minue_image[40] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_240);
		if (pMe->m_dial_minue_image[40] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[40], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[40]);
		break;
	case 41:
		pMe->m_dial_minue_image[41] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_246);
		if (pMe->m_dial_minue_image[41] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[41], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[41]);
		break;
	case 42:
		pMe->m_dial_minue_image[42] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_252);
		if (pMe->m_dial_minue_image[42] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[42], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[42]);
		break;
	case 43:
		pMe->m_dial_minue_image[43] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_258);
		if (pMe->m_dial_minue_image[43] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[43], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[43]);
		break;
	case 44:
		pMe->m_dial_minue_image[44] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_264);
		if (pMe->m_dial_minue_image[44] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[44], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[44]);
		break;
	case 45:
		pMe->m_dial_minue_image[45] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_270);
		if (pMe->m_dial_minue_image[45] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[45], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[45]);
		break;
	case 46:
		pMe->m_dial_minue_image[46] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_276);
		if (pMe->m_dial_minue_image[46] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[46], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[46]);
		break;
	case 47 :
		pMe->m_dial_minue_image[47] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_282);
		if (pMe->m_dial_minue_image[47] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[47], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[47]);
		break;
	case 48:
		pMe->m_dial_minue_image[48] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_288);
		if (pMe->m_dial_minue_image[48] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[48], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[48]);
		break;
	case 49:
		pMe->m_dial_minue_image[49] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_294);
		if (pMe->m_dial_minue_image[49] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[49], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[49]);
		break;
	case 50:
		pMe->m_dial_minue_image[50] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_300);
		if (pMe->m_dial_minue_image[50] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[50], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[50]);
		break;
	case 51:
		pMe->m_dial_minue_image[51] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_306);
		if (pMe->m_dial_minue_image[51] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[51], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[51]);
		break;
	case 52:
		pMe->m_dial_minue_image[52] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_312);
		if (pMe->m_dial_minue_image[52] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[52], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[52]);
		break;
	case 53:
		pMe->m_dial_minue_image[53] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_318);
		if (pMe->m_dial_minue_image[53] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[53], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[53]);
		break;
	case 54:
		pMe->m_dial_minue_image[54] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_324);
		if (pMe->m_dial_minue_image[54] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[54], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[54]);
		break;
	case 55:
		pMe->m_dial_minue_image[55] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_330);
		if (pMe->m_dial_minue_image[55] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[55], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[55]);
		break;
	case 56:
		pMe->m_dial_minue_image[56] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_336);
		if (pMe->m_dial_minue_image[56] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[56], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[56]);
		break;
	case 57:
		pMe->m_dial_minue_image[57] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_342);
		if (pMe->m_dial_minue_image[57] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[57], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[57]);
		break;
	case 58:
		pMe->m_dial_minue_image[58] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_48);
		if (pMe->m_dial_minue_image[58] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[58], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[58]);
		break;
	case 59:
		pMe->m_dial_minue_image[59] = ISHELL_LoadResImage(AEE_GetShell(),AEE_APPSCOMMONRES_IMAGESFILE,IDI_MINUTE_54);
		if (pMe->m_dial_minue_image[59] == NULL)
		{
			break;
		}
		IIMAGE_Draw( pMe->m_dial_minue_image[59], pMe->m_dial_rc.x , pMe->m_dial_rc.y );
		IIMAGE_Release(pMe->m_dial_minue_image[59]);
		break;

	default:
		break;

	}
}

static double CoreApp_sin(uint16 angle)
{
	double sin_value;
	uint16 i;
	i = angle/6;
	switch (i)
	{
	case 1:
		sin_value = 0.1045;
		break;
	case 2:
		sin_value = 0.2079;
		break;
	case 3:
		sin_value = 0.3090;
		break;
	case 4:
		sin_value = 0.4067;
		break;
	case 5:
		sin_value = 0.5000;
		break;
	case 6:
		sin_value = 0.5878;
		break;
	case 7:
		sin_value = 0.6691;
		break;
	case 8:
		sin_value = 0.7431;
		break;
	case 9:
		sin_value = 0.8090;
		break;
	case 10:
		sin_value = 0.8660;
		break;
	case 11:
		sin_value = 0.9135;
		break;
	case 12:
		sin_value = 0.9511;
		break;
	case 13:
		sin_value = 0.9781;
		break;
	case 14:
		sin_value = 0.9945;
		break;
	case 15:
		sin_value = 1.0000;
		break;
	default:
		break;
	}

	return sin_value;
}


static void CoreApp_DrawMinuteHand(CCoreApp *pMe,uint16 Minute_angle)
{/*
	uint16 err;
	AEELine Minute_Z;
	uint16 mx1,mx2,my1,my2;
	IGraphics * pIGraphics;
	IShell           *pShell = AEE_GetShell();
	err = ISHELL_CreateInstance( pShell,
		AEECLSID_GRAPHICS,
		(void **)&pIGraphics);
	if (!pIGraphics)
	{
		return;
	}
	if ((Minute_angle>=0) && (Minute_angle< 90))
	{
		double temp_x1 = CoreApp_sin(Minute_angle)*20.00;
		mx1 = pMe->m_dial_rc.x + pMe->m_dial_rc.dx/2 - 1 + temp_x1;
		my1 = pMe->m_dial_rc.y + pMe->m_dial_rc.dy/2-(20.00*(CoreApp_sin(90-Minute_angle))) ;

		mx2 = pMe->m_dial_rc.x + pMe->m_dial_rc.dx/2 - 1 - (5.00*(CoreApp_sin(Minute_angle))); 
		my2 = pMe->m_dial_rc.y + pMe->m_dial_rc.dy/2 + (5.00*(CoreApp_sin(90-Minute_angle)));

		Minute_Z.sx = mx1;
		Minute_Z.sy = my1;
		Minute_Z.ex = mx2;
		Minute_Z.ey = my2;
		IGRAPHICS_DrawLine(pIGraphics,&Minute_Z);
	}
	if ((Minute_angle >= 90) && (Minute_angle < 180))
	{
		double temp_x1 = CoreApp_sin(180-Minute_angle)*20.00;
		mx1 = pMe->m_dial_rc.x + pMe->m_dial_rc.dx/2 - 1 + temp_x1;
		my1 = pMe->m_dial_rc.y + pMe->m_dial_rc.dy/2+(20.00*(CoreApp_sin(Minute_angle - 90))) ;

		mx2 = pMe->m_dial_rc.x + pMe->m_dial_rc.dx/2 - 1 - (5.00*(CoreApp_sin(180-Minute_angle))); 
		my2 = pMe->m_dial_rc.y + pMe->m_dial_rc.dy/2 - (5.00*(CoreApp_sin(Minute_angle-90)));

		Minute_Z.sx = mx1;
		Minute_Z.sy = my1;
		Minute_Z.ex = mx2;
		Minute_Z.ey = my2;
		IGRAPHICS_DrawLine(pIGraphics,&Minute_Z);
	}
	if ((Minute_angle >= 180) && (Minute_angle < 270))
	{
		double temp_x1 = CoreApp_sin(Minute_angle - 180)*20.00;
		mx1 = pMe->m_dial_rc.x + pMe->m_dial_rc.dx/2 - 1 - temp_x1;
		my1 = pMe->m_dial_rc.y + pMe->m_dial_rc.dy/2+(20.00*(CoreApp_sin(270-Minute_angle))) ;

		mx2 = pMe->m_dial_rc.x + pMe->m_dial_rc.dx/2 - 1 + (5.00*(CoreApp_sin(Minute_angle-180))); 
		my2 = pMe->m_dial_rc.y + pMe->m_dial_rc.dy/2 - (5.00*(CoreApp_sin(270-Minute_angle)));

		Minute_Z.sx = mx1;
		Minute_Z.sy = my1;
		Minute_Z.ex = mx2;
		Minute_Z.ey = my2;
		IGRAPHICS_DrawLine(pIGraphics,&Minute_Z);
	}
	if ((Minute_angle >= 270)&&(Minute_angle < 360))
	{
		double temp_x1 = CoreApp_sin(360-Minute_angle)*20.00;
		mx1 = pMe->m_dial_rc.x + pMe->m_dial_rc.dx/2 - 1 - temp_x1;
		my1 = pMe->m_dial_rc.y + pMe->m_dial_rc.dy/2 - (20.00*(CoreApp_sin(Minute_angle-270))) ;

		mx2 = pMe->m_dial_rc.x + pMe->m_dial_rc.dx/2 - 1 + (5.00*(CoreApp_sin(360 - Minute_angle))); 
		my2 = pMe->m_dial_rc.y + pMe->m_dial_rc.dy/2 + (5.00*(CoreApp_sin(Minute_angle-270)));

		Minute_Z.sx = mx1;
		Minute_Z.sy = my1;
		Minute_Z.ex = mx2;
		Minute_Z.ey = my2;
		IGRAPHICS_DrawLine(pIGraphics,&Minute_Z);
	}


	if (pIGraphics != NULL)
	{
		IGRAPHICS_Release(pIGraphics);
		pIGraphics = NULL;
	}
*/
}






