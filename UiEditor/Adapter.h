﻿#pragma once
#include "helper/SAdapterBase.h"

#define DEFAULT_LINE 10
class CBaseMcAdapterFix : public SMcAdapterBase
{
	SArray<SStringT> m_colNames;
public:
	CBaseMcAdapterFix()
	{		
	}

	virtual int getCount()
	{
		return DEFAULT_LINE;
	}

	virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate)
	{
		if (pItem->GetChildrenCount() == 0)
		{
			pItem->InitFromXml(xmlTemplate);
		}		
	}

	SStringW GetColumnName(int iCol) const {
		return m_colNames[iCol];
	}

	void IniColNames(pugi::xml_node xmlTemplate)
	{		
		for (xmlTemplate = xmlTemplate.first_child(); xmlTemplate; xmlTemplate = xmlTemplate.next_sibling())
		{
			m_colNames.Add(xmlTemplate.attribute(L"name").value());
		}
	}

	virtual void InitByTemplate(pugi::xml_node xmlTemplate)
	{
		IniColNames(xmlTemplate);
	}
};


class CBaseAdapterFix : public SAdapterBase
{	
	const wchar_t*  KAttrName_Height[3];
	SArray<SStringT> m_TemplateNames;
	int m_nItemHeight[3];
public:

	CBaseAdapterFix()
	{
		KAttrName_Height[0] = L"oddHeight";
		KAttrName_Height[1] = L"evenHeight";
		KAttrName_Height[2] = L"evenSelHeight";
	}

	~CBaseAdapterFix()
	{
		
	}

	virtual int getCount()
	{
		return DEFAULT_LINE;
	}
	
	int IniTemplateNames(pugi::xml_node xmlTemplate)
	{
		for (xmlTemplate = xmlTemplate.first_child(); xmlTemplate; xmlTemplate = xmlTemplate.next_sibling())
		{
			//TODO: 此法有待验证
			/*
			if (static_cast<SWindowFactoryMgr*>(SApplication::getSingletonPtr())->HasKey(xmlTemplate.name()))
			{
				return 0;
			}
			*/

			m_TemplateNames.Add(xmlTemplate.name());
		}
		return m_TemplateNames.GetCount();
	}

	virtual void InitByTemplate(pugi::xml_node xmlTemplate)
	{
		if (IniTemplateNames(xmlTemplate) > 0)
		{
			//此处名字是自定义的，view的灵活性也就体现在这些地方。
			m_nItemHeight[0]= xmlTemplate.attribute(KAttrName_Height[0]).as_int(50);
			m_nItemHeight[1] = xmlTemplate.attribute(KAttrName_Height[1]).as_int(60);
			m_nItemHeight[2] = xmlTemplate.attribute(KAttrName_Height[2]).as_int(70);
		}
	}

	virtual int getViewTypeCount() { return m_TemplateNames.GetCount()==0?1:m_TemplateNames.GetCount(); }

	virtual int getItemViewType(int position, DWORD dwState)
	{
		if (m_TemplateNames.GetCount() > 1)
		{
			if (position % 2 == 0)
				return 0;//1,3,5,... odd lines
			else if (dwState & WndState_Hover)
				return 2;//even lines with check state
			else
				return 1;//even lines 
		}
		return __super::getItemViewType(position, dwState);
	}

	virtual SIZE getViewDesiredSize(int position, SWindow *pItem, LPCRECT prcContainer)
	{
		DWORD dwState = pItem->GetState();
		int viewType = getItemViewType(position, dwState);
		return CSize(0, m_nItemHeight[viewType]);//cx在listview，mclistview中没有使用，不需要计算
	}

	virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate)
	{
		if (pItem->GetChildrenCount() == 0)
		{
			if(m_TemplateNames.GetCount()==0)
				pItem->InitFromXml(xmlTemplate);
			else
			{
				int nViewType = getItemViewType(position, pItem->GetState());
				pItem->InitFromXml(xmlTemplate.child(m_TemplateNames[nViewType<m_TemplateNames.GetCount()-1?nViewType: m_TemplateNames.GetCount() - 1]));
			}
		}
	}	
};


