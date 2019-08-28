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

#define  PANEL                            1
#define  PANEL_TREE                       2       /* control type: tree, callback function: (none) */
#define  PANEL_SEARCH_STRING              3       /* control type: string, callback function: (none) */
#define  PANEL_DECORATION_2               4       /* control type: deco, callback function: (none) */
#define  PANEL_RELATION                   5       /* control type: ring, callback function: RelationChanged */
#define  PANEL_STATE                      6       /* control type: tree, callback function: (none) */
#define  PANEL_COMMANDBUTTON              7       /* control type: command, callback function: HighlightAll */
#define  PANEL_QUITBUTTON                 8       /* control type: command, callback function: QuitCallback */
#define  PANEL_TEXTMSG                    9       /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK HighlightAll(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QuitCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RelationChanged(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
