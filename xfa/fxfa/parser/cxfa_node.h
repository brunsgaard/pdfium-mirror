// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_NODE_H_
#define XFA_FXFA_PARSER_CXFA_NODE_H_

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "core/fxcrt/fx_string.h"
#include "core/fxge/fx_dib.h"
#include "fxbarcode/BC_Library.h"
#include "third_party/base/optional.h"
#include "xfa/fxfa/parser/cxfa_object.h"

class CFGAS_GEFont;
class CFX_DIBitmap;
class CFX_XMLNode;
class CXFA_Bind;
class CXFA_Border;
class CXFA_Calculate;
class CXFA_Caption;
class CXFA_Event;
class CXFA_EventParam;
class CXFA_FFDoc;
class CXFA_FFDocView;
class CXFA_FFWidget;
class CXFA_Font;
class CXFA_Margin;
class CXFA_Occur;
class CXFA_Para;
class CXFA_Script;
class CXFA_TextLayout;
class CXFA_Validate;
class CXFA_Value;
class CXFA_WidgetLayoutData;
class IFX_Locale;

#define XFA_NODEFILTER_Children 0x01
#define XFA_NODEFILTER_Properties 0x02
#define XFA_NODEFILTER_OneOfProperty 0x04

enum XFA_CHECKSTATE {
  XFA_CHECKSTATE_On = 0,
  XFA_CHECKSTATE_Off = 1,
  XFA_CHECKSTATE_Neutral = 2,
};

enum XFA_VALUEPICTURE {
  XFA_VALUEPICTURE_Raw = 0,
  XFA_VALUEPICTURE_Display,
  XFA_VALUEPICTURE_Edit,
  XFA_VALUEPICTURE_DataBind,
};

enum XFA_NodeFlag {
  XFA_NodeFlag_None = 0,
  XFA_NodeFlag_Initialized = 1 << 0,
  XFA_NodeFlag_HasRemovedChildren = 1 << 1,
  XFA_NodeFlag_NeedsInitApp = 1 << 2,
  XFA_NodeFlag_BindFormItems = 1 << 3,
  XFA_NodeFlag_UserInteractive = 1 << 4,
  XFA_NodeFlag_SkipDataBinding = 1 << 5,
  XFA_NodeFlag_OwnXMLNode = 1 << 6,
  XFA_NodeFlag_UnusedNode = 1 << 7,
  XFA_NodeFlag_LayoutGeneratedNode = 1 << 8
};

class CXFA_Node : public CXFA_Object {
 public:
  struct PropertyData {
    XFA_Element property;
    uint8_t occurance_count;
    uint8_t flags;
  };

  struct AttributeData {
    XFA_Attribute attribute;
    XFA_AttributeType type;
    void* default_value;
  };

#ifndef NDEBUG
  static WideString ElementToName(XFA_Element elem);
#endif  // NDEBUG

  static WideString AttributeEnumToName(XFA_AttributeEnum item);
  static Optional<XFA_AttributeEnum> NameToAttributeEnum(
      const WideStringView& name);
  static XFA_Attribute NameToAttribute(const WideStringView& name);
  static WideString AttributeToName(XFA_Attribute attr);
  static XFA_Element NameToElement(const WideString& name);
  static std::unique_ptr<CXFA_Node> Create(CXFA_Document* doc,
                                           XFA_Element element,
                                           XFA_PacketType packet);

  ~CXFA_Node() override;

  bool IsValidInPacket(XFA_PacketType packet) const;

  bool HasProperty(XFA_Element property) const;
  bool HasPropertyFlags(XFA_Element property, uint8_t flags) const;
  uint8_t PropertyOccuranceCount(XFA_Element property) const;

  void SendAttributeChangeMessage(XFA_Attribute eAttribute, bool bScriptModify);

  bool HasAttribute(XFA_Attribute attr) const;
  XFA_Attribute GetAttribute(size_t i) const;
  XFA_AttributeType GetAttributeType(XFA_Attribute type) const;

  XFA_PacketType GetPacketType() const { return m_ePacket; }

  void SetFlag(uint32_t dwFlag, bool bNotify);
  void ClearFlag(uint32_t dwFlag);

  CXFA_Node* CreateInstanceIfPossible(bool bDataMerge);
  int32_t GetCount();
  CXFA_Node* GetItemIfExists(int32_t iIndex);
  void RemoveItem(CXFA_Node* pRemoveInstance, bool bRemoveDataBinding);
  void InsertItem(CXFA_Node* pNewInstance,
                  int32_t iPos,
                  int32_t iCount,
                  bool bMoveDataBindingNodes);

  bool IsInitialized() const { return HasFlag(XFA_NodeFlag_Initialized); }
  bool IsOwnXMLNode() const { return HasFlag(XFA_NodeFlag_OwnXMLNode); }
  bool IsUserInteractive() const {
    return HasFlag(XFA_NodeFlag_UserInteractive);
  }
  bool IsUnusedNode() const { return HasFlag(XFA_NodeFlag_UnusedNode); }
  bool IsLayoutGeneratedNode() const {
    return HasFlag(XFA_NodeFlag_LayoutGeneratedNode);
  }

  void SetBindingNodes(std::vector<UnownedPtr<CXFA_Node>> nodes) {
    binding_nodes_ = std::move(nodes);
  }
  std::vector<UnownedPtr<CXFA_Node>>* GetBindingNodes() {
    return &binding_nodes_;
  }
  void SetBindingNode(CXFA_Node* node) {
    binding_nodes_.clear();
    if (node)
      binding_nodes_.emplace_back(node);
  }
  CXFA_Node* GetBindingNode() const {
    if (binding_nodes_.empty())
      return nullptr;
    return binding_nodes_[0].Get();
  }
  // TODO(dsinclair): This should not be needed. Nodes should get un-bound when
  // they're deleted instead of us pointing to bad objects.
  void ReleaseBindingNodes();

  bool BindsFormItems() const { return HasFlag(XFA_NodeFlag_BindFormItems); }
  bool HasRemovedChildren() const {
    return HasFlag(XFA_NodeFlag_HasRemovedChildren);
  }
  bool NeedsInitApp() const { return HasFlag(XFA_NodeFlag_NeedsInitApp); }

  bool IsAttributeInXML();
  bool IsFormContainer() const {
    return m_ePacket == XFA_PacketType::Form && IsContainerNode();
  }
  void SetXMLMappingNode(CFX_XMLNode* pXMLNode) { m_pXMLNode = pXMLNode; }
  CFX_XMLNode* GetXMLMappingNode() const { return m_pXMLNode; }
  CFX_XMLNode* CreateXMLMappingNode();
  bool IsNeedSavingXMLNode();
  uint32_t GetNameHash() const { return m_dwNameHash; }
  bool IsUnnamed() const { return m_dwNameHash == 0; }
  CXFA_Node* GetModelNode();
  void UpdateNameHash();

  size_t CountChildren(XFA_Element eType, bool bOnlyChild);

  template <typename T>
  T* GetChild(size_t index, XFA_Element eType, bool bOnlyChild) {
    return static_cast<T*>(GetChildInternal(index, eType, bOnlyChild));
  }

  int32_t InsertChild(int32_t index, CXFA_Node* pNode);
  bool InsertChild(CXFA_Node* pNode, CXFA_Node* pBeforeNode);
  bool RemoveChild(CXFA_Node* pNode, bool bNotify);

  CXFA_Node* Clone(bool bRecursive);

  CXFA_Node* GetNextSibling() const { return m_pNext; }
  CXFA_Node* GetPrevSibling() const;
  CXFA_Node* GetFirstChild() const { return m_pChild; }
  CXFA_Node* GetParent() const { return m_pParent; }

  CXFA_Node* GetNextContainerSibling() const;
  CXFA_Node* GetPrevContainerSibling() const;
  CXFA_Node* GetFirstContainerChild() const;
  CXFA_Node* GetContainerParent() const;

  std::vector<CXFA_Node*> GetNodeList(uint32_t dwTypeFilter,
                                      XFA_Element eTypeFilter);
  CXFA_Node* CreateSamePacketNode(XFA_Element eType);
  CXFA_Node* CloneTemplateToForm(bool bRecursive);
  CXFA_Node* GetTemplateNodeIfExists() const;
  void SetTemplateNode(CXFA_Node* pTemplateNode);
  CXFA_Node* GetDataDescriptionNode();
  void SetDataDescriptionNode(CXFA_Node* pDataDescriptionNode);
  CXFA_Node* GetBindData();
  std::vector<UnownedPtr<CXFA_Node>>* GetBindItems();
  int32_t AddBindItem(CXFA_Node* pFormNode);
  int32_t RemoveBindItem(CXFA_Node* pFormNode);
  bool HasBindItem();
  CXFA_Node* GetContainerNode();
  IFX_Locale* GetLocale();
  Optional<WideString> GetLocaleName();
  XFA_AttributeEnum GetIntact();

  CXFA_Node* GetFirstChildByName(const WideStringView& wsNodeName) const;
  CXFA_Node* GetFirstChildByName(uint32_t dwNodeNameHash) const;
  template <typename T>
  T* GetFirstChildByClass(XFA_Element eType) const {
    return static_cast<T*>(GetFirstChildByClassInternal(eType));
  }
  CXFA_Node* GetNextSameNameSibling(uint32_t dwNodeNameHash) const;
  template <typename T>
  T* GetNextSameNameSibling(const WideStringView& wsNodeName) const {
    return static_cast<T*>(GetNextSameNameSiblingInternal(wsNodeName));
  }
  template <typename T>
  T* GetNextSameClassSibling(XFA_Element eType) const {
    return static_cast<T*>(GetNextSameClassSiblingInternal(eType));
  }

  int32_t GetNodeSameNameIndex() const;
  int32_t GetNodeSameClassIndex() const;
  CXFA_Node* GetInstanceMgrOfSubform();

  CXFA_Occur* GetOccurIfExists();

  Optional<bool> GetDefaultBoolean(XFA_Attribute attr) const;
  Optional<int32_t> GetDefaultInteger(XFA_Attribute attr) const;
  Optional<CXFA_Measurement> GetDefaultMeasurement(XFA_Attribute attr) const;
  Optional<WideString> GetDefaultCData(XFA_Attribute attr) const;
  Optional<XFA_AttributeEnum> GetDefaultEnum(XFA_Attribute attr) const;

  void SyncValue(const WideString& wsValue, bool bNotify);

  bool IsOpenAccess();

  CXFA_Border* GetBorderIfExists() const;
  CXFA_Border* GetOrCreateBorderIfPossible();
  CXFA_Caption* GetCaptionIfExists() const;

  CXFA_Font* GetFontIfExists() const;
  CXFA_Font* GetOrCreateFontIfPossible();
  float GetFontSize() const;
  FX_ARGB GetTextColor() const;
  float GetLineHeight() const;

  CXFA_Margin* GetMarginIfExists() const;
  CXFA_Para* GetParaIfExists() const;
  CXFA_Calculate* GetCalculateIfExists() const;
  CXFA_Validate* GetValidateIfExists() const;
  CXFA_Validate* GetOrCreateValidateIfPossible();

  CXFA_Value* GetDefaultValueIfExists();
  CXFA_Value* GetFormValueIfExists() const;
  WideString GetRawValue();
  int32_t GetRotate();

  CXFA_Bind* GetBindIfExists() const;

  Optional<float> TryWidth();
  Optional<float> TryHeight();
  Optional<float> TryMinWidth();
  Optional<float> TryMinHeight();
  Optional<float> TryMaxWidth();
  Optional<float> TryMaxHeight();

  CXFA_Node* GetExclGroupIfExists();

  int32_t ProcessEvent(CXFA_FFDocView* docView,
                       XFA_AttributeEnum iActivity,
                       CXFA_EventParam* pEventParam);
  int32_t ProcessEvent(CXFA_FFDocView* docView,
                       CXFA_Event* event,
                       CXFA_EventParam* pEventParam);
  int32_t ProcessCalculate(CXFA_FFDocView* docView);
  int32_t ProcessValidate(CXFA_FFDocView* docView, int32_t iFlags);

  int32_t ExecuteScript(CXFA_FFDocView* docView,
                        CXFA_Script* script,
                        CXFA_EventParam* pEventParam);
  std::pair<int32_t, bool> ExecuteBoolScript(CXFA_FFDocView* docView,
                                             CXFA_Script* script,
                                             CXFA_EventParam* pEventParam);

  // TODO(dsinclair): Figure out how to move this to cxfa_barcode.
  WideString GetBarcodeType();
  Optional<BC_CHAR_ENCODING> GetBarcodeAttribute_CharEncoding();
  Optional<bool> GetBarcodeAttribute_Checksum();
  Optional<int32_t> GetBarcodeAttribute_DataLength();
  Optional<char> GetBarcodeAttribute_StartChar();
  Optional<char> GetBarcodeAttribute_EndChar();
  Optional<int32_t> GetBarcodeAttribute_ECLevel();
  Optional<int32_t> GetBarcodeAttribute_ModuleWidth();
  Optional<int32_t> GetBarcodeAttribute_ModuleHeight();
  Optional<bool> GetBarcodeAttribute_PrintChecksum();
  Optional<BC_TEXT_LOC> GetBarcodeAttribute_TextLocation();
  Optional<bool> GetBarcodeAttribute_Truncate();
  Optional<int8_t> GetBarcodeAttribute_WideNarrowRatio();

  CXFA_Node* GetUIChild();
  XFA_Element GetUIType();
  CFX_RectF GetUIMargin();
  CXFA_Border* GetUIBorder();

  bool IsPreNull() const { return m_bPreNull; }
  void SetPreNull(bool val) { m_bPreNull = val; }
  bool IsNull() const { return m_bIsNull; }
  void SetIsNull(bool val) { m_bIsNull = val; }

  void SetWidgetReady() { is_widget_ready_ = true; }
  bool IsWidgetReady() const { return is_widget_ready_; }

  std::vector<CXFA_Event*> GetEventByActivity(XFA_AttributeEnum iActivity,
                                              bool bIsFormReady);

  void ResetData();

  CXFA_FFWidget* GetNextWidget(CXFA_FFWidget* pWidget);
  void StartWidgetLayout(CXFA_FFDoc* doc,
                         float& fCalcWidth,
                         float& fCalcHeight);
  bool FindSplitPos(CXFA_FFDocView* docView,
                    int32_t iBlockIndex,
                    float& fCalcHeight);

  bool LoadCaption(CXFA_FFDoc* doc);
  CXFA_TextLayout* GetCaptionTextLayout();

  void LoadText(CXFA_FFDoc* doc);
  CXFA_TextLayout* GetTextLayout();

  bool LoadImageImage(CXFA_FFDoc* doc);
  bool LoadImageEditImage(CXFA_FFDoc* doc);
  void GetImageDpi(int32_t& iImageXDpi, int32_t& iImageYDpi);
  void GetImageEditDpi(int32_t& iImageXDpi, int32_t& iImageYDpi);

  RetainPtr<CFX_DIBitmap> GetImageImage();
  RetainPtr<CFX_DIBitmap> GetImageEditImage();
  void SetImageEdit(const WideString& wsContentType,
                    const WideString& wsHref,
                    const WideString& wsData);
  void SetImageImage(const RetainPtr<CFX_DIBitmap>& newImage);
  void SetImageEditImage(const RetainPtr<CFX_DIBitmap>& newImage);
  void UpdateUIDisplay(CXFA_FFDocView* docView, CXFA_FFWidget* pExcept);

  RetainPtr<CFGAS_GEFont> GetFDEFont(CXFA_FFDoc* doc);

  bool IsListBox();
  bool IsAllowNeutral();
  bool IsRadioButton();
  bool IsChoiceListAllowTextEntry();
  bool IsMultiLine();

  XFA_AttributeEnum GetButtonHighlight();
  bool HasButtonRollover();
  bool HasButtonDown();

  bool IsCheckButtonRound();
  XFA_AttributeEnum GetCheckButtonMark();
  float GetCheckButtonSize();

  XFA_CHECKSTATE GetCheckState();
  void SetCheckState(XFA_CHECKSTATE eCheckState, bool bNotify);

  CXFA_Node* GetSelectedMember();
  CXFA_Node* SetSelectedMember(const WideStringView& wsName, bool bNotify);
  void SetSelectedMemberByValue(const WideStringView& wsValue,
                                bool bNotify,
                                bool bScriptModify,
                                bool bSyncData);

  CXFA_Node* GetExclGroupFirstMember();
  CXFA_Node* GetExclGroupNextMember(CXFA_Node* pNode);

  int32_t CountChoiceListItems(bool bSaveValue);
  Optional<WideString> GetChoiceListItem(int32_t nIndex, bool bSaveValue);
  bool IsChoiceListMultiSelect();
  bool IsChoiceListCommitOnSelect();
  std::vector<WideString> GetChoiceListItems(bool bSaveValue);

  int32_t CountSelectedItems();
  int32_t GetSelectedItem(int32_t nIndex);
  std::vector<int32_t> GetSelectedItems();
  std::vector<WideString> GetSelectedItemsValue();
  void SetSelectedItems(const std::vector<int32_t>& iSelArray,
                        bool bNotify,
                        bool bScriptModify,
                        bool bSyncData);
  void InsertItem(const WideString& wsLabel,
                  const WideString& wsValue,
                  bool bNotify);
  bool DeleteItem(int32_t nIndex, bool bNotify, bool bScriptModify);
  void ClearAllSelections();

  bool GetItemState(int32_t nIndex);
  void SetItemState(int32_t nIndex,
                    bool bSelected,
                    bool bNotify,
                    bool bScriptModify,
                    bool bSyncData);

  WideString GetItemValue(const WideStringView& wsLabel);

  bool IsHorizontalScrollPolicyOff();
  bool IsVerticalScrollPolicyOff();
  Optional<int32_t> GetNumberOfCells();

  bool SetValue(XFA_VALUEPICTURE eValueType, const WideString& wsValue);
  WideString GetValue(XFA_VALUEPICTURE eValueType);

  WideString GetPictureContent(XFA_VALUEPICTURE ePicture);
  WideString GetNormalizeDataValue(const WideString& wsValue);
  WideString GetFormatDataValue(const WideString& wsValue);
  WideString NormalizeNumStr(const WideString& wsValue);

  WideString GetPasswordChar();
  std::pair<XFA_Element, int32_t> GetMaxChars();
  int32_t GetFracDigits();
  int32_t GetLeadDigits();

  WideString NumericLimit(const WideString& wsValue,
                          int32_t iLead,
                          int32_t iTread) const;

  virtual XFA_Element GetValueNodeType() const;

 protected:
  CXFA_Node(CXFA_Document* pDoc,
            XFA_PacketType ePacket,
            uint32_t validPackets,
            XFA_ObjectType oType,
            XFA_Element eType,
            const PropertyData* properties,
            const AttributeData* attributes,
            const WideStringView& elementName,
            std::unique_ptr<CJX_Object> js_node);
  CXFA_Node(CXFA_Document* pDoc,
            XFA_PacketType ePacket,
            uint32_t validPackets,
            XFA_ObjectType oType,
            XFA_Element eType,
            const PropertyData* properties,
            const AttributeData* attributes,
            const WideStringView& elementName);

 private:
  void ProcessScriptTestValidate(CXFA_FFDocView* docView,
                                 CXFA_Validate* validate,
                                 int32_t iRet,
                                 bool pRetValue,
                                 bool bVersionFlag);
  int32_t ProcessFormatTestValidate(CXFA_FFDocView* docView,
                                    CXFA_Validate* validate,
                                    bool bVersionFlag);
  int32_t ProcessNullTestValidate(CXFA_FFDocView* docView,
                                  CXFA_Validate* validate,
                                  int32_t iFlags,
                                  bool bVersionFlag);
  WideString GetValidateCaptionName(bool bVersionFlag);
  WideString GetValidateMessage(bool bError, bool bVersionFlag);

  bool HasFlag(XFA_NodeFlag dwFlag) const;
  CXFA_Node* Deprecated_GetPrevSibling();
  const PropertyData* GetPropertyData(XFA_Element property) const;
  const AttributeData* GetAttributeData(XFA_Attribute attr) const;
  Optional<XFA_Element> GetFirstPropertyWithFlag(uint8_t flag);
  void OnRemoved(bool bNotify);
  Optional<void*> GetDefaultValue(XFA_Attribute attr,
                                  XFA_AttributeType eType) const;
  CXFA_Node* GetChildInternal(size_t index, XFA_Element eType, bool bOnlyChild);
  CXFA_Node* GetFirstChildByClassInternal(XFA_Element eType) const;
  CXFA_Node* GetNextSameNameSiblingInternal(
      const WideStringView& wsNodeName) const;
  CXFA_Node* GetNextSameClassSiblingInternal(XFA_Element eType) const;
  void CalcCaptionSize(CXFA_FFDoc* doc, CFX_SizeF& szCap);
  bool CalculateFieldAutoSize(CXFA_FFDoc* doc, CFX_SizeF& size);
  bool CalculateWidgetAutoSize(CFX_SizeF& size);
  bool CalculateTextEditAutoSize(CXFA_FFDoc* doc, CFX_SizeF& size);
  bool CalculateCheckButtonAutoSize(CXFA_FFDoc* doc, CFX_SizeF& size);
  bool CalculatePushButtonAutoSize(CXFA_FFDoc* doc, CFX_SizeF& size);
  CFX_SizeF CalculateImageSize(float img_width,
                               float img_height,
                               float dpi_x,
                               float dpi_y);
  bool CalculateImageEditAutoSize(CXFA_FFDoc* doc, CFX_SizeF& size);
  bool CalculateImageAutoSize(CXFA_FFDoc* doc, CFX_SizeF& size);
  float CalculateWidgetAutoHeight(float fHeightCalc);
  float CalculateWidgetAutoWidth(float fWidthCalc);
  float GetWidthWithoutMargin(float fWidthCalc);
  float GetHeightWithoutMargin(float fHeightCalc);
  void CalculateTextContentSize(CXFA_FFDoc* doc, CFX_SizeF& size);
  void CalculateAccWidthAndHeight(CXFA_FFDoc* doc,
                                  XFA_Element eUIType,
                                  float& fWidth,
                                  float& fCalcHeight);
  void InitLayoutData();
  void StartTextLayout(CXFA_FFDoc* doc, float& fCalcWidth, float& fCalcHeight);

  void InsertListTextItem(CXFA_Node* pItems,
                          const WideString& wsText,
                          int32_t nIndex);
  WideString FormatNumStr(const WideString& wsValue, IFX_Locale* pLocale);
  void GetItemLabel(const WideStringView& wsValue, WideString& wsLabel);
  std::pair<XFA_Element, CXFA_Node*> CreateUIChild();
  void CreateValueNodeIfNeeded(CXFA_Value* value, CXFA_Node* pUIChild);

  const PropertyData* const m_Properties;
  const AttributeData* const m_Attributes;
  const uint32_t m_ValidPackets;
  CXFA_Node* m_pNext;
  CXFA_Node* m_pChild;
  CXFA_Node* m_pLastChild;
  CXFA_Node* m_pParent;
  CFX_XMLNode* m_pXMLNode;
  const XFA_PacketType m_ePacket;
  uint8_t m_ExecuteRecursionDepth = 0;
  uint16_t m_uNodeFlags;
  uint32_t m_dwNameHash;
  CXFA_Node* m_pAuxNode;
  std::vector<UnownedPtr<CXFA_Node>> binding_nodes_;
  CXFA_Node* m_pUiChildNode = nullptr;
  XFA_Element m_eUIType = XFA_Element::Unknown;
  bool m_bIsNull = true;
  bool m_bPreNull = true;
  bool is_widget_ready_ = false;
  std::unique_ptr<CXFA_WidgetLayoutData> m_pLayoutData;
};

#endif  // XFA_FXFA_PARSER_CXFA_NODE_H_
