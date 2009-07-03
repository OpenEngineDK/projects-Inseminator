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
    PictureMovieResource(ITextureResourcePtr t, float d) {
        tex = t;
        duration = d;
        Restart();
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
    ColorFormat GetColorFormat() { return tex->GetColorFormat(); }

    // from IModule
    virtual void Handle(InitializeEventArg arg){}
    virtual void Handle(DeinitializeEventArg arg){}
    virtual void Handle(ProcessEventArg arg){
        float dt = arg.approx / 1000.0;

        if (paused || Ended()) return;
        time += dt; // @todo could overflow if to large
    }

    // from IResource
    virtual void Load(){}
    virtual void Unload(){}

    // from TextureResource
    int GetID() { return tex->GetID(); }
    void SetID(int id) { tex->SetID(id); }
    unsigned int GetHeight() { return tex->GetHeight(); }
    unsigned int GetWidth() { return tex->GetWidth(); }
    unsigned int GetDepth() { return tex->GetDepth(); }
    unsigned char* GetData() { return tex->GetData(); }
};

#endif // _PICTURE_MOVIE_RESOURCE_
