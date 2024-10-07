#pragma once

#include "../engine/LTexture.hpp"
#include "../engine/LWindow.hpp"

#include <SDL_ttf.h>

#include <string>
#include <vector>

enum Dialogue
{
    None,

    test_dialogue,
    test_dialogue_2,
    test_dialogue_3,

    FOX_DIAG_MIN,
    fox_base_dialogue_1,
    fox_base_dialogue_1_1,
    fox_base_dialogue_1_2,
    fox_base_dialogue_2,
    fox_base_dialogue_2_1,
    fox_base_dialogue_3,
    fox_base_dialogue_3_1,
    fox_base_dialogue_3_2,

    fox_town_1,
    fox_town_1_1,
    fox_town_1_2,
    fox_town_1_3,
    fox_town_2,
    fox_town_2_1,
    fox_town_2_2,
    fox_town_2_3,
    FOX_DIAG_MAX,

    BEAR_DIAG_MIN,
    bear_town_1,
    bear_town_1_1,
    bear_town_1_2,
    bear_town_2,
    bear_town_2_1,
    bear_town_2_2,
    bear_town_3,
    bear_town_3_1,
    bear_town_3_2,
    bear_town_4,
    bear_town_4_1,
    bear_town_4_2,
    bear_town_5,
    bear_town_5_1,
    bear_town_5_2,
    BEAR_DIAG_MAX,

    RABBIT_DIAG_MIN,
    rabbit_town_1,
    rabbit_town_1_1,
    rabbit_town_1_2,
    RABBIT_DIAG_MAX
};


class DialogueRender
{
    private:

        std::vector<std::string> strings;
        SDL_Color colour, bkgColour;
        Vector2Int pos;
        std::shared_ptr<LWindow> window = nullptr;
        TextOrientation orientation;

        std::shared_ptr<LTexture> texture0, texture1;

        TTF_Font *font = NULL;

        void (DialogueRender::*renderFunc)();

        int BKG_DISPLACE = 5;

    public: 

        DialogueRender(std::string txt, Vector2Int pos, std::shared_ptr<LWindow> window, SDL_Color colour = {255,255,255,255}, TextOrientation orientation = Centred, void (DialogueRender::*func)() = &DialogueRender::defaultTextRender, std::shared_ptr<LTexture> tex0 = nullptr, std::shared_ptr<LTexture> tex1 = nullptr);
        DialogueRender(std::vector<std::string> strings, Vector2Int pos, std::shared_ptr<LWindow> window, SDL_Color colour = {255,255,255,255}, TextOrientation orientation = Centred, void (DialogueRender::*func)() = &DialogueRender::defaultTextRender, std::shared_ptr<LTexture> tex0 = nullptr, std::shared_ptr<LTexture> tex1 = nullptr);

        ~DialogueRender();

        void set_font(TTF_Font *Font = NULL);

        void render();

        void defaultTextRender();

        void bear_town_5_diag();
        void bear_town_5_2_diag();
        void bear_town_case14();
        void fox_base_case11();
        void rabbit_town_case2();
        void rabbit_town_case4();
        void rabbit_town_case5();
        void rabbit_town_case8();
};