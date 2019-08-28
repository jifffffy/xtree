#include <cvirte.h>
#include <userint.h>
#include <toolbox.h>
#include "xmltree.h"
#include "TestXmltree.h"


//==============================================================================
// Include files

//#include "TestXmltree.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables
static int panelHandle;
static int state;
static int relation = VAL_ALL;
static int prevSelectedItems[256];
static int numPrevSelectedItems;
static int currentSelectedItems[256];
static int numCurrentSelectedItems;
static int markForSelectLaterItems[256];
static int numMarkForSelectLaterItems;
static int anyLabel = 0;

static XTree xTree = NULL;
static Criteria criteria = NULL;
//==============================================================================
// Static functions
int GetStateCriteria(void);
void UpdateState(int, int*);
void DeselectPrevOnlySelected(void);
void GetAllSelectedItems(void);
int IsCurrentSelect(int);
void SelectMarkedItems(void);
void HighlightTreeItem(int);
void ResetActiveItem(int);

int TestLabelValue(char *text, char *label); 
int TestAttributeValue(char *text, char *label, char *attribute);   
//==============================================================================
// Global variables

//==============================================================================
// Global functions

int main (int argc, char *argv[])
{
	int		error = 0;
	
	nullChk (InitCVIRTE (0, argv, 0));
	/*
	if ((panelHandle = LoadPanel (0, "treesearch.uir", PANEL)) < 0)
		return -1;
	errChk(CreateXTree(panelHandle, PANEL_TREE, &xTree));
	errChk(Connect(xTree, "menu.xml"));
	errChk(Debug(xTree));
	errChk (RunUserInterface ());
	*/
	/* Set some initial options and clear the window */
	
	errChk(NewXTree(&xTree));
	errChk(Connect(xTree, "menu.xml"));
	NewCriteria(VAL_ALL, 0, VAL_FIRST, VAL_NEXT, VAL_UNSELECTED, &criteria);
	// errChk(Debug(xTree));
	
    Cls();
    SetStdioWindowOptions (100000, 1, 1);
    SetStdioWindowSize (400, 600);
    SetStdioWindowPosition (50, 50);
    SetStdioWindowVisibility (1);

	TestLabelValue("\r\n获取PC版本:", ""); 
	TestLabelValue("\r\n获取fingerPrinter步骤:", "machine>fingerPrinter>steps");
	TestLabelValue("\r\n获取读卡器目标:", "machine>hcr>target");
	printf ("\r\nPress <Enter> to quit.");
    getchar ();
	
Error:
	if (panelHandle)
		DiscardPanel (panelHandle);
	return 0;
}

int TestLabelValue(char *text, char *label)
{
	char	value[512] = {0};
	printf(text);
	GetLabelValue(xTree, criteria, label, value);
	printf("%s", value);
	return 0;
}

int CVICALLBACK QuitCallback (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			QuitUserInterface (0);
			break;
	}
	return 0;
}

int CVICALLBACK HighlightAll (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int relativeIndex;
	char label[256];
	int item = 0, beginIndex = VAL_FIRST, firstIteration = 1, 
		direction = VAL_NEXT_PLUS_SELF; // in the first search, include the very first tree element for the search
	
	char value[256];
	switch (event)
	{
		case EVENT_COMMIT:
			numCurrentSelectedItems = 0;
			numMarkForSelectLaterItems = 0;

			GetCtrlVal (panelHandle, PANEL_SEARCH_STRING, label);
			GetActiveTreeItem (panelHandle, PANEL_TREE, &relativeIndex);
			state = GetStateCriteria();
			GetAllSelectedItems(); // save currently selected items, to be used in DeselectPrevOnlySelected function. 
			anyLabel = !strlen(label);
			// NewCriteria(relation, relativeIndex, beginIndex, direction, state,  &criteria);
			// GetAttributeValue(xTree, criteria, label, "target", *value);
			// printf("value = %s", value);
			// Iterate from the first item that meets the relation criteria, until there is no more find.
			while (item!=-1)
			{
				if (anyLabel)
				{
					GetTreeItem (panelHandle, PANEL_TREE, relation, relativeIndex,
								 beginIndex, direction, state, &item);
				}
				else
				{
					GetTreeItemFromLabel (panelHandle, PANEL_TREE, relation, relativeIndex,
								 beginIndex, direction, state, label, &item);
				}
	
				beginIndex = item;
	
				if (item!=-1)
				{
					HighlightTreeItem(item);
				}
				else if (firstIteration)
				{
					MessagePopup ("", "There is no match in the tree control.");
				}

				if (firstIteration)
				{
					direction = VAL_NEXT;  
					firstIteration = 0;
				}
			}
			
			if (state & VAL_UNEXPOSED)
			{
				SelectMarkedItems();
			}
			
			DeselectPrevOnlySelected();
			ResetActiveItem(relativeIndex);
			SetActiveCtrl (panelHandle, PANEL_TREE);
			
			break;
	}
	return 0;
}

int CVICALLBACK RelationChanged (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int index;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlIndex (panelHandle, PANEL_RELATION, &index);
			switch(index)
			{
				case 0:
					relation = VAL_ALL;
					break;
				case 1:
					relation = VAL_SIBLING;
					break;
				case 2:
					relation = VAL_CHILD;
					break;
				case 3:
					relation = VAL_DESCENDENT;
					break;
				case 4:
					relation = VAL_ANCESTOR;
					break;
			}
			break;
	}
	return 0;
}

int GetStateCriteria() // Note you can use listbox ctrl functions for tree ctrl
{
	int state = 0;  
	int i, numItems;
	
	GetNumListItems (panelHandle, PANEL_STATE, &numItems);
	for (i=0; i<numItems; i++)
	{
		int checked;
		IsListItemChecked (panelHandle, PANEL_STATE, i, &checked);
		if (checked)
		{
			UpdateState(i, &state);
		}
	}
	return state;
}

void UpdateState(int index, int* state)
{
	switch(index)
	{
		case 0:
			*state |= VAL_SELECTED;
			break;
		case 1:
			*state |= VAL_UNSELECTED;
			break;
		case 2:
			*state |= VAL_EXPOSED;
			break;
		case 3:
			*state |= VAL_UNEXPOSED;
			break;
		case 4:
			*state |= VAL_MARKED;
			break;
		case 5:
			*state |= VAL_UNMARKED;
			break;
		case 6:
			*state |= VAL_PARTIALLY_MARKED;
			break;
		case 7:
			*state |= VAL_EXPANDED;
			break;
		case 8:
			*state |= VAL_COLLAPSED;
			break;
	}
}

// Deselects in user interface all the items that were previously selected, but not currently selected.
// Note you cannot simply clear all the previous selections before selecting new items because VAL_SELECTED or 
// VAL_UNSELECTED state criteria would not work correctly.
void DeselectPrevOnlySelected()
{
	int i;
	for (i=0; i<numPrevSelectedItems; i++)
	{
		if (!IsCurrentSelect(prevSelectedItems[i]))
		{
			SetTreeItemAttribute (panelHandle, PANEL_TREE, prevSelectedItems[i], ATTR_SELECTED, 0);
		}
	}
}

int IsCurrentSelect(int index)
{
	int i;
	for (i=0; i<numCurrentSelectedItems; i++)
	{
		if (currentSelectedItems[i] == index)
		{
			return 1;
		}
	}
	return 0;
}

void GetAllSelectedItems()
{
	int item = 0, beginIndex = 0, direction = VAL_NEXT_PLUS_SELF, firstIteration = 1;
	numPrevSelectedItems = 0;
	while (item!=-1)
	{
		GetTreeItem (panelHandle, PANEL_TREE, VAL_ALL, 0, beginIndex,
					 direction, VAL_SELECTED, &item);
		beginIndex = item;
		if (firstIteration)
		{
			direction = VAL_NEXT;  
			firstIteration = 0;
		}
		if (item!=-1)
		{
			prevSelectedItems[numPrevSelectedItems++] = item;
		}
	}
}

void SelectMarkedItems()
{
	int i;
	for (i=0; i<numMarkForSelectLaterItems; i++)
	{
		MakeTreeItemVisible (panelHandle, PANEL_TREE, markForSelectLaterItems[i]);
		SetTreeItemAttribute (panelHandle, PANEL_TREE, markForSelectLaterItems[i], ATTR_SELECTED, 1);
	}
}

void HighlightTreeItem(int item)
{
	int exposed;
	IsTreeItemExposed (panelHandle, PANEL_TREE, item, &exposed);
	if (!exposed)
	{
		if (state & VAL_UNEXPOSED) // cannot expose item yet, until we check all other unexposed items that would get exposed by calling MakeTreeItemVisible  
		{
			markForSelectLaterItems[numMarkForSelectLaterItems++] = item;
		}
		else 
		{
			MakeTreeItemVisible (panelHandle, PANEL_TREE, item); // First, unexpose item.
			SetTreeItemAttribute (panelHandle, PANEL_TREE, item, ATTR_SELECTED, 1);
			currentSelectedItems[numCurrentSelectedItems++] = item;
		}
	}
	else
	{
		SetTreeItemAttribute (panelHandle, PANEL_TREE, item, ATTR_SELECTED, 1);
		currentSelectedItems[numCurrentSelectedItems++] = item;
	}
}

// If the active item is not selected, it makes the first selected item active
void ResetActiveItem(int activeIndex)
{
	if (!IsCurrentSelect(activeIndex)) 
	{
		int item;
		GetTreeItem (panelHandle, PANEL_TREE, VAL_ALL, 0, VAL_FIRST,
					 VAL_NEXT_PLUS_SELF, 0, &item);
		SetActiveTreeItem (panelHandle, PANEL_TREE, item,
						   VAL_NO_SELECTION_EFFECT);
	}
}



int CVICALLBACK PanelCallback (int panel, int event, void *callbackData,
							   int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			QuitUserInterface (0);
			break;
	}
	return 0;
}
