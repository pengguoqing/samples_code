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

namespace mediaIO
{

    struct CXMeidiaInfo
    {
        CXMeidiaInfo() = default;
        ~CXMeidiaInfo() = default;
        CXMeidiaInfo(const CXMeidiaInfo &another) = default;
        CXMeidiaInfo(CXMeidiaInfo &&another) = default;
        CXMeidiaInfo &operator=(const CXMeidiaInfo &another) = default;
        CXMeidiaInfo &operator=(CXMeidiaInfo &&another) = default;

        // video params
        bool m_hasvideo;
        bool m_hasaudio;
        int m_width;
        int m_height;
        int m_gop_size;
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
    using MediaInfo = CXMeidiaInfo;

    enum class PixFmt : uint8_t
    {
        kYUV422,
        kYUV422P,
        kYUV420P,
        kYUVP,
        kRGB,
    };

    enum class MetaDataType : uint8_t
    {
        kVideo,
        kAudioCh1,
        kAudioCh2,
    };

    struct CXReaderParam
    {
        bool         m_origin;
        MetaDataType m_type;
        
        int          m_width;
        int          m_height;
        PixFmt       m_pixfmt;

        int          m_audiodepth;
        int          m_samplerate;
    };
    using readerinfo = CXReaderParam;

    struct CXFrameInfo
    {
        std::array<uint8_t*, 4> m_data;
        uint64_t                m_pos;
    };
    using Frame = CXFrameInfo;

    class IXMediaReader
    {
    public:

        virtual             ~IXMediaReader()                        = 0; 
        virtual  bool       OpenMediaFile(std::string pathname)     = 0;
        virtual  MediaInfo  GetMediaInfo() const                    = 0;
        virtual  void       SetOutputInfo(const readerinfo& info)   = 0;
        virtual  bool       GetFrame(uint64_t)                      = 0;
    };
}