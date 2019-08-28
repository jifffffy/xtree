/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                           1
#define  PANEL_TREE                      2
#define  PANEL_SEARCH_STRING             3
#define  PANEL_DECORATION_2              4
#define  PANEL_RELATION                  5       /* callback function: RelationChanged */
#define  PANEL_STATE                     6
#define  PANEL_COMMANDBUTTON             7       /* callback function: HighlightAll */
#define  PANEL_QUITBUTTON                8       /* callback function: QuitCallback */
#define  PANEL_TEXTMSG                   9


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK HighlightAll(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QuitCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RelationChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);

int  CVICALLBACK PanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
