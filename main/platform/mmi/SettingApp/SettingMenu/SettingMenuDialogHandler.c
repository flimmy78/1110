/*==============================================================================
// 文件：
//        SettingMenuDialogHandler.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//
// 文件说明：
//
// 作者：Gemsea
// 创建日期：2007-11-01
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人    问题单号  修改内容、位置及原因
-----------      ----------  --------  -----------------------------------------
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "SettingMenu_priv.h"
#ifdef FEATURE_ICM
#include "AEECM.h"
#else
#include "AEETelephone.h"
#include "AEETelDef.h"
#endif
#include "CallApp.h"
#include "OEMRTC.h"
#include "appscommonimages.brh"

#ifdef FEATRUE_SUPPORT_G_SENSOR
#include "g_sensor.h"
#endif

#ifdef FEATURE_PLANEMODE
#include "aeecm.h"
#endif

#ifdef FEATURE_APP_MUSICPLAYER
#include "MusicPlayer.h"
#endif 
#include "nv.h"
#ifdef FEATURE_VERSION_MYANMAR

#endif
#include "cm.h"
boolean m_Aarplne  = FALSE;
boolean start_security_setting_by_user;
#ifdef FEATURE_VERSION_FLEXI203
#elif defined FEATURE_VERSION_SMART
#define FEATURE_SET_SCENEMODE
#elif defined FEATURE_VERSION_M8
#define FEATURE_SET_SCENEMODE
#elif defined FEATURE_VERSION_M8021
#define FEATURE_SET_SCENEMODE
#elif defined FEATURE_VERSION_M8P
#define FEATURE_SET_SCENEMODE
#elif defined FEATURE_VERSION_X3
#else
#define FEATURE_SET_SCENEMODE
#endif
#define FEATURE_SET_SOUND_TITLE
/*==============================================================================
                                 宏定义和常数
==============================================================================*/

#define MENU_ADDITEM(pMenu,ID)                                                \
    (void)IMENUCTL_AddItem((pMenu), AEE_APPSSETTINGMENU_RES_FILE, (ID), (ID), 0, 0) 

// 关闭对话框宏
#define CLOSE_DIALOG(DlgRet)  {    \
                                     pMe->m_eDlgRet = DlgRet; \
                                     (void) ISHELL_EndDialog(pMe->m_pShell);  \
                                 }


#define DRAW_BOTTOMBAR(x)                           \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = x;                         \
    DrawBottomBar(pMe->m_pDisplay, &BarParam);      \
}

/*==============================================================================
                                 类型定义
==============================================================================*/


/*==============================================================================
                                 函数声明
==============================================================================*/
// 对话框 IDD_MAIN_MENU 事件处理函数
static boolean  HandleMainDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_PHONESETTING_MENU 事件处理函数
static boolean  HandlePhoneSettingDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_CALLSETTING_MENU 事件处理函数
static boolean  HandleCallSettingDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

#ifdef FEATURE_SHOW_PHONE_INFO
// 对话框 IDD_PHONE_INFO_MENU 事件处理函数
static boolean  HandlePhoneInfoDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_PHONE_INFO_MENU_SW 事件处理函数
// 对话框 IDD_PHONE_INFO_MENU_HW 事件处理函数
// 对话框 IDD_PHONE_INFO_MENU_PRL 事件处理函数
static boolean  HandlePhoneInfo_SW_HW_PRL_DialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

static boolean  HandlePhoneGeneralInfo_DialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

#endif


// 对话框 IDD_PHONE_SAR_VALUE 事件处理函数
static boolean  HandlePhoneSarValueDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

#ifdef FEATURE_SHORTCUT_IN_SETTINGS
// 对话框 IDD_PHONE_INFO_MENU 事件处理函数
static boolean  HandleShortcutsMenuDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_PHONE_INFO_MENU_SW 事件处理函数
// 对话框 IDD_PHONE_INFO_MENU_HW 事件处理函数
// 对话框 IDD_PHONE_INFO_MENU_PRL 事件处理函数
static boolean  HandleShortcutsSelectMenuDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#endif
static boolean  HandleAirPlaneDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

#if defined(FEATURE_VERSION_W317A)
// 对话框 IDD_SETTING_CALLRECORD 事件处理函数
static boolean HandleAutoCallRecordDialogEvent(CSettingMenu *pMe,
	AEEEvent eCode, 
	uint16 wParam, 
	uint32 dwParam);	
#endif

//对话框 IDD_CALLSETTINGSEL 事件处理函数
static boolean  HandleCallSettingSelDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_DIVERT_MENU 事件处理函数
static boolean  HandleDivertDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_NETWORKSETTING_MENU 事件处理函数
static boolean  HandleNetworkDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_CALLFORWARD_SEL 事件处理函数
static boolean  HandleCallForwardSelDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_CALLFORWARD_INPUT 事件处理函数
static boolean  HandleCallForwardInputDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

#ifdef FEATURE_SET_BANNER
//对话框 IDD_BANNER_TXT 事件处理函数
static boolean  HandleBannerDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#endif

#ifdef FEATURE_TIME_DATA_SETTING
//对话框 IDD_PHONESETTING_MENU 事件处理函数
static boolean  HandleTimeDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
static boolean  HandleDateDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#endif

#if defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_W317A) || defined(FEATURE_VERSION_IC241A_MMX)|| defined(FEATURE_VERSION_KK5)
static boolean   HandleTrackSmsDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#endif


#ifdef  FEATURE_DOUBLE_SIM_CARD
static boolean HandleSimDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
    );

static boolean HandleSimChoiceEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
    );
#ifdef FEATURE_ICM
void HandleSimChoiceTimer(ICM *m_pICM);
#else
void HandleSimChoiceTimer(IPhoneCtl *m_pITelephone);
#endif
#endif
#ifdef	FEATURE_VERSION_MYANMAR
static boolean HandleSearchModeDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
    );
static boolean HandleTimeFontModeDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
    ); 
#endif
#ifdef FEATURE_KEYGUARD
//对话框 IDD_AKG_MENU 事件处理函数
static boolean  HandleAKGDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#endif

#ifdef FEATURE_VERSION_KK5
//对话框 IDD_NETWORK_SETTINGS 事件处理函数
static boolean  HandleNetworkSettingsDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

//对话框 IDD_NETWORK_PARAMS_EDIT 事件处理函数
static boolean  HandleNetworkParamsEditDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

#endif

#ifdef FEATURE_LANGUAGE
// 对话框 IDD_LANGUAGE_MENU 事件处理函数
static boolean  HandleLanguageDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#endif

#ifdef FEATURE_SOUND_BO
// 对话框 IDD_LANGUAGE_MENU 事件处理函数
static boolean  HandleSpeechDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#endif
#if defined(FEATURE_VERSION_K212_ND)
// 对话框 IDD_LANGUAGE_MENU 事件处理函数
static boolean  HandleSosDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#endif

#ifdef FEATURE_PLANEMODE
// 对话框 IDD_LANGUAGE_MENU 事件处理函数
static boolean  HandlePlaneModeDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#endif

// 对话框 IDD_WARNING_MESSEGE 事件处理函数
static boolean  HandleWarningMessegeDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

#ifdef FEATURE_PERU_VERSION
// 对话框 IDD_PHONE_NUMBER 事件处理函数
static boolean  HandlePhoneNumberDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#endif
#ifdef FEATURE_NETWORK_SET
// 对话框 IDD_NET_SELECT 事件处理函数
static boolean  HandleNetSelectDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#endif //FEATURE_PERU_VERSION
// 呼叫一个号码，处理呼叫转移和等待
static void SettingMenu_MakeForwardCall(CSettingMenu *pMe,char *Number);

static void SettingMenu_DialogTimeout(void *pme);

//static void SettingMenu_AutoScroll(IMenuCtl  *pMenu, AEEEvent eCode,   uint16 wParam);

static void SettingMenu_SetItemNumIcon(IMenuCtl   *pMenu);
static void SetMenu_SetCheckBoxAsOff(IMenuCtl *pMenu);
#ifdef FEATRUE_AUTO_POWER
static void SettingMenu_Set_CTL(CSettingMenu *pMe);
static boolean  Setting_Process_Auto_Power(Auto_Power_Cfg *other_set, Auto_Power_Cfg *cur);
static boolean  Setting_HandleAuto_Power_DialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#ifdef FEATURE_SUPPORT_G_SENSOR
static boolean Setting_Handle_Shake(CSettingMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
);
static boolean Setting_Handle_ShakeSub(CSettingMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
);
#endif


static boolean Setting_CClockApps_HandleKeyEvent(CSettingMenu *pMe, uint16 wParam);

static boolean Setting_CClockApps_HandleCmdEvent(CSettingMenu *pMe);

//static boolean Setting_CClockApps_HandleNumKeyEvent(CSettingMenu *pMe, uint16 wParam);
#endif

static boolean Setting_Handle_CallRestrict(CSettingMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
);
static boolean Setting_Handle_OutGoing(CSettingMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
);
static boolean Setting_Handle_Incoming(CSettingMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
);

#ifdef FEATURE_CALL_RESTRICT
static boolean Setting_Handle_RestrictNumbers(CSettingMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
);
static boolean Setting_BuildRestrictList(CSettingMenu *pMe, IMenuCtl *pMenu);
static boolean Setting_RestrictOptsDlgEvent(CSettingMenu  *pMe,
                                                       AEEEvent   eCode,
                                                       uint16     wParam,
                                                       uint32     dwParam);
static boolean Setting_DelRestrict(CSettingMenu *pMe, uint16 index);
static boolean Setting_DelAllRestrict(CSettingMenu *pMe);

static boolean Setting_Handle_Prompt(CSettingMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
);
static boolean Setting_Handle_NumEdit(CSettingMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
);
static boolean Setting_Handle_View_Detail(CSettingMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
);
#endif

static boolean Setting_Handle_Password(CSettingMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
);
static boolean Setting_Handle_Msgbox(CSettingMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
);

static boolean Handle_ANSWER_MODE_DialogEveng(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

#ifdef FEATURE_INTERNATIONAL_PREFIX
static boolean Handle_Prefix_DialogEveng(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
static boolean Handle_PrefixEdit_DialogEveng(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

#endif
static boolean  HandleFMModeDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);


static void SettingMenu_Process_Feature_Code(CSettingMenu *pMe,uint16 feature_code);

#ifdef FEATRUE_SET_IP_NUMBER
static boolean Handle_IP_Number_Set_DialogEveng(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
static number_table * SettingMenu_Search_Node(Ip_number_table *table,uint16 id);
#endif /*FEATRUE_SET_IP_NUMBER*/
static boolean  SettingMenu_HandleKeyToneLengthDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

static boolean  HandleAutoAnswerSubDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

#ifdef FEATURE_VERSION_W208S
static boolean Setting_Handle_SMSRestrict(CSettingMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
);

static boolean Setting_Handle_SMSRestrict_RECEIVE(CSettingMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
);

static boolean Setting_Handle_SMSRestrict_RECEIVE_ADD(CSettingMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
);

#endif

#ifdef FEATURE_CALL_REJECT_AUTO_MSG
static boolean  Setting_Handle_Call_Reject_Auto_Msg_DialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

#endif
/*==============================================================================
                                 全局数据
==============================================================================*/


/*==============================================================================
                                 本地（静态）数据
==============================================================================*/


/*==============================================================================
                                 函数定义
==============================================================================*/


/*==============================================================================
函数:
       SettingMenu_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向SettingMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:


==============================================================================*/
void SettingMenu_ShowDialog(CSettingMenu *pMe,uint16  dlgResId)
{
    int nRet;
    MSG_FATAL("SettingMenu_ShowDialog Start",0,0,0);
    //MSG_FATAL("next show %d dialog", dlgResId, 0, 0);
    // At most one dialog open at once
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // Looks like there is one dialog already opened.
        // Flag an error an return without doing anything.
        MSG_FATAL("Trying to create a dialog without closing the previous one",0,0,0);
        return;
    }

    nRet = ISHELL_CreateDialog(pMe->m_pShell,AEE_APPSSETTINGMENU_RES_FILE,dlgResId,NULL);

    if (nRet != SUCCESS)
    {
        MSG_FATAL("Failed to create the dialog in the SettingMenu applet",0,0,0);
    }
    MSG_FATAL("SettingMenu_ShowDialog END",0,0,0);
}

/*==============================================================================
函数:
       SettingMenu_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向SettingMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean SettingMenu_RouteDialogEvent(CSettingMenu *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
)
{
    MSG_FATAL("SettingMenu_RouteDialogEvent Start",0,0,0);
    if (NULL == pMe)
    {
        MSG_FATAL("SettingMenu_RouteDialogEvent pMe==NULL",0,0,0);
        return FALSE;
    }

    if (NULL == pMe->m_pActiveDlg)
    {
        MSG_FATAL("SettingMenu_RouteDialogEvent NULL == pMe->m_pActiveDlg",0,0,0);
        return FALSE;
    }
    //MSG_FATAL("%d SettingMenu_RouteDialogEvent", pMe->m_pActiveDlgID, 0, 0);
    switch (pMe->m_pActiveDlgID)
    {
        case IDD_MAIN_MENU:
            return HandleMainDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_CALLSETTING_MENU:
            return HandleCallSettingDialogEvent(pMe,eCode,wParam,dwParam);
#ifdef FEATURE_SHOW_PHONE_INFO
        case IDD_PHONE_INFO_MENU:
            return HandlePhoneInfoDialogEvent(pMe,eCode,wParam,dwParam);       
			     
        case IDD_PHONE_INFO_MENU_SW:
            return HandlePhoneInfo_SW_HW_PRL_DialogEvent(pMe,eCode,wParam,dwParam); 
			
        case IDD_PHONE_INFO_MENU_HW:
            return HandlePhoneInfo_SW_HW_PRL_DialogEvent(pMe,eCode,wParam,dwParam); 
			
        case IDD_PHONE_INFO_MENU_PRL:
            return HandlePhoneInfo_SW_HW_PRL_DialogEvent(pMe,eCode,wParam,dwParam); 
            
        case IDD_PHONE_GENERAL_INFO:
            return HandlePhoneGeneralInfo_DialogEvent(pMe,eCode,wParam,dwParam);
#endif	
        case IDD_PHONE_SAR_VALUE:
            return HandlePhoneSarValueDialogEvent(pMe,eCode,wParam,dwParam); 
#ifdef FEATURE_SHORTCUT_IN_SETTINGS
        case IDD_SHORTCUTS_MENU:
            return HandleShortcutsMenuDialogEvent(pMe,eCode,wParam,dwParam);       
			     
        case IDD_SHORTCUTS_SELECT_MENU:
            return HandleShortcutsSelectMenuDialogEvent(pMe,eCode,wParam,dwParam); 
			
 
#endif	
								
        case IDD_PHONESETTING_MENU:
            return HandlePhoneSettingDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_CALLSETTINGSEL:
            return HandleCallSettingSelDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_NETWORKSETTING_MENU:
            return HandleNetworkDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_DIVERT_MENU:
            return HandleDivertDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_CALLFORWARD_SEL:
            return HandleCallForwardSelDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_CALLFORWARD_INPUT:
            return HandleCallForwardInputDialogEvent(pMe,eCode,wParam,dwParam);

#ifdef FEATURE_KEYGUARD
        case IDD_AKG_MENU:
            return HandleAKGDialogEvent(pMe,eCode,wParam,dwParam);
#endif

#ifdef FEATURE_VERSION_KK5
        case IDD_NETWORK_SETTINGS:
            return HandleNetworkSettingsDialogEvent(pMe, eCode, wParam, dwParam);

        case IDD_NETWORK_PARAMS_EDIT:
            return HandleNetworkParamsEditDialogEvent(pMe, eCode, wParam, dwParam);
#endif

#ifdef FEATURE_SET_BANNER
        case IDD_BANNER_TXT:
            return HandleBannerDialogEvent(pMe,eCode,wParam,dwParam);
#endif

#ifdef FEATURE_TIME_DATA_SETTING
        case IDD_TIMESETTING:
            return HandleTimeDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_DATESETTING:
            return HandleDateDialogEvent(pMe,eCode,wParam,dwParam);
#endif
#if defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_W317A) || defined(FEATURE_VERSION_IC241A_MMX)|| defined(FEATURE_VERSION_KK5)
		case IDD_TRACKSMSTIME_MENU:
			return HandleTrackSmsDialogEvent(pMe,eCode,wParam,dwParam);
#endif
#ifdef FEATURE_LANGUAGE
        case IDD_LANGUAGE_MENU:
            return HandleLanguageDialogEvent(pMe,eCode,wParam,dwParam);
#endif
#ifdef FEATURE_PLANEMODE
        case IDD_PLANEMODE:
            return HandlePlaneModeDialogEvent(pMe,eCode,wParam,dwParam);
#endif
        case IDD_WARNING_MESSEGE:
            return HandleWarningMessegeDialogEvent(pMe,eCode,wParam,dwParam);

#ifdef FEATURE_PERU_VERSION
        case IDD_PHONE_NUMBER:
            return HandlePhoneNumberDialogEvent(pMe,eCode,wParam,dwParam);
#endif
#ifdef FEATRUE_AUTO_POWER
        case IDD_AUTO_POWER:
            return Setting_HandleAuto_Power_DialogEvent(pMe,eCode,wParam,dwParam);
#endif
#ifdef FEATURE_SUPPORT_G_SENSOR
        case IDD_SHAKE:
            return Setting_Handle_Shake(pMe,eCode,wParam,dwParam);

        case IDD_SHAKESUB:
            return Setting_Handle_ShakeSub(pMe,eCode,wParam,dwParam);
#endif
        case IDD_PASSWORD:
            return Setting_Handle_Password(pMe,eCode,wParam,dwParam);
                     
        case IDD_RESTRICT:
            return Setting_Handle_CallRestrict(pMe,eCode,wParam,dwParam);
            
        case IDD_RESTRICT_OUTGOING:
            return Setting_Handle_OutGoing(pMe,eCode,wParam,dwParam);
            
        case IDD_RESTRICT_INCOMING:
            return Setting_Handle_Incoming(pMe,eCode,wParam,dwParam);

#ifdef FEATURE_CALL_RESTRICT
        case IDD_RESTRICT_NUMBERS:
            return Setting_Handle_RestrictNumbers(pMe,eCode,wParam,dwParam);

        case IDD_RESTRICT_OPTS:            
            return Setting_RestrictOptsDlgEvent(pMe,eCode,wParam,dwParam);    
        case IDD_PROMPT:
            return Setting_Handle_Prompt(pMe,eCode,wParam,dwParam);

        case IDD_NUM_EDIT:
            return Setting_Handle_NumEdit(pMe,eCode,wParam,dwParam); 

        case IDD_VIEW_DETAIL:
            return Setting_Handle_View_Detail(pMe,eCode,wParam,dwParam);    
#endif

        case IDD_MSGBOX:
            return Setting_Handle_Msgbox(pMe,eCode,wParam,dwParam);

        case IDD_ANSWER_MODE:
            return Handle_ANSWER_MODE_DialogEveng(pMe,eCode,wParam,dwParam);

#ifdef FEATURE_INTERNATIONAL_PREFIX
        case IDD_PREFIX:
            return Handle_Prefix_DialogEveng(pMe,eCode,wParam,dwParam);
        case IDD_PREFIX_EDIT:
            return Handle_PrefixEdit_DialogEveng(pMe,eCode,wParam,dwParam);     
#endif

        case IDD_FM_MENU:
			return HandleFMModeDialogEvent(pMe,eCode,wParam,dwParam);

#ifdef FEATRUE_SET_IP_NUMBER
        case IDD_IP_NUMBER_SET:
            return Handle_IP_Number_Set_DialogEveng(pMe,eCode,wParam,dwParam);
#endif/*FEATRUE_SET_IP_NUMBER*/
        case IDD_KEYTONE_LENGTH:
            return SettingMenu_HandleKeyToneLengthDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_AUTOANSWER_SUB:
            return HandleAutoAnswerSubDialogEvent(pMe,eCode,wParam,dwParam);
#ifdef  FEATURE_DOUBLE_SIM_CARD
        case IDD_SIMSETTING:
            return HandleSimDialogEvent(pMe,eCode,wParam,dwParam);
        case IDD_SIMSETTING_CHOICE_DIALOG:
            return HandleSimChoiceEvent(pMe,eCode,wParam,dwParam);
#endif
#ifdef FEATURE_VERSION_MYANMAR
		case IDD_SEARCHNET:
			return HandleSearchModeDialogEvent(pMe,eCode,wParam,dwParam);
		case IDD_TIME_FONTMODE:
			return HandleTimeFontModeDialogEvent(pMe,eCode,wParam,dwParam);
#endif

//Add by pyuangui 2013-01-04
#ifdef FEATURE_VERSION_W317A
        case IDD_SETTING_CALLRECORD:
			return HandleAutoCallRecordDialogEvent(pMe,eCode,wParam,dwParam);
#endif
//Add Endif
			
#ifdef FEATURE_VERSION_W208S
        case IDD_SMS_RESTRICT:
            return Setting_Handle_SMSRestrict(pMe,eCode,wParam,dwParam);

        case IDD_SMS_RESTRICT_RECEIVE:
            return Setting_Handle_SMSRestrict_RECEIVE(pMe,eCode,wParam,dwParam);         

        case IDD_SMS_RESTRICT_RECEIVE_ADD:
            return Setting_Handle_SMSRestrict_RECEIVE_ADD(pMe,eCode,wParam,dwParam);               
#endif
#ifdef FEATURE_SOUND_BO
	   case IDD_SPEECH_MENU:
	   		return HandleSpeechDialogEvent(pMe,eCode,wParam,dwParam);
#endif
#if defined(FEATURE_VERSION_K212_ND)
	   case IDD_SOS:
	   	    return HandleSosDialogEvent(pMe,eCode,wParam,dwParam);
#endif
#if defined(FEATURE_CALL_REJECT_AUTO_MSG)
        case IDD_CALL_REJECT_AUTO_MSG:
            return Setting_Handle_Call_Reject_Auto_Msg_DialogEvent(pMe,eCode,wParam,dwParam);
#endif
		case IDD_AIRPLANE_MODE:
            return HandleAirPlaneDialogEvent(pMe,eCode,wParam,dwParam);
        default:
            return FALSE;
    }
    MSG_FATAL("SettingMenu_RouteDialogEvent End",0,0,0);
}

/*==============================================================================
函数：
       HandleMainDialogEvent
说明：
       IDD_MAIN_MENU对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
      FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleMainDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)
#ifdef FEATURE_SET_SOUND_TITLE
    ISoundMenu  *pSoundList = NULL;
#endif /*FEATURE_SET_SOUND_TITLE*/
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_MAIN);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("%x, %x ,%x,HandleMainDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    //SettingMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			MSG_FATAL("HandleMainDialogEvent......start......",0,0,0);
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_APP_TITLE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
#ifdef FEATURE_VERSION_K212_ND
			IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_PHONESETTING_TITLE, IDS_PHONESETTING_TITLE, NULL, 0);
			#ifdef FEATURE_SOUND_BO
			IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_YUYIIN_PACKGE, IDS_YUYIIN_PACKGE, NULL, 0);
			#endif
			IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_DISPLAY_TITLE, IDS_DISPLAY_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_CALLSETTING_TITLE, IDS_CALLSETTING_TITLE, NULL, 0);
			IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SECURITY_TITLE, IDS_SECURITY_TITLE, NULL, 0);
			//#ifdef FEATURE_SET_SCENEMODE
			//IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SCENEMODE_TITLE, IDS_SCENEMODE_TITLE, NULL, 0);
			//#endif
			#ifdef FEATRUE_AUTO_POWER
			IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AUTO_POWER_TITLE, IDS_AUTO_POWER_TITLE, NULL, 0);
			#endif
#else
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_DISPLAY_TITLE, IDS_DISPLAY_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_CALLSETTING_TITLE, IDS_CALLSETTING_TITLE, NULL, 0);
#ifdef FEATURE_VERSION_W208S
            //添加短信黑名单
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SMS_RESTRICT, IDS_SMS_RESTRICT, NULL, 0);
#endif
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_PHONESETTING_TITLE, IDS_PHONESETTING_TITLE, NULL, 0);

#ifdef FEATRUE_AUTO_POWER
#ifndef FEATURE_VERSION_C310
#ifndef FEATURE_VERSION_K212
#ifndef FEATURE_VERSION_K212_HUALU
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AUTO_POWER_TITLE, IDS_AUTO_POWER_TITLE, NULL, 0);
 //           IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AUTO_POWER_OFF_TIME, IDS_AUTO_POWER_OFF_TIME, NULL, 0);
#endif
#endif
#endif
#endif
#ifdef FEATURE_SUPPORT_G_SENSOR
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SHAKE_TITLE, IDS_SHAKE_TITLE, NULL, 0);
#endif
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SECURITY_TITLE, IDS_SECURITY_TITLE, NULL, 0);
#ifdef FEATURE_SET_SCENEMODE
#ifdef FEATURE_VERSION_KK5
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SCENEMODE_TITLE, IDS_SCENEMODE_TITLE, NULL, 0);
#endif
#endif

#ifdef FEATURE_SET_SCENEMODE
#ifndef FEATURE_VERSION_H19C 
#ifndef FEATURE_VERSION_KK5
#ifndef FEATURE_VERSION_C337
#ifndef FEATURE_VERSION_IC241A_MMX
#ifndef FEATURE_VERSION_K212
#ifndef FEATURE_VERSION_EC99
#ifndef FEATURE_VERSION_K212_HUALU
#ifndef FEATURE_VERSION_K232_Y100A
#ifndef FEATURE_VERSION_GECOMSA_C204
#ifndef FEATURE_VERSION_K232_Y101 
#ifndef FEATURE_VERSION_K292_WSF_CN
//#ifdef FEATURE_VERSION_K232_Y101    
            //IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_LIST_PROFILE, IDS_LIST_PROFILE, NULL, 0);
//#else
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SCENEMODE_TITLE, IDS_SCENEMODE_TITLE, NULL, 0);
//#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif

#endif
#endif
            
#ifdef FEATURE_SET_SOUND_TITLE
#if !defined (FEATURE_VERSION_C316)&&!defined(FEATURE_VERSION_K292)&&!defined(FEATURE_VERSION_W021_C11)&&!defined(FEATURE_VERSION_K202_LM129C)&&!defined(FEATURE_VERSION_W021_CT100)&&!defined(FEATURE_VERSION_K212)&&!defined(FEATURE_VERSION_K212_ND)&&!defined(FEATURE_VERSION_K212_BH)&&!defined(FEATURE_VERSION_GECOMSA_C204)&&!defined(FEATURE_VERSION_C192_MMX)
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SOUND_TITLE, IDS_SOUND_TITLE, NULL, 0);
#endif
#endif

#ifdef FEATURE_SHOW_PHONE_INFO
            #ifdef FEATURE_VERSION_K232_Y105A
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SOFTWARE_VERSION, IDS_SOFTWARE_VERSION, NULL, 0);
            #else
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_PHONE_INFO_TITLE, IDS_PHONE_INFO_TITLE, NULL, 0);
            #endif
#endif

#ifdef FEATURE_VERSION_K232_Y101
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_PHONE_SAR_VALUE, IDS_PHONE_SAR_VALUE, NULL, 0);
#endif

#ifdef FEATURE_SHORTCUT_IN_SETTINGS
#ifndef FEATURE_VERSION_K232_Y105A
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SHORTCUTS_MENU_TITLE, IDS_SHORTCUTS_MENU_TITLE, NULL, 0);
#endif
#endif
#ifdef FEATURE_PERU_VERSION
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_PHONE_NUMBER, IDS_PHONE_NUMBER, NULL, 0);
#endif
#endif
            return TRUE;

        case EVT_DIALOG_START:
            // 给菜单各菜单项加数字编号图标
            MSG_FATAL("HandleMainDialogEvent......EVT_DIALOG_START",0,0,0);
            IANNUNCIATOR_Redraw(pMe->m_pAnn);
            SettingMenu_SetItemNumIcon(pMenu);
#ifdef FEATRUE_AUTO_POWER
            pMe->m_auto_power = 0;
#endif
            IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            return TRUE;


        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            //pMe->m_currDlgId = IMENUCTL_GetSel(pMenu);
            pMe->m_currDlgId = wParam;
            switch (wParam)
            {
#ifdef FEATURE_SET_SOUND_TITLE
                case IDS_SOUND_TITLE:       //声音设置
                    if (AEE_SUCCESS == ISHELL_CreateInstance(pMe->m_pShell,
                                                             AEECLSID_APP_SOUNDMENU,
                                                             (void **)&pSoundList))
                    {
                        ISoundMenu_SoundList(pSoundList);
                        ISoundMenu_Release(pSoundList);
                    }
                    return TRUE;
#endif

#ifdef FEATURE_SET_SCENEMODE

#ifndef FEATURE_VERSION_H19C  

#ifdef FEATURE_VERSION_K232_Y101
                case IDS_LIST_PROFILE:
#else
                case IDS_SCENEMODE_TITLE:    //情景模式
#endif
                    (void) ISHELL_StartApplet(pMe->m_pShell,
                                              AEECLSID_APP_SOUNDMENU);
                    break;
#endif                    
#endif

                case IDS_DISPLAY_TITLE:      //显示设置
                    (void) ISHELL_StartApplet(pMe->m_pShell,
                                              AEECLSID_APP_DISPLAYMENU);
                    break;

               case IDS_PHONESETTING_TITLE:  //话机设置
                    CLOSE_DIALOG(DLGRET_PHONESETTING)
                    break;

                case IDS_SECURITY_TITLE:      //安全设置
#if 1
                    start_security_setting_by_user = TRUE;
#endif
                    (void) ISHELL_StartApplet(pMe->m_pShell,
                                              AEECLSID_APP_SECURITYMENU);
                    break;

                case IDS_CALLSETTING_TITLE:   //通话设置
                    CLOSE_DIALOG(DLGRET_CALLSETTING)
                    break;
#ifdef FEATURE_SHOW_PHONE_INFO
                case IDS_PHONE_INFO_TITLE: 
#ifdef FEATURE_VERSION_K232_Y101
                    CLOSE_DIALOG(DLGRET_PHONE_GENERAL_INFO)
#else
                    CLOSE_DIALOG(DLGRET_PHONE_INFO)
#endif                    
                    break;
                case IDS_PHONE_SAR_VALUE: 
                    CLOSE_DIALOG(DLGRET_PHONE_SAR_VALUE)
                    break;
                case IDS_SOFTWARE_VERSION:
                    CLOSE_DIALOG(DLGRET_PHONE_INFO_SW)
                    break;
#endif                    
#ifdef FEATURE_SHORTCUT_IN_SETTINGS
#ifndef FEATURE_VERSION_K232_Y105A
                case IDS_SHORTCUTS_MENU_TITLE:                    
                    CLOSE_DIALOG(DLGRET_SHORTCUTS_MENU)
                    break;
#endif  
#endif
#ifdef FEATURE_VERSION_W208S
                case IDS_SMS_RESTRICT:   //短信黑名单
                    CLOSE_DIALOG(DLGRET_SMSRESTRICT)
                    break;
#endif

#ifdef FEATURE_VERSION_K212_ND
#ifdef FEATURE_SOUND_BO
				case IDS_YUYIIN_PACKGE:
    				CLOSE_DIALOG(DLGRET_SPEECH_SETTINGS)
					break;
#endif
#endif

#ifdef FEATRUE_AUTO_POWER
                case IDS_AUTO_POWER_TITLE:/*自动开关机设置*/
                    CLOSE_DIALOG(DLGRET_AUTO_POWER)
                    break;
#endif
#ifdef FEATURE_SUPPORT_G_SENSOR
                case IDS_SHAKE_TITLE:
                    CLOSE_DIALOG(DLGRET_SHAKE)
                    break;
#endif
#ifdef FEATURE_PERU_VERSION
                case IDS_PHONE_NUMBER:
                    CLOSE_DIALOG(DLGRET_PHONE_NUMBER)
                    break;
#endif
                default:
                    ASSERT_NOT_REACHABLE;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandleMainDialogEvent


/*==============================================================================
函数：
       HandleCallSettingDialogEvent
说明：
       IDD_CALLSETTING_MENU对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleCallSettingDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_CALLSETTING);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("%x, %x ,%x,HandleCallSettingDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    //SettingMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_CALLSETTING_TITLE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_ANSWER_MODE, IDS_ANSWER_MODE, NULL, 0);
			#ifndef FEATURE_VERSION_K212
			#ifndef FEATURE_VERSION_K212_HUALU
			#if defined( FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_K202_LM129C)
			IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_CALL_TIME_REMINDER, IDS_CALL_TIME_REMINDER, NULL, 0);
			#else
#ifndef FEATURE_VERSION_K232_Y101            
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_MINUTE_ALERT, IDS_MINUTE_ALERT, NULL, 0);
#endif
			#endif
			#endif
			#endif
#ifndef FEATURE_CARRIER_TAIWAN_APBW
#ifdef FEATRUE_SET_IP_NUMBER
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_IP_NUMBER_SET, IDS_IP_NUMBER_SET, NULL, 0);
#endif
#endif
#if !defined(FEATURE_VERSION_C192_MMX)
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_CALLFORWARD_DIVERT, IDS_CALLFORWARD_DIVERT, NULL, 0);
#endif
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_CALLFORWARD_WAIT, IDS_CALLFORWARD_WAIT, NULL, 0);
#ifdef FEATURE_CARRIER_THAILAND_HUTCH 
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AUTOANSWER_TITLE, IDS_AUTOANSWER_TITLE, NULL, 0);
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH

#ifdef FEATURE_OEMOMH
#ifndef FEATURE_VERSION_K232_Y101 
			IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_KEYTONE_LENGTH, IDS_KEYTONE_LENGTH, NULL, 0);
#endif
#endif
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_CALL_RESTRICT, IDS_CALL_RESTRICT, NULL, 0);   

#ifdef FEATURE_CALL_REJECT_AUTO_MSG
            //IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_REJECT_CALL_SMS, IDS_REJECT_CALL_SMS, NULL, 0);            
#endif            
            #if defined(FEATURE_CALL_RECORDER)&&defined(FEATURE_VERSION_W317A)//def FEATURE_VERSION_W317A
			//Add by pyuangui 2012-01-04
			IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_CALLSETTING_CALLRECORD, IDS_CALLSETTING_CALLRECORD, NULL, 0);
			//Add End
			#endif
            
#ifndef FEATURE_VERSION_K202_LM129C //xxzhen
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_VOICE_PRIVACY, IDS_VOICE_PRIVACY, NULL, 0);
#endif


#ifdef FEATURE_INTERNATIONAL_PREFIX            
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_PREFIX, IDS_PREFIX, NULL, 0);
#endif


#ifndef FEATURE_VERSION_W208S            
            //IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_FMRADIO_OPTION_MENU_PLAY_MODLE, IDS_FMRADIO_OPTION_MENU_PLAY_MODLE, NULL, 0);
#endif
            return TRUE;

        case EVT_DIALOG_START:
            // 给菜单各菜单项加数字编号图标
            pMe->m_input_mode = 0;
            SettingMenu_SetItemNumIcon(pMenu);

            IMENUCTL_SetSel(pMenu, pMe->m_nSubDlgId);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            //pMe->m_nSubDlgId = IMENUCTL_GetSel(pMenu);
            pMe->m_nSubDlgId = wParam;
            //初始化音量
            (void) ICONFIG_GetItem(pMe->m_pConfig,
                                     CFGI_RINGER_VOL,
                                     &pMe->m_RingVol,
                                     sizeof(byte));

            switch (wParam)
            {
#ifndef FEATURE_CARRIER_TAIWAN_APBW
#ifdef FEATRUE_SET_IP_NUMBER
                case IDS_IP_NUMBER_SET:
                    pMe->m_CallSettingSel = IDS_IP_NUMBER_SET;
                    CLOSE_DIALOG(DLGRET_IP_NUMBER_SET)
                    break;
#endif
#endif
                case IDS_ANSWER_MODE:
                    CLOSE_DIALOG(DLGRET_AUTO_ANSWER_MODE)
                    break;
                    
#ifdef FEATURE_INTERNATIONAL_PREFIX
                case IDS_PREFIX:
                    CLOSE_DIALOG(DLGRET_PREFIX)
                    break;
#endif    

#ifdef FEATURE_CARRIER_THAILAND_HUTCH                          
                case IDS_AUTOANSWER_TITLE:
                    CLOSE_DIALOG(DLGRET_AUTO_ANSWER_MODE_SUB)
                    break;
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH  

			    #if defined( FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_K202_LM129C)
				case IDS_CALL_TIME_REMINDER:
				#endif
                case IDS_MINUTE_ALERT:     //50秒提醒
			        #if defined( FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_K202_LM129C)
					pMe->m_CallSettingSel = IDS_CALL_TIME_REMINDER;
					#else
                    pMe->m_CallSettingSel = IDS_MINUTE_ALERT;
					#endif
                    CLOSE_DIALOG(DLGRET_CALLSETTINGSEL)
                    break;

                case IDS_NETWORKSETTING_TITLE:  //网络设置
                    CLOSE_DIALOG(DLGRET_NETWORKSETTING)
                    break;
                    
                case IDS_CALLFORWARD_DIVERT:
                    CLOSE_DIALOG(DLGRET_DIVERT)
                    break;
                    
                case IDS_CALLFORWARD_WAIT:
                    pMe->m_CFType = CALLFORWARD_WAIT;
                    CLOSE_DIALOG(DLGRET_CALLFORWARDSEL)
                    break;

                case IDS_VOICE_PRIVACY:     //Voice privacy
                    pMe->m_CallSettingSel = IDS_VOICE_PRIVACY;
                    CLOSE_DIALOG(DLGRET_CALLSETTINGSEL)
                    break;

                case IDS_AUTO_REDIAL:
                    pMe->m_CallSettingSel = IDS_AUTO_REDIAL;
                    CLOSE_DIALOG(DLGRET_CALLSETTINGSEL)
                    break;

                case IDS_KEYTONE_LENGTH:
                    CLOSE_DIALOG(DLGRET_KEYLENGTH)
                    break;
                    
                case IDS_CALL_RESTRICT:
                    CLOSE_DIALOG(DLGRET_PASSWORD)
                    break;

#ifdef FEATURE_CALL_REJECT_AUTO_MSG
                case IDS_REJECT_CALL_SMS:
                    CLOSE_DIALOG(DLGRET_CALL_REJECT_AUTO_MSG)
                    break;
#endif
					
//Add by pyuangui 20130104
#ifdef FEATURE_VERSION_W317A
				case IDS_CALLSETTING_CALLRECORD:
					CLOSE_DIALOG(DLGRET_AUTOCALLRECORD)
					break;
#endif
//Add End
					
#ifndef FEATURE_VERSION_W208S                     
                case IDS_FMRADIO_OPTION_MENU_PLAY_MODLE: //FM播放模式
                	CLOSE_DIALOG(DLGRET_FMMODE)
                	break;
#endif
                default:
                    ASSERT_NOT_REACHABLE;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandleCallSettingDialogEvent
#ifdef FEATURE_SHOW_PHONE_INFO



/*==============================================================================
函数：
       HandleCallSettingDialogEvent
说明：
       IDD_PHONE_INFO_MENU对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandlePhoneInfoDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_PHONE_INFO_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("%x, %x ,%x,HandleCallSettingDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    //SettingMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_PHONE_INFO_TITLE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }            
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SOFTWARE_VERSION_TITLE, IDS_SOFTWARE_VERSION_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_HARDWARE_VERSION_TITLE, IDS_HARDWARE_VERSION_TITLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_PRL_VERSION_TITLE, IDS_PRL_VERSION_TITLE, NULL, 0);
			
            return TRUE;

        case EVT_DIALOG_START:
            // 给菜单各菜单项加数字编号图标
            pMe->m_input_mode = 0;
            SettingMenu_SetItemNumIcon(pMenu);

            IMENUCTL_SetSel(pMenu, pMe->m_nSubDlgId);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            pMe->m_nSubDlgId = wParam;
            switch (wParam)
            {
                case IDS_SOFTWARE_VERSION_TITLE:                    
                    CLOSE_DIALOG(DLGRET_PHONE_INFO_SW)
                    break;
                    
                case IDS_HARDWARE_VERSION_TITLE:
                    CLOSE_DIALOG(DLGRET_PHONE_INFO_HW)
                    break;

                case IDS_PRL_VERSION_TITLE:
                    CLOSE_DIALOG(DLGRET_PHONE_INFO_PRL)
                    break;					

                default:
                    ASSERT_NOT_REACHABLE;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandlePhoneInfoDialogEvent




/*==============================================================================
函数：
       HandleCallSettingDialogEvent
说明：
       IDD_PHONE_INFO_MENU对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandlePhoneInfo_SW_HW_PRL_DialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    BottomBar_Param_type  BBarParam ={0};
    AECHAR m_wstr[128]={0};    
    AECHAR WTitle[40] ={0};
    int n=0;
    IDialog *p_dlg = NULL;
    IStatic *p_stk = NULL;
    MSG_FATAL("%x, %x ,%x,HandlePhoneInfo_SW_HW_PRL_DialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    //SettingMenu_AutoScroll(pMenu,eCode,wParam);
    p_dlg = ISHELL_GetActiveDialog(pMe->m_pShell); 
    if(p_dlg==NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:                        
          
         //currentLine = ISTATIC_GoToLine(p_stk, currentLine);
          return TRUE;

        case EVT_DIALOG_START:

            //IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            //IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

            //IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_APP_SETTINGMENU,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
                                     
            return TRUE;

        case EVT_USER_REDRAW:
           if(pMe->m_pActiveDlgID==IDD_PHONE_INFO_MENU_SW)
           {    
                  JulianType  jDate;
                  extern const char ver_time[];
                  int i;
                  AECHAR      wszDate[64]= {0};
                  AECHAR      wFormat[64]= {L"%02d:%02d"};
                  GetJulianDate(GETTIMESECONDS(), &jDate);
                  p_stk = (IStatic *) IDIALOG_GetControl(p_dlg, IDC_SW_STAT);
                 (void) ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSSETTINGMENU_RES_FILE,
                                            IDS_SOFTWARE_VERSION_TITLE,
                                            WTitle,
                                            sizeof(WTitle));
                 
 				IANNUNCIATOR_SetFieldText(pMe->m_pAnn,WTitle);
                WSTRCAT(m_wstr+n,L"SW Ver:");
                n = WSTRLEN(m_wstr);
                m_wstr[n++] = (AECHAR) '\n';
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_BUILD_VERSION,
                                       (m_wstr + n),
                                       sizeof(m_wstr));
                n = WSTRLEN(m_wstr);
                m_wstr[n++] = (AECHAR) '\n'; 
                WSTRCAT(m_wstr+n,L"Build Time:");
                n = WSTRLEN(m_wstr);
                m_wstr[n++] = (AECHAR) '\n'; 
                n = WSTRLEN(m_wstr);
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_BUILD_TIME,
                          (m_wstr + n),
                          sizeof(m_wstr));
                n = WSTRLEN(m_wstr);
                m_wstr[n++] = (AECHAR) ' ';
                n = WSTRLEN(m_wstr);
                WSPRINTF(wszDate, sizeof(wszDate), wFormat, jDate.wHour, jDate.wMinute);
                //WSTRCAT(m_wstr+n,wszDate);  
                for(i=0; ver_time[i]!=0; i++)
                {
                    m_wstr[n+i] = (AECHAR)ver_time[i];
                } 
                n = WSTRLEN(m_wstr);
           }                
           else if(IDD_PHONE_INFO_MENU_HW==pMe->m_pActiveDlgID)
           {              
                 p_stk = (IStatic *) IDIALOG_GetControl(p_dlg, IDC_HW_STAT);
                 (void) ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSSETTINGMENU_RES_FILE,
                                            IDS_HARDWARE_VERSION_TITLE,
                                            WTitle,
                                            sizeof(WTitle));
 
 				IANNUNCIATOR_SetFieldText(pMe->m_pAnn,WTitle);
 
                #ifndef HWVERSION
                #define HWVERSION L"W203_WB_V0.1"
                #endif
                 STRTOWSTR(HWVERSION, m_wstr, sizeof(m_wstr)); 
         
           }
           else//IDD_PHONE_INFO_MENU_PRL
           {
                 p_stk = (IStatic *) IDIALOG_GetControl(p_dlg, IDC_PRL_STAT);          
     
                 (void) ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSSETTINGMENU_RES_FILE,
                                            IDS_PRL_VERSION_TITLE,
                                            WTitle,
                                            sizeof(WTitle));
                 
 				IANNUNCIATOR_SetFieldText(pMe->m_pAnn,WTitle);
 
                 WSTRCPY(m_wstr,WTitle);
                 n = WSTRLEN(m_wstr);
                 WSPRINTF((m_wstr + n),
                 sizeof(m_wstr),
                 L": %d ",
                 nv_prl_version(CM_NAM_1));
           }
    
          
            BBarParam.eBBarType = BTBAR_BACK;            
            Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
            //IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
			//WSPRINTF(m_wstr+WSTRLEN(m_wstr),sizeof(m_wstr),L"-%d dbm",pMe->m_rssi);	 
            //IDISPLAY_DrawText(pMe->m_pDisplay, AEE_FONT_NORMAL, m_wstr,-1, 0, 10, 0, IDF_TEXT_TRANSPARENT);  
            if(p_stk!=NULL)
            {
                 ISTATIC_SetProperties(p_stk, ST_UNDERLINE|ST_NOSCROLL|ST_CENTERTITLE);
                 (void) ISTATIC_SetText(p_stk,
                                   NULL,
                                   m_wstr,
                                   AEE_FONT_NORMAL,
                                   AEE_FONT_NORMAL);
                 
                 (void) ISTATIC_Redraw(p_stk);
            } 
            DrawBottomBar(pMe->m_pDisplay, &BBarParam); 
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);  
            //(void)IMENUCTL_Redraw(pMenu);
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        

        default:
            break;
    }
    return FALSE;
} // HandlePhoneInfo_SW_HW_PRL_DialogEvent

static boolean  HandlePhoneGeneralInfo_DialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    BottomBar_Param_type  BBarParam ={0};
    AECHAR m_wstr[128]={0};    
    AECHAR WTitle[40] ={0};
    int n=0;
    IDialog *p_dlg = NULL;
    IStatic *p_stk = NULL;

#if defined(FEATURE_VERSION_K232_Y102)//test20140319.
    AECHAR device_name[]=L"Model: M143\n";    
    AECHAR software_name[]=L"S/W Version: V.MTS.M143.B02\n";    
    AECHAR binary_name[]=L"Brew Version: Brew 3.1.5\n";
#else
    AECHAR device_name[]=L"Model: M141\n";    
    AECHAR software_name[]=L"S/W Version: V.MTS.M141.B02\n";    
    AECHAR binary_name[]=L"Brew Version: Brew 3.1.5\n";
#endif    
    MSG_FATAL("%x, %x ,%x,HandlePhoneGeneralInfo_DialogEvent",eCode,wParam,dwParam);
    
    p_dlg = ISHELL_GetActiveDialog(pMe->m_pShell); 
    if(p_dlg==NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:  
          return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_APP_SETTINGMENU,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
                                     
            return TRUE;

        case EVT_USER_REDRAW:
                 
             p_stk = (IStatic *) IDIALOG_GetControl(p_dlg, IDC_PHONE_GENERAL_INFO);          
 
             (void) ISHELL_LoadResString(pMe->m_pShell,
                                        AEE_APPSSETTINGMENU_RES_FILE,
                                        IDS_PHONE_INFO_TITLE,
                                        WTitle,
                                        sizeof(WTitle));
             
			 IANNUNCIATOR_SetFieldText(pMe->m_pAnn,WTitle);

             WSTRCPY(m_wstr,device_name);
             WSTRCAT(m_wstr,software_name);             
             WSTRCAT(m_wstr,binary_name);
          
            BBarParam.eBBarType = BTBAR_BACK;            
            Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
           
            if(p_stk!=NULL)
            {
                 ISTATIC_SetProperties(p_stk, ST_UNDERLINE|ST_NOSCROLL|ST_CENTERTITLE|ST_GRAPHIC_BG);

                 ISTATIC_SetFontColor(p_stk, RGB_BLACK);
                 
                 (void) ISTATIC_SetText(p_stk,
                                   NULL,
                                   m_wstr,
                                   AEE_FONT_NORMAL,
                                   AEE_FONT_NORMAL);
                 
                 (void) ISTATIC_Redraw(p_stk);
            } 
            DrawBottomBar(pMe->m_pDisplay, &BBarParam); 
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);  
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        

        default:
            break;
    }
    return FALSE;
} // HandlePhoneInfo_SW_HW_PRL_DialogEvent

#endif


static boolean  HandlePhoneSarValueDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)

{
    BottomBar_Param_type  BBarParam ={0};
    AECHAR m_wstr[256]={0};    
    AECHAR WTitle[40] ={0};
    int n=0;
    IDialog *p_dlg = NULL;
    IStatic *p_stk = NULL;
    
    AECHAR sar_value[]=L"SAR: 0.280W/kg\n";    
   
    
    MSG_FATAL("%x, %x ,%x,HandlePhoneSarValueDialogEvent",eCode,wParam,dwParam);
    
    p_dlg = ISHELL_GetActiveDialog(pMe->m_pShell); 
    
    if(p_dlg==NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:  
          return TRUE;

        case EVT_DIALOG_START:
            ISHELL_PostEvent(pMe->m_pShell, AEECLSID_APP_SETTINGMENU, EVT_USER_REDRAW, 0,0);                                     
            return TRUE;

        case EVT_USER_REDRAW:
                 
             p_stk = (IStatic *) IDIALOG_GetControl(p_dlg, IDC_PHONE_SAR_VALUE);          
 
             (void) ISHELL_LoadResString(pMe->m_pShell,
                                        AEE_APPSSETTINGMENU_RES_FILE,
                                        IDS_PHONE_SAR_VALUE,
                                        WTitle,
                                        sizeof(WTitle));
             
			 IANNUNCIATOR_SetFieldText(pMe->m_pAnn, WTitle);

             WSTRCPY(m_wstr,sar_value);            
          
            BBarParam.eBBarType = BTBAR_BACK;            
            Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
           
            if(p_stk!=NULL)
            {
                 ISTATIC_SetProperties(p_stk, ST_UNDERLINE|ST_NOSCROLL|ST_CENTERTITLE|ST_GRAPHIC_BG);

                 ISTATIC_SetFontColor(p_stk, RGB_BLACK);
                 
                 (void) ISTATIC_SetText(p_stk,
                                   NULL,
                                   m_wstr,
                                   AEE_FONT_NORMAL,
                                   AEE_FONT_NORMAL);
                 
                 (void) ISTATIC_Redraw(p_stk);
            } 
            DrawBottomBar(pMe->m_pDisplay, &BBarParam); 
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);  
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        

        default:
            break;
    }
    return FALSE;
}









#ifdef FEATURE_SHORTCUT_IN_SETTINGS



/*==============================================================================
函数：
       HandleCallSettingDialogEvent
说明：
       IDD_PHONE_INFO_MENU对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleShortcutsMenuDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_SHORTCUTS_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("%x, %x ,%x,HandleShortcutsMenuDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    //SettingMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
                #ifdef FEATURE_VERSION_K232_Y105A
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,  
                        IDS_STR_NAVIGATION,
                        WTitle,
                        sizeof(WTitle));
                #else
                (void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,  
                        IDS_SHORTCUTS_MENU_TITLE,
                        WTitle,
                        sizeof(WTitle));
                #endif
				IANNUNCIATOR_SetFieldText(pMe->m_pAnn,WTitle);
            }            
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_KEYPAD_UP, IDS_KEYPAD_UP, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_KEYPAD_DOWN, IDS_KEYPAD_DOWN, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_KEYPAD_LEFT, IDS_KEYPAD_LEFT, NULL, 0);			
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_KEYPAD_RIGHT, IDS_KEYPAD_RIGHT, NULL, 0);
            return TRUE;

        case EVT_DIALOG_START:
            // 给菜单各菜单项加数字编号图标
            pMe->m_input_mode = 0;
            SettingMenu_SetItemNumIcon(pMenu);

            IMENUCTL_SetSel(pMenu, pMe->m_nSubDlgId);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_APP_SETTINGMENU,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;

        case EVT_USER_REDRAW:
            //(void)IMENUCTL_Redraw(pMenu);
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            pMe->m_nSubDlgId = wParam;
            switch (wParam)
            { 
                case IDS_KEYPAD_UP:                                        
                case IDS_KEYPAD_DOWN:
                case IDS_KEYPAD_LEFT:    				                    
                case IDS_KEYPAD_RIGHT:
                    CLOSE_DIALOG(DLGRET_SHORTCUTS_SELECT_MENU)
                    break;  

                default:
                    ASSERT_NOT_REACHABLE;               
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandlePhoneInfoDialogEvent




/*==============================================================================
函数：
       HandleCallSettingDialogEvent
说明：
       IDD_PHONE_INFO_MENU对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static uint32 getShortCutsIndex(CSettingMenu *pMe)
{
    if(pMe->m_nSubDlgId == IDS_KEYPAD_UP)
        return KEYPAD_UP;
    else if(pMe->m_nSubDlgId == IDS_KEYPAD_DOWN)
        return KEYPAD_DOWN;
    else if(pMe->m_nSubDlgId == IDS_KEYPAD_LEFT)        
        return KEYPAD_LEFT;
    else if(pMe->m_nSubDlgId == IDS_KEYPAD_RIGHT)                
        return KEYPAD_RIGHT;
    return KEYPAD_UP;
}

static uint32 getShortCutsArrayIndexWithCls(keypad_shortcuts_type *shortcuts,uint32 size,uint32 cls,char *args)
{
    int i=0;
    
    for (i=0;i<size;i++)
    {
        if(shortcuts[i].cls==cls&&(strlen(args)==0||!strcmp(shortcuts[i].args,args)))
        {
            return i;
        }
    }
    return 0;
}

static uint32 getShortCutsArrayIndexStr(keypad_shortcuts_type *shortcuts,uint32 size,uint32 Strid)
{
    int i=0;
    
    for (i=0;i<size;i++)
    {
        if(shortcuts[i].strid==Strid)
        {
            return i;
        }
    }
    return 0;
}


static boolean  HandleShortcutsSelectMenuDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    IMenuCtl *pMenu;
    BottomBar_Param_type  BBarParam ={0};
    AECHAR WTitle[40] ={0};
    keypad_shortcuts_t m_shortcuts_table[MAX_SHORTCUTS_SIZE];
    int n=0, keyindex = 0,arrayindex = 0;   //
    
    keypad_shortcuts_type m_shortcuts[] = {
            {IDS_SHORTCUTS_MESSAGE,AEECLSID_WMSAPP,{0}},
            {IDS_SHORTCUTS_MUSICPLAYER,AEECLSID_APP_MUSICPLAYER,{0}},
            {IDS_SHORTCUTS_FM,AEECLSID_APP_FMRADIO,{0}},
            {IDS_SHORTCUTS_ALARM,AEECLSID_ALARMCLOCK,{0}},            
            {IDS_SHORTCUTS_CAMERA,AEECLSID_APP_CAMERA,{0}},
            {IDS_SHORTCUTS_CALLLOG,AEECLSID_APP_RECENTCALL,{0}},
            {IDS_SHORTCUTS_SETTINGS,AEECLSID_APP_SETTINGMENU,{0}},
            {IDS_SHORTCUTS_GAMES,AEECLSID_GAME,NULL},
            {IDS_SHORTCUTS_SCHEDULER,AEECLSID_SCHEDULEAPP,{0}},
            {IDS_SHORTCUTS_CALCULATOR,AEECLSID_CALCAPP,{0}},
            {IDS_SHORTCUTS_CONTACTS,AEECLSID_APP_CONTACT,{0}},
            {IDS_SHORTCUTS_EXPLORER,AEECLSID_MEDIAGALLERY,{0}},
            {IDS_SHORTCUTS_VOICERECORDER,AEECLSID_RECORDER,{0}},
            #ifndef FEATURE_VERSION_K232_Y105A
            {IDS_SHORTCUTS_APPS,AEECLSID_APPLICATION,{0}},
            {IDS_SHORTCUTS_VIDEOPLAYER,AEECLSID_VIDEOPLAYER,{0}},
            {IDS_SHORTCUTS_VIDEORECORDER,AEECLSID_APP_CAMERA,"record"},  
            #endif
    };

        
    MSG_FATAL("%x, %x ,%x,HandleShortcutsSelectMenuDialogEvent",eCode,wParam,pMe->m_nSubDlgId);


    //实现菜单循环滚动功能
    //SettingMenu_AutoScroll(pMenu,eCode,wParam);
    pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_SHORTCUTS_SELECT);                                                     
    if(pMenu==NULL)
    {    
        MSG_FATAL("HandleShortcutsSelectMenuDialogEvent IDC_SHORTCUTS_SELECT failed",0,0,0);
        return FALSE;
    }                                                    

    keyindex = getShortCutsIndex(pMe);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:                        
            {
                AECHAR WTitle[40] = {0};
                (void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        pMe->m_nSubDlgId,
                        WTitle,
                        sizeof(WTitle));
                IANNUNCIATOR_SetFieldText(pMe->m_pAnn,WTitle);
            }   

            for (n=0;n<sizeof(m_shortcuts)/sizeof(keypad_shortcuts_type);n++)
            {
                IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, m_shortcuts[n].strid, m_shortcuts[n].strid, NULL, 0);
            }
            return TRUE;

        case EVT_DIALOG_START:
            
            (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_KEYPAD_SHORTCUTS_TABLE,
                                       &m_shortcuts_table,
                                       sizeof(m_shortcuts_table));
            
            arrayindex = getShortCutsArrayIndexWithCls(m_shortcuts,
                                                sizeof(m_shortcuts)/sizeof(keypad_shortcuts_type),
                                                m_shortcuts_table[keyindex].shortcuts_cls,
                                                m_shortcuts_table[keyindex].shortcuts_args);
            
            
            MSG_FATAL("HandleShortcutsSelectMenuDialogEvent %d, %d,",arrayindex,keyindex,0);
            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, m_shortcuts[arrayindex].strid, TRUE);
            IMENUCTL_SetSel(pMenu, m_shortcuts[arrayindex].strid);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);

            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_APP_SETTINGMENU,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);

                                     
            return TRUE;

        case EVT_USER_REDRAW:
        
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {                
                case AVK_CLR:                    
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;   
            
        case EVT_COMMAND:
            //switch (wParam)
            arrayindex = getShortCutsArrayIndexStr(m_shortcuts,
                                      sizeof(m_shortcuts)/sizeof(keypad_shortcuts_type),
                                      wParam);
            MSG_FATAL("HandleShortcutsSelectMenuDialogEvent %d, %d,",arrayindex,wParam,0);
            
            (void) ICONFIG_GetItem(pMe->m_pConfig,
                                                  CFGI_KEYPAD_SHORTCUTS_TABLE,
                                                  &m_shortcuts_table,
                                                  sizeof(m_shortcuts_table));
            m_shortcuts_table[keyindex].shortcuts_cls = m_shortcuts[arrayindex].cls;
            memset(m_shortcuts_table[keyindex].shortcuts_args,0x00,MAX_SHORTCUTS_ARGS_SIZE);
            strcpy(m_shortcuts_table[keyindex].shortcuts_args, m_shortcuts[arrayindex].args);
            
            DBGPRINTF("m_shortcuts_table shortcuts_args=%d %s\n",keyindex,m_shortcuts_table[keyindex].shortcuts_args);
            (void) ICONFIG_SetItem(pMe->m_pConfig,
                                                        CFGI_KEYPAD_SHORTCUTS_TABLE,
                                                        &m_shortcuts_table,
                                                        sizeof(m_shortcuts_table));
            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, wParam, TRUE);
            CLOSE_DIALOG(DLGRET_WARNING);
            return TRUE;
        
        default:
            break;
    }
    return FALSE;
} // HandlePhoneInfo_SW_HW_PRL_DialogEvent
#endif
static boolean  HandleAirPlaneDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)
    //static byte bytData = 0;
    //static boolean isSwitch = FALSE;
    int ret = 0;

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_MENU_AIRPLANE);
    MSG_FATAL("%x, %x ,%x,HandleTimeFontModeDialogEvent",eCode,wParam,dwParam);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_AIRPLANE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_ENABLE, IDS_ENABLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_DISABLE, IDS_DISABLE, NULL, 0);
            return TRUE;

        case EVT_DIALOG_START:
            {
                uint16 wItemID;
                byte nSelItem = 0;
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                
                /*
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_IDLE_DATETIME_MODE,
                                       &nSelItem,
                                       sizeof(nSelItem));
                switch (nSelItem)
                {
                     case TIMEFONTMODE_BIG:
                        wItemID = IDS_ENABLE;
                        break;
                     case TIMEFONTMODE_MEDIUM:
                        wItemID = IDS_DISABLE;
                        break;
                     default:
                     	wItemID = IDS_ENABLE;
                     	break;
                }
                */
                if(m_Aarplne)
                {
                    wItemID = IDS_ENABLE;
                }
                else
                {
                    wItemID = IDS_DISABLE;
                }
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wItemID, TRUE);
                IMENUCTL_SetSel(pMenu, wItemID);
            }
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                  default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {
                int nReturnStatus = -1; 
#ifdef FEATURE_ICM
                ICM *pICM = NULL;
#else
                IPhoneCtl *pIPhoneCtl = NULL;
#endif
#ifdef FEATURE_ICM
                nReturnStatus = ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CM, (void **) &pICM);   
                if((pICM == NULL) || (SUCCESS != nReturnStatus))
#else
                nReturnStatus = ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_PHONECTL, (void **) &pIPhoneCtl);   
                if((pIPhoneCtl == NULL) || (SUCCESS != nReturnStatus))
#endif
                {
                    MSG_FATAL("HandleSimDialogEvent:::::33333 ICM cerate Failed",0,0,0);
                    return FALSE;
                }

                switch (wParam)
                {
                    case  IDS_DISABLE:
                       m_Aarplne = FALSE;
#ifdef FEATURE_ICM
                    ICM_SetOperatingMode(pICM, AEECM_OPRT_MODE_ONLINE);
#else
                    IPHONECTL_SetOperatingMode(pIPhoneCtl, AEECM_OPRT_MODE_ONLINE);
#endif
                       
                       break;
                    case IDS_ENABLE:
                        m_Aarplne = TRUE;
#ifdef FEATURE_ICM
                    ICM_SetOperatingMode(pICM, AEECM_OPRT_MODE_LPM);
#else
                    IPHONECTL_SetOperatingMode(pIPhoneCtl, AEECM_OPRT_MODE_LPM);
#endif
                       break;
                    
                    default:
                       ASSERT_NOT_REACHABLE;

                }
                /*
                (void) ICONFIG_SetItem(pMe->m_pConfig,
                                       CFGI_IDLE_DATETIME_MODE,
                                       &nSelItem,
                                       sizeof(nSelItem));
                */
                //InitMenuIcons(pMenu);
                //SetMenuIcon(pMenu, wParam, TRUE);
                //(void)IMENUCTL_Redraw(pMenu);
                CLOSE_DIALOG(DLGRET_WARNING)
				break;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
}

#if  defined(FEATURE_VERSION_W317A)
//Add by pyuangui 2013-01-04
/*=============================================================================
FUNCTION:  HandleAutoCallRecordDialogEvent

DESCRIPTION:   auto call record

PARAMETERS:

=============================================================================*/

static boolean HandleAutoCallRecordDialogEvent(CSettingMenu *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_MENU_AUTOCALLRECORD);
	boolean m_autocallrecord;
        
    if (pMenu == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("HandleAutoCallRecordDialogEvent---eCode=%d",eCode,0,0);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:	
			IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pAnn,FALSE); 
			
			IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE,IDS_CALLSETTING__YES, IDS_CALLSETTING__YES, NULL, 0); 
			IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE,IDS_CALLSETTING__NO, IDS_CALLSETTING__NO, NULL, 0);  
            return TRUE;
            
        case EVT_DIALOG_START:
            {      
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
                IMENUCTL_SetOemProperties( pMenu, OEMMP_USE_MENU_STYLE);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
				InitMenuIcons(pMenu);
				{
                  uint16 nSelID = IDS_CALLSETTING__YES;
                  (void) ICONFIG_GetItem(pMe->m_pConfig,
                                         CFGI_AUTOCALLRECORD,
                                         &m_autocallrecord,
                                         sizeof(m_autocallrecord));
                                         
                  if (m_autocallrecord)
                  {
                      nSelID = IDS_CALLSETTING__YES;
                  }
                  else
                  {
                      nSelID = IDS_CALLSETTING__NO;
                  }
                  SetMenuIcon(pMenu, nSelID, TRUE);
                  IMENUCTL_SetSel(pMenu, nSelID);
                }
                (void) ISHELL_PostEvent(pMe->m_pShell, AEECLSID_APP_SETTINGMENU, EVT_USER_REDRAW,0,0);
            }
            return TRUE;
            
        case EVT_USER_REDRAW:			
			{				
		  		AECHAR WTitle[40] = {0};
				
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_CALLSETTING_CALLRECORD,
                        WTitle,
                        sizeof(WTitle));
                if(pMe->m_pAnn != NULL)
                {                								
				    IANNUNCIATOR_SetFieldText(pMe->m_pAnn,WTitle);
                }
		    }
		
            (void)IMENUCTL_Redraw(pMenu);
            return TRUE;

        case EVT_DIALOG_END:			
			//ISHELL_CloseApplet(pMe->m_pShell, FALSE);
            return TRUE;
		
        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
					CLOSE_DIALOG(DLGRET_CANCELED)                  
                    return TRUE;
                    
                default:
                    break;
          
            }
            return TRUE;
            
        case EVT_COMMAND:
            switch (wParam)
            {   
                case IDS_CALLSETTING__YES:
                {	
					m_autocallrecord = TRUE;
                    break;
                }
				case IDS_CALLSETTING__NO:
				{	
					m_autocallrecord = FALSE;
                    break;
                }
				
				default:
				{
					return FALSE;
				}
            }
			 (void) ICONFIG_SetItem(pMe->m_pConfig,
                                       CFGI_AUTOCALLRECORD,
                                       &m_autocallrecord,
                                       sizeof(m_autocallrecord));
			  CLOSE_DIALOG(DLGRET_CANCELED)
            return TRUE;       
            
        default:
            break;
    }             
    return FALSE;
}
//Add End
#endif

/*==============================================================================
函数：
       HandlePhoneSettingDialogEvent
说明：
       IDD_PHONESETTING_MENU对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandlePhoneSettingDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_PHONESETTING);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("%x, %x ,%x,HandlePhoneSettingDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    //SettingMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_PHONESETTING_TITLE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
#ifdef FEATURE_PLANEMODE
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_PLANE_MODE, IDS_PLANE_MODE, NULL, 0);
#endif
#ifndef FEATURE_VERSION_H19C
#ifdef FEATURE_LANGUAGE
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_LANGUAGE, IDS_LANGUAGE, NULL, 0);
#endif
#endif
#ifdef FEATURE_SET_BANNER
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_BANNER_TITLE, IDS_BANNER_TITLE, NULL, 0);
#endif
#ifdef FEATURE_DUAL_UIMCARD
#ifndef FEATURE_VERSION_S1000
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE,IDS_SIM_SET,IDS_SIM_SET,NULL,0);
#endif
#endif
#ifdef FEATURE_TIME_DATA_SETTING
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_TIMESETTING, IDS_TIMESETTING, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_DATESETTING, IDS_DATESETTING, NULL, 0);
#endif
#if defined(FEATURE_VERSION_W317A)
			//IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SMS_TREACKER_TIME, IDS_SMS_TREACKER_TIME, NULL, 0);
#endif
#ifdef FEATURE_KEYGUARD
	        #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)||defined(FEATURE_VERSION_W317A)
	        #else
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AUTOKEYGUARD_TITLE, IDS_AUTOKEYGUARD_TITLE, NULL, 0);
            #endif
#endif

#ifdef FEATURE_VERSION_KK5
            //IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_NETWORK_SETTINGS, IDS_NETWORK_SETTINGS, NULL, 0);
#endif

#ifdef	FEATURE_VERSION_MYANMAR
			IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SEARCHNET_MODE, IDS_SEARCHNET_MODE, NULL, 0);
			IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_IDLE_DATETIME_MOCD, IDS_IDLE_DATETIME_MOCD, NULL, 0);
#endif			
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh 200904007 add 触摸校准
            //IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_ADJUSTSETTING, IDS_ADJUSTSETTING, NULL, 0);
#endif //FEATURE_LCD_TOUCH_ENABLE
			
#ifdef FEATURE_SOUND_BO
#ifndef FEATURE_VERSION_K212_ND
			IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_YUYIIN_PACKGE, IDS_YUYIIN_PACKGE, NULL, 0);
#endif
#endif
#ifdef FEATURE_SHORTCUT_IN_SETTINGS
#ifdef FEATURE_VERSION_K232_Y105A
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_STR_NAVIGATION, IDS_STR_NAVIGATION, NULL, 0);
#endif
#endif
#ifdef FEATURE_VERSION_K232_Y105A1
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AIRPLANE, IDS_AIRPLANE, NULL, 0);
#endif

            return TRUE;

        case EVT_DIALOG_START:
             // 给菜单各菜单项加数字编号图标
            SettingMenu_SetItemNumIcon(pMenu);

            IMENUCTL_SetSel(pMenu, pMe->m_nSubDlgId);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            return TRUE;

        case EVT_DIALOG_END:
			ISHELL_CancelTimer( pMe->m_pShell, NULL, pMe);//wlh 20090407 add 消除电池充电显示
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            //pMe->m_nSubDlgId = IMENUCTL_GetSel(pMenu);
            pMe->m_nSubDlgId = wParam;
            switch (wParam)
            {
                
#ifdef FEATURE_PLANEMODE
                case IDS_PLANE_MODE:
                        /*(void) cm_ph_cmd_oprt_mode(NULL,
                             NULL,
                             CM_CLIENT_ID_ANONYMOUS,
                             SYS_OPRT_MODE_LPM);
                             */
                        CLOSE_DIALOG(DLGRET_PLANEMODE)
                    break;
#endif //FEATURE_PLANEMODE
#ifndef FEATURE_VERSION_H19C
#ifdef FEATURE_LANGUAGE
                case IDS_LANGUAGE:    //语言选择
                    CLOSE_DIALOG(DLGRET_LANGUAGE)
                    break;
#endif
#endif
#ifdef FEATURE_KEYGUARD
                case IDS_AUTOKEYGUARD_TITLE:         //自动锁键盘
                    CLOSE_DIALOG(DLGRET_AKG)
                    break;
#endif

#ifdef FEATURE_VERSION_KK5
                case IDS_NETWORK_SETTINGS:         
                    //CLOSE_DIALOG(DLGRET_NETWORK_SETTINGS)
                    CLOSE_DIALOG(DLGRET_NETWORK_PARAMS_EDIT)
                    break;
#endif

#ifdef FEATURE_SET_BANNER
                case IDS_BANNER_TITLE:      //待机问候语
                    CLOSE_DIALOG(DLGRET_BANNER)
                    break;
#endif
#ifdef FEATURE_DUAL_UIMCARD
                case IDS_SIM_SET:          //开机选卡
                    CLOSE_DIALOG(DLGRET_SIMSETTING)
                    break;
#endif
#ifdef FEATURE_TIME_DATA_SETTING
                case IDS_TIMESETTING:      //时间设置
                    CLOSE_DIALOG(DLGRET_TIMESETTING)
                    break;

                case IDS_DATESETTING:      //日期设置
                    CLOSE_DIALOG(DLGRET_DATESETTING)
                    break;
#endif
#if defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_W317A) || defined(FEATURE_VERSION_IC241A_MMX)|| defined (FEATURE_VERSION_KK5)
		 	   case IDS_SMS_TREACKER_TIME:     //TRRACK SMS设置
			   		CLOSE_DIALOG(DLGRET_TRACKERSMSSETTING)
			   		break;
#endif

#ifdef	FEATURE_VERSION_MYANMAR
				case IDS_SEARCHNET_MODE:  //搜网设置
					CLOSE_DIALOG(DLGRET_SEARCHMODE)
					break;
				case IDS_IDLE_DATETIME_MOCD:   //待机界面时间大小设置
					CLOSE_DIALOG(DLGRET_TIMEFONTMODE)
					break;
#endif
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh 20090407 add
				case IDS_ADJUSTSETTING:      //触摸校准设置
					ISHELL_StartApplet(pMe->m_pShell, AEECLSID_ADJUSTPENAPP); 
					break;
#endif
#ifdef FEATURE_SOUND_BO
				case IDS_YUYIIN_PACKGE:
    				CLOSE_DIALOG(DLGRET_SPEECH_SETTINGS)
					break;
#endif
#if defined(FEATURE_VERSION_K212_ND)
				case IDS_SET_SOS:
					CLOSE_DIALOG(DLGRET_SOS_SETTINGS)
					break;
#endif
#ifdef FEATURE_SHORTCUT_IN_SETTINGS
#ifdef FEATURE_VERSION_K232_Y105A
                case IDS_STR_NAVIGATION:                    
                    CLOSE_DIALOG(DLGRET_SHORTCUTS_MENU)
                    break;
#endif  
#endif
#ifdef FEATURE_SHORTCUT_IN_SETTINGS
                case IDS_AIRPLANE:
                    CLOSE_DIALOG(DLGRET_AIRPLANE_MODE)
                    break;
#endif
                default:
                    ASSERT_NOT_REACHABLE;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandlePhoneSettingDialogEvent


/*==============================================================================
函数：
       HandleCallSettingSelDialogEvent
说明：
       IDD_CALLSETTINGSEL 对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleCallSettingSelDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_CALLSETTINGSEL);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("%x, %x ,%x,HandleCallSettingSelDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    //SettingMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
#ifdef FEATRUE_SET_IP_NUMBER
            if(pMe->m_CallSettingSel ==  IDS_SET_IP_CALL_OPTION)
            {
                IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SET_DEFAULT, IDS_SET_DEFAULT, NULL, 0);
                IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SET_EDIT, IDS_SET_EDIT, NULL, 0);
                IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SET_DELETE, IDS_SET_DELETE, NULL, 0);
            }
            else
#endif /*FEATRUE_SET_IP_NUMBER*/
#ifdef FEATRUE_AUTO_POWER
            if(pMe->m_CallSettingSel == IDS_AM_PM_SET)
            {
                IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SETMENU_AM, IDS_SETMENU_AM, NULL, 0);
                IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SETMENU_PM, IDS_SETMENU_PM, NULL, 0);
            }
            else
#endif //#ifdef FEATRUE_AUTO_POWER
            {
                IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_ENABLE, IDS_ENABLE, NULL, 0);
                IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_DISABLE, IDS_DISABLE, NULL, 0);
            }
            return TRUE;

        case EVT_DIALOG_START:
            {
                byte   byte_return = 0;        //默认为打开。

                switch(pMe->m_CallSettingSel)
                {
                    case IDS_ANYKEY_ANSWER_TITLE://0x1
                    case IDS_COVER_ANSWER://0x2
                    case IDS_SEND_ANSWER://0x4
                    case IDS_HEADSET_ANSWER://0x8
                        ICONFIG_GetItem(pMe->m_pConfig,CFGI_ANYKEY_ANSWER,&byte_return,sizeof(byte_return));
                        break;


			        #if defined( FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_K202_LM129C)
					case IDS_CALL_TIME_REMINDER:
					#endif
                    case IDS_MINUTE_ALERT:      //50秒提醒
                        ICONFIG_GetItem(pMe->m_pConfig,CFGI_MINUTE_ALERT,&byte_return,sizeof(byte_return));
                        break;

                   case IDS_VOICE_PRIVACY:     //voice privacy
                        ICONFIG_GetItem(pMe->m_pConfig,CFGI_VOICEPRIVACY, &byte_return,sizeof(byte_return));
                        break;
#ifdef FEATRUE_SET_IP_NUMBER
                    case IDS_SET_IP_CALL_OPTION:
                    {
                        //IP_Number_Cfg   num_cfg;
                        //ICONFIG_GetItem(pMe->m_pConfig, CFGI_IP_NUMBER, (void*)&num_cfg, sizeof(IP_Number_Cfg));
                        //byte_return = (byte)pMe->ip_number_tab.ip_state;
                        break;
                    }
#ifdef FEATURE_CARRIER_TAIWAN_APBW
                    case IDS_SET_POUND:
                    {
                        boolean val = FALSE;
                        ICONFIG_GetItem(pMe->m_pConfig,CFGI_IP_POUND, &val,sizeof(val));
                        byte_return = (byte)val;
                        break;
                    }
#endif
#endif
                    case IDS_AUTO_REDIAL:
                    {
                        nv_auto_redial_type m_redial;
                        ICONFIG_GetItem(pMe->m_pConfig, CFGI_AUTO_REDIAL, (void*)&m_redial, sizeof(nv_auto_redial_type));
                        byte_return = (byte)m_redial.enable;
                        break;
                    }
#ifdef FEATRUE_AUTO_POWER
                    case IDS_AM_PM_SET:
                        byte_return = pMe->m_b_selete_Pm;
                        break;
#endif //#ifdef FEATRUE_AUTO_POWER
                    default:
                        return FALSE;
                }
#ifdef FEATRUE_SET_IP_NUMBER
                if(pMe->m_CallSettingSel !=  IDS_SET_IP_CALL_OPTION)
#endif /*FEATRUE_SET_IP_NUMBER*/
                {
                    InitMenuIcons(pMenu);

                    if(pMe->m_CallSettingSel == IDS_ANYKEY_ANSWER_TITLE )
                    {
                        if(byte_return & 0x1)
                        {
                            IMENUCTL_SetSel(pMenu, IDS_ENABLE);
                            SetMenuIcon(pMenu, IDS_ENABLE, TRUE);
                        }
                        else
                        {
                            IMENUCTL_SetSel(pMenu, IDS_DISABLE);
                            SetMenuIcon(pMenu, IDS_DISABLE, TRUE);
                        }
                    }
                    else if(pMe->m_CallSettingSel == IDS_COVER_ANSWER )
                    {
                        if(byte_return & 0x2)
                        {
                            IMENUCTL_SetSel(pMenu, IDS_ENABLE);
                            SetMenuIcon(pMenu, IDS_ENABLE, TRUE);
                        }
                        else
                        {
                            IMENUCTL_SetSel(pMenu, IDS_DISABLE);
                            SetMenuIcon(pMenu, IDS_DISABLE, TRUE);
                        }
                    }
                    else if(pMe->m_CallSettingSel == IDS_SEND_ANSWER )
                    {
                        if(byte_return & 0x4)
                        {
                            IMENUCTL_SetSel(pMenu, IDS_ENABLE);
                            SetMenuIcon(pMenu, IDS_ENABLE, TRUE);
                        }
                        else
                        {
                            IMENUCTL_SetSel(pMenu, IDS_DISABLE);
                            SetMenuIcon(pMenu, IDS_DISABLE, TRUE);
                        }
                    }
                    else if(pMe->m_CallSettingSel == IDS_HEADSET_ANSWER )
                    {
                        if(byte_return & 0x8)
                        {
                            IMENUCTL_SetSel(pMenu, IDS_ENABLE);
                            SetMenuIcon(pMenu, IDS_ENABLE, TRUE);
                        }
                        else
                        {
                            IMENUCTL_SetSel(pMenu, IDS_DISABLE);
                            SetMenuIcon(pMenu, IDS_DISABLE, TRUE);
                        }
                    }
#ifdef FEATRUE_AUTO_POWER
                    else if(pMe->m_CallSettingSel == IDS_AM_PM_SET)
                    {
                        if(byte_return == 0)    //am
                        {
                            IMENUCTL_SetSel(pMenu, IDS_SETMENU_AM);
                            SetMenuIcon(pMenu, IDS_SETMENU_AM, TRUE);
                        }
                        else                    //pm
                        {
                            IMENUCTL_SetSel(pMenu, IDS_SETMENU_PM);
                            SetMenuIcon(pMenu, IDS_SETMENU_PM, TRUE);
                        }
                    }
#endif //#ifdef FEATRUE_AUTO_POWER
                    else if(pMe->m_CallSettingSel == IDS_VOICE_PRIVACY)
                    {
#ifdef FEATURE_ICM
                        if(byte_return == AEECM_PRIVACY_PREF_ENHANCED-1)    //on
#else
                        if(byte_return == AEET_PRIVACY_PREF_ENHANCED-1)    //on
#endif
                        {
                            IMENUCTL_SetSel(pMenu, IDS_ENABLE);
                            SetMenuIcon(pMenu, IDS_ENABLE, TRUE);
                        }
#ifdef FEATURE_ICM
                        else if(byte_return == (AEECM_PRIVACY_PREF_STANDARD-1) || byte_return == (AEECM_PRIVACY_PREF_NONE-1))                   //off
#else
                        else if(byte_return == (AEET_PRIVACY_PREF_STANDARD-1) || byte_return == (AEET_PRIVACY_PREF_NONE-1))
#endif
                        {
                            IMENUCTL_SetSel(pMenu, IDS_DISABLE);
                            SetMenuIcon(pMenu, IDS_DISABLE, TRUE);
                        }
                    }
                    else
                    {
                        if(byte_return != 0)    //开
                        {
                            IMENUCTL_SetSel(pMenu, IDS_ENABLE);
                            SetMenuIcon(pMenu, IDS_ENABLE, TRUE);
                        }
                        else                    //关
                        {
                            IMENUCTL_SetSel(pMenu, IDS_DISABLE);
                            SetMenuIcon(pMenu, IDS_DISABLE, TRUE);
                        }
                    }
                }
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
				#if 0
                IMENUCTL_SetTitle(pMenu,AEE_APPSSETTINGMENU_RES_FILE,pMe->m_CallSettingSel,NULL);
				#else
			    {
			  		AECHAR WTitle[40] = {0};
					(void)ISHELL_LoadResString(pMe->m_pShell,
	                        AEE_APPSSETTINGMENU_RES_FILE,                                
	                        pMe->m_CallSettingSel,
	                        WTitle,
	                        sizeof(WTitle));
					IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
			    }
				#endif
            }
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
        {
            byte   callset;
            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, wParam, TRUE);
            switch(wParam)
            {
                case IDS_DISABLE:      //关
                    switch(pMe->m_CallSettingSel)
                    {
                        case IDS_ANYKEY_ANSWER_TITLE:
                        case IDS_COVER_ANSWER:
                        case IDS_SEND_ANSWER:
                        case IDS_HEADSET_ANSWER:
                            ICONFIG_GetItem(pMe->m_pConfig,CFGI_ANYKEY_ANSWER,  &callset, sizeof(callset));
                            if(pMe->m_CallSettingSel == IDS_ANYKEY_ANSWER_TITLE)
                            {
                                callset= callset & 0xE;
                            }
                            else  if(pMe->m_CallSettingSel == IDS_COVER_ANSWER)
                            {
                                callset= callset & 0xD;
                            }
                            else  if(pMe->m_CallSettingSel == IDS_SEND_ANSWER)
                            {
                                callset= callset & 0xB;
                            }
                            else  if(pMe->m_CallSettingSel == IDS_HEADSET_ANSWER)
                            {
                                callset= callset & 0x7;
                            }
                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_ANYKEY_ANSWER,&callset, sizeof(callset));
                            CLOSE_DIALOG(DLGRET_CANCELED)
                            return TRUE;

			            #if defined( FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_K202_LM129C)
						case IDS_CALL_TIME_REMINDER:
						#endif
                        case IDS_MINUTE_ALERT:      //50秒提醒
                            callset = OEMNV_ALERT_DISABLE;
                            ICONFIG_SetItem(pMe->m_pConfig,CFGI_MINUTE_ALERT,&callset,sizeof(callset));
                            CLOSE_DIALOG(DLGRET_WARNING)
                            return TRUE;

                       case IDS_VOICE_PRIVACY:     //voice privacy
#ifdef FEATURE_ICM
                            callset = AEECM_PRIVACY_PREF_STANDARD-1;
#else
                            callset = AEET_PRIVACY_PREF_STANDARD-1;
#endif							
                            ICONFIG_SetItem(pMe->m_pConfig,CFGI_VOICEPRIVACY,&callset,sizeof(callset));
                            CLOSE_DIALOG(DLGRET_WARNING)
                            return TRUE;
#ifdef FEATRUE_SET_IP_NUMBER
                        case IDS_IP_NUMBER_SET:
                        {
                            pMe->ip_number_tab.ip_state = 0;
                            CLOSE_DIALOG(DLGRET_CANCELED)
                            return TRUE;
                        }
#ifdef FEATURE_CARRIER_TAIWAN_APBW
                        case IDS_SET_POUND:
                        {
                            boolean set = FALSE;
                            ICONFIG_SetItem(pMe->m_pConfig,CFGI_IP_POUND,(void*)&set,sizeof(set));
                            CLOSE_DIALOG(DLGRET_CANCELED)
                            return TRUE;
                        }
#endif
#endif

                        case IDS_AUTO_REDIAL:
                        {
                            nv_auto_redial_type m_auto_redial;
                            m_auto_redial.enable = FALSE;
                            m_auto_redial.rings = 10;
                            ICONFIG_SetItem(pMe->m_pConfig,CFGI_AUTO_REDIAL,(void*)&m_auto_redial,sizeof(nv_auto_redial_type));
                            CLOSE_DIALOG(DLGRET_WARNING)
                            return TRUE;
                        }
                        default:
                            return FALSE;
                    }
                    break;

                case IDS_ENABLE:      //开
                    switch(pMe->m_CallSettingSel)
                    {
                        case IDS_ANYKEY_ANSWER_TITLE:     //任意键应答
                        case IDS_COVER_ANSWER:
                        case IDS_SEND_ANSWER:
                        case IDS_HEADSET_ANSWER:
                            ICONFIG_GetItem(pMe->m_pConfig,CFGI_ANYKEY_ANSWER,  &callset, sizeof(callset));
                            if(pMe->m_CallSettingSel == IDS_ANYKEY_ANSWER_TITLE)
                            {
                                callset= callset |0x1;
                            }
                            else  if(pMe->m_CallSettingSel == IDS_COVER_ANSWER)
                            {
                                callset= callset |0x2;
                            }
                            else  if(pMe->m_CallSettingSel == IDS_SEND_ANSWER)
                            {
                                callset= callset |0x4;
                            }
                            else  if(pMe->m_CallSettingSel == IDS_HEADSET_ANSWER)
                            {
                                callset= callset |0x8;
                            }
                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_ANYKEY_ANSWER,&callset, sizeof(callset));
                            CLOSE_DIALOG(DLGRET_CANCELED)
                            return TRUE;

			             #if defined( FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_K202_LM129C)
						case IDS_CALL_TIME_REMINDER:
						#endif
                        case IDS_MINUTE_ALERT:      //50秒提醒
                            callset = OEMNV_ALERT_ENABLE;
                            ICONFIG_SetItem(pMe->m_pConfig,CFGI_MINUTE_ALERT,&callset,sizeof(callset));
                            CLOSE_DIALOG(DLGRET_WARNING)
                            return TRUE;

                        case IDS_VOICE_PRIVACY:     //voice privacy
#ifdef FEATURE_ICM
                            callset = AEECM_PRIVACY_PREF_ENHANCED-1;
#else
                            callset = AEET_PRIVACY_PREF_ENHANCED-1;
#endif							
                            ICONFIG_SetItem(pMe->m_pConfig,CFGI_VOICEPRIVACY,&callset,sizeof(callset));
                            CLOSE_DIALOG(DLGRET_WARNING)
                            return TRUE;
#ifdef FEATURE_CARRIER_TAIWAN_APBW
#ifdef FEATRUE_SET_IP_NUMBER
                        case IDS_SET_POUND:
                        {
                            boolean set = TRUE;
                            ICONFIG_SetItem(pMe->m_pConfig,CFGI_IP_POUND,(void*)&set,sizeof(set));
                            CLOSE_DIALOG(DLGRET_CANCELED)
                            return TRUE;
                        }
#endif
#endif
                        case IDS_AUTO_REDIAL:
                        {
                            nv_auto_redial_type m_auto_redial;
                            m_auto_redial.enable = TRUE;
                            m_auto_redial.rings = 10;
                            ICONFIG_SetItem(pMe->m_pConfig,CFGI_AUTO_REDIAL,(void*)&m_auto_redial,sizeof(nv_auto_redial_type));
                            CLOSE_DIALOG(DLGRET_WARNING)
                            return TRUE;
                        }
                        default:
                            return FALSE;
                    }
                    break;
#ifdef FEATRUE_SET_IP_NUMBER
                case IDS_SET_DELETE:
                {
                    uint16 i = 0;
                    if(pMe->m_ip_sel_num == 0) //del the first node
                    {
                        if(pMe->ip_number_tab.head)
                        {
                            number_table * phead = NULL;
                            phead = pMe->ip_number_tab.head;
                            if(WSTRNCMP(pMe->ip_number_tab.default_ip,phead->number,OEM_IP_NUMBER_MAX+1) == 0)
                            {
                                MEMSET(pMe->ip_number_tab.default_ip, 0 ,sizeof(pMe->ip_number_tab.default_ip));
                            }
                            pMe->ip_number_tab.head = pMe->ip_number_tab.head->next;
                            FREEIF(phead);
                        }
                    }
                    else
                    {
                        number_table * phead = pMe->ip_number_tab.head;
                        while(phead)
                        {
                            if( i == (pMe->m_ip_sel_num -1))//find the node,del the next node
                            {
                                number_table * temp = NULL;
                                temp = phead->next;
                                if(temp)
                                {
                                    phead->next = temp->next;
                                    if(phead->next == NULL) //del the last node,need change tial point
                                    {
                                        pMe->ip_number_tab.tial = phead;
                                    }
                                    if(WSTRNCMP(pMe->ip_number_tab.default_ip,temp->number,OEM_IP_NUMBER_MAX+1) == 0)
                                    {
                                        MEMSET(pMe->ip_number_tab.default_ip, 0 ,sizeof(pMe->ip_number_tab.default_ip));
                                    }
                                    FREEIF(temp);
                                    break;
                                }
                            }
                            phead = phead->next;
                            i++;
                        }
                    }
                    pMe->ip_number_tab.count --;
                    if(pMe->ip_number_tab.count == 0)
                    {
                        pMe->ip_number_tab.ip_state =0;
                        pMe->ip_number_tab.head = NULL;
                        pMe->ip_number_tab.tial = NULL;
                    }
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    //ISHELL_PostEvent( pMe->m_pShell,AEECLSID_APP_SETTINGMENU,EVT_USER_REDRAW, 0,0);
                    return TRUE;
                }

                case IDS_SET_EDIT:
                {
                    pMe->m_input_mode = IDS_SET_IP_CALL_OPTION;
                    CLOSE_DIALOG(DLGRET_CALLFORWARDINPUT)
                    return TRUE;
                }

                case IDS_SET_DEFAULT:
                {
                    number_table * temp = NULL;
                    temp = SettingMenu_Search_Node(&pMe->ip_number_tab,pMe->m_ip_sel_num);
                    MEMSET(pMe->ip_number_tab.default_ip, 0 ,sizeof(pMe->ip_number_tab.default_ip));
                    WSTRLCPY(pMe->ip_number_tab.default_ip,temp->number,OEM_IP_NUMBER_MAX+1);
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                }
#endif //#ifdef FEATRUE_SET_IP_NUMBER
#ifdef FEATRUE_AUTO_POWER
                case IDS_SETMENU_AM:
                case IDS_SETMENU_PM:
                    pMe->m_b_selete_Pm = wParam - IDS_SETMENU_AM;
                    if(Setting_CClockApps_HandleCmdEvent(pMe))
                    {
                        CLOSE_DIALOG(DLGRET_CALLSETTING)
                    }
                    return TRUE;
#endif//#ifdef FEATRUE_AUTO_POWER
                default:
                    ASSERT_NOT_REACHABLE;
            }
            return TRUE;
        }

        default:
            break;
    }
    return FALSE;
} // HandleOtherSelDialogEvent


/*==============================================================================
函数：
       HandleNetworkDialogEvent
说明：
       IDD_NETWORKSETTING_MENU对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleNetworkDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_NETWORKSETTING);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("%x, %x ,%x,HandleNetworkDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    //SettingMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_NETWORKSETTING_TITLE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
#if !defined(FEATURE_VERSION_C192_MMX)
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_CALLFORWARD_DIVERT, IDS_CALLFORWARD_DIVERT, NULL, 0);
#endif
#ifdef FEATURE_CARRIER_ISRAEL_PELEPHONE
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_CUSTOMER_SERVER, IDS_CUSTOMER_SERVER, NULL, 0);
#else
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_CALLFORWARD_WAIT, IDS_CALLFORWARD_WAIT, NULL, 0);
#endif
#ifdef FEATURE_CARRIER_SUDAN_SUDATEL
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_STRING_DND, IDS_STRING_DND, NULL, 0);
#endif
            return TRUE;

        case EVT_DIALOG_START:
            // 给菜单各菜单项加数字编号图标
            SettingMenu_SetItemNumIcon(pMenu);

            IMENUCTL_SetSel(pMenu, pMe->m_sSubDlgId);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            pMe->m_sSubDlgId = IMENUCTL_GetSel(pMenu);
            switch (wParam)
            {
                case IDS_CALLFORWARD_DIVERT:     //呼叫转移
                    CLOSE_DIALOG(DLGRET_DIVERT)
                    break;

                case IDS_CALLFORWARD_WAIT:       //呼叫等待
                    pMe->m_CFType = CALLFORWARD_WAIT;
                    CLOSE_DIALOG(DLGRET_CALLFORWARDSEL)
                    break;
#ifdef FEATURE_CARRIER_ISRAEL_PELEPHONE
                case IDS_CUSTOMER_SERVER:
                    STRLCPY(pMe->m_callnumber,"*166",sizeof(pMe->m_callnumber));
                    SettingMenu_MakeForwardCall(pMe,pMe->m_callnumber);
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    break;
#endif
#ifdef FEATURE_CARRIER_SUDAN_SUDATEL
                 case IDS_STRING_DND:
                    pMe->m_CFType = CALLFORWARD_DND;
                    CLOSE_DIALOG(DLGRET_CALLFORWARDSEL)
                    return TRUE;
#endif
                default:
                    ASSERT_NOT_REACHABLE;
            }
            return TRUE;

        default:
            break;
    }

    return FALSE;
} // HandleNetworkDialogEvent


/*==============================================================================
函数：
       HandleDivertDialogEvent
说明：
       IDD_DIVERT_MENU对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleDivertDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_DIVERT);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("%x, %x ,%x,HandleDivertDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    //SettingMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_CALLFORWARD_DIVERT,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_CALLFORWARD_BUSY, IDS_CALLFORWARD_BUSY, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_CALLFORWARD_NOANSWER, IDS_CALLFORWARD_NOANSWER, NULL, 0);
#ifndef FEATURE_CARRIER_VENEZUELA_MOVILNET   
#ifdef FEATURE_VERSION_K202_LM129C //xxzhen
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_CALLFORWARD_NOCONNECT_UNCONDITIONAL, IDS_CALLFORWARD_NOCONNECT_UNCONDITIONAL, NULL, 0);
#else
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_CALLFORWARD_NOCONNECT, IDS_CALLFORWARD_NOCONNECT, NULL, 0);
#endif
#endif
#ifndef FEATURE_VERSION_W021_C11
#ifndef FEATURE_VERSION_K232_Y101
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_CALLFORWARD_ANYWAY, IDS_CALLFORWARD_ANYWAY, NULL, 0);
#endif
#endif
#if (!defined FEATURE_CARRIER_VENEZUELA_MOVILNET) && (!defined FEATURE_CARRIER_THAILAND_HUTCH)  && (!defined FEATURE_VERSION_K232_Y101)   
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_CALLFORWARD_CANCELALL, IDS_CALLFORWARD_CANCELALL, NULL, 0);
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH  
            return TRUE;

        case EVT_DIALOG_START:
            // 给菜单各菜单项加数字编号图标
            SettingMenu_SetItemNumIcon(pMenu);

            IMENUCTL_SetSel(pMenu, pMe->m_sSubDlgId);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            //pMe->m_sSubDlgId = IMENUCTL_GetSel(pMenu);
            pMe->m_sSubDlgId = wParam;
            switch (wParam)
            {
                case IDS_CALLFORWARD_BUSY:      //遇忙转移
                    pMe->m_CFType = CALLFORWARD_BUSY;
                    CLOSE_DIALOG(DLGRET_CALLFORWARDSEL)
                    return TRUE;

                case IDS_CALLFORWARD_NOANSWER:  //无应答转移
                    pMe->m_CFType = CALLFORWARD_NOANSWER;
                    CLOSE_DIALOG(DLGRET_CALLFORWARDSEL)
                    return TRUE;
				#ifdef FEATURE_VERSION_K202_LM129C //xxzhen
                case IDS_CALLFORWARD_NOCONNECT_UNCONDITIONAL:
				#endif

#if defined(FEATURE_VERSION_K232_Y101)||defined(FEATURE_VERSION_K232_Y105A)
                case IDS_CALLFORWARD_NOCONNECT: //未接通转移
                    pMe->m_CFType = CALLFORWARD_ANYWAY;
                    CLOSE_DIALOG(DLGRET_CALLFORWARDSEL)
                    return TRUE;

                case IDS_CALLFORWARD_ANYWAY:     //无条件转移
                    pMe->m_CFType = CALLFORWARD_NOCONNECT;
                    CLOSE_DIALOG(DLGRET_CALLFORWARDSEL)
                    return TRUE;
#else
                case IDS_CALLFORWARD_NOCONNECT: //未接通转移
                    pMe->m_CFType = CALLFORWARD_NOCONNECT;
                    CLOSE_DIALOG(DLGRET_CALLFORWARDSEL)
                    return TRUE;

                case IDS_CALLFORWARD_ANYWAY:     //无条件转移
                    pMe->m_CFType = CALLFORWARD_ANYWAY;
                    CLOSE_DIALOG(DLGRET_CALLFORWARDSEL)
                    return TRUE;
#endif                    

                case IDS_CALLFORWARD_CANCELALL: //取消全部转移
                {
					#ifdef FEATURE_VERSION_W021_C11
					STRCPY(pMe->m_callnumber,OEMNV_CALLFORWARD_UNCONDITIONAL_DISABLE); 
					#else
                    SettingMenu_Process_Feature_Code(pMe,CFGI_CALLFORWARD_DISABLE_ALL);
					#endif
                    if(STRLEN(pMe->m_callnumber) == 0)
                    {
#ifdef FEATURE_CALL_FORWARD_USER_INPUT					
						CLOSE_DIALOG(DLGRET_CALLFORWARDINPUT)
#else						
                        CLOSE_DIALOG(DLGRET_WARNING)                         
#endif                        
                    }
                    else
                    {
                        SettingMenu_MakeForwardCall(pMe,pMe->m_callnumber);
#ifndef FEATURE_CARRIER_THAILAND_HUTCH                           
                        CLOSE_DIALOG(DLGRET_CANCELED)
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH  
                    }
                    return TRUE;
                }

                default:
                    ASSERT_NOT_REACHABLE;
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE
        case EVT_PEN_UP:
            {
                int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
                int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
                AEEDeviceInfo devinfo;
                int nBarH ;
                AEERect rc;
                MSG_FATAL("Setting_HandleAuto_Power_DialogEvent wXPos=%d ,wYPos=%d",wXPos,wYPos,0);
                 
                nBarH = GetBottomBarHeight(pMe->m_pDisplay);
                MEMSET(&devinfo, 0, sizeof(devinfo));
                ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
                SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);  
                if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
                {
                    if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                    {
                         boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_SETTINGMENU,EVT_USER,AVK_SELECT,0);
                         return rt;
                    } 
                    else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//右
                    {                       
                         boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_SETTINGMENU,EVT_USER,AVK_CLR,0);
                         return rt;
                    }
                }  
            
            }
            break;
                        
#endif

        default:
            break;
    }
    return FALSE;
} // HandleDivertDialogEvent


/*==============================================================================
函数：
       HandleCallForwardSelDialogEvent
说明：
       IDD_CALLFORWARD_SEL对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleCallForwardSelDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_CALLFORWARD_SEL);
    uint16 selete_item = 0;
    if (pMenu == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("%x, %x ,%x,HandleCallForwardSelDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    //SettingMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            pMe->m_input_mode = 0;
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_CALLFORWARD_START, IDS_CALLFORWARD_START, NULL, 0);
            #if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)|| defined (FEATURE_VERSION_KK5)
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_CALLFORWARD_DEACTIVATE, IDS_CALLFORWARD_DEACTIVATE, NULL, 0);
            #else
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_CALLFORWARD_CANCEL, IDS_CALLFORWARD_CANCEL, NULL, 0);
            #endif
#ifdef FEATURE_CARRIER_SUDAN_SUDATEL
            if(CALLFORWARD_CNIR != pMe->m_CFType && CALLFORWARD_DND != pMe->m_CFType)
            {
                if(CALLFORWARD_WAIT == pMe->m_CFType)
                {
                    //IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_STRING_TEMP_CANCAL, IDS_STRING_TEMP_CANCAL, NULL, 0);
                }
                else
                {
                    IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_TO_VOICEMAIL, IDS_TO_VOICEMAIL, NULL, 0);
                }
            }
#endif
            return TRUE;

        case EVT_DIALOG_START:
            switch (pMe->m_CFType)
            {
                case CALLFORWARD_BUSY:      //遇忙转移
                    MSG_FATAL("CALLFORWARD_BUSY",0,0,0);
                    pMe->m_nResID = IDS_CALLFORWARD_BUSY;
                    selete_item = CFGI_CALLFORWARD_BUSY_ENABLE;
					#ifdef FEATURE_VERSION_W021_C11
					STRCPY(pMe->m_callnumber,OEMNV_CALLFORWARD_BUSY_ENABLE); 
					#endif
                    break;

                case CALLFORWARD_NOANSWER:  //无应答转移
                    pMe->m_nResID = IDS_CALLFORWARD_NOANSWER;
                    selete_item = CFGI_CALLFORWARD_NOANSWER_ENABLE;
					#ifdef FEATURE_VERSION_W021_C11
					STRCPY(pMe->m_callnumber,OEMNV_CALLFORWARD_NOANSWER_ENABLE);
					#endif
                    break;

                case CALLFORWARD_NOCONNECT: //未接通转移
                	#ifdef FEATURE_VERSION_K202_LM129C //xxzhen
					 pMe->m_nResID = IDS_CALLFORWARD_NOCONNECT_UNCONDITIONAL;
                    #else
                    pMe->m_nResID = IDS_CALLFORWARD_NOCONNECT;
					#endif
                    //selete_item = CFGI_CALLFORWARD_UNCONDITIONAL_ENABLE;
                    selete_item = CFGI_CALLFORWARD_UNREACHABLE_ENABLE;
					
                    #ifdef FEATURE_VERSION_W021_C11
					STRCPY(pMe->m_callnumber,OEMNV_CALLFORWARD_UNCONDITIONAL_ENABLE);
                    #endif
                    break;

                case CALLFORWARD_ANYWAY:    //无条件转移
                    pMe->m_nResID = IDS_CALLFORWARD_ANYWAY;
                    //selete_item = CFGI_CALLFORWARD_UNREACHABLE_ENABLE;
                    selete_item = CFGI_CALLFORWARD_UNCONDITIONAL_ENABLE;
                    break;

                case CALLFORWARD_WAIT:      //呼叫等待
                    pMe->m_nResID = IDS_CALLFORWARD_WAIT;
                    selete_item = CFGI_CALLFORWARD_WAIT_ENABLE;
					#ifdef FEATURE_VERSION_W021_C11
					STRCPY(pMe->m_callnumber,OEMNV_CALLFORWARD_WAIT_ENABLE);
					#endif
                    break;
#ifdef FEATURE_CARRIER_SUDAN_SUDATEL
                //case CALLFORWARD_CNIR:
                //    pMe->m_nResID = IDS_STRING_CNIR;
                //    selete_item = CFGI_CALLFORWARD_CNIR_ENABLE;
                //    break;

                case CALLFORWARD_DND:
                    pMe->m_nResID = IDS_STRING_DND;
                    selete_item = CFGI_CALLFORWARD_DND_ENABLE;
                    break;
#endif
                default:
                    return FALSE;
            }
			#ifndef FEATURE_VERSION_W021_C11
            SettingMenu_Process_Feature_Code(pMe,selete_item);
			#endif
            // 给菜单各菜单项加数字编号图标
            SettingMenu_SetItemNumIcon(pMenu);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
			#if 0
            (void)IMENUCTL_SetTitle(pMenu,
                                    AEE_APPSSETTINGMENU_RES_FILE,
                                    pMe->m_nResID,
                                    NULL);
			#else
		    {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        pMe->m_nResID,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
		    }
			#endif
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            pMe->m_input_mode = wParam;
            switch (wParam)
            {
                case IDS_CALLFORWARD_START:     //启动
#ifdef FEATURE_CARRIER_ISRAEL_PELEPHONE
                    if(pMe->m_CFType == CALLFORWARD_NOCONNECT)
                    {
                        SettingMenu_Process_Feature_Code(pMe,CFGI_CALLFORWARD_NOCONNECT_ENABLE);
                    }
#endif
                    if(STRLEN(pMe->m_callnumber) == 0)
                    {
#ifdef FEATURE_CALL_FORWARD_USER_INPUT					
                        CLOSE_DIALOG(DLGRET_CALLFORWARDINPUT)
#else
						CLOSE_DIALOG(DLGRET_WARNING)
#endif						
                        return TRUE;
                    }

                    if(pMe->m_CFType == CALLFORWARD_WAIT
/*                        
#ifdef FEATURE_VERSION_C337
                        ||pMe->m_CFType == CALLFORWARD_BUSY
                        ||pMe->m_CFType == CALLFORWARD_NOANSWER
                        ||pMe->m_CFType == CALLFORWARD_NOCONNECT
                        ||pMe->m_CFType == CALLFORWARD_ANYWAY                        
#endif
*/
#ifdef FEATURE_CARRIER_ISRAEL_PELEPHONE
                        ||pMe->m_CFType == CALLFORWARD_NOCONNECT
#endif
#ifdef FEATURE_CARRIER_SUDAN_SUDATEL
                        ||pMe->m_CFType == CALLFORWARD_CNIR
                        ||pMe->m_CFType == CALLFORWARD_DND
#endif
                        )
                    {
                        SettingMenu_MakeForwardCall(pMe,pMe->m_callnumber);
#ifndef FEATURE_CARRIER_THAILAND_HUTCH                           
                        CLOSE_DIALOG(DLGRET_CANCELED)
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH  
                    }
                    else
                    {
                        CLOSE_DIALOG(DLGRET_CALLFORWARDINPUT)
                    }
                    break;
                case IDS_CALLFORWARD_DEACTIVATE:
                case IDS_CALLFORWARD_CANCEL:  //取消
                    {

                        switch (pMe->m_CFType)
                        {
                            case CALLFORWARD_BUSY:      //遇忙转移
                                selete_item = CFGI_CALLFORWARD_BUSY_DISABLE;
								#ifdef FEATURE_VERSION_W021_C11
								STRCPY(pMe->m_callnumber,OEMNV_CALLFORWARD_BUSY_DISABLE);
								#endif
                                break;

                            case CALLFORWARD_NOANSWER:  //无应答转移
                                selete_item = CFGI_CALLFORWARD_NOANSWER_DISABLE;
								#ifdef FEATURE_VERSION_W021_C11
								STRCPY(pMe->m_callnumber,OEMNV_CALLFORWARD_NOANSWER_DISABLE);
								#endif
                                break;

                            case CALLFORWARD_NOCONNECT: //未接通转移
                                //selete_item = CFGI_CALLFORWARD_UNCONDITIONAL_DISABLE;
                                selete_item = CFGI_CALLFORWARD_UNREACHABLE_DISABLE;
								#ifdef FEATURE_VERSION_W021_C11
								STRCPY(pMe->m_callnumber,OEMNV_CALLFORWARD_UNCONDITIONAL_DISABLE);
								#endif
                                break;

                            case CALLFORWARD_ANYWAY:    //无条件转移
                                //selete_item = CFGI_CALLFORWARD_UNREACHABLE_DISABLE;
                                selete_item = CFGI_CALLFORWARD_UNCONDITIONAL_DISABLE;
                                break;

                            case CALLFORWARD_WAIT:      //呼叫等待
                                selete_item = CFGI_CALLFORWARD_WAIT_DISABLE;
								#ifdef FEATURE_VERSION_W021_C11
								STRCPY(pMe->m_callnumber,OEMNV_CALLFORWARD_WAIT_DISABLE);
								#endif
                                break;
#ifdef FEATURE_CARRIER_SUDAN_SUDATEL
                            case CALLFORWARD_DND:
                                selete_item = CFGI_CALLFORWARD_DND_DISABLE;
                                break;

                            case CALLFORWARD_CNIR:
                                selete_item = CFGI_CALLFORWARD_CNIR_DISABLE;
                                break;
#endif
                            default:
                                return FALSE;
                        }
						#ifndef FEATURE_VERSION_W021_C11
                        SettingMenu_Process_Feature_Code(pMe,selete_item);
						#endif
                        if(STRLEN(pMe->m_callnumber) == 0)
                        {
#ifdef 	FEATURE_CALL_FORWARD_USER_INPUT					
                            CLOSE_DIALOG(DLGRET_CALLFORWARDINPUT)
#else
							CLOSE_DIALOG(DLGRET_WARNING)
#endif							
      
                        }
                        else
                        {
                            SettingMenu_MakeForwardCall(pMe,pMe->m_callnumber);
#ifndef FEATURE_CARRIER_THAILAND_HUTCH                                
                            CLOSE_DIALOG(DLGRET_CANCELED)
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH  
                        }
                        break;
                    }
#ifdef FEATURE_CARRIER_SUDAN_SUDATEL
                //case IDS_STRING_TEMP_CANCAL:
                //    SettingMenu_Process_Feature_Code(pMe,CFGI_CALLFORWARD_WAIT_TEMP_DISABLE);
                //    if(STRLEN(pMe->m_callnumber) == 0)
                //    {
                //        CLOSE_DIALOG(DLGRET_WARNING)
                //    }
                //    else
                //    {
                //        SettingMenu_MakeForwardCall(pMe,pMe->m_callnumber);
                //        CLOSE_DIALOG(DLGRET_CANCELED)
                //    }
                //    break;

                case IDS_TO_VOICEMAIL:
                    switch (pMe->m_CFType)
                    {
                        case CALLFORWARD_BUSY:
                            selete_item = CFGI_CALLFORWARD_BUSY_VOICEMAIL;
                            break;

                        case CALLFORWARD_NOANSWER:
                            selete_item = CFGI_CALLFORWARD_NOANSWER_VOICEMAIL;
                            break;

                        case CALLFORWARD_NOCONNECT:
                            selete_item = CFGI_CALLFORWARD_NOCONNECT_VOICEMAIL;
                            break;

                        case CALLFORWARD_ANYWAY:
                            selete_item = CFGI_CALLFORWARD_ANYWAY_VOICEMAIL;
                            break;
                    }
                    SettingMenu_Process_Feature_Code(pMe,selete_item);
                    if(STRLEN(pMe->m_callnumber) == 0)
                    {
                        CLOSE_DIALOG(DLGRET_WARNING)
                    }
                    else
                    {
                        SettingMenu_MakeForwardCall(pMe,pMe->m_callnumber);
                        CLOSE_DIALOG(DLGRET_CANCELED)
                    }
                    break;
#endif
                default:
                    ASSERT_NOT_REACHABLE;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandleCallForwardSelDialogEvent


/*==============================================================================
函数：
       HandleCallForwardInputDialogEvent
说明：
       IDD_CALLFORWARD_INPUT对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleCallForwardInputDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    ITextCtl *pTextCtl= (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                      IDC_CALLFORWARD_INPUT);
    if (pTextCtl == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("%x, %x ,%x,HandleCallForwardInputDialogEvent",eCode,wParam,dwParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
#ifdef FEATRUE_SET_IP_NUMBER
            pMe->need_save_default = FALSE;
#endif
            return TRUE;

        case EVT_DIALOG_START:
        {
            AEERect rc;

            SETAEERECT(&rc, 0, 0, pMe->m_rc.dx, 32);//pMe->m_rc.dy - 16);
            ITEXTCTL_SetRect(pTextCtl,  &rc);
            //ICONTROL_SetRect((IControl*)pTextCtl, &pMe->m_rc);
#ifdef FEATRUE_SET_IP_NUMBER
            if(pMe->m_input_mode == IDS_IP_NUMBER_SET)
            {
                ITEXTCTL_SetMaxSize (pTextCtl, OEM_IP_NUMBER_MAX);
            }
            else if (pMe->m_input_mode == IDS_SET_IP_CALL_OPTION)
            {
                number_table * temp = NULL;
                temp = SettingMenu_Search_Node(&pMe->ip_number_tab,pMe->m_ip_sel_num);
                ITEXTCTL_SetMaxSize (pTextCtl, OEM_IP_NUMBER_MAX);                 
                if(temp)
                {
                    if(WSTRNCMP(pMe->ip_number_tab.default_ip,temp->number,OEM_IP_NUMBER_MAX+1) == 0)
                    {
                        pMe->need_save_default = TRUE;
                    }
                    ITEXTCTL_SetText(pTextCtl, temp->number, -1);
                }
                ITEXTCTL_SetCursorPos (pTextCtl, TC_CURSOREND);
            }
            else
#endif
            {
                ITEXTCTL_SetMaxSize (pTextCtl, 32);
            }
            ITEXTCTL_SetProperties( pTextCtl, TP_FRAME | TP_NOUPDATE|TP_FOCUS_NOSEL);
            //ITEXTCTL_SetProperties( pTextCtl, TP_FRAME | TP_FIXSETRECT |TP_EDITNUMBER_PTSTRING );
            (void) ITEXTCTL_SetInputMode(pTextCtl, AEE_TM_NUMBERS);            
            (void) ISHELL_PostEvent( pMe->m_pShell,AEECLSID_APP_SETTINGMENU,EVT_USER_REDRAW,0,0);
            return TRUE;
        }

        case EVT_KEY_RELEASE:
        case EVT_USER_REDRAW:
            if ( WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0 &&
                 ITEXTCTL_GetCursorPos(pTextCtl) != TC_CURSORSTART )
            {
                //drawBottomBar(pMe->m_pDisplay, AEE_FONT_NORMAL,BTBAR_OK_DELETE);
                #ifndef FEATURE_ALL_KEY_PAD
                SETTING_MENU_DRAW_BOTTOMBAR(BTBAR_OK_DELETE)
                #else
                SETTING_MENU_DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                #endif
            }
            else
            {
                //drawBottomBar(pMe->m_pDisplay, AEE_FONT_NORMAL,BTBAR_OK_BACK);
                SETTING_MENU_DRAW_BOTTOMBAR(BTBAR_OK_BACK)
            } 
            // 统一更新界面
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            #ifdef FEATURE_LCD_TOUCH_ENABLE
            TSIM_NumberKeypad(TRUE);
            #endif 
            (void)ITEXTCTL_Redraw(pTextCtl);

            return TRUE;

        case EVT_DIALOG_END:
#ifdef FEATRUE_SET_IP_NUMBER
            pMe->need_save_default = FALSE;
#endif
#ifdef FEATURE_LCD_TOUCH_ENABLE
            TSIM_NumberKeypad(FALSE);
#endif

            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                case AVK_SELECT:
                case AVK_INFO:
                    {
                        AECHAR   CharNum[32] = {(AECHAR)'\0'};    //存储转移号码

                        ITEXTCTL_GetText(pTextCtl, CharNum, sizeof(CharNum));

                        if(CharNum[0] == (AECHAR)'\0')
                        {
                            CLOSE_DIALOG(DLGRET_WARNING)
                        }
                        else
                        {
#ifdef FEATRUE_SET_IP_NUMBER
                            if(pMe->m_input_mode == IDS_IP_NUMBER_SET)
                            {
                                number_table *num_tab = NULL;
                                num_tab = (number_table *)MALLOC(sizeof(number_table));
                                if(num_tab)
                                {
                                    //ITEXTCTL_GetText(pTextCtl,  num_tab->number, OEM_IP_NUMBER_MAX);
                                    WSTRLCPY(num_tab->number,CharNum,OEM_IP_NUMBER_MAX+1);
                                    num_tab->next = NULL;
                                    if(pMe->ip_number_tab.head == NULL)
                                    {
                                        pMe->ip_number_tab.head = num_tab;
                                    }

                                    if(pMe->ip_number_tab.tial)
                                    {
                                        pMe->ip_number_tab.tial->next = num_tab;//add to last node
                                    }
                                    pMe->ip_number_tab.tial = num_tab;
                                    pMe->ip_number_tab.ip_state = 1;
                                    pMe->ip_number_tab.count ++;
                                    SettingMenu_Print_IP_Number(pMe);
                                }
                            }
                            else if(pMe->m_input_mode == IDS_SET_IP_CALL_OPTION)
                            {
                                number_table * temp = NULL;
                                temp = SettingMenu_Search_Node(&pMe->ip_number_tab,pMe->m_ip_sel_num);
                                if(temp)
                                {
                                    //ITEXTCTL_GetText(pTextCtl,  temp->number, OEM_IP_NUMBER_MAX);
                                    WSTRLCPY(temp->number,CharNum,OEM_IP_NUMBER_MAX+1);
                                    if(pMe->need_save_default)
                                    {
                                        MEMSET(pMe->ip_number_tab.default_ip, 0 ,sizeof(pMe->ip_number_tab.default_ip));
                                        WSTRLCPY(pMe->ip_number_tab.default_ip,temp->number,OEM_IP_NUMBER_MAX+1);
                                    }
                                }
                            }
                            else
#endif
                            {
                                char   Number[65];
                                char   temp[32];    //存储转移号码

                                MEMSET(Number, 0, sizeof(Number));
                                STRCPY(Number,pMe->m_callnumber);

                                WSTRTOSTR(ITEXTCTL_GetTextPtr(pTextCtl), temp, sizeof(temp));
                                STRCAT(Number, temp);
                                SettingMenu_MakeForwardCall(pMe,Number);
                            }
#ifndef FEATURE_CARRIER_THAILAND_HUTCH   
                            CLOSE_DIALOG(DLGRET_CANCELED)
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH  
                        }
                    }
                    return TRUE;

                default:
                    break;
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE
        case EVT_PEN_UP:
            {
                int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
                int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
                AEEDeviceInfo devinfo;
                int nBarH ;
                AEERect rc;
                MSG_FATAL("Setting_HandleAuto_Power_DialogEvent wXPos=%d ,wYPos=%d",wXPos,wYPos,0);
                 
                nBarH = GetBottomBarHeight(pMe->m_pDisplay);
                MEMSET(&devinfo, 0, sizeof(devinfo));
                ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
                SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);  
                if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
                {
                    if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                    {
                         boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_SETTINGMENU,EVT_USER,AVK_SELECT,0);
                         return rt;
                    } 
                    else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//右
                    {     
                        if ( WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0 &&
                             ITEXTCTL_GetCursorPos(pTextCtl) != TC_CURSORSTART )
                        {
                             return ITEXTCTL_HandleEvent(pTextCtl,EVT_KEY,AVK_CLR, 0);
                        }
                        else
                        {
                             boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_SETTINGMENU,EVT_USER,AVK_CLR,0);
                             return rt;
                        }
                    }
                }  
            
            }
            break;
                        
#endif

        case EVT_COMMAND:
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandleCallForwardInputDialogEvent


//*==============================================================================
//函数：
//       HandleWarningMessegeDialogEvent
//说明：
//       IDD_WARNING_MESSEGE对话框事件处理函数
//
//参数：
//       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
//       eCode [in]：事件代码。
//       wParam：事件相关数据。
//       dwParam：事件相关数据。
//
//返回值：
//       TRUE：传入事件被处理。
//       FALSE：传入事件被忽略。
//
//备注：
//
//==============================================================================*/
static boolean  HandleWarningMessegeDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    //PARAM_NOT_REF(dwParam)
    static IStatic * pStatic = NULL;
    MSG_FATAL("%x, %x ,%x,HandleWarningMessegeDialogEvent",eCode,wParam,dwParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            if (NULL == pStatic)
            {
                AEERect rect = {0};
                if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_STATIC,(void **)&pStatic))

                {
                    MSG_FATAL("ISHELL_CreateInstance,AEECLSID_STATIC 2",0,0,0);
                    return FALSE;
                }
                ISTATIC_SetRect(pStatic, &rect);
            }
            return TRUE;

        case EVT_DIALOG_START:
#if defined(FEATURE_VERSION_K212)||defined(FEATURE_VERSION_K212_HUALU)
            ISHELL_SetTimer(pMe->m_pShell,500,SettingMenu_DialogTimeout,pMe);
#else
			ISHELL_SetTimer(pMe->m_pShell,1000,SettingMenu_DialogTimeout,pMe);
#endif
            ISHELL_PostEvent( pMe->m_pShell,AEECLSID_APP_SETTINGMENU,EVT_USER_REDRAW,0,0);
            return TRUE;

        case EVT_USER_REDRAW:
        {
            PromptMsg_Param_type m_PromptMsg;
            MEMSET(&m_PromptMsg,0,sizeof(PromptMsg_Param_type));
            m_PromptMsg.nMsgResID= pMe->m_msg_id ;
            m_PromptMsg.ePMsgType = MESSAGE_INFORMATIVE;
            STRLCPY(m_PromptMsg.strMsgResFile, AEE_APPSSETTINGMENU_RES_FILE,MAX_FILE_NAME);
            m_PromptMsg.eBBarType = BTBAR_BACK;
            DrawPromptMessage(pMe->m_pDisplay,pStatic,&m_PromptMsg);
            IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
            return TRUE;
        }

        case EVT_DIALOG_END:
            ISTATIC_Release(pStatic);
            pStatic = NULL;
            ISHELL_CancelTimer(pMe->m_pShell, NULL,  pMe);
            return TRUE;

        case EVT_DISPLAYDIALOGTIMEOUT:
            CLOSE_DIALOG(DLGRET_MSGBOX_OK)
            return TRUE;

        case EVT_KEY:
            ISHELL_CancelTimer(pMe->m_pShell, NULL,  pMe);
            CLOSE_DIALOG(DLGRET_MSGBOX_OK)
            return TRUE;

        default:
            break;
    }

    return FALSE;
}//HandleWarningMessegeDialogEvent

#ifdef FEATURE_KEYGUARD
/*==============================================================================
函数：
       HandleAKGDialogEvent
说明：
       IDD_AKG_MENU 对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleAKGDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl  *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                     IDC_AKG);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("%x, %x ,%x,HandleAKGDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    //SettingMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_AUTOKEYGUARD_TITLE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
			#if defined (FEATURE_VERSION_W317A) || defined (FEATURE_VERSION_EC99)
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AKG_10SEC, IDS_AKG_10SEC, NULL, 0);
            #endif
		    #ifdef FEATURE_5SEC_AUTOLOCK_KEYPAD //xxzhen
			IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AKG_5SEC, IDS_AKG_5SEC, NULL, 0);
			#endif
            
            #ifdef FEATURE_VERSION_W021_CT100_QVGA            
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AKG_5SEC, IDS_AKG_5SEC, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AKG_10SEC, IDS_AKG_10SEC, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AKG_15SEC, IDS_AKG_15SEC, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AKG_30SEC, IDS_AKG_30SEC, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AKG_1MIN, IDS_AKG_1MIN, NULL, 0);               
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_OFF, IDS_OFF, NULL, 0);
            #else
           IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AKG_30SEC, IDS_AKG_30SEC, NULL, 0);
            #ifdef FEATURE_VERSION_W208S
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AKG_1MIN, IDS_AKG_1MIN, NULL, 0);
            #endif
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AKG_2MIN, IDS_AKG_2MIN, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AKG_5MIN, IDS_AKG_5MIN, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_OFF, IDS_OFF, NULL, 0);
            #endif

            
            return TRUE;

        case EVT_DIALOG_START:
            {
                uint16    ui16_return = IDS_OFF;
                byte   AKG = 0;

                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                      CFGI_KEY_LOCK_CHECK,
                                      &AKG,
                                      sizeof(AKG));
				//Add By zzg 2012_10_29
                #if defined (FEATURE_VERSION_W317A) || defined (FEATURE_VERSION_EC99) || defined(FEATURE_VERSION_K212_20D)||defined(FEATURE_VERSION_K212_ND)
					switch (AKG)
	                {
	                	case 1:   //10秒
	                      ui16_return = IDS_AKG_10SEC;
	                      break;
	                    case 2:   //30秒
	                      ui16_return = IDS_AKG_30SEC;
	                      break;
	                    case 3:    // 两分钟
	                      ui16_return = IDS_AKG_2MIN;
	                      break;
	                    case 4:    // 五分钟
	                      ui16_return = IDS_AKG_5MIN;
	                      break;
	                    case 5:
	                      ui16_return = IDS_AKG_1MIN;
	                      break;   //一分钟
	                   default:
	                    case 0: //关
	                      ui16_return = IDS_OFF;
	                      break;
	                }
                #elif defined(FEATURE_VERSION_W021_CT100_QVGA) 
					switch (AKG)
	                {
                        case 1:   //10秒
							ui16_return = IDS_AKG_5SEC;
							break;
						case 2:    // 30秒
							ui16_return = IDS_AKG_10SEC;
							break;
						case 3:    // 两分钟
							ui16_return = IDS_AKG_15SEC;
							break;
						case 4:    // 五分钟
							ui16_return = IDS_AKG_30SEC;
							break;
	                    case 5:    // 五分钟
							ui16_return = IDS_AKG_1MIN;
							break;                            
						default:
						case 0: //关
							ui16_return = IDS_OFF;
						break;      
					}              
				#elif defined (FEATURE_5SEC_AUTOLOCK_KEYPAD)//xxzhen
					switch (AKG)
	                {
	                	case 1:   //10秒
	                      ui16_return = IDS_AKG_5SEC;
	                      break;
	                    case 2:   //30秒
	                      ui16_return = IDS_AKG_30SEC;
	                      break;
	                    case 3:    // 两分钟
	                      ui16_return = IDS_AKG_2MIN;
	                      break;
	                    case 4:    // 五分钟
	                      ui16_return = IDS_AKG_5MIN;
	                      break;
	                    case 5:
	                      ui16_return = IDS_AKG_1MIN;
	                      break;   //一分钟
	                   default:
	                    case 0: //关
	                      ui16_return = IDS_OFF;
	                      break;
						}
				#else
					switch (AKG)
	                {
	                    case 1:   //30秒
	                      ui16_return = IDS_AKG_30SEC;
	                      break;
	                    case 2:    // 两分钟
	                      ui16_return = IDS_AKG_2MIN;
	                      break;
	                    case 3:    // 五分钟
	                      ui16_return = IDS_AKG_5MIN;
	                      break;
	                    case 4:
	                      ui16_return = IDS_AKG_1MIN;
	                      break;   //一分钟
	                   default:
	                    case 0: //关
	                      ui16_return = IDS_OFF;
	                      break;
	                }
				#endif
				//Add End
				
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, ui16_return, TRUE);
                IMENUCTL_SetSel(pMenu, ui16_return);

                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            }
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {
                byte AKG = 0;

               	//Add By zzg 2012_10_29
                #if defined (FEATURE_VERSION_W317A) || defined (FEATURE_VERSION_EC99) || defined(FEATURE_VERSION_K212_20D)||defined(FEATURE_VERSION_K212_ND)
					switch (wParam)
	                {
	                	case IDS_AKG_10SEC:     //10miao
	                        AKG = 1;
	                        break;
	                    case IDS_AKG_30SEC:     //30miao
	                        AKG = 2;
	                        break;
	                    case IDS_AKG_2MIN:     // 2minute
	                        AKG = 3;
	                        break;
	                    case IDS_AKG_5MIN:     //5minute
	                        AKG = 4;
	                        break;
	                    case IDS_AKG_1MIN:     //1minute
	                        AKG = 5;
	                        break;
	                    case IDS_OFF:     //关
	                        AKG = 0;
	                        break;

	                    default:
	                        ASSERT_NOT_REACHABLE;
	                }
                    #elif defined(FEATURE_VERSION_W021_CT100_QVGA) //xxzhen
					switch (wParam)
	                {
	                	case IDS_AKG_5SEC:     //10miao
	                        AKG = 1;
	                        break;
	                    case IDS_AKG_10SEC:     //30miao
	                        AKG = 2;
	                        break;
	                    case IDS_AKG_15SEC:     // 2minute
	                        AKG = 3;
	                        break;
	                    case IDS_AKG_30SEC:     //5minute
	                        AKG = 4;
	                        break;
                        case IDS_AKG_1MIN:     // 2minute
	                        AKG = 5;
	                        break;                            
	                    case IDS_OFF:     //关
	                        AKG = 0;
	                        break;

	                    default:
	                        ASSERT_NOT_REACHABLE;
	                }     
				#elif defined(FEATURE_5SEC_AUTOLOCK_KEYPAD) //xxzhen
					switch (wParam)
	                {
	                	case IDS_AKG_5SEC:     //10miao
	                        AKG = 1;
	                        break;
	                    case IDS_AKG_30SEC:     //30miao
	                        AKG = 2;
	                        break;
	                    case IDS_AKG_2MIN:     // 2minute
	                        AKG = 3;
	                        break;
	                    case IDS_AKG_5MIN:     //5minute
	                        AKG = 4;
	                        break;
						case IDS_AKG_1MIN:
	                        AKG = 5;
	                        break;
	                    case IDS_OFF:     //关
	                        AKG = 0;
	                        break;

	                    default:
	                        ASSERT_NOT_REACHABLE;
	                }
				#else
					switch (wParam)
	                {
	                    case IDS_AKG_30SEC:     //30miao
	                        AKG = 1;
	                        break;
	                    case IDS_AKG_2MIN:     // 2minute
	                        AKG = 2;
	                        break;
	                    case IDS_AKG_5MIN:     //5minute
	                        AKG = 3;
	                        break;
	                    case IDS_AKG_1MIN:     //1minute
	                        AKG = 4;
	                        break;
	                    case IDS_OFF:     //关
	                        AKG = 0;
	                        break;

	                    default:
	                        ASSERT_NOT_REACHABLE;
	                }
				#endif
				//Add End
				
                

                (void) ICONFIG_SetItem(pMe->m_pConfig,
                                       CFGI_KEY_LOCK_CHECK,
                                       &AKG,
                                       sizeof(AKG));
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wParam, TRUE);
                CLOSE_DIALOG(DLGRET_WARNING)
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandleAKGDialogEvent
#endif


#ifdef FEATURE_VERSION_KK5
static boolean  HandleNetworkSettingsDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl  *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                     IDC_NETWORK_SETTINGS);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("%x, %x ,%x,HandleAKGDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    //SettingMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        {         
			AECHAR WTitle[40] = {0};
            
			(void)ISHELL_LoadResString(pMe->m_pShell,
                    AEE_APPSSETTINGMENU_RES_FILE,                                
                    IDS_NETWORK_SETTINGS,
                    WTitle,
                    sizeof(WTitle));
			IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
       
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_NETWORK_PROFILE_MTS, IDS_NETWORK_PROFILE_MTS, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_NETWORK_PROFILE_RELIANCE, IDS_NETWORK_PROFILE_RELIANCE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_NETWORK_PROFILE_VIRGIN, IDS_NETWORK_PROFILE_VIRGIN, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_NETWORK_PROFILE_GARUDA, IDS_NETWORK_PROFILE_GARUDA, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_NETWORK_PROFILE_TATA, IDS_NETWORK_PROFILE_TATA, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_NETWORK_PROFILE_CUSTOM, IDS_NETWORK_PROFILE_CUSTOM, NULL, 0);            
            
            return TRUE;
        }

        case EVT_DIALOG_START:
            {
                byte value = 0;   
                uint16    ui16_return = IDS_NETWORK_PROFILE_MTS;               

                char strUser[64];
                char strPwd[64];

                OEM_GetAdsAccount_Param(strUser, strPwd);
				
				switch (value)
                {
                    case 0: 
                      ui16_return = IDS_NETWORK_PROFILE_MTS;
                      break;
                    case 1:    
                      ui16_return = IDS_NETWORK_PROFILE_RELIANCE;
                      break;
                    case 2:    
                      ui16_return = IDS_NETWORK_PROFILE_VIRGIN;
                      break;
                    case 3:
                      ui16_return = IDS_NETWORK_PROFILE_GARUDA;
                      break;  
                    case 4:
                      ui16_return = IDS_NETWORK_PROFILE_TATA;
                      break;  
                    default:  
                    case 5:
                      ui16_return = IDS_NETWORK_PROFILE_CUSTOM;
                      break;   
                      
                }
				
				
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, ui16_return, TRUE);
                IMENUCTL_SetSel(pMenu, ui16_return);

                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            }
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {
                byte AKG = 0;

                char strUser[64];
                char strPwd[64];

				switch (wParam)
                {
                    case IDS_NETWORK_PROFILE_MTS:    
                        //STRCPY();
                        //STRCPY();
                        break;
                    case IDS_NETWORK_PROFILE_RELIANCE:     
                        //STRCPY();
                        //STRCPY();
                        break;
                    case IDS_NETWORK_PROFILE_VIRGIN:     
                        //STRCPY();
                        //STRCPY();
                        break;
                    case IDS_NETWORK_PROFILE_GARUDA:     
                        //STRCPY();
                        //STRCPY();
                        break;
                    case IDS_NETWORK_PROFILE_TATA:     
                        //STRCPY(strUser, "");
                        //STRCPY(strPwd, "");
                        break;
                        
                    default:    
                    case IDS_NETWORK_PROFILE_CUSTOM:     //关
                        CLOSE_DIALOG(DLGRET_CANCELED)
                        break;
                        
                }

                OEM_SetAdsAccount_Param(strUser, strPwd);
                
				
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wParam, TRUE);
                CLOSE_DIALOG(DLGRET_WARNING)
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
}



static boolean  HandleNetworkParamsEditDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{    
	AEERect rt = {0};	
	uint16  time = 0;
   
	AECHAR      wstrUsernameTitle[20]; 
    AECHAR      wstrPasswordTitle[20];        
    AECHAR      wstrUsernameValue[64];  
    AECHAR      wstrPasswordValue[64]; 

    ITextCtl    *pUser = NULL;
    ITextCtl    *pPwd = NULL;
  
    if (NULL == pMe)
    {
        return FALSE;
    }
   
  	pUser= (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_NETWORK_PARAMS_USERNAME);  
    pPwd= (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_NETWORK_PARAMS_PASSWORD); 

	if ((pUser == NULL) || (pPwd == NULL))      
    {
        return FALSE;
    }  
    
    MSG_FATAL("***zzg HandleNetworkParamsEditDialogEvent eCode=%x, wParam=%x, dwParam=%d***", eCode, wParam, dwParam);

	switch (eCode)
    {
        case EVT_DIALOG_INIT:	            
		{
            AECHAR WTitle[2] = {0};
            char strUser[64];
            char strPwd[64];
                
            OEM_GetAdsAccount_Param(strUser, strPwd);
            
            SETAEERECT(&rt,0,20,128,40);
			ITEXTCTL_SetRect(pUser, &rt);
			ITEXTCTL_SetProperties(pUser, TP_FRAME | TP_MULTILINE | TP_STARKEY_SWITCH | TP_FIXSETRECT|TP_NOUPDATE);
            ITEXTCTL_SetMaxSize(pUser, 64); 
            (void)ITEXTCTL_SetTitle(pUser, NULL,0,WTitle);
            //设置文本
            STRTOWSTR(strUser, wstrUsernameValue, sizeof(wstrUsernameValue));
            (void)ITEXTCTL_SetText(pUser, wstrUsernameValue, -1);	            

            SETAEERECT(&rt,0,90,128,40);
			ITEXTCTL_SetRect(pPwd, &rt);
            ITEXTCTL_SetProperties(pPwd, TP_FRAME | TP_MULTILINE | TP_STARKEY_SWITCH | TP_FIXSETRECT|TP_NOUPDATE); 
            ITEXTCTL_SetMaxSize(pPwd, 64);     
            (void)ITEXTCTL_SetTitle(pPwd, NULL,0,WTitle);
            
            STRTOWSTR(strPwd, wstrPasswordValue, sizeof(wstrPasswordValue));
            (void)ITEXTCTL_SetText(pPwd,wstrPasswordValue, -1);			

            ITEXTCTL_SetActive(pUser, TRUE);     
            return TRUE;
        }
        
        case EVT_DIALOG_START:
        {
            ISHELL_PostEvent( pMe->m_pShell,AEECLSID_APP_SETTINGMENU,EVT_USER_REDRAW,0,0);

            return TRUE;
        }    
        
        case EVT_USER_REDRAW:        
        {
            BottomBar_Param_type  BBarParam ={0};                 
            BBarParam.eBBarType = BTBAR_OK_DELETE;				 
                            
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                         AEE_APPSSETTINGMENU_RES_FILE,
                                         IDS_NETWORK_PARAMS_USERNAME,
                                         wstrUsernameTitle,
                                         sizeof(wstrUsernameTitle));

            (void) ISHELL_LoadResString(pMe->m_pShell,
                                         AEE_APPSSETTINGMENU_RES_FILE,
                                         IDS_NETWORK_PARAMS_PASSWORD,
                                         wstrPasswordTitle,
                                         sizeof(wstrPasswordTitle));

            
             SETAEERECT(&pMe->m_rc,0,0,128,160);                 
			 Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);    
			 
			 IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);		             
			 IDISPLAY_DrawText(pMe->m_pDisplay, AEE_FONT_NORMAL, wstrUsernameTitle, -1, 0, 0, 0, IDF_TEXT_TRANSPARENT);         
             IDISPLAY_DrawText(pMe->m_pDisplay, AEE_FONT_NORMAL, wstrPasswordTitle, -1, 0, 70, 0, IDF_TEXT_TRANSPARENT);
			                  
			 IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);	             
			               	
             ITEXTCTL_SetActive(pUser,TRUE);             
             ITEXTCTL_SetActive(pPwd,FALSE); 

             DrawBottomBar(pMe->m_pDisplay, &BBarParam);    
             
        	 // 更新显示
        	 IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);  
             return TRUE;
        }            
            
        case EVT_DIALOG_END:
        {
            CLOSE_DIALOG(DLGRET_CANCELED)  
            return TRUE;
        }

        case EVT_KEY:
        {
			switch (wParam)
            {
            	case AVK_INFO:
				case AVK_SELECT:
				{      
                    char           str1[MAS_BREWSETINT_STRING]; 
                    char           str2[MAS_BREWSETINT_STRING]; 
                    
                    AECHAR *pwstrUsername  = ITEXTCTL_GetTextPtr(pUser);  
                    AECHAR *pwstrPassword = ITEXTCTL_GetTextPtr(pPwd);   
                    
					WSTRTOSTR(pwstrUsername,str1,sizeof(str1));	                    
                    WSTRTOSTR(pwstrPassword,str2,sizeof(str2));	

                    OEM_SetAdsAccount_Param(str1, str2);
                    
                    CLOSE_DIALOG(DLGRET_CANCELED)
					break;
				}
					
            	case AVK_CLR:
				{
                    CLOSE_DIALOG(DLGRET_CANCELED)
                }
                
				default:					
				{
                    break;
                }
					
			}
            return TRUE;
        }
            
        default:
        {
            break;
        }
    }

    return FALSE;
}

#endif


#ifdef FEATURE_SET_BANNER
/*==============================================================================
函数：
       HandleBannerDialogEvent
说明：
       IDD_BANNER_TXT 对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleBannerDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    ITextCtl *pTextCtl= (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                      IDC_BANNER);
    if (pTextCtl == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("%x, %x ,%x,HandleBannerDialogEvent",eCode,wParam,dwParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            {
                AECHAR szBuf[OEMNV_BANNER_MAXLEN];
                AEERect rc;
                MEMSET(szBuf, '\0', sizeof(szBuf));

                SETAEERECT(&rc, 0, 0, pMe->m_rc.dx, 32);//pMe->m_rc.dy - 16);
                ITEXTCTL_SetRect(pTextCtl,  &rc);
                //ICONTROL_SetRect((IControl*)pTextCtl, &pMe->m_rc);
                // 长按*键切换输入法
                //ITEXTCTL_SetProperties( pTextCtl, TP_FRAME );
                //ITEXTCTL_SetProperties( pTextCtl, TP_FRAME | TP_STARKEY_SWITCH);
#ifdef Temp_remove
                // 长按*键切换输入法
                ITEXTCTL_SetProperties( pTextCtl, TP_FRAME | TP_STARKEY_SWITCH|TP_FOCUS_NOSEL);
#endif/*Temp_remove*/
                //设置最多字符数
                ITEXTCTL_SetMaxSize(pTextCtl, BANNER_MAXTXT);
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_BANNER/*CFGI_PHONE_BANNER*/,
                                       szBuf,
                                       sizeof(szBuf)) ;
                (void) ITEXTCTL_SetText(pTextCtl, szBuf, -1);

                ITEXTCTL_SetActive(pTextCtl, TRUE);
                ITEXTCTL_Redraw(pTextCtl);
                {
                    AECHAR WTitle[40] = {0};
                    (void)ISHELL_LoadResString(pMe->m_pShell,
                    AEE_APPSSETTINGMENU_RES_FILE,                                
                    IDS_BANNER_TITLE,
                    WTitle,
                    sizeof(WTitle));
                    if(pMe->m_pAnn != NULL)
                    {
                        IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
                    }
                }    

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_APP_SETTINGMENU,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            }
            return TRUE;

        case EVT_KEY_RELEASE:
        case EVT_USER_REDRAW:
            {
                AEETextInputMode nInputMode;
                nInputMode = ITEXTCTL_GetInputMode(pTextCtl,NULL);
                if (nInputMode == AEE_TM_SYMBOLS)
                {
                    return TRUE;
                }

                // 绘制底部操作提示条
                if ( WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0 &&
                     ITEXTCTL_GetCursorPos(pTextCtl) != TC_CURSORSTART )
                {
                    //drawBottomBar(pMe->m_pDisplay, AEE_FONT_NORMAL,BTBAR_OK_DELETE);
                    #ifndef FEATURE_ALL_KEY_PAD
                    SETTING_MENU_DRAW_BOTTOMBAR(BTBAR_OK_DELETE)
                    #else
                    SETTING_MENU_DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                    #endif
                }
                else
                {
                    //drawBottomBar(pMe->m_pDisplay, AEE_FONT_NORMAL,BTBAR_OK_BACK);
                    SETTING_MENU_DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                }

                // 统一更新界面
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);

            }
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                case AVK_SELECT:
                    {
                        AECHAR banner[OEMNV_BANNER_MAXLEN];

                        (void) ITEXTCTL_GetText(pTextCtl,
                                        banner,
                                        OEMNV_BANNER_MAXLEN);
                        (void) ICONFIG_SetItem(pMe->m_pConfig,
                                               CFGI_BANNER/*CFGI_PHONE_BANNER*/,
                                               banner,
                                               sizeof(banner));
                        CLOSE_DIALOG(DLGRET_CANCELED)
                    }
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandleBannerDialogEvent

#endif
/*==============================================================================
函数：
       HandleTimeDialogEvent
说明：
       IDD_PHONESETTING_MENU 对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
#ifdef FEATURE_TIME_DATA_SETTING
static boolean  HandleTimeDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)
    static byte bytData = 0;
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_TIMESET);
    MSG_FATAL("%x, %x ,%x,HandleTimeDialogEvent",eCode,wParam,dwParam);
    if (pMenu == NULL)
    {
        return FALSE;
    }
     //实现菜单循环滚动功能
    //SettingMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_TIMESETTING,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AMPM, IDS_AMPM, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_DAY, IDS_DAY, NULL, 0);
            return TRUE;

        case EVT_DIALOG_START:
            {
                uint16 wItemID;

                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_TIME_FORMAT,
                                       &bytData,
                                       sizeof(bytData));

                switch (bytData)
                {
                    case  OEMNV_TIMEFORM_AMPM:
                        wItemID = IDS_AMPM;
                        break;

                    case OEMNV_TIMEFORM_24HR :
                        wItemID = IDS_DAY;
                        break;

                    default:
                        wItemID = IDS_AMPM;
                        break;
                }

                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wItemID, TRUE);
                IMENUCTL_SetSel(pMenu, wItemID);
            }
            return TRUE;
        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                  default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {
                byte bytNewData = 0;

                switch (wParam)
                {
                    case  IDS_AMPM:
                       bytNewData = OEMNV_TIMEFORM_AMPM;
                       break;

                    case IDS_DAY:
                       bytNewData = OEMNV_TIMEFORM_24HR;
                       break;

                    default:
                       ASSERT_NOT_REACHABLE;

                }

                if (bytNewData != bytData)
                {
                    (void)ICONFIG_SetItem(pMe->m_pConfig,
                                          CFGI_TIME_FORMAT,
                                          &bytNewData, sizeof(bytNewData));
                    //将选中的选项标出
                    bytData = bytNewData;
                    InitMenuIcons(pMenu);
                    SetMenuIcon(pMenu, wParam, TRUE);
                    (void)IMENUCTL_Redraw(pMenu);
                }
                CLOSE_DIALOG(DLGRET_WARNING)
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandleTimeDialogEvent


/*==============================================================================
函数：
       HandledateDialogEvent
说明：
       IDD_PHONESETTING_MENU 对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/

static boolean  HandleDateDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)
    static byte bytData = 0;
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_DATESET);
    MSG_FATAL("%x, %x ,%x,HandleDateDialogEvent",eCode,wParam,dwParam);
    if (pMenu == NULL)
    {
        return FALSE;
    }
     //实现菜单循环滚动功能
    //SettingMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_DATESETTING,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_DMY, IDS_DMY, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_MDY, IDS_MDY, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_YMD, IDS_YMD, NULL, 0);
            //IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_DMY, IDS_DMY, NULL, 0);
            return TRUE;

        case EVT_DIALOG_START:
            {
                uint16 wItemID;

                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_DATE_FORMAT,
                                       &bytData,
                                       sizeof(bytData));

                switch (bytData)
                {
                    case  OEMNV_DATEFORM_DMY:
                        wItemID = IDS_DMY;
                        break;

                    case OEMNV_DATEFORM_MDY :
                        wItemID = IDS_MDY;
                        break;

                    default:
                    case OEMNV_DATEFORM_YMD :
                        wItemID = IDS_YMD;
                        break;
                }

                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wItemID, TRUE);
                IMENUCTL_SetSel(pMenu, wItemID);
            }
            return TRUE;
        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                  default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {
                byte bytNewData = 0;

                switch (wParam)
                {
                    case  IDS_DMY:
                       bytNewData = OEMNV_DATEFORM_DMY ;
                       break;

                    case IDS_MDY:
                       bytNewData = OEMNV_DATEFORM_MDY ;
                       break;

                    case IDS_YMD:
                       bytNewData = OEMNV_DATEFORM_YMD ;
                       break;

                    default:
                       ASSERT_NOT_REACHABLE;

                }

                if (bytNewData != bytData)
                {
                    (void)ICONFIG_SetItem(pMe->m_pConfig,
                                          CFGI_DATE_FORMAT,
                                          &bytNewData, sizeof(bytNewData));
                    //将选中的选项标出
                    bytData = bytNewData;
                    InitMenuIcons(pMenu);
                    SetMenuIcon(pMenu, wParam, TRUE);
                    (void)IMENUCTL_Redraw(pMenu);
                }
                CLOSE_DIALOG(DLGRET_WARNING)
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandleDATEDialogEvent

#endif

#if defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_W317A) || defined(FEATURE_VERSION_IC241A_MMX)|| defined (FEATURE_VERSION_KK5)


static boolean   HandleTrackSmsDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
	PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_TRACKSMSTIME);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    //实现菜单循环滚动功能
    //DisplayMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
        {
			uint16 m_alarm_time = OEMNV_TRACK_SMS_5;
			uint16 wItemID;
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_SMS_TREACKER_TIME,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }

            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SMS_TREACKER_TIME_5, IDS_SMS_TREACKER_TIME_5, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SMS_TREACKER_TIME_10, IDS_SMS_TREACKER_TIME_10, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SMS_TREACKER_TIME_15, IDS_SMS_TREACKER_TIME_15, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SMS_TREACKER_TIME_30, IDS_SMS_TREACKER_TIME_30, NULL, 0);
			IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SMS_TREACKER_TIME_60, IDS_SMS_TREACKER_TIME_60, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SMS_TREACKER_TIME_240, IDS_SMS_TREACKER_TIME_240, NULL, 0);

  		    IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
  #ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
  #endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            (void)OEM_GetConfig(CFGI_SMS_TRACKER_TIME,
                           &m_alarm_time, 
                           sizeof(uint16));
			MSG_FATAL("m_alarm_time======%d",m_alarm_time,0,0);
            switch (m_alarm_time)
            {
                case OEMNV_TRACK_SMS_5:      //30s
                    wItemID = IDS_SMS_TREACKER_TIME_5;
                    break;

                case OEMNV_TRACK_SMS_10:      //50秒
                    wItemID = IDS_SMS_TREACKER_TIME_10;
                    break;

                case OEMNV_TRACK_SMS_15:      //ALWAYS_ON
                    wItemID = IDS_SMS_TREACKER_TIME_15;
                    break;
				case OEMNV_TRACK_SMS_30:      //30s
                    wItemID = IDS_SMS_TREACKER_TIME_30;
                    break;

                case OEMNV_TRACK_SMS_60:      //50秒
                    wItemID = IDS_SMS_TREACKER_TIME_60;
                    break;

                case OEMNV_TRACK_SMS_240:      //ALWAYS_ON
                    wItemID = IDS_SMS_TREACKER_TIME_240;
                    break;
            }


            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, wItemID, TRUE);
            IMENUCTL_SetSel(pMenu, wItemID);
            return TRUE;
		}
        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
        {
            uint16      m_alarm_time = OEMNV_TRACK_SMS_5;

            switch (wParam)
            {

                case IDS_SMS_TREACKER_TIME_5:          //关
                    m_alarm_time = OEMNV_TRACK_SMS_5;
                    break;

                case IDS_SMS_TREACKER_TIME_10:          //7秒
                    m_alarm_time = OEMNV_TRACK_SMS_10;
                    break;
				
                case IDS_SMS_TREACKER_TIME_15:          //10秒
                    m_alarm_time = OEMNV_TRACK_SMS_15;
                    break;

                case IDS_SMS_TREACKER_TIME_30:          //30秒
                    m_alarm_time = OEMNV_TRACK_SMS_30;
                    break;

                case IDS_SMS_TREACKER_TIME_60:          //50秒
                    m_alarm_time = OEMNV_TRACK_SMS_60;
                    break;

                case IDS_SMS_TREACKER_TIME_240:          //50秒
                    m_alarm_time = OEMNV_TRACK_SMS_240;
                    break;

                default:
                    ASSERT_NOT_REACHABLE;
            }

			MSG_FATAL("m_alarm_time======%d",m_alarm_time,0,0);

            (void)OEM_SetConfig(   CFGI_SMS_TRACKER_TIME,
                                   &m_alarm_time,
                                   sizeof(uint16));

            //IBacklight_Enable(pMe->m_pIBacklight);      //NV立即生效
            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, wParam, TRUE);
			(void)IMENUCTL_Redraw(pMenu);
            CLOSE_DIALOG(DLGRET_WARNING)
        }
        return TRUE;

        default:
            break;
    }
    return FALSE;
}

#endif


/*==============================================================================
函数：
       HandleSearchModeDialogEvent
说明：
       IDC_MENU_SEARCHTIME 对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/

#ifdef FEATURE_VERSION_MYANMAR
static boolean HandleSearchModeDialogEvent(CSettingMenu *pMe,
    									AEEEvent eCode,
    									uint16 wParam,
    									uint32 dwParam
    									)
{
    PARAM_NOT_REF(dwParam)
    //static byte bytData = 0;
    //static boolean isSwitch = FALSE;
    int ret = 0;

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_MENU_SEARCHTIME);
    MSG_FATAL("%x, %x ,%x,HandleDateDialogEvent",eCode,wParam,dwParam);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_SEARCHNET_MODE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AUTO_SEARCH, IDS_AUTO_SEARCH, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SEARCH_FIVEMINTU, IDS_SEARCH_FIVEMINTU, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SEARCH_TENMIINUTES, IDS_SEARCH_TENMIINUTES, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SEARCH_TENTY, IDS_SEARCH_TENTY, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SEARCH_THIRTYMINUTE, IDS_SEARCH_THIRTYMINUTE, NULL, 0);
            return TRUE;

        case EVT_DIALOG_START:
            {
                uint16 wItemID;
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                ret =OEMNV_Get(NV_SD_CFG_ITEMS_I, (nv_item_type*) &pMe->nvsearchmode);
                if( NV_NOTACTIVE_S != ret)
                {
                	MSG_FATAL("1pMe->nvsearchmode.sd_cfg_items.items[10]=%d",pMe->nvsearchmode.sd_cfg_items.items[10],0,0);
                    switch (pMe->nvsearchmode.sd_cfg_items.items[10])
                    {
                         case SEARCHMODE_AOTE:
                            wItemID = IDS_AUTO_SEARCH;
                            break;

                         case SEARCHMODE_FIVE:
                            wItemID = IDS_SEARCH_FIVEMINTU;
                            break;
                         case SEARCHMODE_TEN:
                            wItemID = IDS_SEARCH_TENMIINUTES;
                            break;
                         case SEARCHMODE_TUENTY:
                            wItemID = IDS_SEARCH_TENTY;
                            break;
                         case SEARCHMODE_THIRTY:
                            wItemID = IDS_SEARCH_THIRTYMINUTE;
                            break;
                         default:
                         	wItemID = IDS_AUTO_SEARCH;
                         	break;
                    }
                }
                else
                {
                	pMe->nvsearchmode.sd_cfg_items.items[10] = 0; 
                    wItemID = IDS_AUTO_SEARCH;
                }
                
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wItemID, TRUE);
                IMENUCTL_SetSel(pMenu, wItemID);
            }
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                  default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {
            	int m_items = pMe->nvsearchmode.sd_cfg_items.items[10];
                switch (wParam)
                {
                    case  IDS_AUTO_SEARCH:
                       pMe->nvsearchmode.sd_cfg_items.items[10] = SEARCHMODE_AOTE ;
                       break;
                    case IDS_SEARCH_FIVEMINTU:
                       pMe->nvsearchmode.sd_cfg_items.items[10] = SEARCHMODE_FIVE;
                       break;
                    case  IDS_SEARCH_TENMIINUTES:
                       pMe->nvsearchmode.sd_cfg_items.items[10] = SEARCHMODE_TEN;
                       break;
                    case IDS_SEARCH_TENTY:
                       pMe->nvsearchmode.sd_cfg_items.items[10] = SEARCHMODE_TUENTY;
                       break;
                    case  IDS_SEARCH_THIRTYMINUTE:
                       pMe->nvsearchmode.sd_cfg_items.items[10] = SEARCHMODE_THIRTY;
                       break;
                    default:
                       ASSERT_NOT_REACHABLE;

                }
                if ((pMe->nvsearchmode.sd_cfg_items.items[10] != m_items))
                {
                    MSG_FATAL("2pMe->nvsearchmode.sd_cfg_items.items[10]=%d",pMe->nvsearchmode.sd_cfg_items.items[10],0,0);		
                    //将选中的选项标出
                    OEMNV_Put(NV_SD_CFG_ITEMS_I, (nv_item_type*) &pMe->nvsearchmode);
                    InitMenuIcons(pMenu);
                    SetMenuIcon(pMenu, wParam, TRUE);
                    (void)IMENUCTL_Redraw(pMenu);
                     CLOSE_DIALOG(DLGRET_WARNING)
					 break;
                }
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
}



static boolean HandleTimeFontModeDialogEvent(CSettingMenu *pMe,
    											AEEEvent eCode,
    											uint16 wParam,
    											uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
    //static byte bytData = 0;
    //static boolean isSwitch = FALSE;
    int ret = 0;

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_MENU_TIMEFONTMODE);
    MSG_FATAL("%x, %x ,%x,HandleTimeFontModeDialogEvent",eCode,wParam,dwParam);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_IDLE_DATETIME_MOCD,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_FONT_BIG, IDS_FONT_BIG, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_FONT_MIEDIUM, IDS_FONT_MIEDIUM, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_FONT_SMALL, IDS_FONT_SMALL, NULL, 0);
            return TRUE;

        case EVT_DIALOG_START:
            {
                uint16 wItemID;
                byte nSelItem = 0;
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                
                
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_IDLE_DATETIME_MODE,
                                       &nSelItem,
                                       sizeof(nSelItem));
                switch (nSelItem)
                {
                     case TIMEFONTMODE_BIG:
                        wItemID = IDS_FONT_BIG;
                        break;
                     case TIMEFONTMODE_MEDIUM:
                        wItemID = IDS_FONT_MIEDIUM;
                        break;
                     case TIMEFONTMODE_SMALL:
                        wItemID = IDS_FONT_SMALL;
                        break;
                     default:
                     	wItemID = IDS_FONT_BIG;
                     	break;
                }
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wItemID, TRUE);
                IMENUCTL_SetSel(pMenu, wItemID);
            }
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                  default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {
            	byte nSelItem = 0;
                switch (wParam)
                {
                    case  IDS_FONT_BIG:
                       nSelItem = TIMEFONTMODE_BIG ;
                       break;
                    case IDS_FONT_MIEDIUM:
                       nSelItem = TIMEFONTMODE_MEDIUM;
                       break;
                    case  IDS_FONT_SMALL:
                       nSelItem = TIMEFONTMODE_SMALL;
                       break;
                    default:
                       ASSERT_NOT_REACHABLE;

                }
                (void) ICONFIG_SetItem(pMe->m_pConfig,
                                       CFGI_IDLE_DATETIME_MODE,
                                       &nSelItem,
                                       sizeof(nSelItem));
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wParam, TRUE);
                (void)IMENUCTL_Redraw(pMenu);
                CLOSE_DIALOG(DLGRET_WARNING)
				break;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
}


#endif

/*==============================================================================
函数：
       HandleSimDialogEvent
说明：
       IDD_PHONESETTING_MENU 对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
#ifdef  FEATURE_DOUBLE_SIM_CARD

static boolean HandleSimDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
    )
{
    PARAM_NOT_REF(dwParam)
    //static byte bytData = 0;
    //static boolean isSwitch = FALSE;
    int ret = 0;

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_SIMSET);
    MSG_FATAL("%x, %x ,%x,HandleDateDialogEvent",eCode,wParam,dwParam);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_SIM_SET,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SIM_ONE, IDS_SIM_ONE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SIM_TWO, IDS_SIM_TWO, NULL, 0);
            //IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SIM_SWITCH, IDS_SIM_SWITCH, NULL, 0);
            return TRUE;

        case EVT_DIALOG_START:
            {
                uint16 wItemID;

                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
               
                ret =OEMNV_Get(NV_SIM_SELECT_I,&pMe->nviOldSimChoice);
                if( NV_NOTACTIVE_S != ret)
                {
                    switch (pMe->nviOldSimChoice.sim_select)
                    {
                        case OEMNV_SIMFORM_TWO:
                            wItemID = IDS_SIM_TWO;
                            break;
                            
                         default:
                         case OEMNV_SIMFORM_ONE:
                            wItemID = IDS_SIM_ONE;
                            break;
                    }
                }
                else
                {
                    pMe->nviOldSimChoice.sim_select = OEMNV_SIMFORM_ONE;
                    wItemID = IDS_SIM_ONE;
                }
                
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wItemID, TRUE);
                IMENUCTL_SetSel(pMenu, wItemID);
            }
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                  default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {
                switch (wParam)
                {
                    case  IDS_SIM_ONE:
                       pMe->nviNewSimChoice.sim_select = OEMNV_SIMFORM_ONE ;
                       break;

                    case IDS_SIM_TWO:
                       pMe->nviNewSimChoice.sim_select = OEMNV_SIMFORM_TWO ;
                       break;
                    default:
                       ASSERT_NOT_REACHABLE;

                }
                if ((pMe->nviNewSimChoice.sim_select != pMe->nviOldSimChoice.sim_select))
                {
                    MSG_FATAL("HandleSimDialogEvent:::::22222 sim_select=%d",pMe->nviNewSimChoice.sim_select,0,0);				
                    //将选中的选项标出
                    InitMenuIcons(pMenu);
                    SetMenuIcon(pMenu, wParam, TRUE);
                    (void)IMENUCTL_Redraw(pMenu);
                    CLOSE_DIALOG(DLGRET_SIMSETTING_CHOICE)
					 break;
                }
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
    
}

#ifdef FEATURE_ICM
void HandleSimChoiceTimer(ICM *m_pICM)
{
	ICM_SetOperatingMode(m_pICM, AEECM_OPRT_MODE_RESET);
    ICM_Release(m_pICM);
    m_pICM = NULL;
}
#else
void HandleSimChoiceTimer(IPhoneCtl *m_pIPhoneCtl)
{
	IPHONECTL_SetOperatingMode(m_pIPhoneCtl, AEET_OPRT_MODE_RESET);
    IPHONECTL_Release(m_pIPhoneCtl);
    m_pIPhoneCtl = NULL;
}
#endif

static boolean HandleSimChoiceEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
    )
{
    PARAM_NOT_REF(dwParam)
    nv_item_type nvi;
    int ret = 0;

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        {      
            return TRUE;
        }

        case EVT_DIALOG_START:
        {
            ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_SETTINGMENU,EVT_USER_REDRAW,0,0);
            return TRUE;
        }
        case EVT_USER_REDRAW:
        {
            BottomBar_Param_type BarParam;                  
            MEMSET(&BarParam, 0, sizeof(BarParam));         
            BarParam.eBBarType = BTBAR_OK_CANCEL;                         
            DrawBottomBar(pMe->m_pDisplay, &BarParam);                  
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
        }
        case EVT_DIALOG_END:
            return TRUE;
        case EVT_KEY:
		case EVT_KEY_PRESS:
            switch(wParam)
            {
                case AVK_CLR:
                {
                    MSG_FATAL("HandleSimChoiceEvent AVK_CLR1",0,0,0);
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    MSG_FATAL("HandleSimChoiceEvent AVK_CLR2",0,0,0);
                    return TRUE;
                }
                case AVK_SEND:
                case AVK_INFO:
				case AVK_SELECT:
                {
                    nv_item_type nviNew;
#ifdef FEATURE_ICM
                    ICM *pICM = NULL;
#else
                    IPhoneCtl *pIPhoneCtl = NULL;
#endif
                    int nReturnStatus = -1;    
                    MSG_FATAL("HandleSimChoiceEvent AVK_SEND",0,0,0);

                    MSG_FATAL("HandleSimChoiceEvent:::::22222 sim_select=%d",pMe->nviNewSimChoice.sim_select,0,0);
                    (void)OEMNV_Put(NV_SIM_SELECT_I,&pMe->nviNewSimChoice);
                    pMe->nviOldSimChoice.sim_select = pMe->nviNewSimChoice.sim_select;
#ifdef FEATURE_ICM
                    nReturnStatus = ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CM, (void **) &pICM);   
                    if((pICM == NULL) || (SUCCESS != nReturnStatus))
#else
                    nReturnStatus = ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_PHONECTL, (void **) &pIPhoneCtl);   
                    if((pIPhoneCtl == NULL) || (SUCCESS != nReturnStatus))
#endif
                    {
                        MSG_FATAL("HandleSimDialogEvent:::::33333 ICM cerate Failed",0,0,0);
                        return FALSE;
                    }
#ifdef FEATURE_ICM
                    ICM_SetOperatingMode(pICM, AEECM_OPRT_MODE_OFFLINE);
					(void)ISHELL_SetTimer(pMe->m_pShell,
                                500,
                                (PFNNOTIFY)HandleSimChoiceTimer,
                                pICM);
#else
                    IPHONECTL_SetOperatingMode(pIPhoneCtl, AEET_OPRT_MODE_OFFLINE);
					(void)ISHELL_SetTimer(pMe->m_pShell,
                                500,
                                (PFNNOTIFY)HandleSimChoiceTimer,
                                pIPhoneCtl);
#endif
                    MSG_FATAL("HandleSimChoiceEvent AVK_SEND2",0,0,0);
                    return TRUE;                  
                }

                  default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {
                break;
            }
            return TRUE;

        default:
            break;
    }
    MSG_FATAL("HandleSimChoiceEvent END",0,0,0);
    return FALSE;
    
}

#endif
#if defined(FEATURE_VERSION_K212_ND)
static boolean HandleSosDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
	PARAM_NOT_REF(dwParam)
	int            nLen = 0;	
	char  wstrNum[MAX_EMERGENCY_NUM_LEN+1];
	AECHAR  W_wstrNum[MAX_EMERGENCY_NUM_LEN+1] ;
	switch (eCode)
    {
        case EVT_DIALOG_INIT:   
			 return TRUE;
        case EVT_DIALOG_START:
   			(void) ISHELL_PostEvent(pMe->m_pShell,
                                    AEECLSID_APP_SETTINGMENU,
                                    EVT_USER_REDRAW,
                                    NULL,
                                    NULL);
            return TRUE;

		case EVT_USER_REDRAW:
            // 绘制相关信息
            {
            	AECHAR      text[32] = {0};
            	RGBVAL nOldFontColor;
                TitleBar_Param_type  TitleBar_Param = {0};
                
                // 先清屏
#ifdef FEATURE_CARRIER_CHINA_VERTU
                {
                    IImage *pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SECURITY_BACKGROUND);
                    
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
                (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                AEE_APPSSETTINGMENU_RES_FILE,
                                                IDS_SET_SOS, 
                                                text,
                                                sizeof(text));
                // 画标题条
                TitleBar_Param.pwszTitle = text;
                TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
                #if 0
                DrawTitleBar(pMe->m_pDisplay, &TitleBar_Param);
				#else
				IANNUNCIATOR_SetFieldText(pMe->m_pAnn,text);
				#endif
               (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                AEE_APPSSETTINGMENU_RES_FILE,
                                                IDS_SOS, 
                                                text,
                                                sizeof(text));
                nOldFontColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                    AEE_FONT_BOLD, 
                                    text,
                                    -1, 
                                    5, 
                                    MENUITEM_HEIGHT*1/2, 
                                    NULL, 
                                    IDF_TEXT_TRANSPARENT);
                   
                nLen = (wstrNum == NULL)?(0):(STRLEN(wstrNum)); 
                
                (void) STRTOWSTR(wstrNum, W_wstrNum, sizeof(W_wstrNum));
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                AEE_FONT_BOLD, 
                                W_wstrNum,
                                -1, 
                                2*5, 
                                MENUITEM_HEIGHT*3/2,
                                NULL, 
                                IDF_TEXT_TRANSPARENT);
                (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, nOldFontColor);
            	#ifndef FEATURE_ALL_KEY_PAD
                // 绘制底条提示
                if(nLen > 0)
                {// 删除
                    
					BottomBar_Param_type BarParam;                  
				    MEMSET(&BarParam, 0, sizeof(BarParam));         
				    BarParam.eBBarType = BTBAR_OK_DELETE;                         
				    DrawBottomBar(pMe->m_pDisplay, &BarParam);  
                }
                else
                #else
                 // 绘制底条提示
                if(nLen > 0)
                {// 删除
                	BottomBar_Param_type BarParam;                  
				    MEMSET(&BarParam, 0, sizeof(BarParam));         
				    BarParam.eBBarType = BTBAR_OK_BACK;                         
				    DrawBottomBar(pMe->m_pDisplay, &BarParam); 
                }
                else
                #endif
                {// 取消
                	BottomBar_Param_type BarParam;                  
				    MEMSET(&BarParam, 0, sizeof(BarParam));         
				    BarParam.eBBarType = BTBAR_CANCEL;                         
				    DrawBottomBar(pMe->m_pDisplay, &BarParam); 
                }
                // 更新显示
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE); 
        
                return TRUE;
            }
			

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
                        
                    case AVK_CLR:
                        chEnter = 0;
                        #ifndef FEATURE_ALL_KEY_PAD
                        if (wstrNum == NULL || STRLEN(wstrNum) == 0)
                        {
                            CLOSE_DIALOG(DLGRET_CANCELED)
                            return TRUE;
                        }
                        #else
                        if(dwParam == 0)
                        {
                            CLOSE_DIALOG(DLGRET_CANCELED)
                            return TRUE;
                        }
                        else
                        {
                        	if (wstrNum == NULL || STRLEN(wstrNum) == 0)
                        	{
                            	CLOSE_DIALOG(DLGRET_CANCELED)
                            	return TRUE;
                        	}
                        }
                        #endif
                        break;

                    case AVK_DEL:    
                        chEnter = 0;
#ifdef FEATURE_ALL_KEY_PAD 
                        if (wstrNum == NULL || STRLEN(wstrNum) == 0)
                        {
                            CLOSE_DIALOG(DLGRET_CANCELED)
                            return TRUE;
                        }
#endif
                    break;

                    case AVK_SELECT:
                    case AVK_INFO:
					{
                        EmergencyNum_Table m_entable = {0};
                        if (wstrNum == NULL || STRLEN(wstrNum) < 1)
                        {
                            return TRUE;
                        }
                        else
                        {
                            uint16 wPWD=0;
				       	   	m_entable.emerg_num[0].num_len = nLen;
				       	   	STRCPY(m_entable.emerg_num[0].num_buf,wstrNum);
				       	   	(void)OEM_SetConfig(CFGI_EMERGENCYNUM_TABLE,
					                          (void*)&m_entable,
					                          sizeof(EmergencyNum_Table));
                        }
                    }
             
                    return TRUE;
                        
                default:
                    return TRUE;
                }
                nLen = (wstrNum == NULL)?(0):(STRLEN(wstrNum));
                if (chEnter == 0)
                {
                    // 删除字符
                    if (nLen > 0)
                    {
                        bRedraw = TRUE;
                        wstrNum[nLen-1] = chEnter;
                    }
                }
                else if (nLen < MAX_EMERGENCY_NUM_LEN)
                {
                    wstrNum[nLen] = chEnter;
                    nLen++;
                    wstrNum[nLen] = 0;
                    bRedraw = TRUE;
                }
                
                if (bRedraw)
                {
                    (void) ISHELL_PostEvent(pMe->m_pShell,
                                            AEECLSID_APP_SETTINGMENU,
                                            EVT_USER_REDRAW,
                                            NULL,
                                            NULL);
                }
            }
            return TRUE;

        case EVT_COMMAND:
        default:
            break;
    }
    return FALSE;	
}
#endif

#ifdef FEATURE_SOUND_BO
static boolean  HandleSpeechDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
	 PARAM_NOT_REF(dwParam)

    static boolean bData = 0;
    IMenuCtl  *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_SPEECH);
    MSG_FATAL("%x, %x ,%x,HandleSpeechDialogEvent",eCode,wParam,dwParam);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("pMenu is not null.................",0,0,0);
    //实现菜单循环滚动功能
    //SettingMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:   
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_YUYIIN_PACKGE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }

            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_DIA_SPEECH, IDS_DIA_SPEECH, NULL, 0);
            #if !defined(FEATURE_VERSION_K212_HUALU)&&!defined(FEATURE_VERSION_K292_WSF_CN)//ndef FEATURE_VERSION_K212_HUALU
			IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_MAIN_SPEECH, IDS_MAIN_SPEECH, NULL, 0);
			IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_TIME_SPEECH, IDS_TIME_SPEECH, NULL, 0);
            #endif
 #ifdef FEATURE_VERSION_K212_ND			
			///IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_INDIAL_SPEECH, IDS_INDIAL_SPEECH, NULL, 0);
			//IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_PHONEBOOK_SPEECH, IDS_PHONEBOOK_SPEECH, NULL, 0);
#endif			
            return TRUE;

        case EVT_DIALOG_START:
  
            //设定标题格式
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_MULTI_SEL|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE);
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SAVE_BACK);
			
			(void) ICONFIG_GetItem(pMe->m_pConfig,CFGI_SOUND_BO_DIA,&bData,sizeof(bData));
                SetCheckBoxItem(pMenu, IDS_DIA_SPEECH, bData);

			(void) ICONFIG_GetItem(pMe->m_pConfig,CFGI_SOUND_BO_MAIN,&bData,sizeof(bData));
                SetCheckBoxItem(pMenu, IDS_MAIN_SPEECH, bData);
			(void) ICONFIG_GetItem(pMe->m_pConfig,CFGI_SOUND_BO_CORE,&bData,sizeof(bData));
                SetCheckBoxItem(pMenu, IDS_TIME_SPEECH, bData);
 #ifdef FEATURE_VERSION_K212_ND				
			//(void) ICONFIG_GetItem(pMe->m_pConfig,CFGI_SOUND_BO_INDIAL,&bData,sizeof(bData));
               // SetCheckBoxItem(pMenu, IDS_INDIAL_SPEECH, bData);
			//(void) ICONFIG_GetItem(pMe->m_pConfig,CFGI_SOUND_BO_PHONEBOOK,&bData,sizeof(bData));
               // SetCheckBoxItem(pMenu, IDS_PHONEBOOK_SPEECH, bData);
#endif				
		    IMENUCTL_SetSel(pMenu, IDS_DIA_SPEECH);
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;
			
		case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                    
                default:
                    break;
            }

            return TRUE;

        case EVT_KEY_RELEASE:
            switch(wParam)
            {
                case AVK_SELECT:
                    {
					   bData = GetCheckBoxVal(pMenu, IDS_DIA_SPEECH);
					   (void) ICONFIG_SetItem(pMe->m_pConfig,CFGI_SOUND_BO_DIA,&bData,sizeof(bData));
					    bData = GetCheckBoxVal(pMenu, IDS_MAIN_SPEECH);
					   (void) ICONFIG_SetItem(pMe->m_pConfig,CFGI_SOUND_BO_MAIN,&bData,sizeof(bData));
					    bData = GetCheckBoxVal(pMenu, IDS_TIME_SPEECH);
					   (void) ICONFIG_SetItem(pMe->m_pConfig,CFGI_SOUND_BO_CORE,&bData,sizeof(bData));
					    #ifdef FEATURE_VERSION_K212_ND
					//	bData = GetCheckBoxVal(pMenu, IDS_INDIAL_SPEECH);
					//   (void) ICONFIG_SetItem(pMe->m_pConfig,CFGI_SOUND_BO_INDIAL,&bData,sizeof(bData));
					//	bData = GetCheckBoxVal(pMenu, IDS_PHONEBOOK_SPEECH);
					  // (void) ICONFIG_SetItem(pMe->m_pConfig,CFGI_SOUND_BO_PHONEBOOK,&bData,sizeof(bData));
					   #endif
						CLOSE_DIALOG(DLGRET_WARNING)
					}
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
        default:
            break;
    }
    return FALSE;
}
#endif
/*==============================================================================
函数：
       HandleLanguageDialogEvent
说明：
       IDD_LANGUAGE_MENU对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
#ifdef FEATURE_LANGUAGE
static boolean  HandleLanguageDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)
    nv_language_enum_type language;
    byte inputmode;

    IMenuCtl  *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                     IDC_LANGUAGE);
    MSG_FATAL("%x, %x ,%x,HandleLanguageDialogEvent",eCode,wParam,dwParam);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    //实现菜单循环滚动功能
    //SettingMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:   
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_LANGUAGE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
#ifdef FEATURE_LANG_CHINESE            
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_CHINESE, IDS_CHINESE, NULL, 0);
#endif //FEATURE_LANG_TCHINESE
#ifdef FEATURE_LANG_TCHINESE            
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_TCHINESE, IDS_TCHINESE, NULL, 0);
#endif //FEATURE_LANG_TCHINESE
#if defined(FEATURE_LANG_ITALIAN)
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_ITALIAN, IDS_ITALIAN, NULL, 0);
#endif
#if defined(FEATURE_LANG_SPANISH)
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SPANISH, IDS_SPANISH, NULL, 0);
#endif
#if defined(FEATURE_LANG_PORTUGUESE)
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_PORTUGUESE, IDS_PORTUGUESE, NULL, 0);
#endif
#if defined(FEATURE_LANG_INDONESIAN)
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_INDONESIAN, IDS_INDONESIAN, NULL, 0);
#endif
#if defined(FEATURE_LANG_MYANMAR)
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_MYANMAR, IDS_MYANMAR, NULL, 0);
#endif

#if defined(FEATURE_LANG_HINDI)
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_HINDI, IDS_HINDI, NULL, 0);
#endif
#if defined(FEATURE_LANG_VIETNAMESE)
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_VIETNAMESE, IDS_VIETNAMESE, NULL, 0);
#endif
#if defined(FEATURE_LANG_ARABIC)
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_ARABIC, IDS_ARABIC, NULL, 0);
#endif
#if defined(FEATURE_LANG_HEBREW)
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_HEBREW, IDS_HEBREW, NULL, 0);
#endif

#if defined(FEATURE_LANG_THAI)
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_THAI, IDS_THAI, NULL, 0);
#endif

#if defined(FEATURE_LANG_RUSSIAN)
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_RUSSIAN, IDS_RUSSIAN, NULL, 0);
#endif
#if defined(FEATURE_LANG_FRENCH)
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_FRENCH,IDS_FRENCH,NULL, 0);
#endif

#ifdef FEATURE_LANG_ENGLISH
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_ENGLISH, IDS_ENGLISH, NULL, 0);
#endif //FEATURE_LANG_ENGLISH
            return TRUE;

        case EVT_DIALOG_START:
            {
                uint16 nSelItem = 0;
                
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_LANGUAGE_SELECTION,
                                       &language,
                                       sizeof(language));

                switch (language)
                {
#ifdef FEATURE_LANG_CHINESE
                    case NV_LANGUAGE_CHINESE:       //中文
                        nSelItem = IDS_CHINESE;
                        break;
#endif /* FEATURE_LANG_CHINESE */

#ifdef FEATURE_LANG_TCHINESE
                    case NV_LANGUAGE_TCHINESE:       //繁体中文
                        nSelItem = IDS_TCHINESE;
                        break;
#endif /* FEATURE_LANG_TCHINESE */

#ifdef FEATURE_LANG_ITALIAN
                    case NV_LANGUAGE_ITALIAN:       //意大利文
                        nSelItem = IDS_ITALIAN;
                        break;
#endif /* FEATURE_LANG_ITALIAN */

#ifdef FEATURE_LANG_SPANISH
                    case NV_LANGUAGE_SPANISH:       //西班牙文
                        nSelItem = IDS_SPANISH;
                        break;
#endif /* FEATURE_LANG_SPANISH */

#ifdef FEATURE_LANG_PORTUGUESE
                    case NV_LANGUAGE_PORTUGUESE:    //葡萄牙文
                        nSelItem = IDS_PORTUGUESE;
                        break;
#endif /* FEATURE_LANG_PORTUGUESE */

#ifdef FEATURE_LANG_INDONESIAN
                    case NV_LANGUAGE_INDONESIAN:    //印度尼西亚
                        nSelItem = IDS_INDONESIAN;
                        break;
#endif //FEATURE_LANG_INDONESIAN
#ifdef FEATURE_LANG_MYANMAR   //add by yangdecai 2010-1224
					case NV_LANGUAGE_MYANMAR:    //印度尼西亚
                        nSelItem = IDS_MYANMAR;
                        break;
#endif
#ifdef FEATURE_LANG_HINDI
                    case NV_LANGUAGE_HINDI:    //印度语
                        nSelItem = IDS_HINDI;
                        break;
#endif /*FEATURE_LANG_HINDI*/

#ifdef FEATURE_LANG_ARABIC
                    case NV_LANGUAGE_ARABIC:    //阿拉伯语
                        nSelItem = IDS_ARABIC;
                        break;
#endif /*FEATURE_LANG_ARABIC*/

#ifdef FEATURE_LANG_HEBREW
                    case NV_LANGUAGE_HEBREW:    //希伯来语
                        nSelItem = IDS_HEBREW;
                        break;
#endif /*FEATURE_LANG_HEBREW*/

#ifdef FEATURE_LANG_THAI
                    case NV_LANGUAGE_THAI:    //泰国语
                        nSelItem = IDS_THAI;
                        break;
#endif /*FEATURE_LANG_THAI*/

#ifdef FEATURE_LANG_VIETNAMESE
                    case NV_LANGUAGE_VIETNAMESE:    //越南语
                        nSelItem = IDS_VIETNAMESE;
                        break;
#endif /*FEATURE_LANG_VIETNAMESE*/

#ifdef FEATURE_LANG_RUSSIAN
                    case NV_LANGUAGE_RUSSIAN:    //俄文
                        nSelItem = IDS_RUSSIAN;
                        break;
#endif /*FEATURE_LANG_RUSSIAN*/

#if defined(FEATURE_LANG_FRENCH)
                    case NV_LANGUAGE_FRENCH:
                        nSelItem = IDS_FRENCH;
                        break;
#endif //#if defined(FEATURE_LANG_FRENCH)

                    case NV_LANGUAGE_ENGLISH:       //英文
                    default:
                        nSelItem = IDS_ENGLISH;
                        break;
                }

                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, nSelItem, TRUE);
                IMENUCTL_SetSel(pMenu, nSelItem);
            }

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            switch (wParam)
            {
#ifdef FEATURE_USES_ZI
			#ifdef FEATURE_LANG_CHINESE
                case IDS_CHINESE:       //中文
                    language = NV_LANGUAGE_CHINESE;
                    inputmode = OEM_MODE_ZI_PINYIN;    
                    break;
#endif /* FEATURE_LANG_CHINESE */

#ifdef FEATURE_LANG_TCHINESE
                case IDS_TCHINESE:       //繁体中文
                    language = NV_LANGUAGE_TCHINESE;
                    inputmode = OEM_MODE_ZI_ZHUYIN;    
                    break;
#endif /* FEATURE_LANG_TCHINESE */

#ifdef FEATURE_LANG_ITALIAN
                case IDS_ITALIAN:       //意大利文
                    language = NV_LANGUAGE_ITALIAN;
                    inputmode = OEM_MODE_ZI_ITALIAN;     
                    break;
#endif /* FEATURE_LANG_ITALIAN */

#ifdef FEATURE_LANG_SPANISH
                case IDS_SPANISH:       //西班牙文
                    language = NV_LANGUAGE_SPANISH;
                    inputmode = OEM_MODE_ZI_RAPID_SPANISH;  //OEM_MODE_T9_MT_SPANISH;
                    break;
#endif /* FEATURE_LANG_SPANISH */

#ifdef FEATURE_LANG_PORTUGUESE
                case IDS_PORTUGUESE:    //葡萄牙文
                    language = NV_LANGUAGE_PORTUGUESE;
                    inputmode = OEM_MODE_ZI_MT_ENGLISH;//OEM_MODE_T9_RAPID_PORTUGUESE;//OEM_MODE_T9_MT_PORTUGUESE;
                    break;
#endif /* FEATURE_LANG_PORTUGUESE */

#ifdef FEATURE_LANG_INDONESIAN
                case IDS_INDONESIAN:    //印度尼西亚
                    language = NV_LANGUAGE_INDONESIAN;
                    inputmode = OEM_MODE_ZI_MT_ENGLISH; //OEM_MODE_T9_MT_INDONESIAN;                           
                    break;
#endif /* FEATURE_LANG_INDONESIAN */
#ifdef FEATURE_LANG_MYANMAR   //add by yangdecai 2010-1224
				case IDS_MYANMAR:
					language = NV_LANGUAGE_MYANMAR;
                    #if defined(FEATURE_VERSION_VG68) || defined(FEATURE_VERSION_X3)
					inputmode = OEM_MT_MODE_MYANMAR;
					#else
                    inputmode = OEM_MODE_MYANMAR; //OEM_MODE_T9_MT_INDONESIAN;  
                    #endif                        
                    break;
#endif
#ifdef FEATURE_LANG_HINDI
                case IDS_HINDI:    //印度语
                    language = NV_LANGUAGE_HINDI;
                    inputmode = OEM_MODE_ZI_MT_HINDI;                          
                    break;
#endif /*FEATURE_LANG_HINDI*/

#ifdef FEATURE_LANG_ARABIC
                case IDS_ARABIC:    //阿拉伯语
                    language = NV_LANGUAGE_ARABIC;
                    inputmode = OEM_MODE_ZI_MT_ARABIC;                             
                    break;
#endif /*FEATURE_LANG_ARABIC*/

#ifdef FEATURE_LANG_HEBREW
                case IDS_HEBREW:    //希伯来语
                    language = NV_LANGUAGE_HEBREW;
                    inputmode = OEM_MODE_ZI_MT_HEBREW;                          
                    break;
#endif /*FEATURE_LANG_HEBREW*/

#ifdef FEATURE_LANG_THAI
                case IDS_THAI:    //泰国语
                    language = NV_LANGUAGE_THAI;
                    #ifdef FEATURE_ZI_RAPID_THAI
                    inputmode = OEM_MODE_ZI_MT_THAI;   
                    #else
                    inputmode = OEM_MODE_ZI_MT_THAI;
                    #endif
                    break;
#endif /*FEATURE_LANG_THAI*/

#ifdef FEATURE_LANG_VIETNAMESE
                case IDS_VIETNAMESE:    //越南语
                    language = NV_LANGUAGE_VIETNAMESE;
                    inputmode = OEM_MODE_ZI_MT_VIETNAMESE;                         
                    break;
#endif /*FEATURE_LANG_VIETNAMESE*/

#ifdef FEATURE_LANG_RUSSIAN
                case IDS_RUSSIAN:    //俄文
                    language = NV_LANGUAGE_RUSSIAN;               
                    break;
#endif /*FEATURE_LANG_RUSSIAN*/

#if defined(FEATURE_LANG_FRENCH)
                case IDS_FRENCH:
                    language = NV_LANGUAGE_FRENCH;
                    inputmode = OEM_MODE_ZI_MT_FRENCH;
                    break;
#endif //#if defined(FEATURE_LANG_FRENCH)

                case IDS_ENGLISH:       //英文
                default:
                    language = NV_LANGUAGE_ENGLISH;
              // inputmode = OEM_MODE_ZI_MT_ENGLISH;
#if defined FEATURE_LANG_SPANISH
                    inputmode = OEM_MODE_ZI_RAPID_ENGLISH;
#else
                    inputmode = OEM_MODE_ZI_MT_ENGLISH;
#endif
                    break;
            #else
#ifdef FEATURE_LANG_CHINESE
                case IDS_CHINESE:       //中文
                    language = NV_LANGUAGE_CHINESE;
                    inputmode = OEM_MODE_T9_PINYIN;    
                    break;
#endif /* FEATURE_LANG_CHINESE */

#ifdef FEATURE_LANG_TCHINESE
                case IDS_TCHINESE:       //繁体中文
                    language = NV_LANGUAGE_TCHINESE;
                    inputmode = OEM_MODE_T9_ZHUYIN;    
                    break;
#endif /* FEATURE_LANG_TCHINESE */

#ifdef FEATURE_LANG_ITALIAN
                case IDS_ITALIAN:       //意大利文
                    language = NV_LANGUAGE_ITALIAN;
                    inputmode = OEM_MODE_T9_ITALIAN;     
                    break;
#endif /* FEATURE_LANG_ITALIAN */

#ifdef FEATURE_LANG_SPANISH
                case IDS_SPANISH:       //西班牙文
                    language = NV_LANGUAGE_SPANISH;
                    inputmode = OEM_MODE_T9_RAPID_SPANISH;  //OEM_MODE_T9_MT_SPANISH;
                    break;
#endif /* FEATURE_LANG_SPANISH */

#ifdef FEATURE_LANG_PORTUGUESE
                case IDS_PORTUGUESE:    //葡萄牙文
                    language = NV_LANGUAGE_PORTUGUESE;
                    inputmode = OEM_MODE_T9_MT_ENGLISH;//OEM_MODE_T9_RAPID_PORTUGUESE;//OEM_MODE_T9_MT_PORTUGUESE;
                    break;
#endif /* FEATURE_LANG_PORTUGUESE */

#ifdef FEATURE_LANG_INDONESIAN
                case IDS_INDONESIAN:    //印度尼西亚
                    language = NV_LANGUAGE_INDONESIAN;
                    inputmode = OEM_MODE_T9_MT_ENGLISH; //OEM_MODE_T9_MT_INDONESIAN;                           
                    break;
#endif /* FEATURE_LANG_INDONESIAN */
#ifdef FEATURE_LANG_MYANMAR   //add by yangdecai 2010-1224
				case IDS_MYANMAR:
					language = NV_LANGUAGE_MYANMAR;
					#if defined(FEATURE_VERSION_VG68) || defined(FEATURE_VERSION_X3) || defined(FEATURE_VERSION_C11)||defined(FEATURE_VERSION_W021_CT100_MYA)
					inputmode = OEM_MT_MODE_MYANMAR;
					#else
                    inputmode = OEM_MODE_MYANMAR; //OEM_MODE_T9_MT_INDONESIAN;  
                    #endif
                    break;
#endif
#ifdef FEATURE_LANG_HINDI
                case IDS_HINDI:    //印度语
                    language = NV_LANGUAGE_HINDI;
                    inputmode = OEM_MODE_T9_MT_HINDI;                          
                    break;
#endif /*FEATURE_LANG_HINDI*/

#ifdef FEATURE_LANG_ARABIC
                case IDS_ARABIC:    //阿拉伯语
                    language = NV_LANGUAGE_ARABIC;
                    inputmode = OEM_MODE_T9_MT_ARABIC;                             
                    break;
#endif /*FEATURE_LANG_ARABIC*/

#ifdef FEATURE_LANG_HEBREW
                case IDS_HEBREW:    //希伯来语
                    language = NV_LANGUAGE_HEBREW;
                    inputmode = OEM_MODE_T9_MT_HEBREW;                          
                    break;
#endif /*FEATURE_LANG_HEBREW*/

#ifdef FEATURE_LANG_THAI
                case IDS_THAI:    //泰国语
                    language = NV_LANGUAGE_THAI;
                    #ifdef FEATURE_T9_RAPID_THAI
                    inputmode = OEM_MODE_T9_MT_THAI;   
                    #else
                    inputmode = OEM_MODE_T9_MT_THAI;
                    #endif
                    break;
#endif /*FEATURE_LANG_THAI*/

#ifdef FEATURE_LANG_VIETNAMESE
                case IDS_VIETNAMESE:    //越南语
                    language = NV_LANGUAGE_VIETNAMESE;
                    inputmode = OEM_MODE_T9_MT_VIETNAMESE;                         
                    break;
#endif /*FEATURE_LANG_VIETNAMESE*/

#ifdef FEATURE_LANG_RUSSIAN
                case IDS_RUSSIAN:    //俄文
                    language = NV_LANGUAGE_RUSSIAN;               
                    break;
#endif /*FEATURE_LANG_RUSSIAN*/

#if defined(FEATURE_LANG_FRENCH)
                case IDS_FRENCH:
                    language = NV_LANGUAGE_FRENCH;
                    inputmode = OEM_MODE_T9_MT_FRENCH;
                    break;
#endif //#if defined(FEATURE_LANG_FRENCH)

                case IDS_ENGLISH:       //英文
                default:
                    language = NV_LANGUAGE_ENGLISH;
              // inputmode = OEM_MODE_T9_MT_ENGLISH;
#if defined FEATURE_LANG_SPANISH
                    inputmode = OEM_MODE_T9_RAPID_ENGLISH;
#else
                    inputmode = OEM_MODE_T9_MT_ENGLISH;
#endif
                    break;
#endif
            }
            (void) ICONFIG_SetItem(pMe->m_pConfig,
                                   CFGI_LANGUAGE_SELECTION,
                                   &language,
                                   sizeof(language));
            (void) ICONFIG_SetItem(pMe->m_pConfig,
                                   CFGI_INPUTMODE,
                                   &inputmode,
                                   sizeof(inputmode));  
            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, wParam, TRUE);
            CLOSE_DIALOG(DLGRET_WARNING)
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandleLanguageDialogEvent
#endif

/*==============================================================================
函数：
       SettingMenu_MakeForwardCall
说明：
       呼叫一个号码，处理呼叫转移和等待

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       pNum: 呼叫号码指针
       nNum: 呼叫号码长度

返回值：
       void
备注：

==============================================================================*/
static void SettingMenu_MakeForwardCall(CSettingMenu *pMe,char *Number)
{
    if((pMe->m_pShell) && (Number))
    {
#ifdef FEATURE_ICM
        AECHAR w_buf[AEECM_MAX_DIGITS_LENGTH] = {0};
#else
        AECHAR w_buf[AEET_MAX_DIGITS_LENGTH] = {0};
#endif
        ICallApp         *pCallApp = NULL;
        if ( SUCCESS != ISHELL_CreateInstance( pMe->m_pShell,
                                                        AEECLSID_DIALER,
                                                        (void **)&pCallApp))
        {
            return ;
        }
        STRTOWSTR(Number, w_buf, sizeof(w_buf));
        //(void)MakeVoiceCall(pMe->m_pShell, FALSE, w_buf);
        ICallApp_CallNumber(pCallApp,w_buf);
        if (pCallApp)
        {
            ICallApp_Release(pCallApp);
            pCallApp = NULL;
        }
    }
}
/*==============================================================================
函数:
       SettingMenu_DialogTimeout

说明:
       定时器回调函数。主要用于自动关闭消息对话框。

参数:
       pme [in]：void *类型指针，暗指CSettingMenu结构指针。

返回值:
       无。

备注:

==============================================================================*/
static void SettingMenu_DialogTimeout(void *pme)
{
    CSettingMenu *pMe = (CSettingMenu *)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent(pMe->m_pShell,
                            AEECLSID_APP_SETTINGMENU,
                            EVT_DISPLAYDIALOGTIMEOUT,
                            0,
                            0);
}

#ifdef FEATURE_PERU_VERSION
/*==============================================================================
函数：
       HandlePhoneNumberDialogEvent
说明：
       IDD_PHONE_NUMBER对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandlePhoneNumberDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    MSG_FATAL("%x, %x ,%x,HandlePhoneNumberDialogEvent",eCode,wParam,dwParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                                    AEECLSID_APP_SETTINGMENU,
                                                    EVT_USER_REDRAW,
                                                    0,
                                                    0);
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_SELECT:
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    break;
            }
            return TRUE;

        case EVT_USER_REDRAW:
            {
                AECHAR        pText[20];
                AEERect       rc;
                RGBVAL        old_color;
                uint32 dwFlags = IDF_ALIGN_CENTER |
                                                        IDF_ALIGN_MIDDLE |
                                                        IDF_TEXT_TRANSPARENT;
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                                        CFGI_PHONE_NUMBER,
                                                        pText,
                                                        sizeof(pText));
                SETAEERECT(&rc,
                                                        0,
                                                        0,
                                                        102,
                                                        64);
                (void) IDISPLAY_DrawText(pMe->m_pDisplay,
                                                        AEE_FONT_LARGE,
                                                        pText,
                                                        -1,
                                                        0,
                                                        0,
                                                        &rc,
                                                        dwFlags);
                SETAEERECT(&rc,
                                                        0,
                                                        64,
                                                        102,
                                                        16);
                IDISPLAY_DrawRect(pMe->m_pDisplay, &rc,
                                                        MAKE_RGB(192, 192, 192),
                                                        MAKE_RGB(0, 0, 255),
                                                        IDF_RECT_FILL );

                ISHELL_LoadResString(pMe->m_pShell,
                                                        AEE_APPSSETTINGMENU_RES_FILE,
                                                        IDS_PHONE_NUMBER,
                                                        pText,
                                                        sizeof(pText));

                old_color = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, MAKE_RGB(192, 192, 192));

                (void) IDISPLAY_DrawText(pMe->m_pDisplay,
                                                        AEE_FONT_LARGE,
                                                        pText,
                                                        -1,
                                                        0,
                                                        0,
                                                        &rc,
                                                        dwFlags);
                IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, old_color);
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            }
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        default:
            break;
    }
    return FALSE;
}
#endif
#ifdef FEATURE_NETWORK_SET
/*==============================================================================
函数：
       HandleNetSelectDialogEvent
说明：
       IDD_NET_SELECT对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleNetSelectDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_NET_SELECT);
    nv_stat_enum_type ret;
    if (pMenu == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("%x, %x ,%x,HandleNetSelectDialogEvent",eCode,wParam,dwParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_NET_SEL,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_NET_A, IDS_NET_A, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_NET_B, IDS_NET_B, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_NET_HOME, IDS_NET_HOME, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_NET_STANDARD, IDS_NET_STANDARD, NULL, 0);
            return TRUE;

        case EVT_DIALOG_START:
        {
            nv_item_type nvi;
            nvi.system_pref.nam = cm_get_curr_nam();
            ret = OEMNV_Get(NV_SYSTEM_PREF_I,&nvi);
            MSG_FATAL("NV_SYSTEM_PREF_I %d %d %d",ret,nvi.system_pref.sys,nvi.system_pref.nam);
            InitMenuIcons(pMenu);
            switch(nvi.system_pref.sys)
            {
                case NV_SP_STANDARD:
                default:
                    IMENUCTL_SetSel(pMenu, IDS_NET_STANDARD);
                    SetMenuIcon(pMenu, IDS_NET_STANDARD, TRUE);
                    break;

                case NV_SP_HOME_ONLY:
                    IMENUCTL_SetSel(pMenu, IDS_NET_HOME);
                    SetMenuIcon(pMenu, IDS_NET_HOME, TRUE);
                    break;

                case NV_SP_B_ONLY:
                    IMENUCTL_SetSel(pMenu, IDS_NET_B);
                    SetMenuIcon(pMenu, IDS_NET_B, TRUE);
                    break;

                case NV_SP_A_ONLY:
                    IMENUCTL_SetSel(pMenu, IDS_NET_A);
                    SetMenuIcon(pMenu, IDS_NET_A, TRUE);
                    break;

            }
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            return TRUE;
        }


        case EVT_COMMAND:
        {
           nv_item_type nvi;
           nvi.system_pref.nam = cm_get_curr_nam();
           OEMNV_Get(NV_SYSTEM_PREF_I,&nvi);
           switch(wParam)
           {
             case IDS_NET_STANDARD:
                nvi.system_pref.sys = NV_SP_STANDARD;
                break;

             case IDS_NET_HOME:
                nvi.system_pref.sys = NV_SP_HOME_ONLY;
                break;

             case IDS_NET_B:
                nvi.system_pref.sys = NV_SP_B_ONLY;
                break;

             case IDS_NET_A:
                nvi.system_pref.sys = NV_SP_A_ONLY;
                break;
             }
             ret = OEMNV_Put(NV_SYSTEM_PREF_I,&nvi);
             MSG_FATAL("NV_SYSTEM_PREF_I1 %d %d %d",ret,nvi.system_pref.sys,nvi.system_pref.nam);
             CLOSE_DIALOG(DLGRET_CANCELED)
             return TRUE;
        }

        case EVT_KEY:
            if(wParam == AVK_CLR)
            {
                CLOSE_DIALOG(DLGRET_CANCELED)
            }
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        default:
            break;
     }
     return FALSE;
}
#endif

static void SettingMenu_SetItemNumIcon(IMenuCtl   *pMenu)
{
}
/*==============================================================================
函数：
       SetMenu_SetCheckBoxAsOff
说明：
       用于设定给定菜单控件的全部菜单项单选按钮图标为非选中图标。

参数：
       pMenu {in]：IMenuCtl控件指针。

返回值：
       无。

备注：

==============================================================================*/
static void SetMenu_SetCheckBoxAsOff(IMenuCtl *pMenu)
{
    int nItemCount, index;
    uint16 wItemID;

    if (NULL == pMenu)
    {
        return;
    }

    nItemCount = IMENUCTL_GetItemCount(pMenu);
    for (index = 0; index < nItemCount; index++)
    {
        wItemID = IMENUCTL_GetItemID(pMenu, index);
        SetMenuIcon(pMenu, wItemID, FALSE);
    }
} // SetMenu_SetCheckBoxAsOff
#ifdef FEATRUE_AUTO_POWER
static void SettingMenu_Set_CTL(CSettingMenu *pMe)
{
    //设置控件焦点
    if (pMe->m_nCtlID == IDC_AUTO_POWER_STATUS)
    {
        IMENUCTL_SetActive(pMe->m_pStatus,TRUE);
        IMENUCTL_SetActive(pMe->m_pState, FALSE);
        ITIMECTL_SetActive(pMe->m_pTime, FALSE);
    }       
    if (pMe->m_nCtlID == IDC_AUTO_POWER_STATE)
    {
        IMENUCTL_SetActive(pMe->m_pStatus,FALSE);
        IMENUCTL_SetActive(pMe->m_pState, TRUE);
        ITIMECTL_SetActive(pMe->m_pTime, FALSE);
    }
    else if (pMe->m_nCtlID == IDC_AUTO_POWER_TIME)
    {
        IMENUCTL_SetActive(pMe->m_pStatus,FALSE);       
        IMENUCTL_SetActive(pMe->m_pState, FALSE);
        ITIMECTL_SetActive(pMe->m_pTime, TRUE);
    }
}

/*==============================================================================
函数：
       HandleAlarmSubDialogEvent
说明：
       IDD_ALARMCLOCK_SUB对话框事件处理函数

参数：
       pMe [in]：指向ClockApps Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  Setting_HandleAuto_Power_DialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{

    static byte timeFormatType = 0;
	
    MSG_FATAL("%x, %x ,%x,Setting_HandleAuto_Power_DialogEvent",eCode,wParam,dwParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            
            //功能选择控件
            if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                             AEECLSID_LISTCTL,
                             (void **)&pMe->m_pStatus))
            {
                return FALSE;
            }
            
            //状态控件
            if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                            AEECLSID_LISTCTL,
                            (void **)&pMe->m_pState))
            {
                (void)IMENUCTL_Release(pMe->m_pStatus);
                return FALSE;
            }
                

            //时间控件
            if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                 AEECLSID_COUNTDOWNCTL,
                                 (void **)&pMe->m_pTime))
            {
                (void)IMENUCTL_Release(pMe->m_pState);
                (void)IMENUCTL_Release(pMe->m_pStatus);
                return FALSE;
            }

            if(pMe->m_pTime == NULL ||pMe->m_pState == NULL)
            {
                MSG_FATAL("EVT_DIALOG_INIT FAIL", 0, 0, 0);
                return FALSE;
            }
#ifdef SET_REP_MOD_LIST
            //循环方式LIST 控件
            pMe->m_pRepMode = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_AUTO_POWER_REP_MODE);
            if (pMe->m_pRepMode == NULL )
            {
                return FALSE;
            }
#endif
            IMENUCTL_AddItem(pMe->m_pStatus, AEE_APPSSETTINGMENU_RES_FILE, IDS_AUTO_POWER_ON_TIME, IDS_AUTO_POWER_ON_TIME, NULL, 0);
            IMENUCTL_AddItem(pMe->m_pStatus, AEE_APPSSETTINGMENU_RES_FILE, IDS_AUTO_POWER_OFF_TIME, IDS_AUTO_POWER_OFF_TIME, NULL, 0);
            IMENUCTL_AddItem(pMe->m_pState, AEE_APPSSETTINGMENU_RES_FILE, IDS_ENABLE, IDS_ENABLE, NULL, 0);
            IMENUCTL_AddItem(pMe->m_pState, AEE_APPSSETTINGMENU_RES_FILE, IDS_DISABLE, IDS_DISABLE, NULL, 0);

            pMe->m_nCtlID = IDC_AUTO_POWER_STATUS;
            pMe->m_b_selete_Pm = FALSE;
            pMe->m_b_ampm_mode = OEMNV_TIMEFORM_24HR;
#if 0
            ICONFIG_GetItem(pMe->m_pConfig, CFGI_TIME_FORMAT, (void*)&pMe->m_b_ampm_mode, sizeof(byte));
#endif

        ITIMECTL_SetFont( pMe->m_pTime, AEE_FONT_BOLD, AEE_FONT_BOLD);
#if defined( FEATURE_CARRIER_THAILAND_HUTCH)
        ITIMECTL_SetFont( pMe->m_pTime, AEE_FONT_BOLD, AEE_FONT_BOLD);
#endif
            return TRUE;

        case EVT_DIALOG_START:
        {
            extern int OEM_GetConfig( AEEConfigItem i, void * pBuff, int nSize);
            OEM_GetConfig( CFGI_TIME_FORMAT, &timeFormatType, sizeof( byte));
            ITIMECTL_SetOemProperties( pMe->m_pTime, TP_OEM_COUNTDOWNCTL_EDITABLE|((timeFormatType==OEMNV_TIMEFORM_AMPM)?TP_OEM_COUNTDOWNCTL_12_FORMAT:0));

            (void) ISHELL_PostEvent( pMe->m_pShell,  AEECLSID_APP_SETTINGMENU, EVT_USER_REDRAW, 0,0);
            return TRUE;
        }

        case EVT_USER_REDRAW:
        {
            AEEMenuColors color;        //List Menu 的文本及背景颜色
            TitleBar_Param_type title;
            AECHAR      wszTitle[32];    //闹钟标题
            AECHAR      wszState[16];    //闹钟开关状态
            AECHAR      wszTime[16];     //时间
            AECHAR      wszStatus[16];
            AEERect     rc;
            int16       titleheight = 0;
            int16       bottomheight = GetBottomBarHeight(pMe->m_pDisplay);
            int16       itemheight = IDISPLAY_GetFontMetrics(pMe->m_pDisplay, AEE_FONT_BOLD, NULL, NULL);
            int         lineSpace = ( pMe->m_rc.dy - titleheight  - bottomheight - itemheight*3) / 4;
            nv_language_enum_type language = NV_LANGUAGE_ENGLISH;
            ICONFIG_GetItem(pMe->m_pConfig, CFGI_LANGUAGE_SELECTION, &language, sizeof(language));
            //MSG_FATAL("EVT_USER_REDRAW", 0, 0, 0);
            MEMSET(wszTitle,0,sizeof(wszTitle));
            MEMSET(wszState,0,sizeof(wszState));
            MEMSET(wszTime,0,sizeof(wszTime));
            MEMSET(wszStatus,0,sizeof(wszStatus));

            ITIMECTL_SetProperties(pMe->m_pTime,  TP_NO_SECONDS | TP_AUTOREDRAW);
            IMENUCTL_SetOemProperties(pMe->m_pState, 
            OEMMP_SWITCHNAVIGATIONKEY|OEMMP_IDF_ALIGN_CENTER|OEMMP_USE_MENU_STYLE);
            IMENUCTL_SetOemProperties(pMe->m_pStatus, 
            OEMMP_SWITCHNAVIGATIONKEY|OEMMP_IDF_ALIGN_CENTER|OEMMP_USE_MENU_STYLE);

			//Add By zzg 2010_11_08
			//IMENUCTL_SetProperties(pMe->m_pStatus, MP_WRAPSCROLL);
            //IMENUCTL_SetOemProperties(pMe->m_pStatus, OEMMP_USE_MENU_STYLE);
			//IMENUCTL_SetProperties(pMe->m_pState, MP_WRAPSCROLL);
            //IMENUCTL_SetOemProperties(pMe->m_pState, OEMMP_USE_MENU_STYLE);
			//Add End

            if(IMENUCTL_GetSel(pMe->m_pStatus) == IDS_AUTO_POWER_ON_TIME)
            {
                ICONFIG_GetItem(pMe->m_pConfig, CFGI_AUTO_POWER_ON, (void*)&pMe->m_ClockCfg, sizeof(Auto_Power_Cfg));
            }
            else if(IMENUCTL_GetSel(pMe->m_pStatus) == IDS_AUTO_POWER_OFF_TIME)
            {
                IMENUCTL_SetSel(pMe->m_pStatus, IDS_AUTO_POWER_OFF_TIME);
                ICONFIG_GetItem(pMe->m_pConfig, CFGI_AUTO_POWER_OFF, (void*)&pMe->m_ClockCfg, sizeof(Auto_Power_Cfg));
            }
            //STATE为真时,状态选中"开"                        

            if(pMe->m_ClockCfg.bStateOn == TRUE)
            {
                IMENUCTL_SetSel(pMe->m_pState, IDS_ENABLE);
            }
            else
            {
                IMENUCTL_SetSel(pMe->m_pState, IDS_DISABLE);
            }
            //记录响闹时间
            pMe->m_dwDispTime =   pMe->m_ClockCfg.dwWATime;

#if 0
            if(pMe->m_dwDispTime >= (12*MSSEC_PER_HOUR) && pMe->m_b_ampm_mode == OEMNV_TIMEFORM_AMPM)
            {
                pMe->m_dwDispTime = pMe->m_dwDispTime - 12*MSSEC_PER_HOUR;
                pMe->m_ClockCfg.dwWATime = pMe->m_dwDispTime;
                pMe->m_b_selete_Pm = TRUE;
            }
#endif
            pMe->m_dwHour = pMe->m_dwDispTime/MSSEC_PER_HOUR;
            pMe->m_dwMin   = pMe->m_dwDispTime%MSSEC_PER_HOUR;

            MSG_FATAL("m_dwHour = %d;m_dwMin = %d cfg= %d",pMe->m_dwHour,pMe->m_dwMin,pMe->m_dwDispTime);

            //设置给定的时间,并刷新屏幕
            (void)ITIMECTL_SetTimeEx(pMe->m_pTime,  pMe->m_dwDispTime, TRUE);
#ifdef SET_REP_MOD_LIST
            //根据重复方式选择方式List选项
            switch(pMe->m_ClockCfg.RepMode)
            {
                default:
                case WEEK_ALARM_REP1:   //每天
                    IMENUCTL_SetSel(pMe->m_pRepMode, IDL_REP_MODE_0);
                    break;

                case WEEK_ALARM_REP2:   //周一~周五
                    IMENUCTL_SetSel(pMe->m_pRepMode, IDL_REP_MODE_1);
                    break;

                case WEEK_ALARM_REP3:   //周一~周六
                    IMENUCTL_SetSel(pMe->m_pRepMode, IDL_REP_MODE_2);
                    break;

                case WEEK_ALARM_REP4:   //周一
                    IMENUCTL_SetSel(pMe->m_pRepMode, IDL_REP_MODE_3);
                    break;

                case WEEK_ALARM_REP5:   //周二
                    IMENUCTL_SetSel(pMe->m_pRepMode, IDL_REP_MODE_4);
                    break;

                case WEEK_ALARM_REP6:   //周三
                    IMENUCTL_SetSel(pMe->m_pRepMode, IDL_REP_MODE_5);
                    break;

                case WEEK_ALARM_REP7:   //周四
                    IMENUCTL_SetSel(pMe->m_pRepMode, IDL_REP_MODE_6);
                    break;

                case WEEK_ALARM_REP8:   //周五
                    IMENUCTL_SetSel(pMe->m_pRepMode, IDL_REP_MODE_7);
                    break;

                case WEEK_ALARM_REP9:   //周六
                    IMENUCTL_SetSel(pMe->m_pRepMode, IDL_REP_MODE_8);
                    break;

                case WEEK_ALARM_REP10:  //周日
                    IMENUCTL_SetSel(pMe->m_pRepMode, IDL_REP_MODE_9);
                    break;

            }
#endif
            //初始化List Menu的颜色
            color.wMask = MC_SEL_TEXT | MC_SEL_BACK | MC_TEXT | MC_BACK;
            color.cBack = RGB_WHITE;
            color.cText =  RGB_GRAY;
            color.cSelBack = RGB_WHITE;
            color.cSelText = RGB_BLACK;

            //填充背景颜色
#ifdef FEATURE_CARRIER_CHINA_VERTU
            {
                IImage *pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
                
                Appscommon_ResetBackground(pMe->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &pMe->m_rc, 0, 0);
                if(pImageBg != NULL)
                {
                    IImage_Release(pImageBg);
                }
            }
#else
            Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
#endif
            //IDISPLAY_FillRect(pMe->m_pDisplay,  &pMe->m_rc,  RGB_BLACK);
            
            //开机，关机选择控件矩形
            {
                SETAEERECT(&rc,  CONTROL_RECT_START_X+10, titleheight + lineSpace-CONTROL_RECT_RESET_Y,
                                            pMe->m_rc.dx - CONTROL_RECT_START_X-20,
                                            itemheight + 8);
                IDISPLAY_EraseRect(pMe->m_pDisplay,&rc);
                IMENUCTL_SetRect(pMe->m_pStatus, &rc);          
            }
            
            //状态list控件矩形
            {   
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)
                SETAEERECT(&rc,  10, titleheight + lineSpace*2 + itemheight+CONTROL_RECT_RESET_Y,
                                            pMe->m_rc.dx-20,
                                            itemheight + 8);
#else
                SETAEERECT(&rc,  CONTROL_RECT_START_X+10, titleheight + lineSpace*2 + itemheight+CONTROL_RECT_RESET_Y,
                                            pMe->m_rc.dx-CONTROL_RECT_START_X-20,
                                            itemheight + 8);
#endif
                IDISPLAY_EraseRect(pMe->m_pDisplay,&rc);
                IMENUCTL_SetRect(pMe->m_pState, &rc);
            }
            

            // 时间控件的矩形
            {
#ifdef FEATURE_VERSION_KK5
                SETAEERECT(&rc,  CONTROL_RECT_START_X, titleheight + lineSpace*3 +itemheight*2+CONTROL_RECT_RESET_Y,
                                                pMe->m_rc.dx - CONTROL_RECT_START_X-10,
                                                itemheight + 8);
#else
                SETAEERECT(&rc,  CONTROL_RECT_START_X+10, titleheight + lineSpace*3 +itemheight*2+CONTROL_RECT_RESET_Y,
                                                pMe->m_rc.dx - CONTROL_RECT_START_X - 20,
                                                itemheight + 8);
#endif
                ITIMECTL_SetRect(pMe->m_pTime, &rc);
                IDISPLAY_FillRect(pMe->m_pDisplay, &rc,  RGB_WHITE);
            }

            //闹钟标题
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSSETTINGMENU_RES_FILE,
                                            IDS_AUTO_POWER_TITLE,
                                            wszTitle,
                                            sizeof(wszTitle));
                                            
            //功能选择标题
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSSETTINGMENU_RES_FILE,
                                            IDS_AUTOPOWER_STATUS,
                                            wszStatus,
                                            sizeof(wszStatus));

            //状态标题
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSSETTINGMENU_RES_FILE,
                                            IDS_AUTO_POWER_STATE,
                                            wszState,
                                            sizeof(wszState));

            //时间标题
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSSETTINGMENU_RES_FILE,
                                            IDS_AUTO_POWER_TIME,
                                            wszTime,
                                            sizeof(wszTime));

            rc.x = 0;
            rc.dx = pMe->m_rc.dx;
            rc.y = 0;
            rc.dy = titleheight;
            MEMSET(&title,0,sizeof(TitleBar_Param_type));
            title.dwAlignFlags = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER;
            title.pwszTitle = wszTitle;
			#if 0
            DrawTitleBar(pMe->m_pDisplay,&title);
			#else
			IANNUNCIATOR_SetFieldText(pMe->m_pAnn,wszTitle);
			#endif
            {
                RGBVAL nOldFontColor;
                
                nOldFontColor = IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
                 //功能
                (void)IDISPLAY_DrawText(pMe->m_pDisplay, AEE_FONT_NORMAL,
                                      wszStatus,-1,1,2+ titleheight + lineSpace-CONTROL_RECT_RESET_Y,NULL,
                                      IDF_ALIGN_LEFT | IDF_TEXT_TRANSPARENT);
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)
                //状态
                (void)IDISPLAY_DrawText(pMe->m_pDisplay, AEE_FONT_NORMAL,
                                      wszState,-1,1,2+ titleheight + lineSpace*2 + itemheight-CONTROL_RECT_RESET_Y,NULL,
                                      IDF_ALIGN_CENTER| IDF_TEXT_TRANSPARENT);
#else
                //状态
                (void)IDISPLAY_DrawText(pMe->m_pDisplay, AEE_FONT_NORMAL,
                                      wszState,-1,1,2+ titleheight + lineSpace*2 + itemheight-CONTROL_RECT_RESET_Y,NULL,
                                      IDF_ALIGN_LEFT| IDF_TEXT_TRANSPARENT);    
#endif
                //时间
                (void)IDISPLAY_DrawText(pMe->m_pDisplay, AEE_FONT_NORMAL,
                                      wszTime,-1,1,2+ titleheight + lineSpace*3 + itemheight*2+CONTROL_RECT_RESET_Y,NULL,
                                      IDF_ALIGN_LEFT | IDF_TEXT_TRANSPARENT);
                IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, nOldFontColor);
            }
            //设置LIST menu控件的颜色
            IMENUCTL_SetColors(pMe->m_pStatus, &color);
            IMENUCTL_SetColors(pMe->m_pState, &color);

            if( pMe->m_nCtlID == IDC_AUTO_POWER_TIME && timeFormatType == OEMNV_TIMEFORM_AMPM)
            {
                if( ITIMECTL_GetTime( pMe->m_pTime) >= 12*3600000)
                {
                    SETTING_MENU_DRAW_BOTTOMBAR( BTBAR_SAVE_AM_BACK);
                }
                else
                {
                    SETTING_MENU_DRAW_BOTTOMBAR( BTBAR_SAVE_PM_BACK);
                }
            }
            else
            {
                SETTING_MENU_DRAW_BOTTOMBAR( BTBAR_SAVE_BACK);
            }

            //Draw ARROW
            {
                IImage      *pR_ResImg = NULL;
                IImage      *pL_ResImg = NULL;
				AEEImageInfo iInfo;
                pL_ResImg  = ISHELL_LoadResImage(pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDB_LEFTARROW);
                pR_ResImg  = ISHELL_LoadResImage(pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDB_RIGHTARROW);
                IImage_GetInfo(pL_ResImg,&iInfo);
                if(pR_ResImg != NULL)
                {
                    //在功能和状态模式后面画该ICON,表示上下键改变值
                    IIMAGE_Draw(pR_ResImg, pMe->m_rc.dx - 10, titleheight+ lineSpace + 4-CONTROL_RECT_RESET_Y);
                    IIMAGE_Draw(pR_ResImg, pMe->m_rc.dx - 10, titleheight + lineSpace*2 + itemheight + 4+CONTROL_RECT_RESET_Y);
                }

                if(pL_ResImg != NULL)
                {
                    //在功能和状态模式后面画该ICON,表示上下键改变值
                    IIMAGE_Draw(pL_ResImg, CONTROL_RECT_START_X,titleheight + lineSpace + 4-CONTROL_RECT_RESET_Y);
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)
                    IIMAGE_Draw(pL_ResImg, 0,titleheight + lineSpace*2 + itemheight + 4+CONTROL_RECT_RESET_Y);
#else
                    IIMAGE_Draw(pL_ResImg, CONTROL_RECT_START_X,titleheight + lineSpace*2 + itemheight + 4+CONTROL_RECT_RESET_Y);
#endif
                }
#ifdef FEATURE_LCD_TOUCH_ENABLE
				SETAEERECT(&pMe->pL_Rect1, pMe->m_rc.dx - 10, titleheight+ lineSpace + 4-CONTROL_RECT_RESET_Y, iInfo.cx, iInfo.cy); 
				SETAEERECT(&pMe->pL_Rect2, pMe->m_rc.dx - 10, titleheight + lineSpace*2 + itemheight + 4+CONTROL_RECT_RESET_Y, iInfo.cx, iInfo.cy); 
				SETAEERECT(&pMe->pR_Rect1, CONTROL_RECT_START_X, titleheight + lineSpace + 4-CONTROL_RECT_RESET_Y, iInfo.cx, iInfo.cy); 
				SETAEERECT(&pMe->pR_Rect2, CONTROL_RECT_START_X, titleheight + lineSpace*2 + itemheight + 4+CONTROL_RECT_RESET_Y, iInfo.cx, iInfo.cy); 
#endif
                IIMAGE_Release(pR_ResImg);
                pR_ResImg = NULL;
                IIMAGE_Release(pL_ResImg);
                pL_ResImg = NULL;
            }
            SettingMenu_Set_CTL(pMe);
            (void)IMENUCTL_Redraw(pMe->m_pStatus);
            (void)IMENUCTL_Redraw(pMe->m_pState);
            (void)ITIMECTL_Redraw(pMe->m_pTime);

            //drawBottomBar(pMe->m_pDisplay, AEE_FONT_NORMAL, BTBAR_SELECT_BACK);
            
            // 统一更新界面
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            return TRUE;
        }


        case EVT_DIALOG_END:
            //重置控件切换指示值
            pMe->m_nCtlID = IDC_AUTO_POWER_STATUS;   //控件ID
            pMe->m_nTimeCtlCount = 0;       //0代表焦点在小时上,1代表在分钟上
            pMe->m_nNumKeyCount = 0;        //重置编辑为小时的十位

            if(ITIMECTL_IsActive(pMe->m_pTime))
            {
                ITIMECTL_SetActive(pMe->m_pTime, FALSE);
            }
            IMENUCTL_Release(pMe->m_pState);
            ITIMECTL_Release(pMe->m_pTime);
            IMENUCTL_Release(pMe->m_pStatus);
            return TRUE;

        case EVT_KEY:
            MSG_FATAL("Setting_HandleAuto_Power_DialogEvent EVT_KEY 1", 0, 0, 0);
            //时间控件下处理数字键设置时间
            if (ITIMECTL_IsActive(pMe->m_pTime))
            {
                MSG_FATAL("Setting_HandleAuto_Power_DialogEvent EVT_KEY 2", 0, 0, 0);
#if MIN_BREW_VERSION(3,0)
                // do not want to handle au
                if (((dwParam & KB_AUTOREPEAT) != 0) &&
                    ((wParam == AVK_UP) ||
                    (wParam == AVK_DOWN) ||
                    (wParam == AVK_LEFT) ||
                    (wParam == AVK_RIGHT)))
                {
                    return TRUE;
                }
#endif
                if(ITIMECTL_HandleEvent(pMe->m_pTime, eCode, wParam, dwParam))
                {
                    return TRUE;
                }
            }
            
            if (IMENUCTL_IsActive(pMe->m_pState))
            {
                MSG_FATAL("Setting_HandleAuto_Power_DialogEvent EVT_KEY 3", 0, 0, 0);
                if(IMENUCTL_HandleEvent(pMe->m_pState, eCode, wParam, dwParam))
                {
                    return TRUE;
                }
            }
            
            if (IMENUCTL_IsActive(pMe->m_pStatus))
            {
                MSG_FATAL("Setting_HandleAuto_Power_DialogEvent EVT_KEY 4", 0, 0, 0);
                if(IMENUCTL_HandleEvent(pMe->m_pStatus, eCode, wParam, dwParam))
                {
                    return TRUE;
                }
            }            

#if 0
            if((pMe->m_nCtlID == IDC_AUTO_POWER_TIME) &&
                (wParam == AVK_0 ||
                wParam == AVK_1 ||
                wParam == AVK_2 ||
                wParam == AVK_3 ||
                wParam == AVK_4 ||
                wParam == AVK_5 ||
                wParam == AVK_6 ||
                wParam == AVK_7 ||
                wParam == AVK_8 ||
                wParam == AVK_9 ))//为了避免焦点是时间控件时按其他键的多余处理
            {
                Setting_CClockApps_HandleNumKeyEvent(pMe, wParam);
            }
#endif

            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                case AVK_SELECT:
                case AVK_INFO:
                    //MSG_FATAL("EVT_KEY_RELEASE->AVK_SELECT", 0, 0, 0);
                    //当处于clockctl控件,不接收command事件
                    //处理Clock编辑界面应用COMMAND键保存设置内容.
#if 0
                    if(pMe->m_b_ampm_mode == OEMNV_TIMEFORM_AMPM)
                    {
                        if(IMENUCTL_GetSel(pMe->m_pState) == IDS_ENABLE)
                        {
                            pMe->m_ClockCfg.bStateOn = TRUE;
                        }
                        else
                        {
                            pMe->m_ClockCfg.bStateOn = FALSE;
                        }
                        pMe->m_ClockCfg.dwWATime = ITIMECTL_GetTime(pMe->m_pTime);
                        if(pMe->m_ClockCfg.bStateOn ==  FALSE)
                        {
                            if(Setting_CClockApps_HandleCmdEvent(pMe))
                            {
                                //MSG_FATAL("EVT_KEY->AVK_SELECT", 0, 0, 0);
                                CLOSE_DIALOG(DLGRET_CANCELED)
                                return TRUE;
                            }
                            return FALSE;
                        }
                        pMe->m_CallSettingSel = IDS_AM_PM_SET;
                        CLOSE_DIALOG(DLGRET_CALLSETTINGSEL)
                    }
                    else
#endif
                    {
                        if(Setting_CClockApps_HandleCmdEvent(pMe))
                        {
                            //MSG_FATAL("EVT_KEY->AVK_SELECT", 0, 0, 0);
                            pMe->m_msg_id = IDS_DONE;
                            CLOSE_DIALOG(DLGRET_WARNING)
                        }
                    }
                    break;

                default:
                    break;
            }
            return TRUE;

        case EVT_KEY_PRESS:
            //传送给自定义事件EVT_FOCUS_SWITCH处理,是为了避免切换控件的同时
            //控件提前处理方向键事件
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                            AEECLSID_APP_SETTINGMENU,
                                            EVT_FOCUS_SWITCH,
                                            wParam,
                                            0);
            return TRUE;
		#ifdef FEATURE_LCD_TOUCH_ENABLE
		case EVT_PEN_UP:
			{
				int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
				int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
                AEEDeviceInfo devinfo;
                int nBarH ;
                AEERect rc;
                AEERect rcStatus;
                AEERect rcState;
                AEERect rcTime;
                MSG_FATAL("Setting_HandleAuto_Power_DialogEvent wXPos=%d ,wYPos=%d",wXPos,wYPos,0);
                IMENUCTL_GetRect(pMe->m_pStatus,&rcStatus);
                IMENUCTL_GetRect(pMe->m_pState, &rcState);
                ITIMECTL_GetRect(pMe->m_pTime, &rcTime);    
                nBarH = GetBottomBarHeight(pMe->m_pDisplay);
                MEMSET(&devinfo, 0, sizeof(devinfo));
                ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
                SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);  
                if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
                {
                    if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                    {
                        return  Setting_HandleAuto_Power_DialogEvent(pMe,EVT_KEY,AVK_SELECT,0);
                    } 
                    else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//右
                    {                       
                        return Setting_HandleAuto_Power_DialogEvent(pMe,EVT_KEY,AVK_CLR,0);
                    }
                }                
				else if(TOUCH_PT_IN_RECT(wXPos, wYPos, rcStatus))
				{
                    pMe->m_nCtlID = IDC_AUTO_POWER_STATUS;
                    SettingMenu_Set_CTL(pMe);
                    pMe->m_nNumKeyCount = 0; 
                    pMe->m_nTimeCtlCount = 0;                    
                    IMENUCTL_HandleEvent(pMe->m_pStatus, EVT_KEY, AVK_LEFT, dwParam); 
                    if(IMENUCTL_GetSel(pMe->m_pStatus) == IDS_AUTO_POWER_ON_TIME)
                    {
                        ICONFIG_GetItem(pMe->m_pConfig, CFGI_AUTO_POWER_ON, (void*)&pMe->m_ClockCfg, sizeof(Auto_Power_Cfg));
                    }
                    else if(IMENUCTL_GetSel(pMe->m_pStatus) == IDS_AUTO_POWER_OFF_TIME)
                    {
                        IMENUCTL_SetSel(pMe->m_pStatus, IDS_AUTO_POWER_OFF_TIME);
                        ICONFIG_GetItem(pMe->m_pConfig, CFGI_AUTO_POWER_OFF, (void*)&pMe->m_ClockCfg, sizeof(Auto_Power_Cfg));
                    }
                    //STATE为真时,状态选中"开"                        
        
                    if(pMe->m_ClockCfg.bStateOn == TRUE)
                    {
                        IMENUCTL_SetSel(pMe->m_pState, IDS_ENABLE);
                    }
                    else
                    {
                        IMENUCTL_SetSel(pMe->m_pState, IDS_DISABLE);
                    }
                    //记录响闹时间
                    pMe->m_dwDispTime =   pMe->m_ClockCfg.dwWATime;
                    pMe->m_dwHour = pMe->m_dwDispTime/MSSEC_PER_HOUR;
                    pMe->m_dwMin   = pMe->m_dwDispTime%MSSEC_PER_HOUR;
                    //设置给定的时间,并刷新屏幕
                    (void)ITIMECTL_SetTimeEx(pMe->m_pTime,  pMe->m_dwDispTime, TRUE);                         
                    return TRUE;
				}
				else if(TOUCH_PT_IN_RECT(wXPos, wYPos, rcState))
				{
                    pMe->m_nCtlID = IDC_AUTO_POWER_STATE;
                    SettingMenu_Set_CTL(pMe);
                    if(IMENUCTL_GetSel(pMe->m_pState) == IDS_DISABLE)
                    {
                        return IMENUCTL_HandleEvent(pMe->m_pState, EVT_KEY, AVK_RIGHT, dwParam);  
                    }
                    else if(IMENUCTL_GetSel(pMe->m_pState) == IDS_ENABLE)
                    {
                        return IMENUCTL_HandleEvent(pMe->m_pState, EVT_KEY, AVK_LEFT, dwParam);  
                    }

				}
				else if(TOUCH_PT_IN_RECT(wXPos, wYPos, rcTime))
				{
                    pMe->m_nCtlID = IDC_AUTO_POWER_TIME;
                    SettingMenu_Set_CTL(pMe);   
                    ITIMECTL_SetProperties(pMe->m_pTime,
                                           TP_NO_SECONDS | TP_AUTOREDRAW);
                    
                    ITIMECTL_SetEditField(pMe->m_pTime, ITF_HOUR);
                    return ITIMECTL_HandleEvent(pMe->m_pTime, eCode, wParam, dwParam);
				}                
			}
			break;
		
		#endif
        case EVT_KEY_RELEASE:
            switch(wParam)
            {
                case AVK_SELECT:
                case AVK_LEFT:
                case AVK_RIGHT: 
                    return TRUE;
                default:
                    break;
            }
            break;
        case EVT_FOCUS_SWITCH:
        {
            //处理Clock编辑界面应用左右键切换控件
            MSG_FATAL("Setting_HandleAuto_Power_DialogEvent EVT_FOCUS_SWITCH",0,0,0);
            Setting_CClockApps_HandleKeyEvent(pMe, wParam);

            if( wParam == AVK_UP || wParam == AVK_DOWN || wParam == AVK_INFO || wParam == AVK_STAR)
            {
                if( pMe->m_nCtlID == IDC_AUTO_POWER_TIME && timeFormatType == OEMNV_TIMEFORM_AMPM)
                {
                    if( ITIMECTL_GetTime( pMe->m_pTime) >= 12*3600000)
                    {
                        SETTING_MENU_DRAW_BOTTOMBAR( BTBAR_SAVE_AM_BACK);
                    }
                    else
                    {
                        SETTING_MENU_DRAW_BOTTOMBAR( BTBAR_SAVE_PM_BACK);
                    }
                }
                else
                {
                    SETTING_MENU_DRAW_BOTTOMBAR( BTBAR_SAVE_BACK);
                }
            }

            return TRUE;
        }
#if 1
        case EVT_COMMAND:
            MSG_FATAL("Setting_HandleAuto_Power_DialogEvent EVT_COMMAND",0,0,0);
            if(IMENUCTL_GetSel(pMe->m_pState) == IDS_ENABLE)
            {
                pMe->m_ClockCfg.bStateOn = TRUE;
            }
            else
            {
                pMe->m_ClockCfg.bStateOn = FALSE;
            }
            //处理Clock编辑界面应用COMMAND键保存设置内容.
            if(Setting_CClockApps_HandleCmdEvent(pMe))
            {
                pMe->m_msg_id = IDS_DONE;
                CLOSE_DIALOG(DLGRET_WARNING)
            }
            return TRUE;
#endif
        default:
            break;
    }

    return FALSE;
} // HandleAlarmSubDialogEvent


/*==============================================================================
函数：
       Setting_CClockApps_HandleKeyEvent

说明：
       处理Clock编辑界面应用左右键切换控件.

参数：
       pMe [in]：指向Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam[in]: 按键代码

返回值：
       boolean。

备注：:

==============================================================================*/
static boolean Setting_CClockApps_HandleKeyEvent(CSettingMenu *pMe, uint16 wParam)
{
    if (pMe == NULL)
    {
        return FALSE;
    }

    MSG_FATAL("%x %d Setting_CClockApps_HandleKeyEvent", wParam, pMe->m_nCtlID, 0);
    MSG_FATAL("CtlCount= %d KeyCount= %d ",pMe->m_nTimeCtlCount ,pMe->m_nNumKeyCount,0);
    switch(wParam)
    {
        case AVK_DOWN:
            if(pMe->m_nCtlID == IDC_AUTO_POWER_STATUS)     //控件为状态,切换到重复方式
            {
                pMe->m_nCtlID = IDC_AUTO_POWER_STATE;
            }
            else if(pMe->m_nCtlID == IDC_AUTO_POWER_STATE) //重复方式
            {
                ITIMECTL_SetProperties(pMe->m_pTime,
                                       TP_NO_SECONDS | TP_AUTOREDRAW);

                ITIMECTL_SetEditField(pMe->m_pTime, ITF_HOUR);
                pMe->m_nCtlID = IDC_AUTO_POWER_TIME;

            }
            else if(pMe->m_nCtlID == IDC_AUTO_POWER_TIME)
            {
                pMe->m_nCtlID = IDC_AUTO_POWER_STATUS;
                pMe->m_nNumKeyCount = 0; 
                pMe->m_nTimeCtlCount = 0;
            }
            else
            {
                pMe->m_nCtlID--;
            }
            SettingMenu_Set_CTL(pMe);
            return TRUE;
                        
        case AVK_UP:
            if(pMe->m_nCtlID == IDC_AUTO_POWER_TIME) 
            {
                
                ITIMECTL_SetProperties(pMe->m_pTime, TP_NO_SECONDS | TP_AUTOREDRAW);
                //编辑区域设置为分钟
                ITIMECTL_SetEditField(pMe->m_pTime, ITF_HOUR);

                pMe->m_nTimeCtlCount = 0;  //0代表焦点在小时上,1代表在分钟上
                pMe->m_nCtlID = IDC_AUTO_POWER_STATE;
            }
            else if(pMe->m_nCtlID == IDC_AUTO_POWER_STATE)   //时间设置
            {
                pMe->m_nCtlID = IDC_AUTO_POWER_STATUS;
            }
            else if(pMe->m_nCtlID == IDC_AUTO_POWER_STATUS)
            {
                ITIMECTL_SetProperties(pMe->m_pTime,
                                       TP_NO_SECONDS | TP_AUTOREDRAW);

                ITIMECTL_SetEditField(pMe->m_pTime, ITF_HOUR); 
                pMe->m_nCtlID = IDC_AUTO_POWER_TIME;
            }
            SettingMenu_Set_CTL(pMe);
            return TRUE;

        case AVK_RIGHT:
        case AVK_LEFT:
            if(pMe->m_nCtlID == IDC_AUTO_POWER_TIME)   //时间控件
            {
                if(pMe->m_nTimeCtlCount == 0)
                {
                    pMe->m_nTimeCtlCount = 1;
                    pMe->m_nNumKeyCount = 2;
                }
                else if(pMe->m_nTimeCtlCount == 1)
                {
                    pMe->m_nTimeCtlCount = 0;
                    pMe->m_nNumKeyCount = 0;
                }
            }
            if(pMe->m_nCtlID == IDC_AUTO_POWER_STATUS) //功能选择控件
            {
                if(IMENUCTL_GetSel(pMe->m_pStatus) == IDS_AUTO_POWER_ON_TIME)
                {
                    ICONFIG_GetItem(pMe->m_pConfig, CFGI_AUTO_POWER_ON, (void*)&pMe->m_ClockCfg, sizeof(Auto_Power_Cfg));
                }
                else if(IMENUCTL_GetSel(pMe->m_pStatus) == IDS_AUTO_POWER_OFF_TIME)
                {
                    IMENUCTL_SetSel(pMe->m_pStatus, IDS_AUTO_POWER_OFF_TIME);
                    ICONFIG_GetItem(pMe->m_pConfig, CFGI_AUTO_POWER_OFF, (void*)&pMe->m_ClockCfg, sizeof(Auto_Power_Cfg));
                }
                //STATE为真时,状态选中"开"                        
    
                if(pMe->m_ClockCfg.bStateOn == TRUE)
                {
                    IMENUCTL_SetSel(pMe->m_pState, IDS_ENABLE);
                }
                else
                {
                    IMENUCTL_SetSel(pMe->m_pState, IDS_DISABLE);
                }
                //记录响闹时间
                pMe->m_dwDispTime =   pMe->m_ClockCfg.dwWATime;
                pMe->m_dwHour = pMe->m_dwDispTime/MSSEC_PER_HOUR;
                pMe->m_dwMin   = pMe->m_dwDispTime%MSSEC_PER_HOUR;
                //设置给定的时间,并刷新屏幕
                (void)ITIMECTL_SetTimeEx(pMe->m_pTime,  pMe->m_dwDispTime, TRUE);                           
            }

            return TRUE;

#if 0
        case AVK_UP:
        case AVK_DOWN:
            //在时间控件激活的情况下,忽略按上下键所设置的时间
            if(pMe->m_nCtlID == IDC_AUTO_POWER_TIME)   //时间控件
            {
                (void)ITIMECTL_SetTime(pMe->m_pTime, pMe->m_dwDispTime);
            }
            return TRUE;
#endif
        default:
            break;
    }
    return TRUE;

}//end CClockApps_HandleKeyEvent
/*==============================================================================
函数：
       Setting_Process_Auto_Power

说明：

参数：

返回值：
       boolean。
       TRUE : cur is ok to set CFGI
       FALSE :cann't  set to CFGI

备注：:

==============================================================================*/
static boolean Setting_Process_Auto_Power(Auto_Power_Cfg *other_set, Auto_Power_Cfg *cur)
{
    if(other_set == NULL || cur == NULL)
    {
        return FALSE;
    }

    if((boolean)other_set->bStateOn == FALSE || (boolean)cur->bStateOn == FALSE)
    {
        return TRUE;
    }
    else
    {
        uint32 time = 0;
        if(other_set->dwWATime > cur->dwWATime)
        {
            time = other_set->dwWATime - cur->dwWATime;
        }
        else
        {
            time = cur->dwWATime - other_set->dwWATime;
        }
        MSG_FATAL("%d %d %d ",time ,other_set->dwWATime, cur->dwWATime);
        if(time >= 60000/*180000 3 min*/) //xxzhen modify power on time and poewr off time ,only 1min
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    
}
/*==============================================================================
函数：
       Setting_CClockApps_HandleCmdEvent

说明：
       处理Clock编辑界面应用COMMAND键及SELECT键保存设置内容.

参数：
       pMe [in]：指向Applet对象结构的指针。该结构包含小程序的特定信息。


返回值：
       boolean。

备注：:

==============================================================================*/
    #include "err.h"

static boolean Setting_CClockApps_HandleCmdEvent(CSettingMenu *pMe)
{
    Auto_Power_Cfg   another_set;
    if (pMe == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("Setting_CClockApps_HandleCmdEvent %d %d %d", pMe->m_ClockCfg.dwWATime, pMe->m_b_selete_Pm, pMe->m_ClockCfg.bStateOn);

    //保存现在设置的时间
#if 0
    if(pMe->m_b_ampm_mode == OEMNV_TIMEFORM_AMPM)
    {
        pMe->m_ClockCfg.dwWATime = pMe->m_ClockCfg.dwWATime + pMe->m_b_selete_Pm *12 *  MSSEC_PER_HOUR;
    }
    else
#endif
    {
    ERR("pMe->m_pStatus= %d pMe->m_ClockCfg.dwWATime=%d",IMENUCTL_GetSel(pMe->m_pState),ITIMECTL_GetTime(pMe->m_pTime),0);
        //保存STATE的值
        if(IMENUCTL_GetSel(pMe->m_pState) == IDS_ENABLE)
        {
            pMe->m_ClockCfg.bStateOn = TRUE;
        }
        else
        {
            pMe->m_ClockCfg.bStateOn = FALSE;
        }
        pMe->m_ClockCfg.dwWATime = ITIMECTL_GetTime(pMe->m_pTime);
    }

    ERR("pMe->m_pStatus= %d",IMENUCTL_GetSel(pMe->m_pStatus),0,0);
    if(IMENUCTL_GetSel(pMe->m_pStatus) == IDS_AUTO_POWER_ON_TIME)
    {
        ICONFIG_GetItem(pMe->m_pConfig, CFGI_AUTO_POWER_OFF, (void*)&another_set, sizeof(Auto_Power_Cfg));
        if(Setting_Process_Auto_Power(&another_set,&pMe->m_ClockCfg) == FALSE)
        {
            pMe->m_msg_id = IDS_AUTOPOWER_WARNING;
            CLOSE_DIALOG(DLGRET_WARNING)
            return FALSE;
        }
        else
        {
            ICONFIG_SetItem(pMe->m_pConfig, CFGI_AUTO_POWER_ON, (void*)&pMe->m_ClockCfg, sizeof(Auto_Power_Cfg));
            OEMRTC_Process_Auto_Power_On();
        }
    }
    else if(IMENUCTL_GetSel(pMe->m_pStatus) == IDS_AUTO_POWER_OFF_TIME)
    {
        ICONFIG_GetItem(pMe->m_pConfig, CFGI_AUTO_POWER_ON, (void*)&another_set, sizeof(Auto_Power_Cfg));
        if(Setting_Process_Auto_Power(&another_set,&pMe->m_ClockCfg) == FALSE)
        {
            pMe->m_msg_id = IDS_AUTOPOWER_WARNING;
            CLOSE_DIALOG(DLGRET_WARNING)
            return FALSE;
        }
        else
        {
            ICONFIG_SetItem(pMe->m_pConfig, CFGI_AUTO_POWER_OFF, (void*)&pMe->m_ClockCfg, sizeof(Auto_Power_Cfg));
	#ifdef FEATURE_VERSION_V3CM301
			OEMRTC_Process_Auto_Power_Down(pMe->m_pShell);
	#else
	#endif
        }
    }
#ifdef SET_REP_MOD_LIST
    //保存重复方式的值
    switch(IMENUCTL_GetSel(pMe->m_pRepMode))
    {
        default:
        case IDL_REP_MODE_0:   //每天
            pMe->m_ClockCfg.RepMode = WEEK_ALARM_REP1;
            break;
        case IDL_REP_MODE_1:   //周一~周五
            pMe->m_ClockCfg.RepMode = WEEK_ALARM_REP2;
            break;
        case IDL_REP_MODE_2:   //周一~周六
            pMe->m_ClockCfg.RepMode = WEEK_ALARM_REP3;
            break;
        case IDL_REP_MODE_3:   //周一
            pMe->m_ClockCfg.RepMode = WEEK_ALARM_REP4;
            break;
        case IDL_REP_MODE_4:   //周二
            pMe->m_ClockCfg.RepMode = WEEK_ALARM_REP5;
            break;
        case IDL_REP_MODE_5:   //周三
            pMe->m_ClockCfg.RepMode = WEEK_ALARM_REP6;
            break;
        case IDL_REP_MODE_6:   //周四
            pMe->m_ClockCfg.RepMode = WEEK_ALARM_REP7;
            break;
        case IDL_REP_MODE_7:   //周五
            pMe->m_ClockCfg.RepMode = WEEK_ALARM_REP8;
            break;
        case IDL_REP_MODE_8:   //周六
            pMe->m_ClockCfg.RepMode = WEEK_ALARM_REP9;
            break;
        case IDL_REP_MODE_9:   //周日
            pMe->m_ClockCfg.RepMode = WEEK_ALARM_REP10;
            break;
    }
#endif
    return TRUE;
}//end CClockApps_HandleCmdEvent
#if 0
/*==============================================================================
函数：
       Setting_CClockApps_HandleNumKeyEvent

说明：
       时间控件下按数字键设置时间

参数：
       pMe [in]：指向Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam [in]: 按键事件.


返回值：
       boolean。

备注：:

==============================================================================*/
static boolean Setting_CClockApps_HandleNumKeyEvent(CSettingMenu *pMe, uint16 wParam)
{
    uint16 nKeyMask = 0;

    if (pMe == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("%x %d Setting_CClockApps_HandleNumKeyEvent", wParam, pMe->m_nNumKeyCount, 0);
    switch(wParam)
    {
        case AVK_0:
            nKeyMask = 0;
            break;

        case AVK_1:
            nKeyMask = 1;
            break;

        case AVK_2:
            nKeyMask = 2;
            break;

        case AVK_3:
            nKeyMask = 3;
            break;

        case AVK_4:
            nKeyMask = 4;
            break;

        case AVK_5:
            nKeyMask = 5;
            break;

        case AVK_6:
            nKeyMask = 6;
            break;

        case AVK_7:
            nKeyMask = 7;
            break;

        case AVK_8:
            nKeyMask = 8;
            break;

        case AVK_9:
            nKeyMask = 9;
            break;

        default:
            break;
    }

    if(pMe->m_nNumKeyCount == 0)    //小时的十位
    {
        if(pMe->m_b_ampm_mode == OEMNV_TIMEFORM_AMPM)
        {
            //小时的十位只有012可以设置,否则设置的时间将超界
            if(nKeyMask == 0 || nKeyMask == 1)
            {
                pMe->m_dwHour = 10* nKeyMask * MSSEC_PER_HOUR;
                pMe->m_dwDispTime = pMe->m_dwHour + pMe->m_dwMin;

                (void)ITIMECTL_SetTime(pMe->m_pTime, pMe->m_dwDispTime);
            }
            else
            {
                return TRUE;
            }
        }
        else
        {
            //小时的十位只有012可以设置,否则设置的时间将超界
            if(nKeyMask == 0 || nKeyMask == 1 || nKeyMask == 2)
            {
                pMe->m_dwHour = 10* nKeyMask * MSSEC_PER_HOUR;
                pMe->m_dwDispTime = pMe->m_dwHour + pMe->m_dwMin;

                (void)ITIMECTL_SetTime(pMe->m_pTime, pMe->m_dwDispTime);
            }
            else
            {
                return TRUE;
            }
        }
    }
    else if(pMe->m_nNumKeyCount == 1)   //小时的个位
    {
        if(pMe->m_b_ampm_mode == OEMNV_TIMEFORM_AMPM)
        {
            if(pMe->m_dwHour >= 10 *  MSSEC_PER_HOUR)//十位数是2时
            {
                if(nKeyMask == 0 || nKeyMask == 1)   //只处理01按键
                {
                    pMe->m_dwHour += nKeyMask * MSSEC_PER_HOUR;
                    pMe->m_dwDispTime = pMe->m_dwHour + pMe->m_dwMin;
                }
                else    //其他按键直接返回
                {
                    return TRUE;
                }
            }
            else    //十位数小于2时
            {
                pMe->m_dwHour += nKeyMask * MSSEC_PER_HOUR;
                pMe->m_dwDispTime = pMe->m_dwHour + pMe->m_dwMin;
            }
        }
        else
        {
            if(pMe->m_dwHour >= 10 * 2 * MSSEC_PER_HOUR)//十位数是2时
            {
                if(nKeyMask == 0 || nKeyMask == 1
                   || nKeyMask == 2 || nKeyMask == 3)   //只处理0123按键
                {
                    pMe->m_dwHour += nKeyMask * MSSEC_PER_HOUR;
                    pMe->m_dwDispTime = pMe->m_dwHour + pMe->m_dwMin;
                }
                else    //其他按键直接返回
                {
                    return TRUE;
                }
            }
            else    //十位数小于2时
            {
                pMe->m_dwHour += nKeyMask * MSSEC_PER_HOUR;
                pMe->m_dwDispTime = pMe->m_dwHour + pMe->m_dwMin;
            }
        }
        (void)ITIMECTL_SetTime(pMe->m_pTime, pMe->m_dwDispTime);

        //编辑区域自动设置为分钟
        ITIMECTL_SetEditField(pMe->m_pTime, ITF_MIN);
        pMe->m_nTimeCtlCount = 1;  //0代表焦点在小时上,1代表在分钟上

        (void)ITIMECTL_Redraw(pMe->m_pTime);
    }
    else if(pMe->m_nNumKeyCount == 2)   //分钟的十位
    {
        //分钟的十位不可以设置6789,否则设置的时间将超界
        if(nKeyMask != 6 && nKeyMask != 7
           && nKeyMask != 8 && nKeyMask != 9)
        {
            pMe->m_dwMin = 10 * nKeyMask * MSSEC_PER_MIN;
            pMe->m_dwDispTime = pMe->m_dwHour + pMe->m_dwMin;
            if(pMe->m_dwDispTime >= (12*MSSEC_PER_HOUR) && pMe->m_b_ampm_mode == OEMNV_TIMEFORM_AMPM)
            {
                pMe->m_dwDispTime = pMe->m_dwDispTime - 12*MSSEC_PER_HOUR;
                pMe->m_b_selete_Pm = TRUE;
            }
            (void)ITIMECTL_SetTime(pMe->m_pTime, pMe->m_dwDispTime);
        }
        else
        {
            return TRUE;
        }
    }
    else if(pMe->m_nNumKeyCount == 3)  //分钟的个位
    {
        pMe->m_dwMin += nKeyMask * MSSEC_PER_MIN;
        pMe->m_dwDispTime = pMe->m_dwHour + pMe->m_dwMin;
        if(pMe->m_dwDispTime >= (12*MSSEC_PER_HOUR) && pMe->m_b_ampm_mode == OEMNV_TIMEFORM_AMPM)
        {
            pMe->m_dwDispTime = pMe->m_dwDispTime - 12*MSSEC_PER_HOUR;
            pMe->m_b_selete_Pm = TRUE;
        }
        (void)ITIMECTL_SetTime(pMe->m_pTime, pMe->m_dwDispTime);

        ITIMECTL_SetActive(pMe->m_pTime, FALSE);
#ifdef SET_REP_MOD_LIST
        //自动设置控件焦点
        IDIALOG_SetFocus(pMe->m_pActiveDlg, IDC_REP_MOD_LIST);
        pMe->m_nCtlID = IDC_REP_MOD_LIST;
#endif
        pMe->m_nNumKeyCount = 0;    //重置m_nNumKeyCount

        return TRUE;    //重置了m_nNumKeyCount,直接返回
    }

    //时间控件按键次数增加
    pMe->m_nNumKeyCount++;

    return TRUE;
}
#endif
#endif

#ifdef FEATURE_CARRIER_THAILAND_HUTCH
static boolean Handle_ANSWER_MODE_DialogEveng(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_ANSWER_MODE);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("%x, %x ,%x,Handle_ANSWER_MODE_DialogEveng",eCode,wParam,dwParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_ANSWER_MODE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_ANYKEY_ANSWER_TITLE, IDS_ANYKEY_ANSWER_TITLE, NULL, 0);
#ifdef FEATRUE_DEVICETYPE_FLIP
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_COVER_ANSWER, IDS_COVER_ANSWER, NULL, 0);
#endif
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SEND_ANSWER, IDS_SEND_ANSWER, NULL, 0);
            //IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_HEADSET_ANSWER, IDS_HEADSET_ANSWER, NULL, 0);
//#ifdef FETRUE_SET_AUTOANSWER
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AUTOANSWER_TITLE, IDS_AUTOANSWER_TITLE, NULL, 0);
//#endif
            return TRUE;

        case EVT_DIALOG_START:
            SettingMenu_SetItemNumIcon(pMenu);
            IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            pMe->m_currDlgId = wParam;
            pMe->m_CallSettingSel = wParam;
            switch (wParam)
            {
                case IDS_ANYKEY_ANSWER_TITLE:
#ifdef FEATRUE_DEVICETYPE_FLIP
                case IDS_COVER_ANSWER:
#endif
                case IDS_SEND_ANSWER:
                case IDS_HEADSET_ANSWER:
                    CLOSE_DIALOG(DLGRET_CALLSETTINGSEL)
                    break;
//#ifdef FETRUE_SET_AUTOANSWER
                case IDS_AUTOANSWER_TITLE:
                    CLOSE_DIALOG(DLGRET_AUTO_ANSWER_MODE_SUB)
                    break;
//#endif
                default:
                    ASSERT_NOT_REACHABLE;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
}
#else //FEATURE_CARRIER_THAILAND_HUTCH
static boolean Handle_ANSWER_MODE_DialogEveng(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_ANSWER_MODE);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("%x, %x ,%x,Handle_ANSWER_MODE_DialogEveng",eCode,wParam,dwParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_ANSWER_MODE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SEND_ANSWER, IDS_SEND_ANSWER, NULL, 0);            
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_ANYKEY_ANSWER_TITLE, IDS_ANYKEY_ANSWER_TITLE, NULL, 0);
            #if !defined(FEATURE_VERSION_K212_HUALU)
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AUTOANSWER_TITLE, IDS_AUTOANSWER_TITLE, NULL, 0);
            #endif
            return TRUE;

        case EVT_DIALOG_START:
            {
                uint16    ui16_return = IDS_SEND_ANSWER;
                byte      byte_return, autoanswer;
                ICONFIG_GetItem(pMe->m_pConfig, CFGI_HEADSET_AUTOANSWER,&autoanswer,sizeof(autoanswer));
                if(autoanswer == OEMNV_HEADSET_AUTOANSWER_OFF)
                {
                    ICONFIG_GetItem(pMe->m_pConfig,CFGI_ANYKEY_ANSWER,&byte_return,sizeof(byte_return));
                    switch (byte_return)
                    {
                        case OEM_ANYKEY_ANSWER_MODE:  
                          ui16_return = IDS_ANYKEY_ANSWER_TITLE;
                          break;
                          
                        default:
                        case OEM_AUTO_ANSWER_MODE: 
                          ui16_return = IDS_SEND_ANSWER;
                          break;
                    }
                }
                else
                {
                    #if !defined(FEATURE_VERSION_K212_HUALU)
                    ui16_return = IDS_AUTOANSWER_TITLE;
                    #else
                    ui16_return = IDS_ANYKEY_ANSWER_TITLE;
                    #endif
                }

                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, ui16_return, TRUE);
                IMENUCTL_SetSel(pMenu, ui16_return);
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            }            
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {             
                byte   callset;
                byte   autoanswertype;
                ICONFIG_GetItem(pMe->m_pConfig, CFGI_ANYKEY_ANSWER, &callset,sizeof(callset));
                callset = callset & 0x0;

                switch (wParam)
                {
                    case IDS_ANYKEY_ANSWER_TITLE:                            
                        callset = callset | 0x1;
                        autoanswertype = OEMNV_HEADSET_AUTOANSWER_OFF;
                        break;
                        
                    default:
                    case IDS_SEND_ANSWER:  
                        callset = callset | 0x4;
                        autoanswertype = OEMNV_HEADSET_AUTOANSWER_OFF;
                        break;

                    case IDS_AUTOANSWER_TITLE:
                        callset = callset | 0x8;
                        autoanswertype = OEMNV_HEADSET_AUTOANSWER_5SEC;
                        break;
                        
                }
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wParam, TRUE);
                ICONFIG_SetItem(pMe->m_pConfig,CFGI_ANYKEY_ANSWER, &callset,sizeof(callset));
                ICONFIG_SetItem(pMe->m_pConfig,CFGI_HEADSET_AUTOANSWER, &autoanswertype,sizeof(autoanswertype));
                CLOSE_DIALOG(DLGRET_WARNING)
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
}
#endif //FEATURE_CARRIER_THAILAND_HUTCH


#ifdef FEATURE_INTERNATIONAL_PREFIX
static boolean Handle_Prefix_DialogEveng(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_PREFIX);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("%x, %x ,%x,Handle_ANSWER_MODE_DialogEveng",eCode,wParam,dwParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_PREFIX,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_PREFIX_MANUAL, IDS_PREFIX_MANUAL, NULL, 0);            
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_PREFIX_AUTO, IDS_PREFIX_AUTO, NULL, 0);            
            return TRUE;

        case EVT_DIALOG_START:
            {
                uint16    ui16_return = IDS_PREFIX_MANUAL;
              
                AECHAR    wstr[FEATURE_CODE_MAX_LENTH];
                boolean   b_Prefix = FALSE;
                ICONFIG_GetItem(pMe->m_pConfig, CFGI_PREFIX_AUTO_MANUAL, &b_Prefix,  sizeof(boolean));
                if(b_Prefix)
                {
                    ui16_return = IDS_PREFIX_AUTO;                  
                }
                else
                {
                    ui16_return = IDS_PREFIX_MANUAL;                   
                }

                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, ui16_return, TRUE);
                IMENUCTL_SetSel(pMenu, ui16_return);
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            }            
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {             
                AECHAR wstr[FEATURE_CODE_MAX_LENTH];    

                switch (wParam)
                {                      
                    case IDS_PREFIX_MANUAL:
                        {
                            boolean b_Prefix = FALSE;
                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_PREFIX_AUTO_MANUAL, &b_Prefix,  sizeof(boolean));
                            CLOSE_DIALOG(DLGRET_PREFIX_EDIT)
                        }
                       return TRUE;

                    case IDS_PREFIX_AUTO:
                        {
                            boolean b_Prefix = TRUE;
                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_PREFIX_AUTO_MANUAL, &b_Prefix,  sizeof(boolean));
                        }
                        break;
                    default:    
                        break;                           
                }
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wParam, TRUE);
                CLOSE_DIALOG(DLGRET_WARNING)
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
}

static boolean  Handle_PrefixEdit_DialogEveng(CSettingMenu *pMe,
                                               AEEEvent       eCode,
                                               uint16         wParam,
                                               uint32         dwParam)
{
    ITextCtl *pTextCtl = NULL;
    IMenuCtl *pMenuCtl = NULL;
    
    if (NULL == pMe)
    {
        return FALSE;
    }

    pTextCtl = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_PREFIX_EDIT);
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_PREFIX_EDIT_MENU);
    
    if (NULL == pTextCtl)
    {
        return FALSE;
    }

    switch(eCode)
    {
        case EVT_DIALOG_INIT:
            ITEXTCTL_SetProperties(pTextCtl, TP_FRAME | TP_NOUPDATE|TP_FOCUS_NOSEL);
            return TRUE;

        case EVT_DIALOG_START:
        {
            AEERect ctlRect;
            AEERect menuRt;
            AECHAR  WTitle[40] = {0};
            AECHAR  wstr[FEATURE_CODE_MAX_LENTH];
            
            (void)ISHELL_LoadResString(pMe->m_pShell,
                                      AEE_APPSSETTINGMENU_RES_FILE,                                
                                      IDS_PREFIX_MANUAL,
                                      WTitle,
                                      sizeof(WTitle));
            if(pMe->m_pAnn != NULL)
            {
                IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }

            SETAEERECT(&menuRt,  0, MENUITEM_HEIGHT,
                        SCREEN_WIDTH,
                        (SCREEN_HEIGHT-STATEBAR_HEIGHT-BOTTOMBAR_HEIGHT-MENUITEM_HEIGHT));     

            IMENUCTL_SetRect(pMenuCtl, &menuRt);
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_GRAPHIC_BG|OEMMP_USE_MENU_STYLE);

            
            SETAEERECT(&ctlRect,  0, 0,
                        SCREEN_WIDTH,
                        MENUITEM_HEIGHT);    //pMe->m_rc.dy - GetBottomBarHeight(pMe->m_pDisplay));     
                      
            ICONTROL_SetRect((IControl*)pTextCtl, &ctlRect);
            ITEXTCTL_SetMaxSize(pTextCtl, FEATURE_CODE_MAX_LENTH);              
            (void) ITEXTCTL_SetInputMode(pTextCtl, AEE_TM_NUMBERS);   
            
            ICONFIG_GetItem(pMe->m_pConfig, CFGI_PREFIX, wstr, FEATURE_CODE_MAX_LENTH);
            
            if (WSTRLEN(wstr) > 0)
            {
                ITEXTCTL_SetText(pTextCtl, wstr, -1);
            }

            ITEXTCTL_SetActive(pTextCtl,TRUE);        
            IMENUCTL_SetActive(pMenuCtl,FALSE);
            
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                        EVTFLG_ASYNC,
                                        AEECLSID_APP_SETTINGMENU,
                                        EVT_USER_REDRAW,
                                        0, 
                                        0);
            return TRUE;
        }
        case EVT_USER_REDRAW:
        case EVT_KEY_RELEASE:
            // 绘制底条提示
            {
                AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pTextCtl);
                int nLen = 0;
                
                if (NULL != pwstrText)
                {
                    nLen = WSTRLEN(pwstrText);
                }

                (void)ITEXTCTL_Redraw(pTextCtl);

                ITEXTCTL_SetActive(pTextCtl,TRUE);
                IMENUCTL_SetActive(pMenuCtl,FALSE);                
                
                if (nLen > 0)
                {
                	#ifndef FEATURE_ALL_KEY_PAD
                    if (ITEXTCTL_GetCursorPos(pTextCtl) != TC_CURSORSTART)
                    {
                        SETTING_MENU_DRAW_BOTTOMBAR(BTBAR_SAVE_DELETE)
                    }
                    else
                    #endif
                    {
                        SETTING_MENU_DRAW_BOTTOMBAR(BTBAR_SAVE_BACK)
                    }
                }
                else
                {
                   SETTING_MENU_DRAW_BOTTOMBAR(BTBAR_BACK)
                }
            }
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            
            return TRUE;
            
        case EVT_DIALOG_END:            
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                case AVK_INFO:
                case AVK_SELECT:
                    { 
                        if (WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) == 0) 
                        {
                            return TRUE;
                        }   
                       
                       (void) ICONFIG_SetItem(pMe->m_pConfig,
                                              CFGI_PREFIX,
                                              ITEXTCTL_GetTextPtr(pTextCtl),
                                              FEATURE_CODE_MAX_LENTH);                       
                    }
      
                    CLOSE_DIALOG(DLGRET_WARNING)
                    return TRUE;
  
                default:
                    break;
            }
            return TRUE;
  
        default:
            break;
    }
}

#endif

#if defined(FEATURE_VERSION_K232_Y101)||defined(FEATURE_VERSION_K232_Y105A) ||defined(FEATURE_VERSION_K232_Y100A)  
extern char charsvc_p_name[UIM_CDMA_HOME_SERVICE_SIZE+1];
#endif

static void SettingMenu_Process_Feature_Code(CSettingMenu *pMe,uint16 feature_code)
{
    MSG_FATAL("SettingMenu_Process_Feature_Code Start %d",feature_code,0,0);
    if(NULL== pMe)
    {
        return;
    }
    
    pMe->m_callnumber[0]= 0;
    
    if(IsRunAsUIMVersion())
    {
        uint16 Ruim_Active_code    = 0;
        uint16 Ruim_Register_code = 0;
        char  Assnum[18];
        switch(feature_code)
        {
            case CFGI_CALLFORWARD_DISABLE_ALL:
                Ruim_Active_code    = DE_ACTIVE_CFU;
                Ruim_Register_code  = DE_REGISTER_CFU;
                break;

            case CFGI_CALLFORWARD_BUSY_ENABLE:
                Ruim_Active_code    = ACTIVE_CFB;
                Ruim_Register_code  = REGISTER_CFB;
                break;

            case CFGI_CALLFORWARD_BUSY_DISABLE:
                Ruim_Active_code    = DE_ACTIVE_CFB;
                Ruim_Register_code  = DE_REGISTER_CFB;
                break;

            case CFGI_CALLFORWARD_NOANSWER_ENABLE:
                Ruim_Active_code    = ACTIVE_CFNA;
                Ruim_Register_code  = REGISTER_CFNA;
                break;

            case CFGI_CALLFORWARD_NOANSWER_DISABLE:
                Ruim_Active_code    = DE_ACTIVE_CFNA;
                Ruim_Register_code  = DE_REGISTER_CFNA;
                break;
                
            case CFGI_CALLFORWARD_UNCONDITIONAL_ENABLE:
                Ruim_Active_code    = ACTIVE_CFU;
                Ruim_Register_code  = REGISTER_CFU;
                break;

            case CFGI_CALLFORWARD_UNCONDITIONAL_DISABLE:
                Ruim_Active_code    = DE_ACTIVE_CFU;
                Ruim_Register_code  = DE_REGISTER_CFU;
                break;

            case CFGI_CALLFORWARD_UNREACHABLE_ENABLE:
                Ruim_Active_code    = ACTIVE_CFD;
                Ruim_Register_code  = REGISTER_CFD;
                break;

            case CFGI_CALLFORWARD_UNREACHABLE_DISABLE:
                Ruim_Active_code    = DE_ACTIVE_CFD;
                Ruim_Register_code  = DE_REGISTER_CFD;
                break;

            case CFGI_CALLFORWARD_WAIT_ENABLE:
                Ruim_Active_code     = ACTIVE_CALLWAITING;
                Ruim_Register_code   = ACTIVE_CALLWAITING;
                break;

            case CFGI_CALLFORWARD_WAIT_DISABLE:
                Ruim_Active_code    = DE_ACTIVE_CALLWAITING;
                Ruim_Register_code  = DE_ACTIVE_CALLWAITING;              
                break;

            default:
                Ruim_Active_code    = DE_ACTIVE_CFU;
                Ruim_Register_code  = DE_REGISTER_CFU;
                break;
        }
        
        //Read supplement service number from RUIM Active section, if read fail or the number is wrong, then from RUIM Register section or config file
        MEMSET(Assnum, 0x00, sizeof(Assnum));        
        
        if( SUCCESS == IRUIM_Get_Feature_Code(pMe->m_pIRUIM,(byte*)Assnum, Ruim_Active_code) )     
        {
            MSG_FATAL("IRUIM_Get_Feature_Code == %d %d",Ruim_Active_code,STRLEN(Assnum),0);

            DBGPRINTF("IRUIM_Get_Feature_Code 111 Assnum == %s", Assnum);
            
            //if the supplement service number is "**"(Wrong number), then read supplement service number from RUIM Register section or config file
            if ( STRNCMP(Assnum,"**",2) && STRNCMP(Assnum,"0000",4))
            {
                MEMCPY(pMe->m_callnumber, Assnum, sizeof(Assnum));
                return;
            }
        }
        
        if( SUCCESS == IRUIM_Get_Feature_Code(pMe->m_pIRUIM,(byte*)Assnum, Ruim_Register_code))
        {
            MSG_FATAL("IRUIM_Get_Feature_Code == %d %d",Ruim_Active_code,STRLEN(Assnum),0);

            DBGPRINTF("IRUIM_Get_Feature_Code 222 Assnum == %s", Assnum);
            
            //if the supplement service number is "**"(Wrong number), then read supplement service number from RUIM Register section or config file
            if ( STRNCMP(Assnum,"**",2) && STRNCMP(Assnum,"0000",4))
            {
                MEMCPY(pMe->m_callnumber, Assnum, sizeof(Assnum));
                return;
            }
        }                
        
        DBGPRINTF("SettingMenu_Process_Feature_Code STRLEN(pMe->m_callnumber)=%d",STRLEN(pMe->m_callnumber));

        DBGPRINTF("SettingMenu_Process_Feature_Code feature_code=%d",feature_code);        

        DBGPRINTF("SettingMenu_Process_Feature_Code charsvc_p_name=%s",charsvc_p_name);
#if defined(FEATURE_VERSION_K232_Y101)||defined(FEATURE_VERSION_K232_Y105A)    
        if (STRLEN(pMe->m_callnumber) == 0) // did not read the code from the card.
        {            
            if (feature_code == CFGI_CALLFORWARD_UNCONDITIONAL_DISABLE)
            {                
                if((STRISTR (charsvc_p_name,"Reliance"))||(STRISTR (charsvc_p_name,"reliance")))
            	{
        			STRCPY(pMe->m_callnumber, "*302");
            	}	
                else if ((STRISTR (charsvc_p_name,"Virgin"))||(STRISTR (charsvc_p_name,"virgin")))
                {
                    STRCPY(pMe->m_callnumber, "*73");
            	}
                else if ((STRISTR (charsvc_p_name,"MTS"))||(STRISTR (charsvc_p_name,"mts"))||(STRISTR (charsvc_p_name,"Mts")))
                {
                    STRCPY(pMe->m_callnumber, "*115");
            	}
                else if ((STRISTR (charsvc_p_name,"Tata"))||(STRISTR (charsvc_p_name,"tata")))
                {
                    STRCPY(pMe->m_callnumber, "*73");
            	}
                else
                {
                    STRCPY(pMe->m_callnumber, "*115");
                }                
            }

            
            if (feature_code == CFGI_CALLFORWARD_DISABLE_ALL)
            {
                if((STRISTR (charsvc_p_name,"Reliance"))||(STRISTR (charsvc_p_name,"reliance")))
            	{
        			STRCPY(pMe->m_callnumber, "*308");
            	}	
                else if ((STRISTR (charsvc_p_name,"Virgin"))||(STRISTR (charsvc_p_name,"virgin")))
                {
        			STRCPY(pMe->m_callnumber, "*73");
            	}
                else if ((STRISTR (charsvc_p_name,"MTS"))||(STRISTR (charsvc_p_name,"mts"))||(STRISTR (charsvc_p_name,"Mts")))
                {
        			STRCPY(pMe->m_callnumber, "*115");
            	}                
                else
                {
                    STRCPY(pMe->m_callnumber, "*115");
                }
            }
           
            if (feature_code == CFGI_CALLFORWARD_BUSY_ENABLE)
            {
                if((STRISTR (charsvc_p_name,"Reliance"))||(STRISTR (charsvc_p_name,"reliance")))
            	{
        			STRCPY(pMe->m_callnumber, "*305");
            	}	
                else if ((STRISTR (charsvc_p_name,"Virgin"))||(STRISTR (charsvc_p_name,"virgin")))
                {
        			STRCPY(pMe->m_callnumber, "*75");
            	}
                else if ((STRISTR (charsvc_p_name,"MTS"))||(STRISTR (charsvc_p_name,"mts"))||(STRISTR (charsvc_p_name,"Mts")))
                {
        			STRCPY(pMe->m_callnumber, "*136");
            	}
                else if ((STRISTR (charsvc_p_name,"Tata"))||(STRISTR (charsvc_p_name,"tata")))
                {
        			STRCPY(pMe->m_callnumber, "*75");
            	}
                else
                {
                    STRCPY(pMe->m_callnumber, "*136");
                }
            }

            if (feature_code == CFGI_CALLFORWARD_BUSY_DISABLE)
            {
                if((STRISTR (charsvc_p_name,"Reliance"))||(STRISTR (charsvc_p_name,"reliance")))
            	{
        			STRCPY(pMe->m_callnumber, "*306");
            	}	
                else if ((STRISTR (charsvc_p_name,"Virgin"))||(STRISTR (charsvc_p_name,"virgin")))
                {
        			STRCPY(pMe->m_callnumber, "*65");
            	}
                else if ((STRISTR (charsvc_p_name,"MTS"))||(STRISTR (charsvc_p_name,"mts"))||(STRISTR (charsvc_p_name,"Mts")))
                {
        			STRCPY(pMe->m_callnumber, "*128");
            	}
                else if ((STRISTR (charsvc_p_name,"Tata"))||(STRISTR (charsvc_p_name,"tata")))
                {
        			STRCPY(pMe->m_callnumber, "*65");
            	}
                else
                {
                    STRCPY(pMe->m_callnumber, "*128");
                }
            }

          
            if (feature_code == CFGI_CALLFORWARD_NOANSWER_ENABLE)
            {
                if((STRISTR (charsvc_p_name,"Reliance"))||(STRISTR (charsvc_p_name,"reliance")))
            	{
        			STRCPY(pMe->m_callnumber, "*303");
            	}	
                else if ((STRISTR (charsvc_p_name,"Virgin"))||(STRISTR (charsvc_p_name,"virgin")))
                {
        			STRCPY(pMe->m_callnumber, "*74");
            	}
                else if ((STRISTR (charsvc_p_name,"MTS"))||(STRISTR (charsvc_p_name,"mts"))||(STRISTR (charsvc_p_name,"Mts")))
                {
        			STRCPY(pMe->m_callnumber, "*138");
            	}
                else if ((STRISTR (charsvc_p_name,"Tata"))||(STRISTR (charsvc_p_name,"tata")))
                {
        			STRCPY(pMe->m_callnumber, "*73");
            	}
                else
                {
                    STRCPY(pMe->m_callnumber, "*138");
                }
            }

            if (feature_code == CFGI_CALLFORWARD_NOANSWER_DISABLE)
            {
                if((STRISTR (charsvc_p_name,"Reliance"))||(STRISTR (charsvc_p_name,"reliance")))
            	{
        			STRCPY(pMe->m_callnumber, "*304");
            	}	
                else if ((STRISTR (charsvc_p_name,"Virgin"))||(STRISTR (charsvc_p_name,"virgin")))
                {
        			STRCPY(pMe->m_callnumber, "*67");
            	}
                else if ((STRISTR (charsvc_p_name,"MTS"))||(STRISTR (charsvc_p_name,"mts"))||(STRISTR (charsvc_p_name,"Mts")))
                {
        			STRCPY(pMe->m_callnumber, "*130");
            	}
                else if ((STRISTR (charsvc_p_name,"Tata"))||(STRISTR (charsvc_p_name,"tata")))
                {
        			STRCPY(pMe->m_callnumber, "*67");
            	}
                else
                {
                    STRCPY(pMe->m_callnumber, "*130");
                }
            }

           
            if (feature_code == CFGI_CALLFORWARD_UNCONDITIONAL_ENABLE)
            {
                if((STRISTR (charsvc_p_name,"Reliance"))||(STRISTR (charsvc_p_name,"reliance")))
            	{
        			STRCPY(pMe->m_callnumber, "*301");
            	}	
                else if ((STRISTR (charsvc_p_name,"Virgin"))||(STRISTR (charsvc_p_name,"virgin")))
                {
        			STRCPY(pMe->m_callnumber, "*72");
            	}
                else if ((STRISTR (charsvc_p_name,"MTS"))||(STRISTR (charsvc_p_name,"mts"))||(STRISTR (charsvc_p_name,"Mts")))
                {
        			STRCPY(pMe->m_callnumber, "*114");
            	}
                else if ((STRISTR (charsvc_p_name,"Tata"))||(STRISTR (charsvc_p_name,"tata")))
                {
        			STRCPY(pMe->m_callnumber, "*72");
            	}
                else
                {
                    STRCPY(pMe->m_callnumber, "*114");
                }
            }

            if (feature_code == CFGI_CALLFORWARD_UNREACHABLE_ENABLE)
            {
                STRCPY(pMe->m_callnumber, "*74");                
            }

            if (feature_code == CFGI_CALLFORWARD_UNREACHABLE_DISABLE)
            {
                STRCPY(pMe->m_callnumber, "*67");  
            }

            if (feature_code == CFGI_CALLFORWARD_WAIT_ENABLE)
            {
                if((STRISTR (charsvc_p_name,"Reliance"))||(STRISTR (charsvc_p_name,"reliance")))
            	{
        			STRCPY(pMe->m_callnumber, "*311");
            	}	                
                else
                {
                    STRCPY(pMe->m_callnumber, "*118");
                }
            }

            if (feature_code == CFGI_CALLFORWARD_WAIT_DISABLE)
            {
                if((STRISTR (charsvc_p_name,"Reliance"))||(STRISTR (charsvc_p_name,"reliance")))
            	{
        			STRCPY(pMe->m_callnumber, "*312");
            	}	                
                else
                {
                    STRCPY(pMe->m_callnumber, "*119");
                }
            }            
        }           
           
#endif

        MSG_FATAL("IRUIM_Get_Feature_Code == %d %d",feature_code,STRLEN(pMe->m_callnumber),0);
        
        DBGPRINTF("IRUIM_Get_Feature_Code m_callnumber == %s",pMe->m_callnumber);
    }
#ifndef FEATURE_CALL_FORWARD_USER_INPUT    
#ifndef FEATURE_OEMOMH
    //Read supplement service number from Config
    ICONFIG_GetItem(pMe->m_pConfig, feature_code, pMe->m_callnumber, FEATURE_CODE_MAX_LENTH);
#endif
#endif/*FEATURE_CALL_FORWARD_USER_INPUT*/
}

static void Sound_App_Add_Menu(IMenuCtl *pMenu,uint16 id)
{
    IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, id, id, NULL, 0);
}


/*==============================================================================
函数：
       HandleFMModeDialogEvent
说明：
       IDD_FMMODE_MENU对话框事件处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/

static boolean  HandleFMModeDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
	PARAM_NOT_REF(dwParam)
    
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,  IDC_MENU_FM);
    MSG_FATAL("HandleFMModeDialogEventeCode=%d,wParam=%d",eCode,wParam,0);                                      
    if (pMenu == NULL)
    {
        return FALSE;
    }
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            pMe->m_bAppIsReady = TRUE;
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_FMRADIO_PLAY_HANDSET, IDS_FMRADIO_PLAY_HANDSET, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_RADIO_PLAY_SPEAKER, IDS_RADIO_PLAY_SPEAKER, NULL, 0);
            return TRUE;

        case EVT_DIALOG_START:
            {
                boolean FMPlaymode = FALSE;
                uint16 ui16_return = IDS_FMRADIO_PLAY_HANDSET;
               	OEM_GetConfig(CFGI_FM_PLAY_MODE,&FMPlaymode, sizeof(FMPlaymode));
               	if(FMPlaymode)
               	{
               		ui16_return = IDS_RADIO_PLAY_SPEAKER;
               	}
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, ui16_return, TRUE);
                IMENUCTL_SetSel(pMenu, ui16_return);
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetOemProperties( pMenu , OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                #if 0
                (void)IMENUCTL_SetTitle(pMenu,
                                        AEE_APPSSOUNDMENU_RES_FILE,
                                        pMe->m_wResID,
                                        NULL);
				#else
			    {
			  	   /*	AECHAR WTitle[40] = {0};
					(void)ISHELL_LoadResString(pMe->m_pShell,
	                        AEE_APPSSETTINGMENU_RES_FILE,                                
	                        pMe->m_wResID,
	                        WTitle,
	                        sizeof(WTitle));
					IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                    */
			    }
			    #endif
            }
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {
                
                boolean FMPlaymode = FALSE;

                switch (wParam)
                {
                    case IDS_FMRADIO_PLAY_HANDSET:      //关
                         FMPlaymode = FALSE;
                         break;

                    case IDS_RADIO_PLAY_SPEAKER:      //开
                         FMPlaymode = TRUE;
                         break;

                    default:
                        ASSERT_NOT_REACHABLE;
                }
                OEM_SetConfig(CFGI_FM_PLAY_MODE,&FMPlaymode, sizeof(FMPlaymode));
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wParam, TRUE);
                CLOSE_DIALOG(DLGRET_WARNING)

            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
}


#ifdef FEATRUE_SET_IP_NUMBER
static boolean Handle_IP_Number_Set_DialogEveng(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_IP_NUMBER_SET);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    MSG_FATAL("%x, %x ,%x,Handle_IP_Number_Set_DialogEveng",eCode,wParam,dwParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        {
            IP_Number_Cfg  ip_cfg;
            MEMSET((void*)&ip_cfg,0 , sizeof(IP_Number_Cfg));

            ICONFIG_GetItem(pMe->m_pConfig,CFGI_IP_NUMBER,(void *)&ip_cfg,sizeof(IP_Number_Cfg));
            return TRUE;
        }

        case EVT_DIALOG_START:
			#if 0
            IMENUCTL_SetTitle(pMenu,AEE_APPSSETTINGMENU_RES_FILE,IDS_IP_NUMBER_SET, NULL);
			#else
		    {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_IP_NUMBER_SET,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
		    }
		    #endif
            ISHELL_PostEvent( pMe->m_pShell,AEECLSID_APP_SETTINGMENU,EVT_USER_REDRAW, 0,0);
            return TRUE;

        case EVT_USER_REDRAW:
        {
            number_table * p_head = NULL;
            uint16 i = 0;
            boolean b_default_ip = TRUE;
            p_head = pMe->ip_number_tab.head;
            IMENUCTL_DeleteAll(pMenu);
#ifdef FEATURE_CARRIER_TAIWAN_APBW
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SET_POUND, IDS_SET_POUND, NULL, 0);
#endif
            if(pMe->ip_number_tab.count < OEM_IP_NUMBER_COUNT)
            {
                IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_ADD_NEW_ITEM, IDS_ADD_NEW_ITEM, NULL, 0);
                IMENUCTL_SetSel(pMenu,IDS_ADD_NEW_ITEM);
            }
            else
            {
                IMENUCTL_SetSel(pMenu,0);
            }

            while(p_head)
            {
                CtlAddItem ai;
                MEMSET(&ai, 0, sizeof(ai));
                ai.pText = p_head->number;
                ai.wItemID = i;
                ai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
                if((b_default_ip == TRUE) &&(WSTRNCMP(pMe->ip_number_tab.default_ip,p_head->number,OEM_IP_NUMBER_MAX+1) == 0)) //only set first to default IP 
                {
                    b_default_ip = FALSE;
                    ai.wImage = IDB_CHECK_ON;
                }
                else
                {
                    ai.wImage = IDB_CHECK_OFF;
                }
                
                IMENUCTL_AddItemEx(pMenu, &ai);
                i ++;
                p_head = p_head->next;
            }

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            // 统一更新界面
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            IMENUCTL_Redraw(pMenu);
            return TRUE;
        }

        case EVT_DIALOG_END:
            if((pMe->ip_number_tab.head != NULL) && (WSTRLEN(pMe->ip_number_tab.default_ip) == 0))//not set default ip,set the first to 
            {
                WSTRLCPY(pMe->ip_number_tab.default_ip,pMe->ip_number_tab.head->number,OEM_IP_NUMBER_MAX+1);
            }
            return TRUE;

        case EVT_CTL_SEL_CHANGED:
            if(wParam == IDS_ADD_NEW_ITEM)
#ifdef FEATURE_CARRIER_TAIWAN_APBW
                || wParam == IDS_SET_POUND
#endif
                )
            {
                SETTING_MENU_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
            }
            else
            {
                SETTING_MENU_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
            }
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            switch (wParam)
            {
#ifdef FEATURE_CARRIER_TAIWAN_APBW
                case IDS_SET_POUND:
                    pMe->m_CallSettingSel =  IDS_SET_POUND;
                    CLOSE_DIALOG(DLGRET_CALLSETTINGSEL)
                    return TRUE;
#endif
                case IDS_ADD_NEW_ITEM:
                    pMe->m_input_mode = IDS_IP_NUMBER_SET;
                    CLOSE_DIALOG(DLGRET_CALLFORWARDINPUT)
                    return TRUE;

                default://del the wParam item
                {
#if 1
                    pMe->m_CallSettingSel =  IDS_SET_IP_CALL_OPTION;
                    pMe->m_ip_sel_num = wParam;
                    CLOSE_DIALOG(DLGRET_CALLSETTINGSEL)
#else
                    uint16 i = 0;
                    if(wParam == 0) //del the first node
                    {
                        if(pMe->ip_number_tab.head)
                        {
                            number_table * phead = NULL;
                            phead = pMe->ip_number_tab.head;
                            pMe->ip_number_tab.head = pMe->ip_number_tab.head->next;
                            FREE(phead);
                        }
                    }
                    else
                    {
                        number_table * phead = pMe->ip_number_tab.head;
                        while(phead)
                        {
                            if( i == (wParam -1))//find the node,del the next node
                            {
                                number_table * temp = NULL;
                                temp = phead->next;
                                if(temp)
                                {
                                    phead->next = temp->next;
                                    FREE(temp);
                                    break;
                                }
                            }
                            phead = phead->next;
                            i++;
                        }
                    }
                    pMe->ip_number_tab.count --;
                    if(pMe->ip_number_tab.count == 0)
                    {
                        pMe->ip_number_tab.ip_state =0;
                    }
                    ISHELL_PostEvent( pMe->m_pShell,AEECLSID_APP_SETTINGMENU,EVT_USER_REDRAW, 0,0);
                    break;
#endif
                }
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
}
static number_table * SettingMenu_Search_Node(Ip_number_table *table,uint16 id)
{
    if(table)
    {
        uint16 i = 0;
        if(id == 0) // the first node
        {
            return table->head;
        }
        else
        {
            number_table * phead = table->head;
            while(phead)
            {
                if( i == (id -1))//find the node,del the next node
                {
                    number_table * temp = NULL;
                    temp = phead->next;
                    return temp;
                }
                phead = phead->next;
                i++;
            }
        }
    }
    return NULL;
}
#endif /* FEATRUE_SET_IP_NUMBER*/

/*void SettingMenu_show_Mem(CSettingMenu *pMe)
{
    IHeap    *heap = NULL;
    uint32     memsize= 0;
    AEEDeviceInfo  di;
    ERR("SettingMenu_show_Mem",0,0,0);
    ISHELL_GetDeviceInfo(pMe->m_pShell, &di);
    (void)ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_HEAP, (void **)&heap);

    if (NULL == heap)
    {
        ERR("MP: Error: Failed to create IHeap instance",0,0,0);
        return;
    }
    memsize = IHEAP_GetMemStats(heap);
    ERR("IHEAP_GetMemStats %d total = %d",memsize,di.dwRAM,0);
    IHEAP_Release(heap);
    heap = NULL;
}*/

/*==============================================================================
函数：
       SettingMenu_HandleKeyToneLengthDialogEvent
说明：
       IDD_KEYTONE_LENGTH对话框事件处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  SettingMenu_HandleKeyToneLengthDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_KEYTONE_LENGTH);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("%x,%x,%x,HandleKeyToneLengthDialogEvent",eCode,wParam,dwParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_KEYTONE_LENGTH,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_KEYTONE_NORMAL, IDS_KEYTONE_NORMAL, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_KEYTONE_LONG, IDS_KEYTONE_LONG, NULL, 0);
            return TRUE;

        case EVT_DIALOG_START:
            {
                uint16     ui16_return = IDS_KEYTONE_NORMAL;
                byte       Keylength;
                ICONFIG_GetItem(pMe->m_pConfig,CFGI_KEYTONE_LENGTH,&Keylength,sizeof(Keylength));
                switch(Keylength)
                {
                    case OEMNV_KEYTONE_NORMAL:            //NORMAL
                    default:
                        ui16_return = IDS_KEYTONE_NORMAL;
                        break;

                    case OEMNV_KEYTONE_LONG:             //LONG
                        ui16_return = IDS_KEYTONE_LONG;
                        break;

                }
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, ui16_return, TRUE);
                IMENUCTL_SetSel(pMenu, ui16_return);
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                ISHELL_PostEvent( pMe->m_pShell,AEECLSID_APP_SETTINGMENU,EVT_USER_REDRAW,0,0);
            }
            return TRUE;

        case EVT_USER_REDRAW:
            // 统一更新界面
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            //(void)IMENUCTL_Redraw(pMenu);   //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
        {
            byte byte_set;
            switch (wParam)
            {
                case IDS_KEYTONE_NORMAL:      //NORMAL
                    byte_set = OEMNV_KEYTONE_NORMAL;
                    break;

                case IDS_KEYTONE_LONG:        //LONG
                    byte_set = OEMNV_KEYTONE_LONG;
                    break;

                default:
                    ASSERT_NOT_REACHABLE;
            }

            ICONFIG_SetItem(pMe->m_pConfig,CFGI_KEYTONE_LENGTH,&byte_set,sizeof(byte_set));
            CLOSE_DIALOG(DLGRET_CANCELED)
            return TRUE;
        }
        default:
            break;
    }
    return FALSE;
} // HandleKeyToneLengthDialogEvent



/*==============================================================================
函数：
       HandleAutoAnswerSubDialogEvent
说明：
       IDD_AUTOANSWER_SUB对话框事件处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleAutoAnswerSubDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    IMenuCtl  *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                     IDC_AUTOANSWER_SUB);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("%x,%x,%x,HandleAutoAnswerSubDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    //SoundMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_AUTOANSWER_TITLE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_OFF, IDS_OFF, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AUTOANSWER_5SEC, IDS_AUTOANSWER_5SEC, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_AUTOANSWER_10SEC, IDS_AUTOANSWER_10SEC, NULL, 0);
            return TRUE;

        case EVT_DIALOG_START:
            {
                uint16    ui16_return = IDS_OFF;
                byte      autoanswertype;
                ICONFIG_GetItem(pMe->m_pConfig, CFGI_HEADSET_AUTOANSWER,&autoanswertype,sizeof(autoanswertype));
                switch (autoanswertype)
                {
                    default:
                    case OEMNV_HEADSET_AUTOANSWER_OFF:  //关
                      ui16_return = IDS_OFF;
                      break;

                    case OEMNV_HEADSET_AUTOANSWER_5SEC: //5秒
                      ui16_return = IDS_AUTOANSWER_5SEC;
                      break;

                    case OEMNV_HEADSET_AUTOANSWER_10SEC://10秒
                      ui16_return = IDS_AUTOANSWER_10SEC;
                      break;
                }

                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, ui16_return, TRUE);
                IMENUCTL_SetSel(pMenu, ui16_return);
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            }
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {
                byte   autoanswertype;
                ICONFIG_GetItem(pMe->m_pConfig, CFGI_HEADSET_AUTOANSWER, &autoanswertype,sizeof(autoanswertype));

                switch (wParam)
                {
                    case IDS_OFF:    //关
                        autoanswertype = OEMNV_HEADSET_AUTOANSWER_OFF;
                        break;

                    case IDS_AUTOANSWER_5SEC:   //5秒
                        autoanswertype = OEMNV_HEADSET_AUTOANSWER_5SEC;
                        break;

                    case IDS_AUTOANSWER_10SEC:  //10秒
                        autoanswertype = OEMNV_HEADSET_AUTOANSWER_10SEC;
                        break;

                    default:
                        autoanswertype = OEMNV_HEADSET_AUTOANSWER_OFF;
                }

                ICONFIG_SetItem(pMe->m_pConfig,CFGI_HEADSET_AUTOANSWER, &autoanswertype,sizeof(autoanswertype));
                CLOSE_DIALOG(DLGRET_CANCELED)

            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandleAutoAnswerSubDialogEvent

#ifdef FEATURE_SUPPORT_G_SENSOR
/*==============================================================================
函数：
       Setting_Handle_Shake
说明：
       IDD_SHAKE对话框事件处理函数

参数：
       pMe [in]：指向ClockApps Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean Setting_Handle_Shake(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                    IDC_SHAKE);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    switch (eCode)  
    {
        case EVT_DIALOG_INIT:
            //add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_SHAKE_TITLE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_MUSIC, IDS_MUSIC, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_FM_RADIO, IDS_FM_RADIO, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_WALLPAPER, IDS_WALLPAPER, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SNOOZE_ALARM, IDS_SNOOZE_ALARM, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_VIDEO, IDS_VIDEO, NULL, 0);
            return TRUE;
            
        case EVT_DIALOG_START:
            pMe->m_shaketype_sel = SET_SHAKE_NONE;
            IMENUCTL_SetSel(pMenu, pMe->m_nSubDlgId);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            return TRUE;

        case EVT_DIALOG_END:    
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            //pMe->m_nSubDlgId = IMENUCTL_GetSel(pMenu);
            pMe->m_nSubDlgId = wParam;
            switch (wParam)
            {
                case IDS_MUSIC:
                    pMe->m_shaketype_sel = SET_MUSIC_CHECK;
                    CLOSE_DIALOG(DLGRET_SHAKESUB)
                    break;
                case IDS_FM_RADIO:
                    pMe->m_shaketype_sel = SET_FM_RADIO_CHECK;
                    CLOSE_DIALOG(DLGRET_SHAKESUB)
                    break;
                    
                case IDS_WALLPAPER:
                    pMe->m_shaketype_sel = SET_WALLPAPER_CHECK;
                    CLOSE_DIALOG(DLGRET_SHAKESUB)
                    break;
                    
                case IDS_SNOOZE_ALARM:
                    pMe->m_shaketype_sel = SET_SNOOZE_ALARM_CHECK;
                    CLOSE_DIALOG(DLGRET_SHAKESUB)
                    break;
                case IDS_VIDEO:
                    pMe->m_shaketype_sel = SET_VIDEO_CHECK;
                    CLOSE_DIALOG(DLGRET_SHAKESUB)
                    break;
                default:
                    ASSERT_NOT_REACHABLE;
            }
            return TRUE;
        default:
            break;              
        }
    return FALSE;    
}

/*==============================================================================
函数：
       Setting_Handle_ShakeSub
说明：
       IDD_SHAKESUB对话框事件处理函数

参数：
       pMe [in]：指向ClockApps Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean Setting_Handle_ShakeSub(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                    IDC_SHAKESUB);
    static boolean bData = FALSE; //用于保存NV项中的 CFGI_SHAKE_MUSIC_CHECK,           
    uint16   cfgi_value = CFGI_SHAKE_MUSIC_CHECK;
    if (pMenu == NULL)
    {
        return FALSE;
    }
    switch (eCode)  
    {
        case EVT_DIALOG_INIT:
            
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_ON, IDS_ON, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_OFF, IDS_OFF, NULL, 0);

            return TRUE;
            
        case EVT_DIALOG_START:
        {
            uint16 wItemID;
            uint16 string_id = IDS_MUSIC;
            IMENUCTL_SetSel(pMenu, pMe->m_sSubDlgId);
            InitMenuIcons(pMenu);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            if(pMe->m_shaketype_sel == SET_MUSIC_CHECK)
            {
                cfgi_value = CFGI_SHAKE_MUSIC_CHECK;
            }
            else if(pMe->m_shaketype_sel == SET_FM_RADIO_CHECK)
            {
                cfgi_value = CFGI_SHAKE_FM_RADIO_CHECK;
                string_id = IDS_FM_RADIO;
            }
            else if(pMe->m_shaketype_sel == SET_WALLPAPER_CHECK)
            {
                cfgi_value = CFGI_SHAKE_WALLPAPER_CHECK;
                string_id = IDS_WALLPAPER;
            }
            else if(pMe->m_shaketype_sel == SET_SNOOZE_ALARM_CHECK)
            {
                cfgi_value = CFGI_SHAKE_SNOOZE_ALARM_CHECK;
                string_id = IDS_SNOOZE_ALARM;
            }
            else if(pMe->m_shaketype_sel == SET_VIDEO_CHECK)
            {
                cfgi_value = CFGI_SHAKE_VIDEO_CHECK;
                string_id = IDS_VIDEO;
            }
            #if 0
            IMENUCTL_SetTitle(pMenu,
                               AEE_APPSSETTINGMENU_RES_FILE,
                               string_id,
                               NULL);
			#else
		    {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        string_id,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
		    }
		    #endif
            (void) ICONFIG_GetItem(pMe->m_pConfig,
                                    cfgi_value,
                                    &bData,
                                    sizeof(bData));
            SetMenu_SetCheckBoxAsOff(pMenu);
            
            if(bData == TRUE)
            {
                wItemID = IDS_ON;
            }
            else
            {
                wItemID = IDS_OFF;
            }
            SetMenuIcon(pMenu, wItemID, TRUE);
            IMENUCTL_SetSel(pMenu, wItemID);
            return TRUE;
        }

        case EVT_DIALOG_END:    
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
        {     
            boolean  bNewData;
            pMe->m_sSubDlgId = IMENUCTL_GetSel(pMenu);
            switch(wParam)
            {
                case IDS_ON:
                    bNewData = TRUE;
                    break;

                case IDS_OFF:
                    bNewData = FALSE;
                    break;

                default:
                    return FALSE;

            }
            if (bNewData != bData)
            {
                if(pMe->m_shaketype_sel == SET_MUSIC_CHECK)
                {
                    cfgi_value = CFGI_SHAKE_MUSIC_CHECK;
                }
                else if(pMe->m_shaketype_sel == SET_FM_RADIO_CHECK)
                {
                    cfgi_value = CFGI_SHAKE_FM_RADIO_CHECK;
                }
                else if(pMe->m_shaketype_sel == SET_WALLPAPER_CHECK)
                {
                    cfgi_value = CFGI_SHAKE_WALLPAPER_CHECK;
                }
                else if(pMe->m_shaketype_sel == SET_SNOOZE_ALARM_CHECK)
                {
                    cfgi_value = CFGI_SHAKE_SNOOZE_ALARM_CHECK;
                }
                else if(pMe->m_shaketype_sel == SET_VIDEO_CHECK)
                {
                    cfgi_value = CFGI_SHAKE_VIDEO_CHECK;
                }           

                (void)ICONFIG_SetItem(pMe->m_pConfig,
                                       cfgi_value,
                                       &bNewData,
                                       sizeof(bNewData));
#ifdef FEATURE_APP_MUSICPLAYER
                if(GetMp3PlayerStatus() == MP3STATUS_RUNONBACKGROUND && bNewData && cfgi_value == CFGI_SHAKE_MUSIC_CHECK)
                {
                    (void)ISHELL_PostEvent(pMe->m_pShell,
                                           AEECLSID_APP_MUSICPLAYER,
                                           EVT_OPENSHAKE,
                                           0,
                                           0);
                }
#endif
                {
                    boolean b_FMBackground = FALSE;
                    OEM_GetConfig(CFGI_FM_BACKGROUND,&b_FMBackground, sizeof(b_FMBackground));
                    if((TRUE == b_FMBackground)&& bNewData && (cfgi_value == CFGI_SHAKE_FM_RADIO_CHECK))
                    {
                        (void)ISHELL_PostEvent(pMe->m_pShell,
                                               AEECLSID_APP_FMRADIO,
                                               EVT_OPENSHAKE,
                                               0,
                                               0);                        
                    }
                }
                //将选中的选项标出
                bData = bNewData;
                SetMenu_SetCheckBoxAsOff(pMenu);
                SetMenuIcon(pMenu, wParam, TRUE);
                (void)IMENUCTL_Redraw(pMenu);
            }

            pMe->m_msg_id = IDS_DONE;
            CLOSE_DIALOG(DLGRET_WARNING);   
            return TRUE;
        }

       //自动返回上级对话框
       case EVT_DISPLAYDIALOGTIMEOUT:
            CLOSE_DIALOG(DLGRET_CANCELED)
        default:
            break;              
    }
    return FALSE;    
}
#endif //#ifdef FEATURE_SUPPORT_G_SENSOR
/*==============================================================================
函数：
       Setting_Handle_CallRestrict
说明：
       IDC_RESTRICT

参数：
       pMe [in]：指向ClockApps Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/

static boolean  Setting_Handle_CallRestrict(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                         IDC_RESTRICT);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_CALL_RESTRICT,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_RESTRICT_OUTGOING, IDS_RESTRICT_OUTGOING, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_RESTRICT_INCOMING, IDS_RESTRICT_INCOMING, NULL, 0);
#ifdef FEATURE_CALL_RESTRICT
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_RESTRICT_NUMBERS, IDS_RESTRICT_NUMBERS, NULL, 0);
#endif
            return TRUE;

        case EVT_DIALOG_START:
            //设定标题格式
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE | MP_WRAPSCROLL | MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);

            IMENUCTL_SetSel(pMenu, pMe->m_sSubDlgId);
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                   CLOSE_DIALOG(DLGRET_CANCELED)
                   return TRUE;

                default:
                   break;
            }
            return TRUE;

        case EVT_COMMAND:
            //pMe->m_sSubDlgId = IMENUCTL_GetSel(pMenu);
            pMe->m_sSubDlgId = wParam;
            switch(wParam)
            {
                case IDS_RESTRICT_OUTGOING:   //呼出限制
                    CLOSE_DIALOG(DLGRET_RESTRICTOUTGOING)
                    break;

                case IDS_RESTRICT_INCOMING:   //呼入限制
                    CLOSE_DIALOG(DLGRET_RESTRICTINCOMING)
                    break;
                    
#ifdef FEATURE_CALL_RESTRICT
                case IDS_RESTRICT_NUMBERS:   //呼出限制
                    CLOSE_DIALOG(DLGRET_RESTRICTNUMBERS)
                    break;
#endif

                default:
                    ASSERT_NOT_REACHABLE;
            }
            return TRUE;

        default:
             break;
    }
    return FALSE;
} // HandleRestrictDialogEvent

/*==============================================================================
函数：
       Setting_Handle_OutGoing
说明：
       IDD_RESTRICT_OUTGOING

参数：
       pMe [in]：指向ClockApps Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/

static boolean  Setting_Handle_OutGoing(CSettingMenu *pMe,
                                           AEEEvent       eCode,
                                           uint16         wParam,
                                           uint32         dwParam)
{
    PARAM_NOT_REF(dwParam)
    static byte bytData = 0;
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_RESTRICT_OUTGOING);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_RESTRICT_OUTGOING,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_RESTRICT_OUTGOING_ALL, IDS_RESTRICT_OUTGOING_ALL, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_RESTRICT_OUTGOING_INTERNATIONAL, IDS_RESTRICT_OUTGOING_INTERNATIONAL, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_RESTRICT_OUTGOING_OUTCONTACT, IDS_RESTRICT_OUTGOING_OUTCONTACT, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_RESTRICT_OUTGOING_OFF, IDS_RESTRICT_OUTGOING_OFF, NULL, 0);
            return TRUE;

        case EVT_DIALOG_START:
            {
                uint16 wItemID;
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_RESTRICT_OUTGOING,
                                       &bytData,
                                       sizeof(bytData));

                switch (bytData)
                {
                    case OEMNV_RESTRICT_OUTGOING_ALL:           //全部
                        wItemID = IDS_RESTRICT_OUTGOING_ALL;
                        break;

                    case OEMNV_RESTRICT_OUTGOING_INTERNATIONAL: //国际长途
                        wItemID = IDS_RESTRICT_OUTGOING_INTERNATIONAL;
                        break;

                    case OEMNV_RESTRICT_OUTGOING_OUTCONTACT:    //通讯簿以外
                        wItemID = IDS_RESTRICT_OUTGOING_OUTCONTACT;
                        break;

                    case OEMNV_RESTRICT_OUTGOING_OFF:           //关闭
                        wItemID = IDS_RESTRICT_OUTGOING_OFF;
                        break;

                    default:
                        wItemID = IDS_RESTRICT_OUTGOING_OFF;
                        break;
                }
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wItemID, TRUE);
                IMENUCTL_SetSel(pMenu, wItemID);
            }
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                  default:
                    break;
            }
            return TRUE;
        case EVT_COMMAND:
            {
                byte bytNewData = 0;

                switch (wParam)
                {
                    case IDS_RESTRICT_OUTGOING_ALL:            //全部
                       bytNewData = OEMNV_RESTRICT_OUTGOING_ALL;
                       break;

                    case IDS_RESTRICT_OUTGOING_INTERNATIONAL:   //国际长途
                       bytNewData = OEMNV_RESTRICT_OUTGOING_INTERNATIONAL;
                       break;

                    case IDS_RESTRICT_OUTGOING_OUTCONTACT:      //通讯簿以外
                       bytNewData = OEMNV_RESTRICT_OUTGOING_OUTCONTACT;
                       break;

                    case IDS_RESTRICT_OUTGOING_OFF:             //关闭
                       bytNewData = OEMNV_RESTRICT_OUTGOING_OFF;
                       break;

                    default:
                       ASSERT_NOT_REACHABLE;

                }

                if (bytNewData != bytData)
                {
                    (void)ICONFIG_SetItem(pMe->m_pConfig,
                                          CFGI_RESTRICT_OUTGOING,
                                          &bytNewData, sizeof(bytNewData));
                    //将选中的选项标出
                    bytData = bytNewData;
                    InitMenuIcons(pMenu);
                    SetMenuIcon(pMenu, wParam, TRUE);
                    (void)IMENUCTL_Redraw(pMenu);
                }
                CLOSE_DIALOG(DLGRET_WARNING)
            }
            return TRUE;
        default:
            break;
    }
    return FALSE;
} // HandleRestrictOutgoingDialogEvent

/*==============================================================================
函数：
       Setting_Handle_Incoming
说明：
       IDD_RESTRICT_Incoming

参数：
       pMe [in]：指向ClockApps Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/

static boolean  Setting_Handle_Incoming(CSettingMenu *pMe,
                                           AEEEvent       eCode,
                                           uint16         wParam,
                                           uint32         dwParam)
{
    PARAM_NOT_REF(dwParam)

    static byte bytData = 0;

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_RESTRICT_INCOMING);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_RESTRICT_INCOMING,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_RESTRICT_INCOMING_ALL, IDS_RESTRICT_INCOMING_ALL, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_RESTRICT_INCOMING_OUTCONTACT, IDS_RESTRICT_INCOMING_OUTCONTACT, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_RESTRICT_INCOMING_OFF, IDS_RESTRICT_INCOMING_OFF, NULL, 0);
            return TRUE;

        case EVT_DIALOG_START:
           {
                uint16 wItemID;
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                    CFGI_RESTRICT_INCOMING,
                                    &bytData,
                                    sizeof(bytData));

                switch (bytData)
                {
                    case OEMNV_RESTRICT_INCOMING_ALL:           //全部
                        wItemID = IDS_RESTRICT_INCOMING_ALL;
                        break;

                    case OEMNV_RESTRICT_INCOMING_OUTCONTACT:    //通讯簿以外
                        wItemID = IDS_RESTRICT_INCOMING_OUTCONTACT;
                        break;

                    case OEMNV_RESTRICT_INCOMING_OFF:           //关闭
                        wItemID = IDS_RESTRICT_INCOMING_OFF;
                        break;

                    default:
                        wItemID = IDS_RESTRICT_INCOMING_OFF;
                        break;
                }
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wItemID, TRUE);
                IMENUCTL_SetSel(pMenu, wItemID);
            }

            return TRUE;

        case EVT_DIALOG_END:
           return TRUE;

        case EVT_KEY:
           switch(wParam)
           {
               case AVK_CLR:
                   CLOSE_DIALOG(DLGRET_CANCELED)
                   return TRUE;

              default:
                   break;
           }

           return TRUE;
        case EVT_COMMAND:
           {
               byte bytNewData = OEMNV_RESTRICT_INCOMING_OFF;

                switch (wParam)
                {
                    case IDS_RESTRICT_INCOMING_ALL:         //全部
                        bytNewData = OEMNV_RESTRICT_INCOMING_ALL;
                        break;

                    case IDS_RESTRICT_INCOMING_OUTCONTACT:  //通讯簿以外
                        bytNewData = OEMNV_RESTRICT_INCOMING_OUTCONTACT;
                        break;

                    case IDS_RESTRICT_INCOMING_OFF:         //关闭
                        bytNewData = OEMNV_RESTRICT_INCOMING_OFF;
                        break;

                    default:
                        ASSERT_NOT_REACHABLE;
                }

                if (bytNewData != bytData)
                {
                    (void)ICONFIG_SetItem(pMe->m_pConfig,
                                          CFGI_RESTRICT_INCOMING,
                                          &bytNewData,
                                          sizeof(bytNewData));
                    bytData = bytNewData;

                    InitMenuIcons(pMenu);
                    SetMenuIcon(pMenu, wParam, TRUE);
                    (void)IMENUCTL_Redraw(pMenu);
                }
                CLOSE_DIALOG(DLGRET_WARNING)
            }
            return TRUE;
        default:
            break;
    }
    return FALSE;
} // HandleRestrictIncomingDialogEvent


#ifdef FEATURE_CALL_RESTRICT
static boolean  Setting_Handle_RestrictNumbers(CSettingMenu *pMe,
                                           AEEEvent       eCode,
                                           uint16         wParam,
                                           uint32         dwParam)
{
    PARAM_NOT_REF(dwParam)
    static byte bytData = 0;
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_RESTRICT_NUMBERS);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    MSG_FATAL("***zzg Setting_Handle_RestrictNumbers eCode=%x, wParam=%x, dwParam=%x***", eCode ,wParam,dwParam);
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSSETTINGMENU_RES_FILE,                                
                                            IDS_RESTRICT_NUMBERS,
                                            WTitle,
                                            sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);

                Setting_BuildRestrictList(pMe, pMenu);
            }
            
            return TRUE;

        case EVT_DIALOG_START:
            {
                //设定标题格式
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE | MP_WRAPSCROLL | MP_ACTIVE_NO_REDRAW);   
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_OPTION_BACK);

                IMENUCTL_SetSel(pMenu, pMe->m_sSubDlgId);      

                return TRUE;
            }            

        case EVT_DIALOG_END:
            return TRUE;

        //case EVT_KEY_PRESS:        
        case EVT_KEY:
            switch(wParam)
            {
                case AVK_SELECT:
                {
                    uint8 byMax = 0;
                    (void) ICONFIG_GetItem(pMe->m_pConfig, CFGI_CALL_RESTRICT_TOTAL, &byMax, sizeof(byte));    

                    if (byMax == 0)                    
                    {
                        pMe->m_index = MAX_CALL_RESTRICT;   
                        CLOSE_DIALOG(DLGRET_RESTRICTOPT)                    
                    }
                    return TRUE;
                }
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;
            
        case EVT_COMMAND:
            {
                MSG_FATAL("***zzg Setting_Handle_RestrictNumbers EVT_COMMAND wParam=%x, dwParam=%x***", wParam, dwParam, 0);
                pMe->m_index = wParam;
                
                CLOSE_DIALOG(DLGRET_RESTRICTOPT)
                return TRUE;
            }
            return TRUE;            
        default:
            break;
    }
    return FALSE;
} 

static boolean Setting_BuildRestrictList(CSettingMenu *pMe, IMenuCtl *pMenu)
{    
    CtlAddItem ai;
   
    uint8 byMax = 0;
    char str[33];
    call_restrict_info		call_restrict_list[MAX_CALL_RESTRICT];
    int i;   

    AECHAR  text[32];

    (void)IMENUCTL_DeleteAll(pMenu);
    
    (void) ICONFIG_GetItem(pMe->m_pConfig, CFGI_CALL_RESTRICT_TOTAL, &byMax, sizeof(byte));              
	
	(void) ICONFIG_GetItem(pMe->m_pConfig,
						   CFGI_CALL_RESTRICT_INFO,
						   (void*)call_restrict_list,
						   sizeof(call_restrict_list));        

    MSG_FATAL("***zzg Setting_BuildRestrictList byMax=%x***", byMax, 0, 0);    

    if (byMax > 0)
    {
        for (i=0; i<byMax; i++)
        {
            MEMSET(&ai, 0, sizeof(ai));

            /*
            if (WSTRLEN(call_restrict_list[i].szName)>0)
            {
                ai.pText       = call_restrict_list[i].szName; 
                ai.wItemID     = i;

                ai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
                ai.wImage = IDB_ALERT;
            }
            else
            */    
            {
                ai.pText       = call_restrict_list[i].szNumber; 
                ai.wItemID     = i;

                ai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
                ai.wImage = IDB_INUSE;
            }

            WSTRTOSTR(call_restrict_list[i].szNumber, str, 33);

            DBGPRINTF("***zzg Setting_BuildRestrictList call_restrict_list[%x].szName=%s***", i, str);
            
            if (FALSE == IMENUCTL_AddItemEx(pMenu, &ai))
            {
                MSG_FATAL("***zzg Setting_BuildRestrictList FAILED i=%x***", i, 0, 0); 
                return EFAILED;
            }
            else
            {
                MSG_FATAL("***zzg Setting_BuildRestrictList SUCCEED i=%x***", i, 0, 0); 
            }
        }         
    }  

    return SUCCESS;
}


static boolean  Setting_RestrictOptsDlgEvent(CSettingMenu  *pMe,
                                                       AEEEvent   eCode,
                                                       uint16     wParam,
                                                       uint32     dwParam)
{
    IMenuCtl  *pMenuCtl;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_RESTRICT_OPTS);

	MSG_FATAL("***zzg Setting_RestrictOptsDlgEvent eCode=%x, wParam=%x***", eCode, wParam, 0);
        
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
            MSG_FATAL("rc.y=%d, rc.dx=%d, rc.dy=%d", rc.y, rc.dx, rc.dy);
            IMENUCTL_SetRect(pMenuCtl, &rc);
            
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
            IMENUCTL_SetPopMenuRect(pMenuCtl); 
          
            IMENUCTL_SetBottomBarType(pMenuCtl, BTBAR_SELECT_BACK);
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);            

            if (pMe->m_index == MAX_CALL_RESTRICT)
            {
                IMENUCTL_AddItem(pMenuCtl, AEE_APPSSETTINGMENU_RES_FILE, IDS_ADD, IDS_ADD, NULL, 0);
            }
            else
            {
                IMENUCTL_AddItem(pMenuCtl, AEE_APPSSETTINGMENU_RES_FILE, IDS_ADD, IDS_ADD, NULL, 0);
                IMENUCTL_AddItem(pMenuCtl, AEE_APPSSETTINGMENU_RES_FILE, IDS_DELETE, IDS_DELETE, NULL, 0);
                IMENUCTL_AddItem(pMenuCtl, AEE_APPSSETTINGMENU_RES_FILE, IDS_RESTRICT_VIEW, IDS_RESTRICT_VIEW, NULL, 0);  
                IMENUCTL_AddItem(pMenuCtl, AEE_APPSSETTINGMENU_RES_FILE, IDS_RESTRICT_DEL_ALL, IDS_RESTRICT_DEL_ALL, NULL, 0); 
            }
            
           
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_SETTINGMENU,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;                        
        }      
        
        case EVT_USER_REDRAW:          
            IMENUCTL_SetSel(pMenuCtl, IMENUCTL_GetItemID(pMenuCtl, 0));                
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:      
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_COMMAND:           
            switch (wParam)
            {
                case IDS_ADD:
                {
                    CLOSE_DIALOG(DLGRET_NUM_EDIT);
                    return TRUE;
                }
                case IDS_DELETE:
                {
                    pMe->m_msgid = IDS_DELETE;
                    CLOSE_DIALOG(DLGRET_PROMPT);
                    return TRUE;
                }
                case IDS_RESTRICT_DEL_ALL:
                {
                    pMe->m_msgid = IDS_RESTRICT_DEL_ALL;    
                    CLOSE_DIALOG(DLGRET_PROMPT);
                    return TRUE;
                }
                case IDS_RESTRICT_VIEW:
                {
                    CLOSE_DIALOG(DLGRET_VIEW_DETAIL);
                    return TRUE;
                }
                default:
                    break;
                    
            }
            break;

        default:
            break;
            
    }
    
    return FALSE;
}

static boolean Setting_DelRestrict(CSettingMenu *pMe, uint16 index)
{
    boolean result = FALSE;      
    char number[30];
    uint8 byMax = 0;
    call_restrict_info		call_restrict_list[MAX_CALL_RESTRICT];
    int i;
    
    call_restrict_info info = {0};
   
    (void) ICONFIG_GetItem(pMe->m_pConfig, CFGI_CALL_RESTRICT_TOTAL, &byMax, sizeof(byte));              
	
	(void) ICONFIG_GetItem(pMe->m_pConfig,
						   CFGI_CALL_RESTRICT_INFO,
						   (void*)call_restrict_list,
						   sizeof(call_restrict_list));      
       
    WSTRTOSTR(call_restrict_list[index].szNumber, number, 30);

    DBGPRINTF("***zzg Setting_DelRestrict byMax=%x, index=%x, numer=%s***", byMax, index, number);

    if (byMax > 0)
    {
        for (i=index; i<(byMax-1); i++)
        {
            WSTRCPY(call_restrict_list[i].szNumber, call_restrict_list[i+1].szNumber);  
            //WSTRCPY(call_restrict_list[i].szName, call_restrict_list[i+1].szName); 
        }  

        MEMSET(call_restrict_list[byMax-1].szNumber, 0, sizeof(call_restrict_list[byMax-1].szNumber));
        //MEMSET(call_restrict_list[byMax-1].szName, 0, sizeof(call_restrict_list[byMax-1].szName));
        byMax --;  
       
       (void) ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_CALL_RESTRICT_TOTAL,
                              &byMax,
                              sizeof(uint8));
       
       (void) ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_CALL_RESTRICT_INFO,
                              (void*) call_restrict_list,
                              sizeof(call_restrict_list));   

       return TRUE;
    }

    return  result;
}

static boolean Setting_DelAllRestrict(CSettingMenu *pMe)
{
    boolean result = FALSE;      
    
    uint8 byMax = 0;
    call_restrict_info		call_restrict_list[MAX_CALL_RESTRICT];
    int i;
    
    call_restrict_info info = {0};
    
    (void) ICONFIG_GetItem(pMe->m_pConfig, CFGI_CALL_RESTRICT_TOTAL, &byMax, sizeof(byte));              
	
	(void) ICONFIG_GetItem(pMe->m_pConfig,
						   CFGI_CALL_RESTRICT_INFO,
						   (void*)call_restrict_list,
						   sizeof(call_restrict_list));    

    if (byMax > 0)
    {
        for (i=0; i<byMax; i++)
        {
            MEMSET(call_restrict_list[i].szNumber, 0, sizeof(call_restrict_list[i].szNumber));
            //MEMSET(call_restrict_list[i].szName, 0, sizeof(call_restrict_list[i].szName));
        }  
        
        byMax = 0;  
       
       (void) ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_CALL_RESTRICT_TOTAL,
                              &byMax,
                              sizeof(uint8));
       
       (void) ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_CALL_RESTRICT_INFO,
                              (void*) call_restrict_list,
                              sizeof(call_restrict_list));        
       return TRUE;
    }

    return  result;
}


static boolean  Setting_Handle_Prompt(CSettingMenu *pMe,
                                               AEEEvent       eCode,
                                               uint16         wParam,
                                               uint32         dwParam)
{
    static IStatic * pStatic = NULL;

    if (NULL == pMe)
    {
        return FALSE;
    }

    if (NULL == pStatic)
    {
        AEERect rect = {0};
        if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                 AEECLSID_STATIC,
                                                 (void **)&pStatic))

        {
            return FALSE;
            ERR("ISHELL_CreateInstance,AEECLSID_STATIC 2",0,0,0);
        }        
        ISTATIC_SetRect(pStatic, &rect);  
    }
   
    switch(eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                        EVTFLG_ASYNC,
                                        AEECLSID_APP_SETTINGMENU,
                                        EVT_USER_REDRAW,
                                        0, 
                                        0);

            return TRUE;

        case EVT_USER_REDRAW:
            {
                PromptMsg_Param_type  Msg_Param={0};
                AECHAR  wstrText[MSGBOX_MAXTEXTLEN];
                
                // 从资源文件取消息内容
                if (pMe->m_msgid != 0)
                {
                    (void)ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSSETTINGMENU_RES_FILE,                                
                                            pMe->m_msgid,
                                            wstrText,
                                            sizeof(wstrText));
                }
                                
                Msg_Param.ePMsgType = MESSAGE_WARNNING;
                Msg_Param.pwszMsg = wstrText;
                Msg_Param.eBBarType = BTBAR_YES_NO;
                DrawPromptMessage(pMe->m_pDisplay, pStatic, &Msg_Param);
            }
            // 更新界面
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);           

            
            return TRUE;

        case EVT_DIALOG_END:

            ISTATIC_Release(pStatic);
            pStatic = NULL;
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_SELECT:
                {
                    if (pMe->m_msgid == IDS_DELETE)
                    {
                        Setting_DelRestrict(pMe, pMe->m_index); 
                    }
                    else if (pMe->m_msgid == IDS_RESTRICT_DEL_ALL)
                    {
                        Setting_DelAllRestrict(pMe); 
                    }
                    
                    CLOSE_DIALOG(DLGRET_MSGBOX_OK)
                    break;
                }
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_MSGBOX_CANCEL)
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


static boolean  Setting_Handle_NumEdit(CSettingMenu *pMe,
                                               AEEEvent       eCode,
                                               uint16         wParam,
                                               uint32         dwParam)
{
    ITextCtl *pTextCtl = NULL;
    IMenuCtl *pMenuCtl = NULL;
    
    if (NULL == pMe)
    {
        return FALSE;
    }

    pTextCtl = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_NUM_EDIT);
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_NUM_EDIT_MENU);
    
    if (NULL == pTextCtl)
    {
        return FALSE;
    }

    switch(eCode)
    {
        case EVT_DIALOG_INIT:
            ITEXTCTL_SetProperties(pTextCtl, TP_FRAME | TP_NOUPDATE|TP_FOCUS_NOSEL);
            return TRUE;

        case EVT_DIALOG_START:
        {
            AEERect ctlRect;
            AEERect menuRt;
            AECHAR  WTitle[40] = {0};
            
            (void)ISHELL_LoadResString(pMe->m_pShell,
                                      AEE_APPSSETTINGMENU_RES_FILE,                                
                                      IDS_SET_EDIT,
                                      WTitle,
                                      sizeof(WTitle));
            if(pMe->m_pAnn != NULL)
            {
                IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }

            MSG_FATAL("***zzg Setting_Handle_NumEdit pMe->m_rc: x=%d, y=%x***", pMe->m_rc.x, pMe->m_rc.y, 0);
            MSG_FATAL("***zzg Setting_Handle_NumEdit pMe->m_rc: dx=%d, dy=%x***", pMe->m_rc.dx, pMe->m_rc.dy, 0);

            SETAEERECT(&menuRt,  0, MENUITEM_HEIGHT,
                        SCREEN_WIDTH,
                        (SCREEN_HEIGHT-STATEBAR_HEIGHT-BOTTOMBAR_HEIGHT-MENUITEM_HEIGHT));     

            IMENUCTL_SetRect(pMenuCtl, &menuRt);
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_GRAPHIC_BG|OEMMP_USE_MENU_STYLE);

            
            SETAEERECT(&ctlRect,  0, 0,
                        SCREEN_WIDTH,
                        MENUITEM_HEIGHT);    //pMe->m_rc.dy - GetBottomBarHeight(pMe->m_pDisplay));     
                      
            ICONTROL_SetRect((IControl*)pTextCtl, &ctlRect);
            ITEXTCTL_SetMaxSize(pTextCtl, OEMNV_VOICEMAIL_MAXLEN-1);              
            (void) ITEXTCTL_SetInputMode(pTextCtl, AEE_TM_NUMBERS);   

            ITEXTCTL_SetActive(pTextCtl,TRUE);        
            IMENUCTL_SetActive(pMenuCtl,FALSE);
            
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                        EVTFLG_ASYNC,
                                        AEECLSID_APP_SETTINGMENU,
                                        EVT_USER_REDRAW,
                                        0, 
                                        0);
            return TRUE;
        }
        case EVT_USER_REDRAW:
        case EVT_KEY_RELEASE:
            // 绘制底条提示
            {
                AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pTextCtl);
                int nLen = 0;
                
                if (NULL != pwstrText)
                {
                    nLen = WSTRLEN(pwstrText);
                }

                (void)ITEXTCTL_Redraw(pTextCtl);

                ITEXTCTL_SetActive(pTextCtl,TRUE);
                IMENUCTL_SetActive(pMenuCtl,FALSE);                
                
                if (nLen > 0)
                {
                	#ifndef FEATURE_ALL_KEY_PAD
                    if (ITEXTCTL_GetCursorPos(pTextCtl) != TC_CURSORSTART)
                    {
                        // Save       Delete
                        DRAW_BOTTOMBAR(BTBAR_SAVE_DELETE)
                    }
                    else
                    #endif
                    {
                        // Save       Back
                        DRAW_BOTTOMBAR(BTBAR_SAVE_BACK)
                    }
                }
                else
                {
                    //            Back
                    DRAW_BOTTOMBAR(BTBAR_BACK)
                }
            }
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            #ifdef FEATURE_LCD_TOUCH_ENABLE
            TSIM_NumberKeypad(FALSE);
            #endif
            
            
            return TRUE;
            
        case EVT_DIALOG_END:            
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                case AVK_INFO:
                case AVK_SELECT:
                    {                         
                        int i;
                        AECHAR    num[OEMNV_VOICEMAIL_MAXLEN]={0};
                        uint16    byMax=0;
                        call_restrict_info  call_restrict_list[MAX_CALL_RESTRICT];
                        call_restrict_info info = {0};
                        
                        (void) ITEXTCTL_GetText(pTextCtl,
                                                num,
                                                OEMNV_VOICEMAIL_MAXLEN);
      
                        if (WSTRLEN(num) == 0) 
                        {
                            return TRUE;
                        }   

                        for (i=0; i<byMax; i++)
                        {
                            if (WSTRCMP(call_restrict_list[i].szNumber, num) == 0)
                            {
                                CLOSE_DIALOG(DLGRET_CANCELED)
                                return TRUE;
                            }
                        }
                        
                        (void) ICONFIG_GetItem(pMe->m_pConfig, CFGI_CALL_RESTRICT_TOTAL, &byMax, sizeof(byte));              
                    	
                    	(void) ICONFIG_GetItem(pMe->m_pConfig,
                    						   CFGI_CALL_RESTRICT_INFO,
                    						   (void*)call_restrict_list,
                    						   sizeof(call_restrict_list));  
                       
                        
                        MEMCPY(call_restrict_list[byMax].szNumber, num, sizeof(call_restrict_list[byMax].szNumber));
                        byMax++;
                        DBGPRINTF("***zzg CContApp_AddToRestrictList szName=%S, byMax=%d***",num, byMax);

                       
                       (void) ICONFIG_SetItem(pMe->m_pConfig,
                                              CFGI_CALL_RESTRICT_TOTAL,
                                              &byMax,
                                              sizeof(uint8));
                       
                       (void) ICONFIG_SetItem(pMe->m_pConfig,
                                              CFGI_CALL_RESTRICT_INFO,
                                              (void*) call_restrict_list,
                                              sizeof(call_restrict_list));     
                    }
      
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                default:
                    break;
            }
            return TRUE;
  
        default:
            break;
    }
}



static boolean  Setting_Handle_View_Detail(CSettingMenu  *pMe,
                                                       AEEEvent   eCode,
                                                       uint16     wParam,
                                                       uint32     dwParam)
{
    IMenuCtl  *pMenuCtl;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_VIEW_DETAIL);

	MSG_FATAL("***zzg Setting_Handle_View_Detail eCode=%x, wParam=%x***", eCode, wParam, 0);
        
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};

            CtlAddItem ai;                    
            call_restrict_info      call_restrict_list[MAX_CALL_RESTRICT];
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
            IMENUCTL_SetRect(pMenuCtl, &rc);
            
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
            IMENUCTL_SetPopMenuRect(pMenuCtl); 
          
            IMENUCTL_SetBottomBarType(pMenuCtl, BTBAR_BACK);
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL);    
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_USE_MENU_STYLE);

                 
            (void)IMENUCTL_DeleteAll(pMenuCtl);        
           
            (void) ICONFIG_GetItem(pMe->m_pConfig,
                                  CFGI_CALL_RESTRICT_INFO,
                                  (void*)call_restrict_list,
                                  sizeof(call_restrict_list)); 

            /*                                    
            if (WSTRLEN(call_restrict_list[pMe->m_index].szName)>0)
            {
               MEMSET(&ai, 0, sizeof(ai));
               ai.pText       = call_restrict_list[pMe->m_index].szName; 
               ai.wItemID     = 0;

               ai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
               ai.wImage = IDB_ALERT;
                                

               if (FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
               {
                   return EFAILED;
               }
               
               MEMSET(&ai, 0, sizeof(ai));
               ai.pText       = call_restrict_list[pMe->m_index].szNumber; 
               ai.wItemID     = 1;

               ai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
               ai.wImage = IDB_INUSE;

               if (FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
               {
                   return EFAILED;
               }
            }
            else
            */    
            {
               MEMSET(&ai, 0, sizeof(ai));
               ai.pText       = call_restrict_list[pMe->m_index].szNumber; 
               ai.wItemID     = 0;

               ai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
               ai.wImage = IDB_INUSE;

               if (FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
               {
                   return EFAILED;
               }
            }            
           
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_SETTINGMENU,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;                        
        }      
        
        case EVT_USER_REDRAW:                    
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:      
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
        

        default:
            break;
            
    }
    
    return FALSE;
}

#endif

/*==============================================================================
函数：
       Setting_Handle_Password
说明：


参数：
       pMe [in]：指向ClockApps Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  Setting_Handle_Password(CSettingMenu *pMe,
                                         AEEEvent       eCode,
                                         uint16         wParam,
                                         uint32         dwParam)
{
    PARAM_NOT_REF(dwParam)
    //static char   *m_strPhonePWD = NULL;
    AECHAR         wstrDisplay[OEMNV_LOCKCODE_MAXLEN+2] = {0};
    int            nLen = 0;
    char           strDisplay[OEMNV_LOCKCODE_MAXLEN+2] = {0};
	


	
    
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            if(NULL == pMe->m_strPhonePWD)
            {
                pMe->m_strPhonePWD = (char *)MALLOC((OEMNV_LOCKCODE_MAXLEN + 1)* sizeof(char)); 
            }
            return TRUE;
            
        case EVT_DIALOG_START:  
            (void) ISHELL_PostEvent(pMe->m_pShell,
                                    AEECLSID_APP_SETTINGMENU,
                                    EVT_USER_REDRAW,
                                    NULL,
                                    NULL);

            return TRUE;
            
        case EVT_USER_REDRAW:
            // 绘制相关信息
            {
                AECHAR  text[32] = {0};
                RGBVAL nOldFontColor;
                TitleBar_Param_type  TitleBar_Param = {0};
                
                // 先清屏
#ifdef FEATURE_CARRIER_CHINA_VERTU
                {
                    IImage *pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SECURITY_BACKGROUND);
                    
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
                (void)ISHELL_LoadResString(pMe->m_pShell, 
                                    AEE_APPSSETTINGMENU_RES_FILE,
                                                IDS_CALL_RESTRICT, 
                                                text,
                                                sizeof(text));                  
                // 画标题条
                TitleBar_Param.pwszTitle = text;
                TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
                #if 0
                DrawTitleBar(pMe->m_pDisplay, &TitleBar_Param);
				#else
				IANNUNCIATOR_SetFieldText(pMe->m_pAnn,text);
				#endif
                (void)ISHELL_LoadResString(pMe->m_pShell, 
                                    AEE_APPSSETTINGMENU_RES_FILE,
                                                IDS_PASSWORD, 
                                                text,
                                                sizeof(text));
                nOldFontColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                    AEE_FONT_BOLD, 
                                    text,
                                    -1, 
                                    5, 
                                    (pMe->m_rc.dy)*2/11, 
                                    NULL, 
                                    IDF_TEXT_TRANSPARENT);
                
                nLen = (pMe->m_strPhonePWD == NULL)?(0):(STRLEN(pMe->m_strPhonePWD));
                MEMSET(strDisplay, '*', nLen);
                strDisplay[nLen] = '|';
                strDisplay[nLen + 1] = '\0';
                (void) STRTOWSTR(strDisplay, wstrDisplay, sizeof(wstrDisplay));
                #ifdef FEATURE_VERSION_X3
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                AEE_FONT_BOLD, 
                                wstrDisplay,
                                -1, 
                                10, 
                                (pMe->m_rc.dy)*2/11+MENUITEM_HEIGHT,
                                NULL, 
                                IDF_TEXT_TRANSPARENT);
                #else
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                AEE_FONT_BOLD, 
                                wstrDisplay,
                                -1, 
                                10, 
                                TITLEBAR_HEIGHT + MENUITEM_HEIGHT*3/2,
                                NULL, 
                                IDF_TEXT_TRANSPARENT);
                #endif
                (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, nOldFontColor);
        		#ifndef FEATURE_ALL_KEY_PAD    //add by yangdecai 
                // 绘制底条提示
                if (nLen > 3)
                {// 确定-----删除
                    SETTING_MENU_DRAW_BOTTOMBAR(BTBAR_OK_DELETE)
                }
                else if(nLen > 0)
                {
                    SETTING_MENU_DRAW_BOTTOMBAR(BTBAR_DELETE)
                }
                else
                #else
                // 绘制底条提示
                if (nLen > 3)
                {// 确定-----删除
                    SETTING_MENU_DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                }
                else if(nLen > 0)
                {
                    SETTING_MENU_DRAW_BOTTOMBAR(BTBAR_BACK)
                }
                else
                #endif
                {// 确定-----取消
                    SETTING_MENU_DRAW_BOTTOMBAR(BTBAR_CANCEL)
                }
                // 更新显示
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);  
                return TRUE;
                }
            
        case EVT_DIALOG_END:
            if(!pMe->m_bSuspending)
            {
                FREEIF(pMe->m_strPhonePWD);
            }
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
                        
                    case AVK_CLR:
                        chEnter = 0;   
                        #ifndef FEATURE_ALL_KEY_PAD    //add by yangdecai 
                        if (pMe->m_strPhonePWD == NULL || STRLEN(pMe->m_strPhonePWD) == 0)
                        {
                            CLOSE_DIALOG(DLGRET_CANCELED)
                            return TRUE;
                        }
                        #else
                        if(dwParam == 0)
                        {
                        	CLOSE_DIALOG(DLGRET_CANCELED)
                            return TRUE;
                        }
                        else
                        {
                        	if (pMe->m_strPhonePWD == NULL || STRLEN(pMe->m_strPhonePWD) == 0)
                        	{
                            	CLOSE_DIALOG(DLGRET_CANCELED)
                            	return TRUE;
                        	}
                        }
                        #endif
                        break;
                        
                    case AVK_SELECT:
                    case AVK_INFO:
                        if (pMe->m_strPhonePWD == NULL || STRLEN(pMe->m_strPhonePWD) < 4)
                        {
                            return TRUE;
                        }
                        else
                        {
                            uint16 wPWD=0;
							//#ifdef FEATURE_VERSION_K212_ND
							//char superpass[6] = {"0000"};
							//#else
							char superpass[6] = {"*#09#"};
							//#endif
                            (void) ICONFIG_GetItem(pMe->m_pConfig, 
                                                   CFGI_PHONE_PASSWORD,
                                                   &wPWD,
                                                   sizeof(uint16));
                            
                            if (wPWD == EncodePWDToUint16(pMe->m_strPhonePWD)||(0==strcmp(superpass,pMe->m_strPhonePWD)))
                            {// 密码符合
                                CLOSE_DIALOG(DLGRET_OK)
                            }
                            else
                            {// 密码错误
                                CLOSE_DIALOG(DLGRET_INVALID)
                            }
                            return TRUE;
                        }
                        
                    default:
                        return TRUE;
                }
                nLen = (pMe->m_strPhonePWD == NULL)?(0):(STRLEN(pMe->m_strPhonePWD));
                if (chEnter == 0)
                {// 删除字符
                    if (nLen > 0)
                    {
                        bRedraw = TRUE;
                        pMe->m_strPhonePWD[nLen-1] = chEnter;
                    }
                }
                else if (nLen < OEMNV_LOCKCODE_MAXLEN)
                {
                    pMe->m_strPhonePWD[nLen] = chEnter;
                    nLen++;
                    pMe->m_strPhonePWD[nLen] = 0;
                    bRedraw = TRUE;
                }
                
                if (bRedraw)
                {
                    (void) ISHELL_PostEvent(pMe->m_pShell,
                                            AEECLSID_APP_SETTINGMENU,
                                            EVT_USER_REDRAW,
                                            NULL,
                                            NULL);
                }
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE
            case EVT_PEN_UP:
                {
                    int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
                    int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
                    AEEDeviceInfo devinfo;
                    int nBarH ;
                    AEERect rc;
                    MSG_FATAL("Setting_HandleAuto_Power_DialogEvent wXPos=%d ,wYPos=%d",wXPos,wYPos,0);
                     
                    nBarH = GetBottomBarHeight(pMe->m_pDisplay);
                    MEMSET(&devinfo, 0, sizeof(devinfo));
                    ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
                    SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);  
                    if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
                    {
                        if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                        {
                             boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_SETTINGMENU,EVT_USER,AVK_SELECT,0);
                             return rt;
                        } 
                        else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//右
                        {                       
                             boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_SETTINGMENU,EVT_USER,AVK_CLR,0);
                             return rt;
                        }
                    }  
                
                }
                break;
            
#endif
            
        default:
            break;
    }
    
    return FALSE;
} 

/*==============================================================================
函数：
       Setting_Handle_Msgbox
说明：


参数：
       pMe [in]：指向ClockApps Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean Setting_Handle_Msgbox(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    static IStatic * pStatic = NULL;
    if (NULL == pMe)
    {
        return FALSE;
    }

   if (NULL == pStatic)
   {
        AEERect rect = {0};
        if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                 AEECLSID_STATIC,
                                                 (void **)&pStatic))
        
        {
            return FALSE;
            ERR("ISHELL_CreateInstance,AEECLSID_STATIC 2",0,0,0);
        }        
        ISTATIC_SetRect(pStatic, &rect);  
   }
   
    switch(eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_APP_SETTINGMENU,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);

            return TRUE;

        case EVT_USER_REDRAW:
            {
                PromptMsg_Param_type  Msg_Param={0};
                AECHAR  wstrText[MSGBOX_MAXTEXTLEN];
                
                // 从资源文件取消息内容
                (void)ISHELL_LoadResString(pMe->m_pShell,
                                AEE_APPSSETTINGMENU_RES_FILE,                                
                                IDS_INVALID,
                                wstrText,
                                sizeof(wstrText));
                                
                Msg_Param.ePMsgType = MESSAGE_WARNNING;// MESSAGE_INFORMATIVE;
                Msg_Param.pwszMsg = wstrText;
                Msg_Param.eBBarType = BTBAR_RETRY_CANCEL;//BTBAR_NONE
                DrawPromptMessage(pMe->m_pDisplay, pStatic, &Msg_Param);
            }
            // 更新界面
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);           

            
            return TRUE;

        case EVT_DIALOG_END:

            ISTATIC_Release(pStatic);
            pStatic = NULL;
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_SELECT:
                    CLOSE_DIALOG(DLGRET_MSGBOX_OK)
                    break;
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_MSGBOX_CANCEL)
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

#ifdef FEATURE_PLANEMODE
/*==============================================================================
函数：
       HandlePlaneModeDialogEvent
说明：
       IDD_PLANEMODE 对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/

static boolean  HandlePlaneModeDialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)
    static byte planeMode_cfg = 0;
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_PLANEMODE);
    MSG_FATAL("%x, %x ,%x,HandlePlaneModeDialogEvent",eCode,wParam,dwParam);
    if (pMenu == NULL)
    {
        return FALSE;
    }
     //实现菜单循环滚动功能
    //SettingMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_PLANE_MODE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_ON, IDS_ON, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_OFF, IDS_OFF, NULL, 0);
            #ifndef FEATURE_VERSION_K232_Y105A
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_QUERY_PLANEMODE, IDS_QUERY_PLANEMODE, NULL, 0);
            #endif
            return TRUE;

        case EVT_DIALOG_START:
            {
                uint16 wItemID;

                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_PLANEMODE,
                                       &planeMode_cfg,
                                       sizeof(planeMode_cfg));

                switch (planeMode_cfg)
                {
                    case  OEMNV_PLANEMODE_ON:
                        wItemID = IDS_ON;
                        break;

                    case OEMNV_PLANEMODE_QUERY:
                        wItemID = IDS_QUERY_PLANEMODE;
                        break;

                    case OEMNV_PLANEMODE_OFF :
                        wItemID = IDS_OFF;
                        break;

                    default:
                        break;
                }

                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wItemID, TRUE);
                IMENUCTL_SetSel(pMenu, wItemID);
            }
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                  default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {
                byte newCfg = 0;
                AEECMPhInfo phInfo;
                ICM  *pCM = NULL;
                int nRet;
                
                nRet = ISHELL_CreateInstance(pMe->m_pShell,
                                             AEECLSID_CALLMANAGER,
                                             (void **) &pCM);
                if ((nRet != SUCCESS) || (pCM == NULL))
                {
                    return FALSE;
                }
                
                ICM_GetPhoneInfo(pCM, &phInfo, sizeof(phInfo));
                
                switch (wParam)
                {
                    case IDS_ON:
                       if(SYS_OPRT_MODE_LPM != phInfo.oprt_mode)
                       {
                           IANNUNCIATOR_SetField(pMe->m_pAnn, ANNUN_FIELD_RSSI, ANNUN_STATE_AIR_MODE_ON );
#ifdef FEATURE_VERSION_EC99
                           IANNUNCIATOR_SetField(pMe->m_pAnn, ANNUN_FIELD_3G_RSSI, ANNUN_STATE_AIR_MODE_ON );
#endif                           
                           cm_ph_cmd_oprt_mode(NULL, NULL, CM_CLIENT_ID_ANONYMOUS, SYS_OPRT_MODE_LPM);
                       }
                       newCfg = OEMNV_DATEFORM_DMY ;
                       break;

                    case IDS_OFF:
                       if(SYS_OPRT_MODE_ONLINE != phInfo.oprt_mode)
                       {
                           IANNUNCIATOR_SetField(pMe->m_pAnn, ANNUN_FIELD_RSSI, ANNUN_STATE_AIR_MODE_OFF );
#ifdef FEATURE_VERSION_EC99
                           IANNUNCIATOR_SetField(pMe->m_pAnn, ANNUN_FIELD_3G_RSSI, ANNUN_STATE_AIR_MODE_OFF );
#endif                            
                           cm_ph_cmd_oprt_mode(NULL, NULL, CM_CLIENT_ID_ANONYMOUS, SYS_OPRT_MODE_ONLINE);
                       }
                       newCfg = OEMNV_DATEFORM_MDY ;
                       break;

                    case IDS_QUERY_PLANEMODE:
                       //cm_ph_cmd_oprt_mode(NULL, NULL, CM_CLIENT_ID_ANONYMOUS, SYS_OPRT_MODE_LPM);
                       newCfg = OEMNV_DATEFORM_YMD ;
                       break;

                    default:
                       ASSERT_NOT_REACHABLE;

                }

                if (newCfg != planeMode_cfg)
                {
                    (void)ICONFIG_SetItem(pMe->m_pConfig,
                                          CFGI_PLANEMODE,
                                          &newCfg, sizeof(newCfg));
                    //将选中的选项标出
                    planeMode_cfg = newCfg;
                    InitMenuIcons(pMenu);
                    SetMenuIcon(pMenu, wParam, TRUE);
                    (void)IMENUCTL_Redraw(pMenu);
                }
                CLOSE_DIALOG(DLGRET_WARNING)
                ICM_Release(pCM);
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandlePlaneModeDialogEvent
#endif

#ifdef FEATURE_VERSION_W208S
/*==============================================================================
函数：
       Setting_Handle_SMSRestrict
说明：
       IDC_SMS_RESTRICT

参数：
       pMe [in]：指向ClockApps Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/

static boolean  Setting_Handle_SMSRestrict(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                         IDC_SMS_RESTRICT);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDC_SMS_RESTRICT,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
            //IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SMS_RESTRICT_SEND, IDS_SMS_RESTRICT_SEND, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_SMS_RESTRICT_RECEIVE, IDS_SMS_RESTRICT_RECEIVE, NULL, 0);
            return TRUE;

        case EVT_DIALOG_START:
            //设定标题格式
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE | MP_WRAPSCROLL | MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);

            IMENUCTL_SetSel(pMenu, pMe->m_sSubDlgId);
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                   CLOSE_DIALOG(DLGRET_CANCELED)
                   return TRUE;

                default:
                   break;
            }
            return TRUE;

        case EVT_COMMAND:
            //pMe->m_sSubDlgId = IMENUCTL_GetSel(pMenu);
            pMe->m_sSubDlgId = wParam;
            switch(wParam)
            {
      //          case IDS_SMS_RESTRICT_SEND:   //发送限制
      //              CLOSE_DIALOG(DLGRET_SMSRESTRICT_SEND)
      //              break;

                case IDS_SMS_RESTRICT_RECEIVE:   //接收限制
                    CLOSE_DIALOG(DLGRET_SMSRESTRICT_RECEIVE)
                    break;

                default:
                    ASSERT_NOT_REACHABLE;
            }
            return TRUE;

        default:
             break;
    }
    return FALSE;
} // HandleRestrictDialogEvent
/*==============================================================================
函数：
       Setting_Handle_SMSRestrict_RECEIVE
说明：
       IDC_SMS_RESTRICT

参数：
       pMe [in]：指向ClockApps Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/

static boolean  Setting_Handle_SMSRestrict_RECEIVE(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                         IDC_SMS_RESTRICT_RECEIVE);
    MSG_FATAL("Setting_Handle_SMSRestrict_RECEIVE Start eCode=0x%x wParam=0x%x",eCode,wParam,0);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("Setting_Handle_SMSRestrict_RECEIVE 1",0,0,0);
    if (pMe->m_pMenu != NULL)
    {
        MSG_FATAL("Setting_Handle_SMSRestrict_RECEIVE 2",0,0,0);
        if (IMENUCTL_HandleEvent(pMe->m_pMenu, eCode, wParam, dwParam))
        {
            MSG_FATAL("Setting_Handle_SMSRestrict_RECEIVE 3",0,0,0);
            return TRUE;
        }
    }   
    MSG_FATAL("Setting_Handle_SMSRestrict_RECEIVE 4",0,0,0);
    //用同一个dialog处理不好事件时，就用另一个Dialog处理输入事件好了
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			{
                uint32 dwMask;
                uint8 byMax = 0;
                uint8 i = 0;
				AECHAR WTitle[40] = {0};
                MSG_FATAL("Setting_Handle_SMSRestrict_RECEIVE EVT_DIALOG_INIT 2",0,0,0);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_OPTION_BACK);
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_SMS_RESTRICT_RECEIVE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
                MSG_FATAL("Setting_Handle_SMSRestrict_RECEIVE EVT_DIALOG_INIT 3",0,0,0);
            	//初始拒收黑名单的总数
            	(void) ICONFIG_GetItem(pMe->m_pConfig,
            						   CFGI_SMS_RESTRICT_RECEIVE_TOTAL,
            						   &byMax,
            						   sizeof(uint8));  
                MSG_FATAL("Setting_Handle_SMSRestrict_RECEIVE EVT_DIALOG_INIT 4 byMax=%d",byMax,0,0);
            	//初始化拒收短信黑名单的信息
            	(void) ICONFIG_GetItem(pMe->m_pConfig,
            						   CFGI_SMS_RESTRICT_RECEIVE_INFO,
            						   (void*)pMe->sms_restrict_reciveList,
            						   sizeof(pMe->sms_restrict_reciveList));

                MSG_FATAL("Setting_Handle_SMSRestrict_RECEIVE EVT_DIALOG_INIT 3 byMax=%d",byMax,0,0);
                if(byMax > 0)
                {
                    for(; i < byMax; ++i)
                    {
                        DBGPRINTF("szName=%S", pMe->sms_restrict_reciveList[i].szName);
                        IMENUCTL_AddItem(pMenu, 0,0,i, pMe->sms_restrict_reciveList[i].szName, 0);
                    }
                    IMENUCTL_SetSel(pMenu, 0);
                }    
            }

        case EVT_DIALOG_START:
            return TRUE;

        case EVT_USER_REDRAW:    
            MSG_FATAL("Setting_Handle_SMSRestrict_RECEIVE EVT_USER_REDRAW 0",0,0,0);
            IMENUCTL_Redraw(pMenu);
            return TRUE;

        case EVT_DIALOG_END:
            if (NULL != pMe->m_pMenu)
            {
                IMENUCTL_Release(pMe->m_pMenu);
                pMe->m_pMenu = NULL;
            }           
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                   if (pMe->m_pMenu == NULL)
                   {
                       CLOSE_DIALOG(DLGRET_CANCELED)
                   }
                   else
                   {
                       IMENUCTL_Release(pMe->m_pMenu);
                       pMe->m_pMenu = NULL;   
                       IMENUCTL_SetActive(pMenu, TRUE);	
                    	(void) ISHELL_PostEvent(pMe->m_pShell, 
                                            AEECLSID_APP_SETTINGMENU,
                                            EVT_USER_REDRAW,
                                            0, 
                                            0);                       
                   }
                   return TRUE;

                case AVK_SELECT:
                    MSG_FATAL("Setting_Handle_SMSRestrict_RECEIVE AVK_SELECT",0,0,0);
                    if (pMe->m_pMenu != NULL)
                    {
                        return TRUE;
                    }
                    // 显示弹出菜单
                    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_MENUCTL, 
                            (void **) &pMe->m_pMenu) == SUCCESS)
                    {
                    
                        AEERect rc={0};
                        AEERect Temprc={0};
                        uint8 byMax = 0;
                    	//初始拒收黑名单的总数
                    	(void) ICONFIG_GetItem(pMe->m_pConfig,
                    						   CFGI_SMS_RESTRICT_RECEIVE_TOTAL,
                    						   &byMax,
                    						   sizeof(uint8));                          
                        // 将文本控件置于非激活状态
                        IMENUCTL_SetActive(pMenu, FALSE);
                        if(byMax > 0)
                        {
                            // 动态添加菜单项
                            MENU_ADDITEM(pMe->m_pMenu, IDS_ADD);
                            MENU_ADDITEM(pMe->m_pMenu, IDS_DELETE);
                        }
                        else
                        {
                            // 动态添加菜单项
                            MENU_ADDITEM(pMe->m_pMenu, IDS_ADD);                            
                        }
                        IMENUCTL_SetPopMenuRect(pMe->m_pMenu);
                    
                        IMENUCTL_SetProperties(pMe->m_pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
                        IMENUCTL_SetOemProperties(pMe->m_pMenu, OEMMP_USE_MENU_STYLE);
                        
                        IMENUCTL_SetBottomBarType(pMe->m_pMenu,BTBAR_SELECT_BACK);
                        
                        IMENUCTL_SetActive(pMe->m_pMenu, TRUE);                     
                        (void)IMENUCTL_Redraw(pMe->m_pMenu);                            
                        
                        IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    
                    }    
                    
                default:
                   break;
            }
            return TRUE;

        case EVT_COMMAND:
            MSG_FATAL("Setting_Handle_SMSRestrict_RECEIVE EVT_COMMAND",0,0,0);
            if (pMe->m_pMenu == NULL)
            {
                MSG_FATAL("Setting_Handle_SMSRestrict_RECEIVE EVT_COMMAND 0",0,0,0);
                // 显示弹出菜单
                if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_MENUCTL, 
                        (void **) &pMe->m_pMenu) == SUCCESS)
                {
                
                    AEERect rc={0};
                    AEERect Temprc={0};
                    uint8 byMax = 0;
                	//初始拒收黑名单的总数
                	(void) ICONFIG_GetItem(pMe->m_pConfig,
                						   CFGI_SMS_RESTRICT_RECEIVE_TOTAL,
                						   &byMax,
                						   sizeof(uint8));                     
                    // 将文本控件置于非激活状态
                    IMENUCTL_SetActive(pMenu, FALSE);
                    // 动态添加菜单项
                    if(byMax > 0)
                    {
                        MENU_ADDITEM(pMe->m_pMenu, IDS_ADD);
                        MENU_ADDITEM(pMe->m_pMenu, IDS_DELETE);
                    }
                    else
                    {
                        MENU_ADDITEM(pMe->m_pMenu, IDS_ADD);
                    }
                    IMENUCTL_SetPopMenuRect(pMe->m_pMenu);

                    IMENUCTL_SetProperties(pMe->m_pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
					IMENUCTL_SetOemProperties(pMe->m_pMenu, OEMMP_USE_MENU_STYLE);
					
                    IMENUCTL_SetBottomBarType(pMe->m_pMenu,BTBAR_SELECT_BACK);
                    
                    IMENUCTL_SetActive(pMe->m_pMenu, TRUE);						
					(void)IMENUCTL_Redraw(pMe->m_pMenu);     						
                    
                    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);

                }                
            }
            else
            {
                if (NULL != pMe->m_pMenu)
                {
                    IMENUCTL_Release(pMe->m_pMenu);
                    pMe->m_pMenu = NULL;
                }
                pMe->m_sSubDlgId = wParam;
                MSG_FATAL("Setting_Handle_SMSRestrict_RECEIVE wParam=0x%x",wParam,0,0);
                switch(wParam)
                {
                    case IDS_ADD:   //添加接收黑名单
                        CLOSE_DIALOG(DLGRET_SMSRESTRICT_RECEIVE_ADD)
                        break;

                    case IDS_DELETE:   //删除接收黑名单
                        {
                            uint8 index = 0, i=0;
                            uint8 MenuSelectdId = IMENUCTL_GetSel(pMenu);
                            uint8 byMax = 0;
                            sms_restrict_recive_info		temp[MAX_SMS_RESTRICT]={0};
                        	(void) ICONFIG_GetItem(pMe->m_pConfig,
                        						   CFGI_SMS_RESTRICT_RECEIVE_TOTAL,
                        						   &byMax,
                        						   sizeof(uint8));                              
                            MSG_FATAL("Setting_Handle_SMSRestrict_RECEIVE IDS_DELETE MenuSelectdId=%d",MenuSelectdId,0,0);
                            if((MenuSelectdId >= 0) && (byMax > 0))
                            {
   
                                for(; index <byMax; ++index)
                                {
                                    if(index != MenuSelectdId)
                                    {
                                        MEMCPY(temp[i++].szName, pMe->sms_restrict_reciveList[index].szName, sizeof(pMe->sms_restrict_reciveList[index].szName));
                                    }
                                }
                                --byMax;
                                (void)IMENUCTL_DeleteAll(pMenu);  
                                MSG_FATAL("sizeof(pMe->sms_restrict_reciveList)=%d", sizeof(pMe->sms_restrict_reciveList),0,0);
                                MEMSET(pMe->sms_restrict_reciveList, 0, sizeof(pMe->sms_restrict_reciveList));
                                if(byMax > 0)
                                {
                                    for(i = 0; i < byMax; ++i)
                                    {
                                        DBGPRINTF("szName=%S", temp[i].szName);
                                        IMENUCTL_AddItem(pMenu, 0,0,i, temp[i].szName, 0);
                                    }
                                    IMENUCTL_SetSel(pMenu, 0);
                                    IMENUCTL_SetActive(pMenu, TRUE);
                                }        
                              	(void) ICONFIG_SetItem(pMe->m_pConfig,
                             						   CFGI_SMS_RESTRICT_RECEIVE_TOTAL,
                             						   &byMax,
                             						   sizeof(uint8));
                             	(void) ICONFIG_SetItem(pMe->m_pConfig,
                             						   CFGI_SMS_RESTRICT_RECEIVE_INFO,
                             						   (void*) temp,
                             						   sizeof(temp));          
                            	(void) ISHELL_PostEvent(pMe->m_pShell, 
                                                    AEECLSID_APP_SETTINGMENU,
                                                    EVT_USER_REDRAW,
                                                    0, 
                                                    0);
                            }
                        }

                        break;
                }
            }
            return TRUE;

        default:
             break;
    }
    return FALSE;
} // HandleRestrictDialogEvent

/*==============================================================================
函数：
       Setting_Handle_SMSRestrict_RECEIVE_ADD
说明：
       IDC_SMS_RESTRICT

参数：
       pMe [in]：指向ClockApps Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/

static boolean  Setting_Handle_SMSRestrict_RECEIVE_ADD(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
    ITextCtl *pIText = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_TEXT_SMS_RESTRICT_RECEIVE);
    MSG_FATAL("Setting_Handle_SMSRestrict_RECEIVE_ADD Start eCode=0x%x wParam=0x%x",eCode,wParam,0);
    if (pIText == NULL)
    {
        return FALSE;
    }
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			{
                uint32 dwMask;
                byte byMax = 0;
                uint8 i = 0;
				AECHAR WTitle[40] = {0};
                AEERect rc={0};
                AEEDeviceInfo devinfo={0};
                MSG_FATAL("Setting_Handle_SMSRestrict_RECEIVE_ADD EVT_DIALOG_INIT 1",0,0,0);
                ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);                
                ITEXTCTL_SetProperties(pIText, TP_GRAPHIC_BG|TP_FRAME | TP_MULTILINE | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT | TP_DISPLAY_SMSCOUNT | TP_NOUPDATE|TP_FOCUS_NOSEL);
                rc.x = 0;
                rc.y = 0;
                rc.dy = GetBottomBarHeight(pMe->m_pDisplay);
                rc.dx = devinfo.cxScreen;
                ITEXTCTL_SetRect( pIText, &rc);
                ITEXTCTL_SetMaxSize ( pIText, 32);
                (void)ITEXTCTL_SetText(pIText,L"",-1);
                (void) ITEXTCTL_SetInputMode(pIText, AEE_TM_NUMBERS); 
                            
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_ADD,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
                ITEXTCTL_SetActive(pIText, TRUE);
                ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);                
            }

        case EVT_DIALOG_START:
            //设定标题格式
            (void) ISHELL_PostEvent(pMe->m_pShell,
                                   AEECLSID_APP_SETTINGMENU,
                                   EVT_USER_REDRAW,
                                   0,
                                   0);
            return TRUE;

        case EVT_USER_REDRAW:    
            MSG_FATAL("Setting_Handle_SMSRestrict_RECEIVE_ADD EVT_USER_REDRAW 0",0,0,0);
            ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
            ITEXTCTL_SetActive(pIText, TRUE);  
            ITEXTCTL_Redraw(pIText);                  
            SETTING_MENU_DRAW_BOTTOMBAR(BTBAR_SAVE_BACK);
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                   CLOSE_DIALOG(DLGRET_CANCELED)
                   return TRUE;

                case AVK_SELECT:
                {
                     uint8 byMax = 0;
                     AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pIText);
                     MSG_FATAL("Setting_Handle_SMSRestrict_RECEIVE_ADD AVK_SELECT 0",0,0,0);
                     DBGPRINTF("pwstrText=%S",pwstrText);
                     if(WSTRLEN(pwstrText) != 0)
                     {
                         sms_restrict_recive_info info = {0};
                         (void) ICONFIG_GetItem(pMe->m_pConfig,
                         CFGI_SMS_RESTRICT_RECEIVE_TOTAL,
                         &byMax,
                         sizeof(byte));   
                         MEMCPY(pMe->sms_restrict_reciveList[byMax].szName, pwstrText, sizeof(pMe->sms_restrict_reciveList[byMax].szName));
                         byMax++;
                         DBGPRINTF("szName=%S, byMax=%d",pwstrText, byMax);
                      	(void) ICONFIG_SetItem(pMe->m_pConfig,
                     						   CFGI_SMS_RESTRICT_RECEIVE_TOTAL,
                     						   &byMax,
                     						   sizeof(uint8));
                     	(void) ICONFIG_SetItem(pMe->m_pConfig,
                     						   CFGI_SMS_RESTRICT_RECEIVE_INFO,
                     						   (void*) pMe->sms_restrict_reciveList,
                     						   sizeof(pMe->sms_restrict_reciveList));                           
                     }
                     CLOSE_DIALOG(DLGRET_MSGBOX_OK)                
                }
                return TRUE;
                    
                default:
                   break;
            }
            return TRUE;

        default:
             break;
    }
    return FALSE;
} // Setting_Handle_SMSRestrict_RECEIVE_ADD

#endif

#ifdef FEATURE_CALL_REJECT_AUTO_MSG
static boolean  Setting_Handle_Call_Reject_Auto_Msg_DialogEvent(CSettingMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_CALL_REJECT_AUTO_MSG);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    MSG_FATAL("%x,%x,%x,SettingMenu_Handle_Call_Reject_Auto_Msg_DialogEvent",eCode,wParam,dwParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSETTINGMENU_RES_FILE,                                
                        IDS_REJECT_CALL_SMS,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldTextEx(pMe->m_pAnn,WTitle,FALSE);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_DISABLE, IDS_DISABLE, NULL, 0);
            IMENUCTL_AddItem(pMenu, AEE_APPSSETTINGMENU_RES_FILE, IDS_ENABLE, IDS_ENABLE, NULL, 0);
            return TRUE;

        case EVT_DIALOG_START:
            {
                uint16     ui16_return = IDS_ENABLE;
                boolean    tmp = FALSE;
                
                OEM_GetConfig(CFGI_CALL_REJECT_AUTO_MSG,&tmp, sizeof(tmp));

                MSG_FATAL("***zzg Call_Reject EVT_DIALOG_START tmp=%d***", tmp, 0, 0);

                if (tmp == TRUE)
                {
                    ui16_return = IDS_ENABLE;
                }
                else
                {
                    ui16_return = IDS_DISABLE;
                }
                
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, ui16_return, TRUE);
                IMENUCTL_SetSel(pMenu, ui16_return);
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_GRAPHIC_BG);
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                ISHELL_PostEvent( pMe->m_pShell,AEECLSID_APP_SETTINGMENU,EVT_USER_REDRAW,0,0);
            }
            return TRUE;

        case EVT_USER_REDRAW:            
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
        {
            boolean set;
            switch (wParam)
            {
                case IDS_DISABLE:      
                    set = FALSE;
                    break;
                
                case IDS_ENABLE: 
                default:    
                    set = TRUE;
                    break;
            }

            ICONFIG_SetItem(pMe->m_pConfig,CFGI_CALL_REJECT_AUTO_MSG,&set,sizeof(set));
            CLOSE_DIALOG(DLGRET_CANCELED)
            return TRUE;
        }
        default:
            break;
    }
    return FALSE;
} // HandleKeyToneLengthDialogEvent
#endif
