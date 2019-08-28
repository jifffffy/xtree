//==============================================================================
//
// Title:		xmltree
// Purpose:		A short description of the interface.
//
// Created on:	2019/8/22 at 14:36:06 by sun.
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __xmltree_H__
#define __xmltree_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants

//==============================================================================
// Types
typedef struct XTreeStruct *XTree;
typedef struct CriteriaStruct *Criteria;
//==============================================================================
// External variables

//==============================================================================
// Global functions

int NewXTree(XTree *pTree);
int NewCriteria(int relation, int relativeIndex, int beginIndex, int direction, int state, Criteria *pCriteria);
int CreateXTree(int panel, int tree, XTree *pTree);  
int Destory(XTree xTree); 
int Connect(XTree xTree, char *path);   
int Debug(XTree xTree);
int SetItemActive(XTree xTree, int activeItem); 
int SearchByPath(XTree xTree, char *path, Criteria criteria, int *item);
int SearchByValue(XTree xTree, char *search, Criteria criteria, int *item);
int GetAttributeValue(XTree xTree, Criteria criteria, char *label, char *attribute, char *value);
int GetLabelValue(XTree xTree, Criteria criteria, char *label, char *value); 
#ifdef __cplusplus
    }
#endif

#endif  /* ndef __xmltree_H__ */
