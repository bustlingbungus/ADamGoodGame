#include "../gameObject/GameObject.hpp"
#include "../gameObject/Player.hpp"
#include "../gameObject/NPC.hpp"
#include "Game.hpp"
#include "Dialogue.hpp"


void NPC::foxDialogue()
{
    // exit dialogue if the player walks too far away
    Vector2 pPos = game->get_playerPos();
    float dist = (pPos - get_pos()).length();
    if (dist > 1.5f*game->interactRange) {
        if (game->currDialogue > FOX_DIAG_MIN && game->currDialogue < FOX_DIAG_MAX) {
            game->enter_dialogue(None);
            game->currNPC = nullptr; 
        }
        GameObject::set_HP(1); 
        return;
    }

    Vector2Int p(get_pos().x - game->camera.x + game->renderOffset.x, 
                 get_hitbox().y - game->camera.y+ game->renderOffset.y);

    switch (get_HP())
    {
        case 2:
        {
            dialogueInterval = 0.5f;
            if (game->currDialogue == fox_town_1_1) {
                set_HP(4); dialogueTimer = dialogueInterval; break;
            } else if (game->currDialogue == fox_town_1_2) {
                set_HP(7); dialogueTimer = dialogueInterval; break;
            } else if (game->currDialogue == fox_town_1_3) {
                set_HP(9); dialogueTimer = dialogueInterval; break;
            }

            std::string rend, txt = "Heyy you made it up here";
            if (dialogueTimer >= 0.0f) {
                float t = 1.0f - (dialogueTimer/dialogueInterval);
                int n = max(1, int(t * txt.size()));
                rend = txt.substr(0, n);
                dialogueTimer -= get_deltaTime();
            } else {
                rend = txt;
                game->enter_dialogue(fox_town_1);
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(p.x, p.y-30), game->window);
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }

        case 3: set_HP(2); dialogueTimer = dialogueInterval; break; // go back

        case 4:
        {
            dialogueInterval = 0.5f;
            std::string rend, txt = "You ask too many questions";
            if (dialogueTimer >= 0.0f) {
                float t = 1.0f - (dialogueTimer/dialogueInterval);
                int n = max(1, int(t * txt.size()));
                rend = txt.substr(0, n);
                dialogueTimer -= get_deltaTime();
            } else {
                rend = txt;
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(p.x, p.y-30), game->window);
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }

        case 5:
        {
            dialogueInterval = 1.25f;
            maySpeak = false;
            std::string rend, txt = "Look just ask around, you\ncan buy stuff, nothing\n too crazy";
            if (dialogueTimer >= 0.0f) {
                float t = 1.0f - (dialogueTimer/dialogueInterval);
                int n = max(1, int(t * txt.size()));
                rend = txt.substr(0, n);
                dialogueTimer -= get_deltaTime();
            } else {
                rend = txt;
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(p.x, p.y-90), game->window);
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }


        case 7:
        {
            dialogueInterval = 0.5f;
            std::string rend, txt = "Don't. Worry about that.";
            if (dialogueTimer >= 0.0f) {
                float t = 1.0f - (dialogueTimer/dialogueInterval);
                int n = max(1, int(t * txt.size()));
                rend = txt.substr(0, n);
                dialogueTimer -= get_deltaTime();
            } else {
                rend = txt;
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(p.x, p.y-30), game->window);
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }

        case 8:
            set_HP(5); dialogueTimer = dialogueInterval;
            break;

        case 9:
        {
            dialogueInterval = 0.25f;
            std::string rend, txt = "Oh, those guys...";
            if (dialogueTimer >= 0.0f) {
                float t = 1.0f - (dialogueTimer/dialogueInterval);
                int n = max(1, int(t * txt.size()));
                rend = txt.substr(0, n);
                dialogueTimer -= get_deltaTime();
            } else {
                rend = txt;
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(p.x, p.y-30), game->window);
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }

        case 10:
        {
            dialogueInterval = 1.0f;

            if (game->currDialogue == fox_town_2_1) {
                set_HP(12); dialogueTimer = dialogueInterval; break;
            } else if (game->currDialogue == fox_town_2_2) {
                set_HP(14); dialogueTimer = dialogueInterval; break;
            } else if (game->currDialogue == fox_town_2_3) {
                set_HP(17); dialogueTimer = dialogueInterval; break;
            }

            std::string rend, txt = "Yeah sorry about them, just do\nyour best to survive, I guess";
            if (dialogueTimer >= 0.0f) {
                float t = 1.0f - (dialogueTimer/dialogueInterval);
                int n = max(1, int(t * txt.size()));
                rend = txt.substr(0, n);
                dialogueTimer -= get_deltaTime();
            } else {
                rend = txt;
                game->enter_dialogue(fox_town_2);
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(p.x, p.y-60), game->window);
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }

        case 11: set_HP(10); dialogueTimer = dialogueInterval; break; // go back

        case 12:
        {
            dialogueInterval = 0.25f;
            std::string rend, txt = "Oops...";
            if (dialogueTimer >= 0.0f) {
                float t = 1.0f - (dialogueTimer/dialogueInterval);
                int n = max(1, int(t * txt.size()));
                rend = txt.substr(0, n);
                dialogueTimer -= get_deltaTime();
            } else {
                rend = txt;
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(p.x, p.y-30), game->window);
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }

        case 13: 
            game->enter_dialogue(fox_town_2);
            set_HP(10); dialogueTimer = dialogueInterval; 
            break;

        case 14:
        {
            dialogueInterval = 0.25f;
            std::string rend, txt = "I mean you've made it so far...";
            if (dialogueTimer >= 0.0f) {
                float t = 1.0f - (dialogueTimer/dialogueInterval);
                int n = max(1, int(t * txt.size()));
                rend = txt.substr(0, n);
                dialogueTimer -= get_deltaTime();
            } else {
                rend = txt;
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(p.x, p.y-30), game->window);
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }

        case 15:
        {
            dialogueInterval = 0.75f;
            std::string rend, txt = "Just build walls, throw rocks, etc.";
            if (dialogueTimer >= 0.0f) {
                float t = 1.0f - (dialogueTimer/dialogueInterval);
                int n = max(1, int(t * txt.size()));
                rend = txt.substr(0, n);
                dialogueTimer -= get_deltaTime();
            } else {
                rend = txt;
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(p.x, p.y-30), game->window);
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }

        case 16: 
            game->enter_dialogue(fox_town_2);
            set_HP(10); dialogueTimer = dialogueInterval; 
            break;

        case 17:
        {
            dialogueInterval = 1.0f;
            maySpeak = false;
            std::string rend, txt = "Yeahh... this is a..\n\"no soliciting\" sorta thing...";
            if (dialogueTimer >= 0.0f) {
                float t = 1.0f - (dialogueTimer/dialogueInterval);
                int n = max(1, int(t * txt.size()));
                rend = txt.substr(0, n);
                dialogueTimer -= get_deltaTime();
            } else {
                rend = txt;
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(p.x, p.y-60), game->window);
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }

        case 6:
        case 18:
            set_HP(1);
            game->enter_dialogue(None);
            break;
    }
}




void NPC::bearDialogue()
{
    // exit dialogue if the player walks too far away
    Vector2 pPos = game->get_playerPos();
    float dist = (pPos - get_pos()).length();
    if (dist > 1.5f*game->interactRange) {
        if (game->currDialogue > BEAR_DIAG_MIN && game->currDialogue < BEAR_DIAG_MAX) {
            game->enter_dialogue(None);
            game->currNPC = nullptr;
        }
        GameObject::set_HP(1); 
        return;
    }

    Vector2Int p(get_pos().x - game->camera.x + game->renderOffset.x, 
                 get_hitbox().y - game->camera.y+ game->renderOffset.y);
    switch(get_HP())
    {
        case 2:
        {
            dialogueInterval = 0.25f;
            if (game->currDialogue == bear_town_1_1) {
                set_HP(4); dialogueTimer = dialogueInterval; break;
            } else if (game->currDialogue == bear_town_1_2) {
                set_HP(6); dialogueTimer = dialogueInterval; break;
            }

            std::string rend, txt = "Hello :)";
            if (dialogueTimer >= 0.0f) {
                float t = 1.0f - (dialogueTimer/dialogueInterval);
                int n = max(1, int(t * txt.size()));
                rend = txt.substr(0, n);
                dialogueTimer -= get_deltaTime();
            } else {
                rend = txt;
                game->enter_dialogue(bear_town_1);
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(p.x, p.y), game->window);
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }

        case 3: set_HP(2); dialogueTimer = dialogueInterval; break;

        case 4:
        {
            dialogueInterval = 0.25f;
            if (game->currDialogue == bear_town_2_1) {
                set_HP(8); dialogueTimer = dialogueInterval; break;
            } else if (game->currDialogue == bear_town_2_2) {
                set_HP(10); dialogueTimer = dialogueInterval; break;
            }

            std::string rend, txt = "Do you like berries?";
            if (dialogueTimer >= 0.0f) {
                float t = 1.0f - (dialogueTimer/dialogueInterval);
                int n = max(1, int(t * txt.size()));
                rend = txt.substr(0, n);
                dialogueTimer -= get_deltaTime();
            } else {
                rend = txt;
                game->enter_dialogue(bear_town_2);
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(p.x, p.y), game->window);
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }

        case 5: set_HP(4); dialogueTimer = dialogueInterval; break;

        case 6:
        {
            dialogueInterval = 0.5f;
            if (game->currDialogue == bear_town_3_1) {
                set_HP(8); dialogueTimer = dialogueInterval; break;
            } else if (game->currDialogue == bear_town_3_2) {
                set_HP(12); dialogueTimer = dialogueInterval; break;
            }

            std::string rend, txt = "Your mom ;)";
            if (dialogueTimer >= 0.0f) {
                float t = 1.0f - (dialogueTimer/dialogueInterval);
                int n = max(1, int(t * txt.size()));
                rend = txt.substr(0, n);
                dialogueTimer -= get_deltaTime();
            } else {
                rend = "Eat berries :)";
                game->enter_dialogue(bear_town_3);
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(p.x, p.y), game->window);
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }

        case 7: set_HP(6); dialogueTimer = dialogueInterval; break;

        case 8:
        {
            dialogueInterval = 0.25f;
            if (game->currDialogue == bear_town_4_1) {
                set_HP(14); dialogueTimer = dialogueInterval; break;
            } else if (game->currDialogue == bear_town_4_2) {
                set_HP(16); dialogueTimer = dialogueInterval; break;
            }

            std::string rend, txt = "You want some? :D";
            if (dialogueTimer >= 0.0f) {
                float t = 1.0f - (dialogueTimer/dialogueInterval);
                int n = max(1, int(t * txt.size()));
                rend = txt.substr(0, n);
                dialogueTimer -= get_deltaTime();
            } else {
                rend = txt;
                game->enter_dialogue(bear_town_4);
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(p.x, p.y), game->window);
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }

        case 9: set_HP(8); dialogueTimer = dialogueInterval; break;

        case 10:
        {
            dialogueInterval = 0.25f;
            maySpeak = false;
            std::string rend, txt = "... >:(";
            if (dialogueTimer >= 0.0f) {
                float t = 1.0f - (dialogueTimer/dialogueInterval);
                int n = max(1, int(t * txt.size()));
                rend = txt.substr(0, n);
                dialogueTimer -= get_deltaTime();
            } else {
                rend = txt;
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(p.x, p.y), game->window);
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }

        
        case 12:
        {
            dialogueInterval = 0.25f;
            std::string rend, txt = "Eat berries :)";
            if (dialogueTimer >= 0.0f) {
                float t = 1.0f - (dialogueTimer/dialogueInterval);
                int n = max(1, int(t * txt.size()));
                rend = txt.substr(0, n);
                dialogueTimer -= get_deltaTime();
            } else {
                rend = txt;
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(p.x, p.y), game->window);
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }

        case 13: set_HP(8); dialogueTimer = dialogueInterval; break;

        case 14:
        {
            if (game->currDialogue == bear_town_5_1) {
                set_HP(1); game->enter_dialogue(None); break;
            } else if (game->currDialogue == bear_town_5_2) {
                set_HP(18); dialogueTimer = dialogueInterval; break;
            }

            game->enter_dialogue(bear_town_5);

            std::vector<std::string> vec = {"Give me 2 ", " for "};
            SDL_Color colour = {255,255,255,255};

            auto diag = std::make_shared<DialogueRender>(
                vec, Vector2Int(p.x, p.y), game->window, colour,
                Centred, &DialogueRender::bear_town_case14,
                game->stoneTex, game->berryTex
            );
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);

            break;
        }

        case 15: set_HP(14); dialogueTimer = dialogueInterval; break;

        case 16:
        {
            dialogueInterval = 0.1f;
            std::string rend, txt = ":P";
            maySpeak = false;
            if (dialogueTimer >= 0.0f) {
                float t = 1.0f - (dialogueTimer/dialogueInterval);
                int n = max(1, int(t * txt.size()));
                rend = txt.substr(0, n);
                dialogueTimer -= get_deltaTime();
            } else {
                rend = txt;
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(p.x, p.y), game->window);
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }

        case 18:
        {
            dialogueInterval = 0.1f;
            maySpeak = false;
            std::string rend, txt = ";)";
            if (dialogueTimer >= 0.0f) {
                float t = 1.0f - (dialogueTimer/dialogueInterval);
                int n = max(1, int(t * txt.size()));
                rend = txt.substr(0, n);
                dialogueTimer -= get_deltaTime();
            } else {
                rend = txt;
            }

            auto diag = std::make_shared<DialogueRender>(rend, Vector2Int(p.x, p.y), game->window);
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }

        case 11:
        case 17:
        case 19:
            set_HP(1);
            dialogueInterval = 0.25f;
            game->enter_dialogue(None);
            break;
    }
}



void NPC::rabbitDialogue()
{
    // exit dialogue if the player walks too far away
    Vector2 pPos = game->get_playerPos();
    float dist = (pPos - get_pos()).length();
    if (dist > 1.5f*game->interactRange) {
        if (game->currDialogue > RABBIT_DIAG_MIN && game->currDialogue < RABBIT_DIAG_MAX) {
            game->enter_dialogue(None);
            game->currNPC = nullptr;
        }
        GameObject::set_HP(1); 
        return;
    }

    Vector2Int p(get_pos().x - game->camera.x + game->renderOffset.x, 
                 get_hitbox().y - game->camera.y+ game->renderOffset.y);
    switch (get_HP())
    {
        case 2:
        {
            dialogueInterval = 0.5f;
            if (game->currDialogue == rabbit_town_1_1) {
                set_HP(4); dialogueTimer = dialogueInterval; break;
            } else if (game->currDialogue == rabbit_town_1_2) {
                set_HP(7); dialogueTimer = dialogueInterval; break;
            }
            
            std::string rend0, rend1, txt0 = "Hey tough guy!", txt1 = "You need any ";
            if (dialogueTimer >= 0.0f) {
                float t = 1.0f - (dialogueTimer/dialogueInterval);
                int n0 = max(1, int(t * txt0.size())), n1 = max(1, int(t * txt1.size()));
                rend0 = txt0.substr(0, n0);
                rend1 = txt1.substr(0, n1);
                dialogueTimer -= get_deltaTime();
            } else {
                rend0 = txt0;
                rend1 = txt1;
                game->enter_dialogue(rabbit_town_1);
            }

            std::vector<std::string> strs = {rend0, rend1, "?"};

            SDL_Color col = {255,255,255,255};
            auto diag = std::make_shared<DialogueRender>(
                strs, Vector2Int(p.x, p.y-60), game->window, col, 
                Centred, &DialogueRender::rabbit_town_case2, game->stoneTex
            );
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }

        case 3: set_HP(2); dialogueTimer = dialogueInterval; break;

        case 4:
        {
            dialogueInterval = 0.5f;
            std::string rend, txt = "Alright! Lemme tell ya,\nI got the BEST.";
            if (dialogueTimer >= 0.0f) {
                float t = 1.0f - (dialogueTimer/dialogueInterval);
                int n = max(1, int(t * txt.size()));
                rend = txt.substr(0, n);
                dialogueTimer -= get_deltaTime();
            } else {
                rend = txt + "\n ";
            }
            std::vector<std::string> strs = {rend, "Freakin' "};

            SDL_Color col = {255,255,255,255};
            auto diag = std::make_shared<DialogueRender>(
                strs, Vector2Int(p.x, p.y-90), game->window, col,
                Centred, &DialogueRender::rabbit_town_case4, game->stoneTex
            );

            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }

        case 5:
        {
            maySpeak = false;
            std::vector<std::string> strs = {"Just gimme 1 ", " for 2"};

            SDL_Color col = {255,255,255,255};
            auto diag = std::make_shared<DialogueRender>(
                strs, Vector2Int(p.x, p.y-30), game->window,
                col, Centred, &DialogueRender::rabbit_town_case5, 
                game->logTex, game->stoneTex
            );
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }

        case 7:
        case 8:
        {
            maySpeak = false;
            std::vector<std::string> strs = {"Aww, come on!", "Just gimme some ", "!"};
            SDL_Color col = {255,255,255,255};

            auto diag = std::make_shared<DialogueRender>(
                strs, Vector2Int(p.x, p.y-30), 
                game->window, col, Centred, 
                &DialogueRender::rabbit_town_case8, game->logTex
            );
            diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
            break;
        }

        case 9:
        case 6: 
            dialogueInterval = 0.5f;
            set_HP(1);
            game->enter_dialogue(None);
            break;

    }
}







void Player::dialogue()
{
    Vector2Int p(get_pos().x - game->camera.x + game->renderOffset.x, 
                 get_hitbox().y - game->camera.y+ game->renderOffset.y);

    switch (game->currDialogue)
        {
            case fox_base_dialogue_1: 
            {
                std::string txt = "1. Where is here?\n2. What are YOU doing here?";

                auto diag = std::make_shared<DialogueRender>(
                    txt, Vector2Int(p.x, p.y - 60),
                    game->window
                );
                diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);

                break;
            }

            case fox_base_dialogue_1_1: 
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "1. Where is here?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 30),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case fox_base_dialogue_1_2:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "2. What are YOU doing here?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 30),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case fox_base_dialogue_2:
            {
                std::string txt = "1. But how do I\nget past that river?";

                auto diag = std::make_shared<DialogueRender>(
                    txt, Vector2Int(p.x, p.y - 60),
                    game->window
                );
                diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);

                break;
            }

            case fox_base_dialogue_2_1:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "1. But how do I\nget past that river?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 60),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case fox_base_dialogue_3:
            {
                std::string txt = "1. How do I do THAT?\n2. How are YOU getting there?";

                auto diag = std::make_shared<DialogueRender>(
                    txt, Vector2Int(p.x, p.y - 60),
                    game->window
                );
                diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);

                break;
            }

            case fox_base_dialogue_3_1:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "1. How do I do THAT?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 30),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case fox_base_dialogue_3_2:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "2. How are YOU getting there?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 30),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case fox_town_1:
            {
                std::string txt = "1. What is this place?\n2. How did YOU make it up here?\n3. Who are all these wolves?";

                auto diag = std::make_shared<DialogueRender>(
                    txt, Vector2Int(p.x, p.y - 90),
                    game->window
                );
                diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);

                break;
            }

            case fox_town_1_1:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "1. What is this place?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 30),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case fox_town_1_2:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "2. How did YOU make it up here?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 30),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }
            
            case fox_town_1_3:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "3. Who are all these wolves?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 30),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case fox_town_2:
            {
                std::string txt = "1. You could've told me that earlier!\n2. Do you have any advice?\n3. Can I just stay here?";

                auto diag = std::make_shared<DialogueRender>(
                    txt, Vector2Int(p.x, p.y - 90),
                    game->window
                );
                diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);

                break;
            }

            case fox_town_2_1:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "1. You could've told me that earlier!";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 30),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }
            
            case fox_town_2_2:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "2. Do you have any advice?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 30),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case fox_town_2_3:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "3. Can I just stay here?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 30),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }
        
            case bear_town_1:
            {
                std::string txt = "1. Hello\n2. What are you doing?";

                auto diag = std::make_shared<DialogueRender>(
                    txt, Vector2Int(p.x, p.y - 60),
                    game->window
                );
                diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);

                break; 
            }

            case bear_town_1_1:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "1. Hello";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 30),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case bear_town_1_2:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "2. What are you doing?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 30),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case bear_town_2:
            {
                std::string txt = "1. Yes\n2. No";

                auto diag = std::make_shared<DialogueRender>(
                    txt, Vector2Int(p.x, p.y - 60),
                    game->window
                );
                diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);

                break; 
            }

            case bear_town_2_1:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "1. Yes";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 30),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }
            
            case bear_town_2_2:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "1. No";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 30),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case bear_town_3:
            {
                std::string txt = "1. Where do you get\nall these berries?\n2. How do you survive\nwith all the wolves?";

                auto diag = std::make_shared<DialogueRender>(
                    txt, Vector2Int(p.x, p.y - 120),
                    game->window
                );
                diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);

                break; 
            }

            case bear_town_3_1:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "1. Where do you get\nall these berries?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 60),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case bear_town_3_2:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "2. How do you survive\nwith all the wolves?";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 60),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case bear_town_4:
            {
                std::string txt = "1. Yeah!\n2. No thanks";

                auto diag = std::make_shared<DialogueRender>(
                    txt, Vector2Int(p.x, p.y - 60),
                    game->window
                );
                diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);

                break; 
            }

            case bear_town_4_1:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "1. Yeah!";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 30),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }
            
            case bear_town_4_2:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "1. No Thanks";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 30),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }
            
            case bear_town_5:
            {
                std::string txt = "1. Sure thing!\n ";

                std::vector<std::string> vec = {txt, "2. What do you need ", " for?"};
                SDL_Color colour = {255,255,255,255};

                auto diag = std::make_shared<DialogueRender>(
                    vec, Vector2Int(p.x, p.y-60), game->window,
                    colour, Centred, &DialogueRender::bear_town_5_diag,
                    game->stoneTex
                );

                diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                break; 
            }

            case bear_town_5_1:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "1. Sure thing!";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 30),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }
            
            case bear_town_5_2:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::vector<std::string> vec = {"2. What do you need ", " for?"};
                        SDL_Color colour = {255,255,255,255};

                        auto diag = std::make_shared<DialogueRender>(
                            vec, Vector2Int(p.x, p.y-30), game->window,
                            colour, Centred, &DialogueRender::bear_town_5_2_diag,
                            game->stoneTex
                        );

                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }

            case rabbit_town_1:
            {
                std::string txt = "1. Yeah, sure\n2. I'm good";

                auto diag = std::make_shared<DialogueRender>(
                    txt, Vector2Int(p.x, p.y - 60),
                    game->window
                );
                diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);

                break; 
            }

            case rabbit_town_1_1:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "1. Yeah, sure";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 30),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }
            
            case rabbit_town_1_2:
            {
                if (dialogueTimer >= 0.0f) {
                    dialogueTimer -= get_deltaTime();
                    if (dialogueTimer < 1.0f) 
                    {
                        std::string txt = "2. I'm good";
                        auto diag = std::make_shared<DialogueRender>(
                            txt, Vector2Int(p.x, p.y - 30),
                            game->window
                        );
                        diag->set_font(game->arcadeClassic24); game->dialogueRenders.push(diag);
                    }
                }
                break;
            }
        }
}





void Game::handle_dialogue( int e )
{
    switch (currDialogue)
    {
        case test_dialogue:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(test_dialogue_2); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
                case SDLK_2:
                case SDLK_KP_2: 
                    enter_dialogue(test_dialogue_3); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
            } break;

        case fox_base_dialogue_1:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(fox_base_dialogue_1_1); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
                case SDLK_2:
                case SDLK_KP_2: 
                    enter_dialogue(fox_base_dialogue_1_2); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
            } break;

        case fox_base_dialogue_2:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(fox_base_dialogue_2_1); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
            } break;

        case fox_base_dialogue_3:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(fox_base_dialogue_3_1); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
                case SDLK_2:
                case SDLK_KP_2: 
                    enter_dialogue(fox_base_dialogue_3_2); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
            } break;
    
        case fox_town_1:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(fox_town_1_1); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
                case SDLK_2:
                case SDLK_KP_2: 
                    enter_dialogue(fox_town_1_2); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
                case SDLK_3:
                case SDLK_KP_3:
                    enter_dialogue(fox_town_1_3);
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
            } break;

        case fox_town_2:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(fox_town_2_1); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
                case SDLK_2:
                case SDLK_KP_2: 
                    enter_dialogue(fox_town_2_2); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
                case SDLK_3:
                case SDLK_KP_3:
                    enter_dialogue(fox_town_2_3);
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
            } break;

        case bear_town_1:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(bear_town_1_1); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
                case SDLK_2:
                case SDLK_KP_2: 
                    enter_dialogue(bear_town_1_2); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
            } break;
        
        case bear_town_2:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(bear_town_2_1); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
                case SDLK_2:
                case SDLK_KP_2: 
                    enter_dialogue(bear_town_2_2); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
            } break;
        
        case bear_town_3:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(bear_town_3_1); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
                case SDLK_2:
                case SDLK_KP_2: 
                    enter_dialogue(bear_town_3_2); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
            } break;
        
        case bear_town_4:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(bear_town_4_1); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
                case SDLK_2:
                case SDLK_KP_2: 
                    enter_dialogue(bear_town_4_2); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
            } break;
        
        case bear_town_5:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1: 
                    enter_dialogue(bear_town_5_1); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
                case SDLK_2:
                case SDLK_KP_2: 
                    enter_dialogue(bear_town_5_2); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
            } break;

        case rabbit_town_1:
            switch (e)
            {
                case SDLK_1:
                case SDLK_KP_1:
                    enter_dialogue(rabbit_town_1_1);
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
                case SDLK_KP_2: 
                    enter_dialogue(rabbit_town_1_2); 
                    currLevel->player->set_dialogueTimer(1.1f);
                    break;
            } break;
    }
}











DialogueRender::DialogueRender(std::string txt, Vector2Int pos, std::shared_ptr<LWindow> window, SDL_Color colour, TextOrientation orientation, void (DialogueRender::*func)(), std::shared_ptr<LTexture> tex0, std::shared_ptr<LTexture> tex1)
: pos(pos), window(window), colour(colour), orientation(orientation), renderFunc(func), texture0(tex0), texture1(tex1)
{
    bkgColour = {Uint8(colour.r/2),Uint8(colour.g/2), Uint8(colour.b/2), Uint8(colour.a-50)};
    strings.push_back(txt);
}

DialogueRender::DialogueRender(std::vector<std::string> strings, Vector2Int pos, std::shared_ptr<LWindow> window, SDL_Color colour, TextOrientation orientation, void (DialogueRender::*func)(), std::shared_ptr<LTexture> tex0, std::shared_ptr<LTexture> tex1)
: strings(strings), pos(pos), window(window), colour(colour), orientation(orientation), renderFunc(func), texture0(tex0), texture1(tex1) 
{
    bkgColour = {Uint8(colour.r/2),Uint8(colour.g/2), Uint8(colour.b/2), Uint8(colour.a-25)};
}

DialogueRender::~DialogueRender() {}

void DialogueRender::set_font( TTF_Font * Font ) { font = Font; };

void DialogueRender::render()
{
    (this->*renderFunc)();
}

void DialogueRender::defaultTextRender()
{
    renderText(strings[0], pos.x-BKG_DISPLACE, pos.y+BKG_DISPLACE, window, bkgColour, font, orientation);
    renderText(strings[0], pos.x, pos.y, window, colour, font, orientation);
}

void DialogueRender::bear_town_5_diag()
{
    if (strings.size() != 3) return;

    renderText(strings[0], pos.x-BKG_DISPLACE, pos.y+BKG_DISPLACE, window, bkgColour, font, orientation);
    renderText(strings[0], pos.x, pos.y, window, colour, font, orientation);

    std::string txt0 = strings[1], txt1 = strings[2];
    auto tex0 = std::make_unique<LTexture>(window),
         tex1 = std::make_unique<LTexture>(window), 
         tex2 = std::make_unique<LTexture>(window), 
         tex3 = std::make_unique<LTexture>(window); 
    
    tex0->loadFromRenderedText(txt0, colour, font);
    tex1->loadFromRenderedText(txt1, colour, font);
    tex2->loadFromRenderedText(txt0, bkgColour, font);
    tex3->loadFromRenderedText(txt1, bkgColour, font);

    int w = tex0->getWidth()+tex1->getWidth()+30,
        x = pos.x - (w/2), y = pos.y+30;
    SDL_Rect rect = {0, 0, 30, 30};

    tex2->render(x-BKG_DISPLACE, y+BKG_DISPLACE); tex0->render(x, y); x += tex0->getWidth();
    texture0->render(x, y, &rect); x += 30;
    tex3->render(x-BKG_DISPLACE, y+BKG_DISPLACE); tex1->render(x, y);

    tex0->free(); tex1->free();
    tex2->free(); tex3->free();
}

void DialogueRender::bear_town_5_2_diag()
{
    if (strings.size() != 2) return;

    std::string txt0 = strings[0], txt1 = strings[1];
    auto tex0 = std::make_unique<LTexture>(window),
         tex1 = std::make_unique<LTexture>(window), 
         tex2 = std::make_unique<LTexture>(window), 
         tex3 = std::make_unique<LTexture>(window); 
    
    tex0->loadFromRenderedText(txt0, colour, font);
    tex1->loadFromRenderedText(txt1, colour, font);
    tex2->loadFromRenderedText(txt0, bkgColour, font);
    tex3->loadFromRenderedText(txt1, bkgColour, font);

    int w = tex0->getWidth()+tex1->getWidth()+30,
        x = pos.x - (w/2), y = pos.y;
    SDL_Rect rect = {0, 0, 30, 30};

    tex2->render(x-BKG_DISPLACE, y+BKG_DISPLACE); tex0->render(x, y); x += tex0->getWidth();
    texture0->render(x, y, &rect); x += 30;
    tex3->render(x-BKG_DISPLACE, y+BKG_DISPLACE); tex1->render(x, y);

    tex0->free(); tex1->free();
    tex2->free(); tex3->free();
}

void DialogueRender::bear_town_case14()
{
    if (strings.size() != 2) return;

    std::string txt0 = strings[0], txt1 = strings[1]; 

    auto tex0 = std::make_unique<LTexture>(window),
         tex1 = std::make_unique<LTexture>(window), 
         tex2 = std::make_unique<LTexture>(window), 
         tex3 = std::make_unique<LTexture>(window); 
    
    tex0->loadFromRenderedText(txt0, colour, font);
    tex1->loadFromRenderedText(txt1, colour, font);
    tex2->loadFromRenderedText(txt0, bkgColour, font);
    tex3->loadFromRenderedText(txt1, bkgColour, font);

    int w = tex0->getWidth()+tex1->getWidth() + 60,
        x = pos.x - (w/2), y = pos.y;
    SDL_Rect rect = {0, 0, 30, 30};

    tex2->render(x-BKG_DISPLACE, y+BKG_DISPLACE); tex0->render(x, y); x += tex0->getWidth();
    texture0->render(x, y, &rect); x += 30;
    tex3->render(x-BKG_DISPLACE, y+BKG_DISPLACE); tex1->render(x, y); x += tex1->getWidth();
    texture1->render(x, y, &rect);

    tex0->free(); tex1->free();
    tex2->free(); tex3->free();
}

void DialogueRender::fox_base_case11()
{
    if (strings.size() != 4) return;

    renderText(strings[0], pos.x-BKG_DISPLACE, pos.y+BKG_DISPLACE, window, bkgColour, font, orientation);
    renderText(strings[0], pos.x, pos.y, window, colour, font, orientation);

    std::string txt0 = strings[1], txt1 = strings[2], txt2 = strings[3];
    auto tex0 = std::make_unique<LTexture>(window),
         tex1 = std::make_unique<LTexture>(window),
         tex2 = std::make_unique<LTexture>(window),
         tex3 = std::make_unique<LTexture>(window),
         tex4 = std::make_unique<LTexture>(window),
         tex5 = std::make_unique<LTexture>(window);

    tex0->loadFromRenderedText(txt0, colour, font);
    tex1->loadFromRenderedText(txt1, colour, font);
    tex2->loadFromRenderedText(txt2, colour, font);
    tex3->loadFromRenderedText(txt0, bkgColour, font);
    tex4->loadFromRenderedText(txt1, bkgColour, font);
    tex5->loadFromRenderedText(txt2, bkgColour, font);

    int w = tex0->getWidth()+tex1->getWidth()+tex2->getWidth()+60,
        x = pos.x - (w/2), y = pos.y + 30;
    SDL_Rect rect = {0, 0, 30, 30};

    tex3->render(x-BKG_DISPLACE, y+BKG_DISPLACE); tex0->render(x, y); x += tex0->getWidth();
    texture0->render(x, y, &rect); x += 30;
    tex4->render(x-BKG_DISPLACE, y+BKG_DISPLACE); tex1->render(x, y); x += tex1->getWidth();
    texture1->render(x, y, &rect); x += 30;
    tex5->render(x-BKG_DISPLACE, y+BKG_DISPLACE); tex2->render(x, y);

    tex0->free(); tex1->free(); tex2->free();
    tex3->free(); tex4->free(); tex5->free();
}

void DialogueRender::rabbit_town_case2()
{
    if (strings.size() != 3) return;

    renderText(strings[0], pos.x-BKG_DISPLACE, pos.y+BKG_DISPLACE, window, bkgColour, font, orientation);
    renderText(strings[0], pos.x, pos.y, window, colour, font, orientation);

    std::string txt0 = strings[1], txt1 = strings[2];

    auto tex0 = std::make_shared<LTexture>(window),
         tex1 = std::make_shared<LTexture>(window),
         tex2 = std::make_shared<LTexture>(window),
         tex3 = std::make_shared<LTexture>(window);

    tex0->loadFromRenderedText(txt0, colour, font);
    tex1->loadFromRenderedText(txt1, colour, font);
    tex2->loadFromRenderedText(txt0, bkgColour, font);
    tex3->loadFromRenderedText(txt1, bkgColour, font);

    int w = tex0->getWidth() + 30,
        x = pos.x - (w/2), y = pos.y + 30;
    SDL_Rect rect = {0, 0, 30, 30};

    tex2->render(x-BKG_DISPLACE, y+BKG_DISPLACE); tex0->render(x, y); x += tex0->getWidth();
    texture0->render(x, y, &rect); x += 30;
    tex3->render(x-BKG_DISPLACE, y+BKG_DISPLACE); tex1->render(x, y);

    tex0->free(); tex1->free(); tex2->free(); tex3->free();
}

void DialogueRender::rabbit_town_case4()
{
    if (strings.size() != 2) return;

    renderText(strings[0], pos.x-BKG_DISPLACE, pos.y+BKG_DISPLACE, window, bkgColour, font, orientation);
    renderText(strings[0], pos.x, pos.y, window, colour, font, orientation);

    auto tex0 = std::make_unique<LTexture>(window),
         tex1 = std::make_unique<LTexture>(window);

    tex0->loadFromRenderedText(strings[1], colour, font);
    tex1->loadFromRenderedText(strings[1], bkgColour, font);

    int w = tex0->getWidth() + 30,
        x = pos.x - (w/2), y = pos.y + 60;
    SDL_Rect rect = {0, 0, 30, 30};

    tex1->render(x-BKG_DISPLACE, y+BKG_DISPLACE); tex0->render(x, y); x += tex0->getWidth();
    texture0->render(x, y, &rect);

    tex1->free(); tex0->free();
}

void DialogueRender::rabbit_town_case5()
{
    if (strings.size() != 2) return;

    std::string txt0 = strings[0], txt1 = strings[1];
    auto tex0 = std::make_unique<LTexture>(window),
         tex1 = std::make_unique<LTexture>(window),
         tex2 = std::make_unique<LTexture>(window),
         tex3 = std::make_unique<LTexture>(window);

    tex0->loadFromRenderedText(txt0, colour, font);
    tex1->loadFromRenderedText(txt1, colour, font);
    tex2->loadFromRenderedText(txt0, bkgColour, font);
    tex3->loadFromRenderedText(txt1, bkgColour, font);

    int w = tex0->getWidth() + tex1->getWidth() + 60,
        x = pos.x -(w/2), y = pos.y;
    SDL_Rect rect{0, 0, 30, 30};

    tex2->render(x-BKG_DISPLACE, y+BKG_DISPLACE); tex0->render(x, y); x += tex0->getWidth();
    texture0->render(x, y, &rect); x += 30;
    tex3->render(x-BKG_DISPLACE, y+BKG_DISPLACE); tex1->render(x, y); x += tex1->getWidth();
    texture1->render(x, y, &rect);

    tex0->free(); tex1->free();
    tex2->free(); tex3->free();
}

void DialogueRender::rabbit_town_case8()
{
    if (strings.size() != 3) return;

    renderText(strings[0], pos.x-BKG_DISPLACE, pos.y+BKG_DISPLACE, window, bkgColour, font, orientation);
    renderText(strings[0], pos.x, pos.y, window, colour, font, orientation);

    std::string txt0 = strings[1], txt1 = strings[2];
    auto tex0 = std::make_unique<LTexture>(window),
         tex1 = std::make_unique<LTexture>(window),
         tex2 = std::make_unique<LTexture>(window),
         tex3 = std::make_unique<LTexture>(window);

    tex0->loadFromRenderedText(txt0, colour, font);
    tex1->loadFromRenderedText(txt1, colour, font);
    tex2->loadFromRenderedText(txt0, bkgColour, font);
    tex3->loadFromRenderedText(txt1, bkgColour, font);

    int w = tex0->getWidth() + tex1->getWidth() + 30,
        x = pos.x -(w/2), y = pos.y + 30;
    SDL_Rect rect{0, 0, 30, 30};

    tex2->render(x-BKG_DISPLACE, y+BKG_DISPLACE); tex0->render(x, y); x += tex0->getWidth();
    texture0->render(x, y, &rect); x += 30;
    tex3->render(x-BKG_DISPLACE, y+BKG_DISPLACE); tex1->render(x, y); x += tex1->getWidth();

    tex0->free(); tex1->free();
    tex2->free(); tex3->free();
}