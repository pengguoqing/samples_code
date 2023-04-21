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

    enum class AudioFmt:uint8_t {
	    AUDIO_FORMAT_UNKNOWN,

	    AUDIO_FORMAT_U8BIT,
	    AUDIO_FORMAT_16BIT,
	    AUDIO_FORMAT_32BIT,
	    AUDIO_FORMAT_FLOAT,

	    AUDIO_FORMAT_U8BIT_PLANAR,
	    AUDIO_FORMAT_16BIT_PLANAR,
	    AUDIO_FORMAT_32BIT_PLANAR,
	    AUDIO_FORMAT_FLOAT_PLANAR,
        };
        
        enum class AudioLayout:uint8_t {
	    UNKNOWN,    
	    MONO,         /**< Channels: MONO */
	    STEREO,       /**< Channels: FL, FR */
	    _2POINT1,     /**< Channels: FL, FR, LFE */
	    _4POINT0,     /**< Channels: FL, FR, FC, RC */
	    _4POINT1,     /**< Channels: FL, FR, FC, LFE, RC */
	    _5POINT1,     /**< Channels: FL, FR, FC, LFE, RL, RR */
	    _7POINT1,     /**< Channels: FL, FR, FC, LFE, RL, RR, SL, SR */
    };

    enum class PixFmt : uint8_t {
        kYUV422,
        kYUV422P,
        kYUV420P,
        kYUVP,
        kYUV,
        kRGB,
    };

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
        std::string m_vcodec_name;
        std::string m_pixfmt_name;

        // audio params
        int m_samplerate;
        int m_samplefmt;
        int m_audio_depth;
        std::string m_acodec_name;

        // clip params
        int64_t m_duration;
        int64_t m_nb_frames;
    };
  
    struct ClipOutputFmt{
      enum class MetaDataType : uint8_t
      {
        kClipClassV  = 1 << 0,
        kClipClassA1 = 1 << 1,
        kClipClassA2 = 1 << 2,
        kClipClassA3 = 1 << 3,
        kClipClassA4 = 1 << 4,
        kClipClassA5 = 1 << 5,
        kClipClassA6 = 1 << 6,
        kClipClassA7 = 1 << 7,
        kClipClassA8 = 1 << 8,
        kClipClassAudioDefault = kClipClassA1 | kClipClassA2 | kClipClassA3 | kClipClassA4 | \
                                 kClipClassA5 | kClipClassA6 | kClipClassA7 | kClipClassA8,
      };

        MetaDataType m_type;
        
        int          m_width;
        int          m_height;
        PixFmt       m_pixfmt;

        int          m_audiodepth;
        int          m_samplerate;
    };

    struct VideoSource {
        VideoSource()
        : m_data{},
          m_linesize{},
          m_width(0),
          m_height(0),
          m_pos(0)
        {}
        std::array<uint8_t*, kMaxAVPlanes> m_data;
        std::array<uint32_t, kMaxAVPlanes> m_linesize;
        uint32_t                           m_width;
        uint32_t                           m_height;
        uint64_t                           m_pos;
    };

    struct AudioSource {
        AudioSource()
        : m_data{},
          m_fmt(AudioFmt::AUDIO_FORMAT_UNKNOWN),
          m_layout(AudioLayout::UNKNOWN),
          m_samplecnt(0),
          m_pos(0)
        {}
        std::array<uint8_t*, kMaxAVPlanes> m_data;
        uint64_t                           m_pos;
        uint32_t                           m_samplecnt;
        AudioFmt                           m_fmt;                          
        AudioLayout                        m_layout;
    };

    class IClipReader
    {
    public:
        virtual              ~IClipReader()                           = 0; 
        virtual  bool        OpenClipFile(std::string filepath)       = 0;
        virtual  void        CloseClipFile()                          = 0;
        virtual  ClipInfo    GetClipInfo() const                      = 0;
        virtual  void        SetOutputFmt(const ClipOutputFmt& info)  = 0;
        virtual  bool        GetSourceV(VideoSource* frame, uint64_t pos)   = 0;
        virtual  bool        GetSourceA(AudioSource* frame, uint64_t pos)   = 0;
        virtual  void        ReleaseFrame(uint64_t pos)               = 0;
    };
}