#include "Image.hpp"
#include "Clock.hpp"
#include "Logger.hpp"

#include "wincodec.h"
#include "combaseapi.h"

namespace nogl
{
  Image::Image(const char* path)
  {
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    
    // CoCreateInstance(CLSID_WICImagingFactory,
    //     NULL, CLSCTX_INPROC_SERVER,
    //     IID_PPV_ARGS(&piFactory));


    IWICBitmapDecoder* decoder;

    // hr = m_pIWICFactory->CreateDecoderFromFilename(
    // szFileName,                      // Image to be decoded
    // NULL,                            // Do not prefer a particular vendor
    // GENERIC_READ,                    // Desired read access to the file
    // WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
    // &pDecoder                        // Pointer to the decoder
    // );

    CoUninitialize();
  }
}
