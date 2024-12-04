#pragma  once

#include "global.h"
#include "graphics.h"
#include "sprite.h"
#include <map>
#include <string>
#include <vector>
class AnmitedSprite : public Sprite
{
    public:
        AnmitedSprite();
        AnmitedSprite(Graphics &graphics, const std::string &filePath, int sourceX, int sourceY, int width, int height, float posX, float posY,float timeToUpdate);

        void playtAnimation(std::string anmition, bool once = false);

        void update(int elapsedTime);

        void draw(Graphics &graphics, int x, int y);

        virtual void setupAnimations();


        // ~AnmitedSprite();

    protected:
        double _timeToUpdate;
        bool _currentAnimationOnce;
        std::string _currentAnimation;

        void addAnimation(int frames, int x, int y, std::string name, int width, int height, Vector2 offset);

        void resetAnimations();

        void stopAnimation();

        void setVisible(bool visible);

        virtual void animationDone(std::string currentAnimation);

    private:
        std::map<std::string, std::vector<SDL_Rect>> _animations;
        std::map<std::string, Vector2> _offsets;

        int _frameIndex;
        double _timeElapsed;
        bool _visible;
};