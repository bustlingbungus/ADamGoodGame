#pragma once

#include "../engine/LTexture.hpp"

enum FacingDirection
{
    Default,
    Forwards,
    Backwards,
    Left, 
    Right
};

class Animations
{
    public:

        Animations(
            std::vector<std::shared_ptr<LTexture>> Front,
            std::vector<std::shared_ptr<LTexture>> Back,
            std::vector<std::shared_ptr<LTexture>> Left,
            std::vector<std::shared_ptr<LTexture>> Right,
            float fInter, float bInter, float lInter, float rInter
        );
        Animations();

        // deallocate resources
        ~Animations();
        void free();

        // sets the current vector
        void setVector( FacingDirection dir = Default, float newIdx = -1.0f );

        // gets the current texture from the specified texture
        std::shared_ptr<LTexture> getTexture( float deltaTime, FacingDirection vec = Default, float newIdx = -1.0f, bool updateIdx = true );


    private:

        // four vectors of images
        std::vector<std::shared_ptr<LTexture>> front, back, left, right;
        // the amount of time to cycle through all images in each vector
        float dFront = 0.0f, dBack = 0.0f, dLeft = 0.0f, dRight = 0.0f;

        // the vector currently being sampled for images
        std::vector<std::shared_ptr<LTexture>> *currVec;
        int *currSize;
        float *currInter;
        
        // the index in the current vector
        float currIdx = 0.0f;
        
        // the sizes of the respective vectors
        int nf = 0, nb = 0, nl = 0, nr = 0; 
};