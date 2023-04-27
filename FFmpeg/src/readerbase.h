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
	     kAudioFmtUNKNOWN,

	     kAudioFmtU8BIT,
	     kAudioFmt16BIT,
	     kAudioFmt32BIT,
	     kAudioFmtFLOAT,
	     kAudioFmtU8BIT_PLANAR,
	     kAudioFmt16BIT_PLANAR,
	     kAudioFmt32BIT_PLANAR,
	     kAudioFmtFLOAT_PLANAR,
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
        ClipInfo()
        : m_hasaudio{0},
          m_hasvideo{0},
          m_width{0},
          m_height{0},
          m_gop_size{0},
          m_vcodec_name{},
          m_pixfmt_name{},
          m_samplerate{0},
          m_audio_depth{0},
          m_samplefmt{AudioFmt::kAudioFmtUNKNOWN},
          m_acodec_name{},
          m_duration{0},
          m_nb_frames{0}
        {}

        ~ClipInfo() = default;

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
        int m_audio_depth;
        AudioFmt    m_samplefmt;
        std::string m_acodec_name;

        // ms duration
        int64_t m_duration;

        //total frames
        int64_t m_nb_frames;
    };

#if 0   
    struct [[deprecated]] ClipOutputFmt{
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

#endif

    enum class SoureType:uint8_t {
        kSourceTypeUnknow ,
        kSourceTypeV,
        kSourceTypeA,
    };


    struct AVSoucreData {
        AVSoucreData()
        :m_data{},
         m_linesize{},
         m_duration{0.f},
         m_pts{0.f}
        {}
        std::array<uint8_t*, kMaxAVPlanes> m_data;
        std::array<uint32_t, kMaxAVPlanes> m_linesize;
        float                              m_duration;
        float                              m_pts;
    };

#if 0
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
          m_fmt(AudioFmt::kAudioFmtUNKNOWN),
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

#endif

    class IClipReader
    {
    public:
        virtual  bool        OpenClipFile(std::string filepath, SoureType metatype) = 0;
        virtual  void        CloseClipFile()                          = 0;
        virtual  ClipInfo    GetClipInfo() const                      = 0;
        virtual  bool        GetSourceData(AVSoucreData* frame, uint64_t pos) = 0;
        virtual  bool		     SeekToFrameNum(uint64_t seek_pos)        = 0;
        virtual  void        ReleaseFrame(uint64_t pos)               = 0;
    };
}