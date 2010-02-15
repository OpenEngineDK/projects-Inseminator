#ifndef _PICTURE_MOVIE_RESOURCE_
#define _PICTURE_MOVIE_RESOURCE_

#include <Resources/IMovieResource.h>

using namespace OpenEngine::Resources;

class PictureMovieResource : public IMovieResource {
 private:
    ITextureResourcePtr tex;
    bool paused;
    double time, duration;
    
 public:
    PictureMovieResource(ITextureResourcePtr t, float d) 
        : ITextureResource() {
        tex = t;
        duration = d;
        Restart();
        Load();
    }
    ~PictureMovieResource() {}

    // from IMovieResource
    void Pause(bool pause) { paused = pause; }
    bool Ended() { if (time > duration) return true; else return false; }
    void Restart() {
        time = 0.0;
        paused = false;
    }
    unsigned int GetMovieHeight() { return tex->GetHeight(); }
    unsigned int GetMovieWidth() { return tex->GetWidth(); }

    // from IModule
    virtual void Handle(InitializeEventArg arg){}
    virtual void Handle(DeinitializeEventArg arg){}
    virtual void Handle(ProcessEventArg arg){
        float dt = arg.approx / 1000.0;

        if (paused || Ended()) return;
        time += dt; // @todo could overflow if to large
    }

    // from IResource
    virtual void Load(){
        tex->Load();
        this->id = tex->GetID();
        this->channels = tex->GetChannels();
        this->format = tex->GetColorFormat();
        this->data = (unsigned char*) tex->GetVoidDataPtr();
        this->mipmapping = tex->UseMipmapping();
        this->height = tex->GetHeight();
        this->width = tex->GetWidth();
    }
    virtual void Unload(){}

    // from TextureResource
    void SetID(int id) { this->id = id; tex->SetID(id); }
};

#endif // _PICTURE_MOVIE_RESOURCE_
