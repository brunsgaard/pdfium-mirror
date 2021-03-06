// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_WIN32_WIN32_INT_H_
#define CORE_FXGE_WIN32_WIN32_INT_H_

#include <windows.h>

#include <memory>
#include <vector>

#include "core/fxcrt/retain_ptr.h"
#include "core/fxge/cfx_pathdata.h"
#include "core/fxge/ifx_renderdevicedriver.h"
#include "core/fxge/win32/cfx_psrenderer.h"
#include "core/fxge/win32/cpsoutput.h"
#include "core/fxge/win32/dwrite_int.h"

class CFX_ImageRenderer;
class FXTEXT_CHARPOS;
struct WINDIB_Open_Args_;

typedef HANDLE(__stdcall* FuncType_GdiAddFontMemResourceEx)(PVOID pbFont,
                                                            DWORD cbFont,
                                                            PVOID pdv,
                                                            DWORD* pcFonts);
typedef BOOL(__stdcall* FuncType_GdiRemoveFontMemResourceEx)(HANDLE handle);

class CGdiplusExt {
 public:
  CGdiplusExt();
  ~CGdiplusExt();

  void Load();
  bool IsAvailable() { return !!m_hModule; }
  bool StretchBitMask(HDC hDC,
                      BOOL bMonoDevice,
                      const RetainPtr<CFX_DIBitmap>& pBitmap,
                      int dest_left,
                      int dest_top,
                      int dest_width,
                      int dest_height,
                      uint32_t argb,
                      const FX_RECT* pClipRect,
                      int flags);
  bool StretchDIBits(HDC hDC,
                     const RetainPtr<CFX_DIBitmap>& pBitmap,
                     int dest_left,
                     int dest_top,
                     int dest_width,
                     int dest_height,
                     const FX_RECT* pClipRect,
                     int flags);
  bool DrawPath(HDC hDC,
                const CFX_PathData* pPathData,
                const CFX_Matrix* pObject2Device,
                const CFX_GraphStateData* pGraphState,
                uint32_t fill_argb,
                uint32_t stroke_argb,
                int fill_mode);

  void* LoadMemFont(uint8_t* pData, uint32_t size);
  void DeleteMemFont(void* pFontCollection);
  bool GdipCreateFromImage(void* bitmap, void** graphics);
  void GdipDeleteGraphics(void* graphics);
  void GdipSetTextRenderingHint(void* graphics, int mode);
  void GdipSetPageUnit(void* graphics, uint32_t unit);
  void GdipSetWorldTransform(void* graphics, void* pMatrix);
  bool GdipDrawDriverString(void* graphics,
                            unsigned short* text,
                            int length,
                            void* font,
                            void* brush,
                            void* positions,
                            int flags,
                            const void* matrix);
  void GdipCreateBrush(uint32_t fill_argb, void** pBrush);
  void GdipDeleteBrush(void* pBrush);
  void GdipCreateMatrix(float a,
                        float b,
                        float c,
                        float d,
                        float e,
                        float f,
                        void** matrix);
  void GdipDeleteMatrix(void* matrix);
  bool GdipCreateFontFamilyFromName(const wchar_t* name,
                                    void* pFontCollection,
                                    void** pFamily);
  void GdipDeleteFontFamily(void* pFamily);
  bool GdipCreateFontFromFamily(void* pFamily,
                                float font_size,
                                int fontstyle,
                                int flag,
                                void** pFont);
  void* GdipCreateFontFromCollection(void* pFontCollection,
                                     float font_size,
                                     int fontstyle);
  void GdipDeleteFont(void* pFont);
  bool GdipCreateBitmap(const RetainPtr<CFX_DIBitmap>& pBitmap, void** bitmap);
  void GdipDisposeImage(void* bitmap);
  void GdipGetFontSize(void* pFont, float* size);
  void* GdiAddFontMemResourceEx(void* pFontdata,
                                uint32_t size,
                                void* pdv,
                                uint32_t* num_face);
  bool GdiRemoveFontMemResourceEx(void* handle);
  RetainPtr<CFX_DIBitmap> LoadDIBitmap(WINDIB_Open_Args_ args);

  std::vector<FARPROC> m_Functions;
  FuncType_GdiAddFontMemResourceEx m_pGdiAddFontMemResourceEx = nullptr;
  FuncType_GdiRemoveFontMemResourceEx m_pGdiRemoveFontMemResourseEx = nullptr;

 protected:
  HMODULE m_hModule = nullptr;
  HMODULE m_GdiModule = nullptr;
};

class CWin32Platform {
 public:
  bool m_bHalfTone;
  CGdiplusExt m_GdiplusExt;
  CDWriteExt m_DWriteExt;
};

class CGdiDeviceDriver : public IFX_RenderDeviceDriver {
 protected:
  CGdiDeviceDriver(HDC hDC, int device_class);
  ~CGdiDeviceDriver() override;

  // IFX_RenderDeviceDriver
  int GetDeviceCaps(int caps_id) const override;
  void SaveState() override;
  void RestoreState(bool bKeepSaved) override;
  bool SetClip_PathFill(const CFX_PathData* pPathData,
                        const CFX_Matrix* pObject2Device,
                        int fill_mode) override;
  bool SetClip_PathStroke(const CFX_PathData* pPathData,
                          const CFX_Matrix* pObject2Device,
                          const CFX_GraphStateData* pGraphState) override;
  bool DrawPath(const CFX_PathData* pPathData,
                const CFX_Matrix* pObject2Device,
                const CFX_GraphStateData* pGraphState,
                uint32_t fill_color,
                uint32_t stroke_color,
                int fill_mode,
                int blend_type) override;
  bool FillRectWithBlend(const FX_RECT* pRect,
                         uint32_t fill_color,
                         int blend_type) override;
  bool DrawCosmeticLine(const CFX_PointF& ptMoveTo,
                        const CFX_PointF& ptLineTo,
                        uint32_t color,
                        int blend_type) override;
  bool GetClipBox(FX_RECT* pRect) override;

  void DrawLine(float x1, float y1, float x2, float y2);

  bool GDI_SetDIBits(const RetainPtr<CFX_DIBitmap>& pBitmap,
                     const FX_RECT* pSrcRect,
                     int left,
                     int top);
  bool GDI_StretchDIBits(const RetainPtr<CFX_DIBitmap>& pBitmap,
                         int dest_left,
                         int dest_top,
                         int dest_width,
                         int dest_height,
                         uint32_t flags);
  bool GDI_StretchBitMask(const RetainPtr<CFX_DIBitmap>& pBitmap,
                          int dest_left,
                          int dest_top,
                          int dest_width,
                          int dest_height,
                          uint32_t bitmap_color,
                          uint32_t flags);

  HDC m_hDC;
  bool m_bMetafileDCType;
  int m_Width;
  int m_Height;
  int m_nBitsPerPixel;
  int m_DeviceClass;
  int m_RenderCaps;
};

class CGdiDisplayDriver : public CGdiDeviceDriver {
 public:
  explicit CGdiDisplayDriver(HDC hDC);
  ~CGdiDisplayDriver() override;

 protected:
  bool GetDIBits(const RetainPtr<CFX_DIBitmap>& pBitmap,
                 int left,
                 int top) override;
  bool SetDIBits(const RetainPtr<CFX_DIBSource>& pBitmap,
                 uint32_t color,
                 const FX_RECT* pSrcRect,
                 int left,
                 int top,
                 int blend_type) override;
  bool StretchDIBits(const RetainPtr<CFX_DIBSource>& pBitmap,
                     uint32_t color,
                     int dest_left,
                     int dest_top,
                     int dest_width,
                     int dest_height,
                     const FX_RECT* pClipRect,
                     uint32_t flags,
                     int blend_type) override;
  bool StartDIBits(const RetainPtr<CFX_DIBSource>& pBitmap,
                   int bitmap_alpha,
                   uint32_t color,
                   const CFX_Matrix* pMatrix,
                   uint32_t render_flags,
                   std::unique_ptr<CFX_ImageRenderer>* handle,
                   int blend_type) override;
  bool UseFoxitStretchEngine(const RetainPtr<CFX_DIBSource>& pSource,
                             uint32_t color,
                             int dest_left,
                             int dest_top,
                             int dest_width,
                             int dest_height,
                             const FX_RECT* pClipRect,
                             int render_flags);
};

class CGdiPrinterDriver : public CGdiDeviceDriver {
 public:
  explicit CGdiPrinterDriver(HDC hDC);
  ~CGdiPrinterDriver() override;

 protected:
  int GetDeviceCaps(int caps_id) const override;
  bool SetDIBits(const RetainPtr<CFX_DIBSource>& pBitmap,
                 uint32_t color,
                 const FX_RECT* pSrcRect,
                 int left,
                 int top,
                 int blend_type) override;
  bool StretchDIBits(const RetainPtr<CFX_DIBSource>& pBitmap,
                     uint32_t color,
                     int dest_left,
                     int dest_top,
                     int dest_width,
                     int dest_height,
                     const FX_RECT* pClipRect,
                     uint32_t flags,
                     int blend_type) override;
  bool StartDIBits(const RetainPtr<CFX_DIBSource>& pBitmap,
                   int bitmap_alpha,
                   uint32_t color,
                   const CFX_Matrix* pMatrix,
                   uint32_t render_flags,
                   std::unique_ptr<CFX_ImageRenderer>* handle,
                   int blend_type) override;
  bool DrawDeviceText(int nChars,
                      const FXTEXT_CHARPOS* pCharPos,
                      CFX_Font* pFont,
                      const CFX_Matrix* pObject2Device,
                      float font_size,
                      uint32_t color) override;

  const int m_HorzSize;
  const int m_VertSize;
};

class CPSPrinterDriver : public IFX_RenderDeviceDriver {
 public:
  CPSPrinterDriver(HDC hDC, int ps_level, bool bCmykOutput);
  ~CPSPrinterDriver() override;

 protected:
  // IFX_RenderDeviceDriver
  int GetDeviceCaps(int caps_id) const override;
  bool StartRendering() override;
  void EndRendering() override;
  void SaveState() override;
  void RestoreState(bool bKeepSaved) override;
  bool SetClip_PathFill(const CFX_PathData* pPathData,
                        const CFX_Matrix* pObject2Device,
                        int fill_mode) override;
  bool SetClip_PathStroke(const CFX_PathData* pPathData,
                          const CFX_Matrix* pObject2Device,
                          const CFX_GraphStateData* pGraphState) override;
  bool DrawPath(const CFX_PathData* pPathData,
                const CFX_Matrix* pObject2Device,
                const CFX_GraphStateData* pGraphState,
                uint32_t fill_color,
                uint32_t stroke_color,
                int fill_mode,
                int blend_type) override;
  bool GetClipBox(FX_RECT* pRect) override;
  bool SetDIBits(const RetainPtr<CFX_DIBSource>& pBitmap,
                 uint32_t color,
                 const FX_RECT* pSrcRect,
                 int left,
                 int top,
                 int blend_type) override;
  bool StretchDIBits(const RetainPtr<CFX_DIBSource>& pBitmap,
                     uint32_t color,
                     int dest_left,
                     int dest_top,
                     int dest_width,
                     int dest_height,
                     const FX_RECT* pClipRect,
                     uint32_t flags,
                     int blend_type) override;
  bool StartDIBits(const RetainPtr<CFX_DIBSource>& pBitmap,
                   int bitmap_alpha,
                   uint32_t color,
                   const CFX_Matrix* pMatrix,
                   uint32_t render_flags,
                   std::unique_ptr<CFX_ImageRenderer>* handle,
                   int blend_type) override;
  bool DrawDeviceText(int nChars,
                      const FXTEXT_CHARPOS* pCharPos,
                      CFX_Font* pFont,
                      const CFX_Matrix* pObject2Device,
                      float font_size,
                      uint32_t color) override;

  HDC m_hDC;
  const bool m_bCmykOutput;
  int m_Width;
  int m_Height;
  int m_nBitsPerPixel;
  int m_HorzSize;
  int m_VertSize;
  CFX_PSRenderer m_PSRenderer;
};

class CTextOnlyPrinterDriver : public IFX_RenderDeviceDriver {
 public:
  explicit CTextOnlyPrinterDriver(HDC hDC);
  ~CTextOnlyPrinterDriver() override;

 protected:
  // IFX_RenderDeviceDriver
  int GetDeviceCaps(int caps_id) const override;
  void SaveState() override{};
  void RestoreState(bool bKeepSaved) override{};
  bool SetClip_PathFill(const CFX_PathData* pPathData,
                        const CFX_Matrix* pObject2Device,
                        int fill_mode) override;
  bool SetClip_PathStroke(const CFX_PathData* pPathData,
                          const CFX_Matrix* pObject2Device,
                          const CFX_GraphStateData* pGraphState) override;
  bool DrawPath(const CFX_PathData* pPathData,
                const CFX_Matrix* pObject2Device,
                const CFX_GraphStateData* pGraphState,
                uint32_t fill_color,
                uint32_t stroke_color,
                int fill_mode,
                int blend_type) override;
  bool GetClipBox(FX_RECT* pRect) override;
  bool SetDIBits(const RetainPtr<CFX_DIBSource>& pBitmap,
                 uint32_t color,
                 const FX_RECT* pSrcRect,
                 int left,
                 int top,
                 int blend_type) override;
  bool StretchDIBits(const RetainPtr<CFX_DIBSource>& pBitmap,
                     uint32_t color,
                     int dest_left,
                     int dest_top,
                     int dest_width,
                     int dest_height,
                     const FX_RECT* pClipRect,
                     uint32_t flags,
                     int blend_type) override;
  bool StartDIBits(const RetainPtr<CFX_DIBSource>& pBitmap,
                   int bitmap_alpha,
                   uint32_t color,
                   const CFX_Matrix* pMatrix,
                   uint32_t render_flags,
                   std::unique_ptr<CFX_ImageRenderer>* handle,
                   int blend_type) override;
  bool DrawDeviceText(int nChars,
                      const FXTEXT_CHARPOS* pCharPos,
                      CFX_Font* pFont,
                      const CFX_Matrix* pObject2Device,
                      float font_size,
                      uint32_t color) override;

  HDC m_hDC;
  const int m_Width;
  const int m_Height;
  int m_nBitsPerPixel;
  const int m_HorzSize;
  const int m_VertSize;
  float m_OriginY;
  bool m_SetOrigin;
};
#endif  // CORE_FXGE_WIN32_WIN32_INT_H_
