//==============================================================================
//
// Title:		xmltree
// Purpose:		A short description of the library.
//
// Created on:	2019/8/22 at 14:36:06 by sun.
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files
#include <cvixml.h>
#include <cvirte.h>
#include <userint.h>
#include <pathctrl.h>
#include <utility.h>
#include "xmltree.h"

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
#ifdef errChk
#undef errChk
#endif

#define errChk(fCall) do { if ((error = (fCall)) < 0) goto Error; } while (0)
	
#ifndef xmlChk
#define xmlChk(f) if (xmlErr = (f), FAILED (xmlErr)) goto Error; else
#endif
	
#define handleError do { if (error < 0 && error != kErrorAlreadyHandled) { MessagePopup("Error", GetGeneralErrorString(error)); error = kErrorAlreadyHandled; } } while (0)
//==============================================================================
// Constants
#define MAX_SIZE 1024
static const unsigned int   kExpectedOccupancy      = 20;
static const int            kErrorAlreadyHandled    = UIEErrorLimit - 1;
//==============================================================================
// Types
typedef struct XTreeStruct{
	int		panel;
	int		tree;
} XTreeStruct;

typedef struct CriteriaStruct{
	int relation;
	int relativeIndex;
	int beginIndex;
	int direction;
	int state;
} CriteriaStruct;

//==============================================================================
// Static global variables
static HashTableType hashTable = NULL;

//==============================================================================
// Static functions
static int ProcessXMLElement (int panel, int tree, int itemIndex, CVIXMLElement element);
static int ProcessXMLAttribute (int panel, int tree, int itemIndex, CVIXMLAttribute attribute);
static int NewXTree(XTree *pTree);
static int CreateXTree(int panel, int tree, XTree *pTree);
static int NewCriteria(int relation, int relativeIndex, int beginIndex, int direction, int state, Criteria *pCriteria); 
static int Destory(XTree xTree); 
static int Connect(XTree xTree, char *path);   
static int Debug(XTree xTree);
static int SearchByValue(XTree xTree, char *search, Criteria criteria, int *item);
static int XTreeConnect(XTree xTree, char *path);
static int Debug(XTree xTree);
static int ConvertAndRenderXMLFileInTree (int panel, int tree, const char *filePath); 
static int CVICALLBACK Shutdown (int panel, int event, void *callbackData,int eventData1, int eventData2);
static int Destory(XTree xTree); 
static int SetItemActive(XTree xTree, int activeItem);
static int GetAttributeValue(XTree xTree, Criteria criteria, char *label, char *attribute, char *value);
static int GetLabelValue(XTree xTree, Criteria criteria, char *label, char *value); 
static int GetItemPath(XTree xTree,int activeIndex, char **path);
static int SearchByPath(XTree xTree, char *path, Criteria criteria, int *item); 
static int SearchByLable(XTree xTree, char *label, Criteria criteria, HashTableType ht);
//==============================================================================
// Global variables

//==============================================================================
// Global functions
int NewXTree(XTree *pTree) 
{
	int width, height;
	int error = 0;
	XTree xTree = malloc(sizeof(XTreeStruct));
	/* Create a new panel and center it */
	GetScreenSize(&width, &height);
    xTree->panel = NewPanel (0, "Sample LabWindows/CVI Panel", VAL_AUTO_CENTER,VAL_AUTO_CENTER, width, height);
    SetPanelAttribute (xTree->panel, ATTR_CONFORM_TO_SYSTEM, 1);
	InstallPanelCallback(xTree->panel, Shutdown, 0);
    /* Create a new "Generate Data" tree control */
    xTree->tree = NewCtrl (xTree->panel, CTRL_TREE_LS,"XTree", 0, 0);
	SetCtrlAttribute (xTree->panel, xTree->tree, ATTR_WIDTH, width); 
	SetCtrlAttribute (xTree->panel, xTree->tree, ATTR_HEIGHT, height);
	SetCtrlAttribute (xTree->panel, xTree->tree, ATTR_COLUMN_LABELS_VISIBLE, 1);
	SetCtrlAttribute (xTree->panel, xTree->tree, ATTR_SELECTION_MODE, VAL_SELECTION_SINGLE);

	(*pTree) = xTree;
	errChk(HashTableCreate(kExpectedOccupancy, C_STRING_KEY, 0 ,sizeof(int), &hashTable));

Error:
    handleError;
    return error;
}

int CreateXTree(int panel, int tree, XTree *pTree)  
{
	int error   = 0;
	XTree xTree = malloc(sizeof(XTreeStruct));
	xTree -> panel = panel;
	xTree -> tree = tree;
	(*pTree) = xTree;
	errChk(HashTableCreate(kExpectedOccupancy, C_STRING_KEY, 0 ,sizeof(int), &hashTable));
Error:
    handleError;
    return error;
}

int NewCriteria(int relation, int relativeIndex, int beginIndex, int direction, int state, Criteria *pCriteria)
{
	 int error   = 0;
	 Criteria criteria = malloc(sizeof(CriteriaStruct)); 
	 criteria->relation = relation;
	 criteria->relativeIndex = relativeIndex;
	 criteria->beginIndex = beginIndex;
	 criteria->direction = direction;
	 criteria->state = state;
	 (*pCriteria) = criteria;
Error:
    handleError;
    return error;	 
}

int Destory(XTree xTree)
{
	int error   = 0;
	 free(xTree);
	 errChk (HashTableDispose(hashTable));
     hashTable = NULL;
Error:
    handleError;
    return error;
}

int Connect(XTree xTree, char *path) 
{
	int		error = 0; 
	int		numCols=0;
	char	key[256];
	errChk(ConvertAndRenderXMLFileInTree(xTree -> panel, xTree -> tree, path));
	errChk (GetNumTreeColumns (xTree -> panel, xTree -> tree, &numCols));
	for(int i=1; i<numCols; i++)
	{
		errChk(GetTreeColumnAttribute(xTree -> panel, xTree -> tree, i, ATTR_LABEL_TEXT, key));
		DebugPrintf("key=%s, value=%d\r\n", key, i); 
		errChk(HashTableInsertItem(hashTable, key, &i));
	}
	 
Error:
    handleError;
    return error;
}

int Debug(XTree xTree)
{
	 DisplayPanel(xTree -> panel);
	 RunUserInterface();
    return 0;
}

int GetItemPath(XTree xTree,int activeIndex, char **path)
{
	int error = 0;
	int pathLength;
	char *itemPath = NULL;
	errChk(GetTreeItemPathLength (xTree->panel, xTree->tree, activeIndex, ">", &pathLength));
	itemPath = malloc ((pathLength + 1) * sizeof (char));
	errChk(GetTreeItemPath (xTree->panel, xTree->tree, activeIndex, ">", itemPath));
	(*path) = itemPath;
	
Error:
	handleError;
	return error;
}

int SearchByPath(XTree xTree, char *path, Criteria criteria, int *item)
{
	int error = 0;
	char *labels = NULL;
	char *label = NULL;
	labels = StrDup(path);
	char *token = NULL;
	int value = 0;
	HashTableType ht = NULL;
	HashTableCreate(kExpectedOccupancy, C_STRING_KEY, 0 ,sizeof(int), &ht);
	/* get the first token */
	token = strtok(labels,">");
	while(token!=NULL)
	{
		label =  token; 
		token = strtok(NULL,">");
	}
	if(label == NULL)
	{
		goto Error;
	}
	errChk(SearchByLable(xTree,label, criteria, ht));
	errChk(HashTableGetItem(ht, path, &value, sizeof(int)));
	(*item) = value;
Error:
	handleError;
	if(ht)
	{
		HashTableDispose(ht);
	}
	return error;
}

int SearchByLable(XTree xTree, char *label, Criteria criteria, HashTableType ht)
{
	int error = 0;
	int index = 0;
	int relation = criteria->relation,
		relativeIndex =  criteria->relativeIndex,
		beginIndex= criteria->beginIndex,
		direction = criteria->direction,
		state = criteria->state,
		anyLabel = 0;
	int firstIteration = 1;
	char *path = NULL;
	
	anyLabel = !strlen(label);
	while (index!=-1)
	{
		if (anyLabel)
		{
			GetTreeItem (xTree->panel, xTree->tree, relation, relativeIndex,
						 beginIndex, direction, state, &index);
		}
		else
		{
			GetTreeItemFromLabel (xTree->panel, xTree->tree, relation, relativeIndex,
								  beginIndex, direction, state, label, &index);
		}

		beginIndex = index;
		if (index!=-1)
		{
			errChk(GetItemPath(xTree, index, &path));
			errChk(HashTableInsertItem(ht, path, &index));
		}
		
		if (firstIteration)
		{
			direction = VAL_NEXT;
			firstIteration = 0;
		}
	}
Error:
	handleError;
	return error;
}

int SearchByValue(XTree xTree, char *search, Criteria criteria, int *item)
{
	int error = 0; 
	errChk(GetTreeItemFromValue (xTree->panel, xTree->tree, 
						  criteria->relation, 
						  criteria->relativeIndex,
						  criteria->beginIndex, 
						  criteria->direction, 
						  criteria->state, 
						  item, search));
Error:
    handleError;
    return error;
}

int SetItemActive(XTree xTree, int activeItem)
{
	int error = 0;
	errChk(SetActiveTreeItem (xTree->panel, xTree->tree, activeItem, VAL_REPLACE_SELECTION_WITH_ITEM));
Error:
    handleError;
    return error;
}

int GetLabelValue(XTree xTree, Criteria criteria, char *label, char *value)
{
	int error = 0;
	int item = 0;
	errChk(SearchByPath(xTree, label, criteria, &item)); 
	errChk(GetTreeCellAttribute(xTree->panel, xTree->tree,item, 1, ATTR_LABEL_TEXT, value)); 
Error:
    handleError;
    return error;
}

int GetAttributeValue(XTree xTree, Criteria criteria, char *label, char *attribute, char *value)
{
	int error = 0;
	int item = 0;
	int column;
	errChk(SearchByLable(xTree, label, criteria, &item));
	errChk(HashTableGetItem(hashTable, attribute, &column, sizeof(int)));
	errChk(GetTreeCellAttribute(xTree->panel, xTree->tree,item, column, ATTR_LABEL_TEXT, value));
Error:
    handleError;
    return error;
}

/*---------------------------------------------------------------------------*/
/* Quit the UI loop.                                                         */
/*---------------------------------------------------------------------------*/
int CVICALLBACK Shutdown (int panel, int event, void *callbackData,
											  int eventData1, int eventData2)
{
    if (event == EVENT_CLOSE)
        QuitUserInterface(0);
    return(0);
}
/********************************************************************************/
/* Parses the XML file and populates tree control with XML data 				*/
/********************************************************************************/
int ConvertAndRenderXMLFileInTree (int panel, int tree, const char *filePath)
{
    int					error = 0;
    CVIXMLStatus		xmlErr = S_OK;
    CVIXMLDocument		document = 0;
    CVIXMLElement		currElem = 0;
    int					numCols, itemIndex, len;
    char				*elemName = NULL, *elemValue = NULL;
    errChk (SetWaitCursor (1));

    /* Clear the tree control */
    errChk (ClearListCtrl (panel, tree));
    errChk (GetNumTreeColumns (panel, tree, &numCols));
    while (numCols > 1)
        errChk (DeleteTreeColumn (panel, tree, --numCols));
    errChk (InsertTreeColumn (panel, tree, 1, "Value"));

    /* Load document and process it */
    xmlChk (CVIXMLLoadDocument (filePath, &document));
    xmlChk (CVIXMLGetRootElement (document, &currElem));

    xmlChk (CVIXMLGetElementTagLength (currElem, &len));
    nullChk (elemName = malloc (len + 1));
    xmlChk (CVIXMLGetElementTag (currElem, elemName));
    xmlChk (CVIXMLGetElementValueLength (currElem, &len));
    nullChk (elemValue = malloc (len + 1));
    xmlChk (CVIXMLGetElementValue (currElem, elemValue));
    RemoveSurroundingWhiteSpace (elemValue);
    errChk (InsertTreeItem (panel, tree, VAL_SIBLING, 0, VAL_NEXT, elemName, 0, 0, 0));
    itemIndex = error;
    errChk (SetTreeCellAttribute (panel, tree, itemIndex, 1, ATTR_LABEL_TEXT, elemValue));
    errChk (ProcessXMLElement (panel, tree, itemIndex, currElem));
    errChk (GetNumTreeColumns (panel, tree, &numCols));
    while (numCols > 0)
        errChk (SetColumnWidthToWidestCellContents (panel, tree, --numCols));

    Error:
    SetWaitCursor (0);
    free (elemName);
    free (elemValue);
    CVIXMLDiscardElement (currElem);
    CVIXMLDiscardDocument (document);
    if (FAILED (xmlErr)) {
        char	errBuf[512];
        CVIXMLGetErrorString (xmlErr, errBuf, sizeof (errBuf));
        MessagePopup ("XML Error", errBuf);
    }
    return error;
}


/********************************************************************************/
/* Process the input element's attributes and child elements 					*/
/********************************************************************************/
static int ProcessXMLElement (int panel, int tree, int itemIndex, CVIXMLElement element)
{
    int					error = 0;
    CVIXMLStatus		xmlErr = S_OK;
    int					index, numAttrs, numChildElems, childItemIndex, len;
    CVIXMLAttribute		currAttr = 0;
    CVIXMLElement		currChildElem = 0;
    char				*elemName = NULL, *elemValue = NULL;
    /* Process attributes */
    xmlChk (CVIXMLGetNumAttributes (element, &numAttrs));
    for (index = 0; index < numAttrs; ++index) {
        xmlChk (CVIXMLGetAttributeByIndex (element, index, &currAttr));
        errChk (ProcessXMLAttribute (panel, tree, itemIndex, currAttr));
        CVIXMLDiscardAttribute (currAttr);
        currAttr = 0;
    }

    /* Process child elements */
    xmlChk (CVIXMLGetNumChildElements (element, &numChildElems));
    for (index = 0; index < numChildElems; ++index) {
        xmlChk (CVIXMLGetChildElementByIndex (element, index, &currChildElem));
        xmlChk (CVIXMLGetElementTagLength (currChildElem, &len));
        nullChk (elemName = malloc (len + 1));
        xmlChk (CVIXMLGetElementTag (currChildElem, elemName));
        xmlChk (CVIXMLGetElementValueLength (currChildElem, &len));
        nullChk (elemValue = malloc (len + 1));
        xmlChk (CVIXMLGetElementValue (currChildElem, elemValue));
        RemoveSurroundingWhiteSpace (elemValue);
        errChk (InsertTreeItem (panel, tree, VAL_CHILD, itemIndex, VAL_LAST, elemName, 0, 0,  0));
        childItemIndex = error;
        errChk (SetTreeCellAttribute (panel, tree, childItemIndex, 1, ATTR_LABEL_TEXT, elemValue));
        free (elemName);
        free (elemValue);
        elemName = elemValue = NULL;
        /* NOTE: Recursion! Make sure stack size can handle input data */
        errChk (ProcessXMLElement (panel, tree, childItemIndex, currChildElem));
        CVIXMLDiscardElement (currChildElem);
        currChildElem = 0;
    }

    Error:
    free (elemName);
    free (elemValue);
    CVIXMLDiscardAttribute (currAttr);
    CVIXMLDiscardElement (currChildElem);
    if (FAILED (xmlErr)) {
        char	errBuf[512];
        CVIXMLGetErrorString (xmlErr, errBuf, sizeof (errBuf));
        MessagePopup ("XML Error", errBuf);
    }
    return error;
}

/********************************************************************************/
/* Process the input XML attribute												*/
/********************************************************************************/
static int ProcessXMLAttribute (int panel, int tree, int itemIndex, CVIXMLAttribute attribute)
{
    int				error = 0;
    CVIXMLStatus	xmlErr = S_OK;
    char			attrName[MAX_SIZE], *attrValue = NULL, colName[MAX_SIZE];
    int				numCols, colIndex, len;

    xmlChk (CVIXMLGetAttributeName (attribute, attrName));
    xmlChk (CVIXMLGetAttributeValueLength (attribute, &len));
    nullChk (attrValue = malloc (len + 1));
    xmlChk (CVIXMLGetAttributeValue (attribute, attrValue));
    RemoveSurroundingWhiteSpace (attrValue);

    /* Check if column with same name exists */
    errChk (GetNumTreeColumns (panel, tree, &numCols));
    assert (numCols >= 2);
    for (colIndex = 2; colIndex < numCols; ++colIndex) {
        errChk (GetTreeColumnAttribute (panel, tree, colIndex, ATTR_LABEL_TEXT, colName));
        if (_mbscmp ((unsigned char*)colName, (unsigned char*)attrName) == 0)
            break;
    }
    if (colIndex >= numCols) /* Column does not exist, create new one */
        errChk (InsertTreeColumn (panel, tree, colIndex, attrName));

    assert (colIndex > 1);
    errChk (SetTreeCellAttribute (panel, tree, itemIndex, colIndex, ATTR_LABEL_TEXT, attrValue));

    Error:
    free (attrValue);
    if (FAILED (xmlErr)) {
        char	errBuf[512];
        CVIXMLGetErrorString (xmlErr, errBuf, sizeof (errBuf));
        MessagePopup ("XML Error", errBuf);
    }
    return error;
}

//==============================================================================
// DLL main entry-point functions

int __stdcall DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason) {
		case DLL_PROCESS_ATTACH:
			if (InitCVIRTE (hinstDLL, 0, 0) == 0)
				return 0;	  /* out of memory */
			break;
		case DLL_PROCESS_DETACH:
			CloseCVIRTE ();
			break;
	}
	
	return 1;
}

int __stdcall DllEntryPoint (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	/* Included for compatibility with Borland */

	return DllMain (hinstDLL, fdwReason, lpvReserved);
}
