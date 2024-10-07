#include "Animations.hpp"
#include "../engine/GameMath.hpp"

Animations::Animations(
    std::vector<std::shared_ptr<LTexture>> Front,
    std::vector<std::shared_ptr<LTexture>> Back,
    std::vector<std::shared_ptr<LTexture>> Left,
    std::vector<std::shared_ptr<LTexture>> Right,
    float fInter, float bInter, float lInter, float rInter
) : front(Front), back(Back), left(Left), right(Right)
{

    nf = front.size(); dFront = fInter / nf;
    nb = back.size(); dBack = bInter / nb;
    nl = left.size(); dLeft = lInter / nl;
    nr = right.size(); dRight = rInter / nr;

    currVec = &front;
    currSize = &nf;
    currInter = &dFront;
}

Animations::Animations()
{
    currVec = &front;
    currSize = &nf;
    currInter = &dFront;
}

Animations::~Animations() { free(); }

void Animations::free()
{
    for (int i = 0; i < nf; i++) front[i]->free();
    front.clear();
    for (int i = 0; i < nb; i++) back[i]->free();
    back.clear();
    for (int i = 0; i < nl; i++) left[i]->free();
    left.clear();
    for (int i = 0; i < nr; i++) right[i]->free();
    right.clear();
}

void Animations::setVector( FacingDirection dir, float newIdx )
{
    switch (dir)
    {
        case Forwards: 
            currVec = &front; currSize = &nf; 
            currInter = &dFront; 
            break;
        case Backwards: 
            currVec = &back; currSize = &nb; 
            currInter = &dBack; 
            break;
        case Left: 
            currVec = &left; currSize = &nl; 
            currInter = &dLeft; 
            break;
        case Right: 
            currVec = &right; currSize = &nl; 
            currInter = &dRight; 
            break;
    }

    if (newIdx >= 0.0f) currIdx = Math::clamp(0.0f, (float)*currSize, newIdx);
}

std::shared_ptr<LTexture> Animations::getTexture( float deltaTime, FacingDirection vec, float newIdx, bool updateIdx )
{
    setVector(vec, newIdx);

    // choose the image based on the current index and vector
    auto res = (*currVec)[(int)currIdx];

    // increment the timer
    if (updateIdx) {
        deltaTime /= *currInter;
        currIdx += deltaTime;
        if ((int)currIdx >= *currSize) currIdx = 0.0f;
    }

    return res;
}