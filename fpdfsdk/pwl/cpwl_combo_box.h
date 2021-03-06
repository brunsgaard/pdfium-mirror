// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_PWL_CPWL_COMBO_BOX_H_
#define FPDFSDK_PWL_CPWL_COMBO_BOX_H_

#include <memory>

#include "core/fxcrt/unowned_ptr.h"
#include "fpdfsdk/pwl/cpwl_edit.h"
#include "fpdfsdk/pwl/cpwl_list_box.h"
#include "fpdfsdk/pwl/cpwl_wnd.h"

class CPWL_CBListBox : public CPWL_ListBox {
 public:
  CPWL_CBListBox() {}
  ~CPWL_CBListBox() override {}

  // CPWL_ListBox
  bool OnLButtonUp(const CFX_PointF& point, uint32_t nFlag) override;

  bool IsMovementKey(uint16_t nChar) const;
  bool OnMovementKeyDown(uint16_t nChar, uint32_t nFlag);
  bool IsChar(uint16_t nChar, uint32_t nFlag) const;
  bool OnCharNotify(uint16_t nChar, uint32_t nFlag);
};

class CPWL_CBButton : public CPWL_Wnd {
 public:
  CPWL_CBButton() {}
  ~CPWL_CBButton() override {}

  // CPWL_Wnd
  void DrawThisAppearance(CFX_RenderDevice* pDevice,
                          const CFX_Matrix& mtUser2Device) override;
  bool OnLButtonDown(const CFX_PointF& point, uint32_t nFlag) override;
  bool OnLButtonUp(const CFX_PointF& point, uint32_t nFlag) override;
};

class CPWL_ComboBox : public CPWL_Wnd {
 public:
  CPWL_ComboBox();
  ~CPWL_ComboBox() override;

  CPWL_Edit* GetEdit() const { return m_pEdit.Get(); }

  // CPWL_Wnd:
  ByteString GetClassName() const override;
  void OnCreate(CreateParams* pParamsToAdjust) override;
  void OnDestroy() override;
  bool OnKeyDown(uint16_t nChar, uint32_t nFlag) override;
  bool OnChar(uint16_t nChar, uint32_t nFlag) override;
  void NotifyLButtonDown(CPWL_Wnd* child, const CFX_PointF& pos) override;
  void NotifyLButtonUp(CPWL_Wnd* child, const CFX_PointF& pos) override;
  void CreateChildWnd(const CreateParams& cp) override;
  bool RePosChildWnd() override;
  CFX_FloatRect GetFocusRect() const override;
  void SetFocus() override;
  void KillFocus() override;
  WideString GetSelectedText() override;
  void ReplaceSelection(const WideString& text) override;

  void SetFillerNotify(IPWL_Filler_Notify* pNotify);

  WideString GetText() const;
  void SetText(const WideString& text);
  void AddString(const WideString& str);
  int32_t GetSelect() const;
  void SetSelect(int32_t nItemIndex);

  void SetEditSelection(int32_t nStartChar, int32_t nEndChar);
  void GetEditSelection(int32_t& nStartChar, int32_t& nEndChar) const;
  void ClearSelection();
  void SelectAll();
  bool IsPopup() const;

  void SetSelectText();

  void AttachFFLData(CFFL_FormFiller* pData) { m_pFormFiller = pData; }

 private:
  void CreateEdit(const CreateParams& cp);
  void CreateButton(const CreateParams& cp);
  void CreateListBox(const CreateParams& cp);

  // Returns |true| iff this instance is still allocated.
  bool SetPopup(bool bPopup);

  UnownedPtr<CPWL_Edit> m_pEdit;
  UnownedPtr<CPWL_CBButton> m_pButton;
  UnownedPtr<CPWL_CBListBox> m_pList;
  CFX_FloatRect m_rcOldWindow;
  bool m_bPopup = false;
  bool m_bBottom = true;
  int32_t m_nSelectItem = -1;
  UnownedPtr<IPWL_Filler_Notify> m_pFillerNotify;
  UnownedPtr<CFFL_FormFiller> m_pFormFiller;
};

#endif  // FPDFSDK_PWL_CPWL_COMBO_BOX_H_
