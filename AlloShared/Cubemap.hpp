#pragma once

#include <boost/cstdint.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/managed_heap_memory.hpp>
#include <boost/interprocess/segment_manager.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <libavutil/pixfmt.h>
#include <boost/chrono/system_clocks.hpp>
#include <array>

#include "Allocator.h"

class CubemapFace
{

public:
	typedef boost::interprocess::offset_ptr<CubemapFace> Ptr;
    
    boost::uint32_t getWidth();
    boost::uint32_t getHeight();
    int getIndex();
    AVPixelFormat getFormat();
    boost::chrono::system_clock::time_point getPresentationTime();
    void* getPixels();
    boost::interprocess::interprocess_mutex& getMutex();
    boost::interprocess::interprocess_condition& getNewPixelsCondition();
    
    void setPresentationTime(boost::chrono::system_clock::time_point presentationTime);
    
    static CubemapFace* create(boost::uint32_t width,
                               boost::uint32_t height,
                               int index,
                               AVPixelFormat format,
                               boost::chrono::system_clock::time_point presentationTime,
                               Allocator& allocator);
    static void destroy(CubemapFace* cubemapFace);
    
protected:
    CubemapFace(boost::uint32_t width,
                boost::uint32_t height,
                int index,
                AVPixelFormat format,
                boost::chrono::system_clock::time_point presentationTime,
                void* pixels,
                Allocator& allocator);
    ~CubemapFace();
    Allocator& allocator;
    
private:
    boost::uint32_t width;
    boost::uint32_t height;
    int index;
    AVPixelFormat format;
    boost::chrono::system_clock::time_point presentationTime;
    boost::interprocess::offset_ptr<void> pixels;
    boost::interprocess::interprocess_mutex mutex;
    boost::interprocess::interprocess_condition newPixelsCondition;
};

class Cubemap
{
public:
    typedef boost::interprocess::offset_ptr<Cubemap> Ptr;
    static const int MAX_FACES_COUNT = 6;
    
    CubemapFace* getFace(int index);
    int getFacesCount();
    
    static Cubemap* create(std::vector<CubemapFace*> faces,
                           Allocator& allocator);
    static void destroy(Cubemap* cubemap);
    
protected:
    Cubemap(std::vector<CubemapFace*>& faces,
            Allocator& allocator);
    ~Cubemap();
    Allocator& allocator;
    
private:
    std::array<CubemapFace::Ptr, MAX_FACES_COUNT> faces;
    boost::interprocess::interprocess_mutex mutex;
};

class StereoCubemap
{
public:
    typedef boost::interprocess::offset_ptr<StereoCubemap> Ptr;
    static const int MAX_EYES_COUNT = 2;
    
    Cubemap* getEye(int index);
    int getEyesCount();
    
    static StereoCubemap* create(std::vector<Cubemap*>& eyes,
                                 Allocator& allocator);
    static void destroy(StereoCubemap* stereoCubemap);
    
protected:
    StereoCubemap(std::vector<Cubemap*>& eyes, Allocator& allocator);
    ~StereoCubemap();
    Allocator& allocator;
    
private:
    std::array<Cubemap::Ptr, MAX_EYES_COUNT> eyes;
};

extern boost::interprocess::offset_ptr<Cubemap> cubemap;