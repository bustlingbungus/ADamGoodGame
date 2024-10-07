#include "TextureManipulator.hpp"




TextureManipulator::TextureManipulator() {}


void TextureManipulator::renderTextureToTexture( std::shared_ptr<LTexture> target, std::shared_ptr<LTexture> source, SDL_Rect *targetRect )
{
    // Ensure both target and source are valid
    if (!target || !source || !target->mTexture || !source->mTexture) {
        std::cerr << "Invalid target or source texture." << std::endl;
        return;
    }

    SDL_Renderer *renderer = source->gHolder->gRenderer;

    // Set the source's renderer to render onto the target texture
    if (SDL_SetRenderTarget(renderer, target->mTexture) != 0) {
        std::cerr << "SDL_SetRenderTarget failed: " << SDL_GetError() << std::endl;
        return;
    }

    // Copy the source texture onto the target texture
    if (SDL_RenderCopy(renderer, source->mTexture, NULL, targetRect) != 0) {
        std::cerr << "SDL_RenderCopy failed: " << SDL_GetError() << std::endl;
    }

    // Reset the target to the default render target
    if (SDL_SetRenderTarget(renderer, NULL) != 0) {
        std::cerr << "SDL_SetRenderTarget failed: " << SDL_GetError() << std::endl;
    }
}


void TextureManipulator::renderTextureToTexture( std::shared_ptr<LTexture> target, std::shared_ptr<LTexture> source, SDL_Rect *targetRect, double angle, SDL_RendererFlip flip )
{
    // Ensure both target and source are valid
    if (!target || !source || !target->mTexture || !source->mTexture) {
        std::cerr << "Invalid target or source texture." << std::endl;
        return;
    }

    SDL_Renderer *renderer = source->gHolder->gRenderer;

    // Set the source's renderer to render onto the target texture
    if (SDL_SetRenderTarget(renderer, target->mTexture) != 0) {
        std::cerr << "SDL_SetRenderTarget failed: " << SDL_GetError() << std::endl;
        return;
    }

    // Copy the source texture onto the target texture
    if (SDL_RenderCopyEx(renderer, source->mTexture, NULL, targetRect, angle, NULL, flip) != 0) {
        std::cerr << "SDL_RenderCopy failed: " << SDL_GetError() << std::endl;
    }

    // Reset the target to the default render target
    if (SDL_SetRenderTarget(renderer, NULL) != 0) {
        std::cerr << "SDL_SetRenderTarget failed: " << SDL_GetError() << std::endl;
    }
}



void TextureManipulator::renderTextureToTexture( std::shared_ptr<LTexture> target, SDL_Texture *source, SDL_Rect *targetRect )
{
    // Ensure both target and source are valid
    if (!target || !source || !target->mTexture) {
        std::cerr << "Invalid target or source texture." << std::endl;
        return;
    }

    SDL_Renderer *renderer = target->gHolder->gRenderer;

    // Set the source's renderer to render onto the target texture
    if (SDL_SetRenderTarget(renderer, target->mTexture) != 0) {
        std::cerr << "SDL_SetRenderTarget failed: " << SDL_GetError() << std::endl;
        return;
    }

    // Copy the source texture onto the target texture
    if (SDL_RenderCopy(renderer, source, NULL, targetRect) != 0) {
        std::cerr << "SDL_RenderCopy failed: " << SDL_GetError() << std::endl;
    }

    // Reset the target to the default render target
    if (SDL_SetRenderTarget(renderer, NULL) != 0) {
        std::cerr << "SDL_SetRenderTarget failed: " << SDL_GetError() << std::endl;
    }
}




// creates an empty LTexture of the specified dimensions with the desired LWindow
std::shared_ptr<LTexture> TextureManipulator::createEmptyTexture( int width, int height, std::shared_ptr<LWindow> gHolder, SDL_TextureAccess access )
{
    // create an LTexture pointer
    auto res = std::make_shared<LTexture>(gHolder);

    SDL_Texture *newTexture = NULL;
    newTexture = SDL_CreateTexture(gHolder->gRenderer, SDL_PIXELFORMAT_RGBA8888, access, width, height);

    // set the blend mode to blend so that the texture can have transparency
    SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);

    // initialise the texture with transparent pixels
    SDL_SetRenderTarget(gHolder->gRenderer, newTexture);
    SDL_SetRenderDrawColor(gHolder->gRenderer, 0, 0, 0, 0);
    SDL_RenderClear(gHolder->gRenderer);
    SDL_SetRenderTarget(gHolder->gRenderer, NULL);

    // set the mTexture of res
    res->mTexture = newTexture;
    res->mWidth = width; res->mHeight = height;
    return res;
}







SDL_Surface* createEmptySurface( int width, int height )
{
    // Create an empty surface with the specified width, height, and 32-bit color depth
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32,
                                                0x00FF0000, // Red mask
                                                0x0000FF00, // Green mask
                                                0x000000FF, // Blue mask
                                                0xFF000000  // Alpha mask
                                                );
    if (!surface) std::cerr << "SDL_CreateRGBSurface failed: "<<SDL_GetError() <<std::endl;

    return surface;
}


std::shared_ptr<LTexture> TextureManipulator::createSolidColour( int width, int height, Uint32 colour, std::shared_ptr<LWindow> gHolder)
{
    SDL_Renderer *renderer = gHolder->gRenderer;

    auto res = createEmptyTexture(width, height, gHolder);

    SDL_SetRenderTarget(renderer, res->mTexture);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    Uint8   r = (colour&0xFF000000)>>24,
            g = (colour&0x00FF0000)>>16,
            b = (colour&0x0000FF00)>>8,
            a = colour&0x000000FF;
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, NULL);
    return res;
}

std::shared_ptr<LTexture> TextureManipulator::createSolidColour( int width, int height, SDL_Color colour, std::shared_ptr<LWindow> gHolder )
{
    SDL_Renderer *renderer = gHolder->gRenderer;

    auto res = createEmptyTexture(width, height, gHolder);

    SDL_SetRenderTarget(renderer, res->mTexture);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, NULL);
    return res;
}

std::shared_ptr<LTexture> TextureManipulator::greyscaleTexture( std::string filename, std::shared_ptr<LWindow> window )
{
    SDL_Surface *loadedSurface = IMG_Load(filename.c_str());

    auto res = std::make_shared<LTexture>(window);
    // the final texture 
    SDL_Texture *newTexture = NULL;

    if (loadedSurface == NULL) 
    {
        std::cerr << "Failed to load " << filename << std::endl;
    } 
    else 
    {
        // Color key image
        SDL_SetColorKey(loadedSurface, SDL_TRUE,
                        SDL_MapRGB(loadedSurface->format, 0xFF, 0xFF, 0xFF));

        // make the surface greyscaled
        int n = loadedSurface->w * loadedSurface->h;
        SDL_PixelFormat *format = loadedSurface->format;
        for (int i = 0; i < n; i++)
        {
            Uint32 *targetPixel = (Uint32 *) ((Uint8 *)loadedSurface->pixels + (i*format->BytesPerPixel));
            Uint8   r = ((*targetPixel)>>format->Rshift)&255,
                    g = ((*targetPixel)>>format->Gshift)&255,
                    b = ((*targetPixel)>>format->Bshift)&255,
                    a = ((*targetPixel)>>format->Ashift)&255;

            int average = (r + g + b) / 3;
            r = g = b = (Uint8)average;

            Uint32 newCol = Uint32(r)<<format->Rshift | Uint32(g)<<format->Gshift | Uint32(b)<<format->Bshift | Uint32(a)<<format->Ashift;
            *targetPixel = newCol;
        }

        // create the texture from the surface
        newTexture = SDL_CreateTextureFromSurface(window->gRenderer, loadedSurface);
        if (newTexture == NULL) 
        {
            std::cerr << "Failed to create texture from " << filename <<' '<< SDL_GetError() << std::endl;
        }
        else 
        {
            // get image dimensions
            res->mWidth = loadedSurface->w;
            res->mHeight = loadedSurface->h;
        }
        // cleanup the surface
        SDL_FreeSurface(loadedSurface);
    }

    res->mTexture = newTexture;
    return res;
}

std::shared_ptr<LTexture> TextureManipulator::createMenuButton(std::string txt, int width, int height, std::shared_ptr<LWindow> window, TTF_Font *font, SDL_Color colour)
{
    auto res = createEmptyTexture(width, height, window);

    auto tex = std::make_unique<LTexture>(window);
    SDL_Renderer *renderer = window->gRenderer;
    SDL_SetRenderTarget(renderer, res->mTexture);

    // if (!tex->loadFromRenderedText(txt, {colour.r,colour.g,colour.b,Uint8(colour.a*0.4)}, font)) {
    //     std::cerr << "Failed to create " << txt <<" button!" << std::endl;
    // }
    // int x = (width-tex->getWidth())/2, y = (height-tex->getHeight())/2;
    // tex->render(x-5, y+5);

    if (!tex->loadFromRenderedText(txt, colour, font)) {
        std::cerr << "Failed to create " << txt <<" button!" << std::endl;
    }
    int x = (width-tex->getWidth())/2, y = (height-tex->getHeight())/2;
    tex->render(x, y);

    SDL_SetRenderTarget(renderer, NULL);
    return res;
}

std::shared_ptr<LTexture> TextureManipulator::createSliderTexture(int width, int height, std::shared_ptr<LWindow> window, SDL_Color colour)
{
    auto res = createSolidColour(width, height, colour, window);
    return res;
}

std::shared_ptr<LTexture> TextureManipulator::createFPSButton(int fps, int width, int height, std::shared_ptr<LWindow> window, TTF_Font *font, SDL_Color colour)
{
    std::string txt = (fps == -1)? "unlimited" : std::to_string(fps)+"FPS";
    auto res = createMenuButton(txt, width, height, window, font, colour);
    return res;
}

void showLoadMessage(std::string txt, int x, int y, std::shared_ptr<LWindow> window, SDL_Color bkgColour, SDL_Color txtColour)
{
    SDL_SetRenderDrawColor(window->gRenderer, bkgColour.r, bkgColour.g, bkgColour.b, bkgColour.a);
    SDL_RenderClear(window->gRenderer);
    renderText(txt, x, y, window, txtColour);
    SDL_RenderPresent(window->gRenderer);
}