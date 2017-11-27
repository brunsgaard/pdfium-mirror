// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_signatureproperties.h"

namespace {

const XFA_Attribute kAttributeData[] = {
    XFA_Attribute::Use, XFA_Attribute::Usehref, XFA_Attribute::Unknown};

constexpr wchar_t kName[] = L"signatureProperties";

}  // namespace

CXFA_SignatureProperties::CXFA_SignatureProperties(CXFA_Document* doc,
                                                   XFA_XDPPACKET packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::SignatureProperties,
                nullptr,
                kAttributeData,
                kName) {}

CXFA_SignatureProperties::~CXFA_SignatureProperties() {}