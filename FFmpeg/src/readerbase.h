/*
* create data: 2023-02-28
* this is media clip reader iface, so the class is pure virtual base class
* I define the recorresponding param here
* it just depend C++ standard include
* contact info: forwardapeng@gmail.com
*/

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <array>

namespace mediaio {

    constexpr int kMaxAVPlanes{8};

    struct ClipInfo {
        ClipInfo()  = default;
        ~ClipInfo() = default;
        ClipInfo(const ClipInfo &another) = default;
        ClipInfo(ClipInfo &&another) = default;
        ClipInfo &operator=(const ClipInfo &another) = default;
        ClipInfo &operator=(ClipInfo &&another) = default;

        // video params
        bool m_hasvideo;
        bool m_hasaudio;
        int  m_width;
        int  m_height;
        int  m_gop_size;
        std::string m_vcodecname;
        std::string m_pixfmtname;

        // audio params
        int m_samplerate;
        int m_samplefmt;
        int m_audio_depth;
        std::string m_acodec_name;

        // clip params
        int64_t m_duration;
        int64_t m_nb_frames;
    };
  

    enum class PixFmt : uint8_t{
        kYUV422,
        kYUV422P,
        kYUV420P,
        kYUV,
        kRGB,
    };

    struct ClipOutputFmt{
      enum class MetaDataType : uint8_t
      {
        kClipClassV,
        kClipClassA1,
        kClipClassA2,
        kClipClassA3,
        kClipClassA4,
        kClipClassA5,
        kClipClassA6,
        kClipClassA7,
        kClipClassA8,
      };

        MetaDataType m_type;
        
        int          m_width;
        int          m_height;
        PixFmt       m_pixfmt;

        int          m_audiodepth;
        int          m_samplerate;
    };


    struct VideoFrame
    {
        std::array<uint8_t*, kMaxAVPlanes> m_data;
        uint64_t                m_pos;
        int                     m_width;
        int                     m_height;
        PixFmt                  m_pixfmt;

    };
    
    struct AuiodFrame{
        std::array<uint8_t, 8> m_data;


    };


    class IClipReader
    {
    public:

        virtual              ~IClipReader()                           = 0; 
        virtual  bool        OpenClipFile(std::string pathname)       = 0;
        virtual  ClipInfo    GetClipInfo() const                      = 0;
        virtual  void        SetOutputFmt(const ClipOutputFmt& info)  = 0;
        virtual  bool        GetFrame(uint64_t)                       = 0;
    };
}