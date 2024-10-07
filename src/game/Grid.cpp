#include "Game.hpp"
#include "CellTypes.hpp"
#include "Scene.hpp"




bool Game::is_barrier( Vector2Int cell, Scene *level ) {
    if (level == NULL) level = currLevel;
    // validate the cell 
    if (cell.x < 0 || cell.y < 0 || cell.x>=level->gridDimensions.x || cell.y>=level->gridDimensions.y) return true;
    return level->grid[cell.x][cell.y]&BARRIER; 
}

bool Game::is_occupied( Vector2Int cell, Scene *level ) {
    if (level == NULL) level = currLevel;
    // validate the cell
    if (cell.x < 0 || cell.y < 0 || cell.x>=level->gridDimensions.x || cell.y>=level->gridDimensions.y) return false;
    return level->grid[cell.x][cell.y]&OCCUPIED;
}



// place an object into a cell in the global grid
int Game::PlaceObjectInCell(Vector2Int cell, int objType, bool playerPlacement, Scene *level)
{
    if (level == NULL) level = currLevel;
    bool draw = level == currLevel;

    // cell is out of bounds and doesn't exist
    if (cell.x<0 || cell.x>=level->gridDimensions.x ||
        cell.y<0 || cell.y>=level->gridDimensions.y) return -1;

    int num = level->grid[cell.x][cell.y];
    // a rect representing the area of the cell in the map

    int sideLen = level->cell_sideLen;
    int x = cell.x * sideLen, y = cell.y * sideLen;
    SDL_Rect cellRect = { x, y, sideLen, sideLen };

    switch (objType & CELL_ID) // just the first byte, containing cell ID
    {
        case 1: // log
        {
            // cell is occupied, AND the placement was made by player, not loading
            if (((num&OCCUPIED) || (num&WATER && !(num&IS_DRIED))) && playerPlacement) return -2;
            // update the grid value to represent a log
            num |= LOG;
            if (playerPlacement) logDestruction->play();
            break;
        }


        case 2: // DAM

            // cell is occupied, AND the placement was made by player, not loading
            if (num&0x4000 && playerPlacement) return -2;
            // update the grid value to represent a DAM
            num |= DAM;
            num &= ~BARRIER; // turn off barrier bit
            if (playerPlacement) logDestruction->play();
            break;

        case 3: // tree
            // set the cell space to be a tree
            // use = (not |=) because previous data SHOULD be overwritten; if the tree grew
            // from a sapling, the cell should no longer contain any data about the sapling
            num = TREE;
            break;

        
        case 4: // stump
            // only occurs when a tree gets removed, thus the cell could only have been TREE prior.
            // so, there is no issue just settig the cell to STUMP, no need for |=
            num = STUMP;
            break;


        case 5: // sapling
            // cell is occupied or water, AND the placement was made by player, not loading
            if (((num&OCCUPIED) || (num&WATER && !(num&IS_DRIED))) && playerPlacement) return -2;
            // update the grid to represent a sapling
            num |= SAPLING;
            if (playerPlacement) leaves->play();
            break;


        case 6: { // world border
            // update the grid to represent a border
            num = BORDER;
            break;
        }

        case 7: // closed door
            // cell is occupied or water, AND the placement was made by player, not loading
            if (((num&OCCUPIED) || (num&WATER && !(num&IS_DRIED))) && playerPlacement) {
                if ((num&CELL_ID) != 8) return -2;
                num &= ~(CELL_ID|BARRIER);
                num |= CLOSED_DOOR;
                doorToggle->play();
            } else {
                num |= CLOSED_DOOR;
                if (playerPlacement) logDestruction->play();
            }
            break;

        case 8: // open door
            // cell is occupied or water, AND the placement was made by player, not loading
            if (((num&OCCUPIED) || (num&WATER && !(num&IS_DRIED))) && playerPlacement) {
                if ((num&255) != 7) return -2;
                num &= ~(CELL_ID|BARRIER);
                num |= OPEN_DOOR;
                doorToggle->play();
            } else {
                num |= OPEN_DOOR;
                if (playerPlacement) logDestruction->play();
            }
            break;

        case 10: // berry bush
            num = BERRY_BUSH;
            break;

        case 11: // empty berry bush
            num = EMPTY_BUSH;
            break;


        case EMPTY:
        {
            // cannot remove cells while holding an object
            if (level->held != nullptr && playerPlacement) return -4;

            // remove building in the cell
            // if it IS indestructible or NOT occupied, return
            if (num&0x8000 && playerPlacement) return -3;

            // first byte represents info about the cell, this is the object being removed
            switch (num&CELL_ID)
            {
                case 1: { // log
                    // spawn a log
                    Vector2 pos(x-10.0f+(cellRect.w/2), y-10.0f+(cellRect.h/2));
                    spawnItemStack(logItem, pos, 1);
                    playerDamage->play();
                    break;
                }

                case 2: { // DAM
                    // spawn a DAM
                    Vector2 pos(x-10.0f+(cellRect.w/2), y-10.0f+(cellRect.h/2));
                    spawnItemStack(damItem, pos, 1);
                    playerDamage->play();
                    break;
                }

                case 3: { // tree
                    int hp = num&HEALTH;
                    if (hp != 0) {
                        // damage the tree until broken
                        damageCell(cell, 2, NULL);
                        return 0;

                    } else {
                        // if the tree is destroyed, remove it and place a stump
                        // remove tree from the grid
                        level->grid[cell.x][cell.y] &= 0xFF9000;
                        // spawn a falling tree in its place
                        float x = ((float)cell.x+0.5f)*(float)sideLen, y = ((float)cell.y - ((float)TREE_HEIGHT/2.0f))*(float)sideLen;
                        Instantiate(fallingTree, Vector2(x, y), 1, level);
                        treeFalling->play();

                        // place a stump where the tree once stood
                        // everything else handled in the STUMP case. don't break, just return
                        return PlaceObjectInCell(cell, STUMP, false, level);
                    }
                    break;
                }

                case 4: { // stump
                    // spawn a log
                    Vector2 pos(x-10.0f+(cellRect.w/2), y-10.0f+(cellRect.h/2));
                    spawnItemStack(logItem, pos, 1);
                    playerDamage->play();
                    break;
                }

                case 5: { // sapling
                    // spawn a pine cone
                    Vector2 pos(x-10.0f+(cellRect.w/2), y-10.0f+(cellRect.h/2));
                    spawnItemStack(pineConeItem, pos, 1);
                    leaves->play();
                    break;
                }

                case 6: return 0; // world border cannot be modified, exit the function

                case 7: // closed door
                case 8: { // open door
                    // spawn a door
                    Vector2 pos(x-10.0f+(cellRect.w/2), y-10.0f+(cellRect.h/2));
                    spawnItemStack(doorItem, pos, 1);
                    playerDamage->play();
                    break;
                }

                case 10: { // berry bush
                    // spawn a berry
                    Vector2 pos(x-10.0f+(cellRect.w/2), y-10.0f+(cellRect.h/2));
                    spawnItemStack(berryItem, pos, 1);
                    // place a depleted bush where the berry bush is
                    leaves->play();
                    return PlaceObjectInCell(cell, EMPTY_BUSH, false, level);
                }

                case 11: return 0; // berry bush can't be removed

                default: break;
            }
            
            // clear bits of data, and toggle collision based on whether the cell is water by default
            num &= ~(CELL_ID|HEALTH|OCCUPIED|BARRIER|TIMER|MAX_HEALTH);
            if (num&WATER && !(num&IS_DRIED)) num |= BARRIER;
            else num &= ~BARRIER;
            break;
        }
    }
    level->grid[cell.x][cell.y] = num;

    if (draw) drawCell(cell);

    if (level == &Base && playerPlacement) damRiver();

    return 0;
}

void Game::drawCell( Vector2Int cell )
{
    // draw the tile background first
    int num = currLevel->grid[cell.x][cell.y], sideLen = currLevel->cell_sideLen;
    SDL_Rect cellRect = { cell.x * sideLen, cell.y * sideLen, sideLen, sideLen };

    if (num&WATER) DrawWaterToCell(cell, cellRect);
    else tEditor.renderTextureToTexture(BGTexture, grassTex, &cellRect);

    switch (num&255)
    {
        case 1: tEditor.renderTextureToTexture(BGTexture, logTex, &cellRect); break;

        case 2: tEditor.renderTextureToTexture(BGTexture, damTex, &cellRect); break;

        case 3: AddTreeToOverlay(cell); break;

        case 4: 
            RemoveTreeFromOverlay(cell);
            tEditor.renderTextureToTexture(BGTexture, stumpTex, &cellRect); 
            break;

        case 5: tEditor.renderTextureToTexture(BGTexture, saplingTex, &cellRect); break;

        case 6: {
            std::shared_ptr<LTexture> black = tEditor.createSolidColour(sideLen, sideLen, 0x000000FF, window);
            tEditor.renderTextureToTexture(BGTexture, black, &cellRect);
            black->free();
            break;
        }

        case 7: tEditor.renderTextureToTexture(BGTexture, closed_doorTex, &cellRect); break;

        case 8: tEditor.renderTextureToTexture(BGTexture, open_doorTex, &cellRect); break;

        case 10: tEditor.renderTextureToTexture(BGTexture, berry_bushTex, &cellRect); break;

        case 11: tEditor.renderTextureToTexture(BGTexture, empty_bushTex, &cellRect); break;
    }
}

void Game::update_cells( Scene *level )
{
    Vector2Int dimensions = level->gridDimensions;

    for (int x = 0; x < dimensions.x; x++) {
        for (int y = 0; y < dimensions.y; y++) 
        {
            Vector2Int currCell(x, y);

            if (is_occupied(currCell, level)) {
                int type = level->grid[x][y];

                if (type&WATER && !(type&IS_DRIED)) 
                {
                    int val = (int)DAY_LENGTH / WATER_DAMAGE_INTERVAL;
                    if ((int)g_time%val == 0 && int(g_time-deltaTime)%val != 0) 
                    {
                        // interpolate between river beginning and end
                        // at the left of the map will damage equal to the cell's max hp,
                        // at the right of the map will deal one damage

                        float maxDamage = float((type&MAX_HEALTH)>>17);
                        
                        // use linear interpolation
                        float t = (float)x / (float)dimensions.x;
                        // t = 0 when x is zero (the max damage should be dealt), and
                        // t = 1 when x = dimensions.x (the minimum damage).
                        int damage = int(ceilf(maxDamage * (1.0f-t)) * WATER_DAMAGE_MULT);

                        damageCell(currCell, damage, level);
                    }
                } 
                else 
                {
                    switch (type&255)
                    {
                        case 5: // sapling
                            // when planted in the base, saplings will grow every three days.
                            // otherwise they grow every day
                            if (g_time == 0.0f && !isNight) {
                                int timer = (level != &Woods)? (type&TIMER)>>26 : 2;
                                if (timer >= 2) {
                                    PlaceObjectInCell(currCell, TREE, false, level);
                                } else {
                                    timer++; timer <<= 26;
                                    type &= ~TIMER;
                                    type |= timer;
                                    level->grid[x][y] = type;
                                }
                            }
                            break;

                        case 11: // empty berry bush
                            // grow new berries every day
                            if (g_time == 0.0f && !isNight) {
                                PlaceObjectInCell(currCell, BERRY_BUSH, false, level);
                            }
                            break;
                    }
                }
            }
        }
    }
}

void Game::update_cells()
{
    // update every 2 seconds
    if (((int(g_time)%2) == 0 && (int(g_time-deltaTime)%2) != 0) || g_time == 0.0f) {
        update_cells(&Base);
        update_cells(&Woods);
        update_cells(&Town);
    }
}


void Game::AddTreeToOverlay( Vector2Int cell )
{
    int sideLen = currLevel->cell_sideLen;
    int w = TREE_WIDTH/2, h = TREE_HEIGHT-1;
    SDL_Rect rect = {(cell.x-w)*sideLen, (cell.y-h)*sideLen, sideLen*TREE_WIDTH, sideLen*TREE_HEIGHT};
    tEditor.renderTextureToTexture(overlayTexture, treeTex, &rect);
}

void Game::RemoveTreeFromOverlay( Vector2Int cell )
{
    // reset the overlay texture
    int sideLen = currLevel->cell_sideLen;
    int nx = currLevel->gridDimensions.x, ny = currLevel->gridDimensions.y;
    map = { 0, 0, nx*sideLen, ny*sideLen };

    if (overlayTexture != nullptr) overlayTexture->free();
    overlayTexture = tEditor.createEmptyTexture(map.w, map.h, window);

    for (int y = 0; y < ny; y++) {
        for (int x = 0; x < nx; x++) {
            int type = currLevel->grid[x][y]&255;
            if (type == 3) AddTreeToOverlay( Vector2Int(x, y) );
        }
    }
}

void Game::render_cell_health()
{
    int nx = currLevel->gridDimensions.x, ny = currLevel->gridDimensions.y;
    int sideLen = currLevel->cell_sideLen;
    for (int x = 0; x < nx; x++) 
    {
        int xPos = x*sideLen - camera.x;
        for (int y = 0; y < ny; y++) 
        {
            Vector2Int p( xPos, y*sideLen - camera.y );
            // not within the camera's view, don't render
            if (p.x != clamp(-sideLen, camera.w, p.x) || p.y != clamp(-sideLen, camera.h, p.y)) {
                continue;
            }

            int num = currLevel->grid[x][y];
            int max_hp = (num&MAX_HEALTH)>>17,
                health = (num&HEALTH)>>8;

            // don't show the health bar for cells at maxHP or 0 hp
            if (health < max_hp && health != 0) 
            {
                float t = (float)health / (float) max_hp;
                int wWhite = sideLen*t, wRed = sideLen*(1.0f-t);

                auto white = tEditor.createSolidColour(wWhite, 20, 0xFFFFFFFF, window);
                auto red   = tEditor.createSolidColour(wRed, 20, 0xFF0000FF, window);

                SDL_Rect wRect = {p.x, p.y, wWhite, 20}, rRect = {p.x+wWhite, p.y, wRed, 20};

                white->render(wRect.x + renderOffset.x, wRect.y + renderOffset.y, &wRect);
                red->render(rRect.x + renderOffset.x, rRect.y + renderOffset.y, &rRect);

                white->free(); red->free();
            }
        }
    }
}


void Game::DrawWaterToCell( Vector2Int cell, SDL_Rect cellRect )
{
    // dimensions of grid -1
    Vector2Int dimensions = currLevel->gridDimensions - Vector2Int_One;
    // ensure the cell is in bounds
    if (cell.x<0 || cell.x>dimensions.x || cell.y<0 || cell.y>dimensions.y) return;

    // get the grid
    std::vector<std::vector<int>> grid = currLevel->grid;
    // ensure the cell is actuall water
    if (!(grid[cell.x][cell.y]&WATER)) return;

    // draw the base water image to the cell
    if (grid[cell.x][cell.y]&IS_DRIED) tEditor.renderTextureToTexture(BGTexture, dirtTex, &cellRect);
    else tEditor.renderTextureToTexture(BGTexture, waterTex, &cellRect);

    // sample the four adjacent cells to see where shoreline should be drawn
    Uint8 shoreLocations = 0b00000000; // lrLR TBLR
    /*
        METHOD:
        
        the eight bits of shoreLocations represent the following:

        1: there is a shoreline on the TOP side of the cell
        2: there is a shoreline on the RIGHT side of the cell
        3: there is a shoreline on the BOTTOM of the cell
        4: there is a shoreline on the LEFT of the cell

        5: there is shore in the TOP LEFT corner of the cell
        6: there is shore in the TOP RIGHT corner of the cell
        7: there is shore in the BOTTOM RIGHT corner of the cell
        8: there is shore in the BOTTOM LEFT corner of the cell

        run checks on the surrounding cells to see where there is and isn't land, in order to determine
        where there should and shouldn't be shore, setting these bits
    */
    // check the cell above
    if (cell.y >= 1) {
        int num = grid[cell.x][cell.y-1];
        if (!(num&WATER || num==BORDER)) shoreLocations |= 0x01;
    }
    // check the cell to the right
    if (cell.x < dimensions.x) {
        int num = grid[cell.x+1][cell.y];
        if (!(num&WATER || num==BORDER)) shoreLocations |= 0x02;
    }
    // check the cell below
    if (cell.y < dimensions.y) {
        int num = grid[cell.x][cell.y+1];
        if (!(num&WATER || num==BORDER)) shoreLocations |= 0x04;
    }
    // check the cell to the left
    if (cell.x >= 1) {
        int num = grid[cell.x-1][cell.y];
        if (!(num&WATER || num==BORDER)) shoreLocations |= 0x08;
    }
    // check the corners
    if (!(shoreLocations&0x09)) { // no top or left shoreline
        // check cell to the top left
        if (cell.x >= 1 && cell.y >= 1) {
            int num = grid[cell.x-1][cell.y-1];
            if (!(num&WATER || num==BORDER)) shoreLocations |= 0x10;
        }
    }
    if (!(shoreLocations&0x03)) { // no top or right shoreline
        // check the cell to the top right
        if (cell.x < dimensions.x && cell.y >= 1) {
            int num = grid[cell.x+1][cell.y-1];
            if (!(num&WATER || num==BORDER)) shoreLocations |= 0x20;
        }
    }
    if (!(shoreLocations&0x06)) { // no bottom or right shoreline
        // check the cell to the bottom right
        if (cell.x < dimensions.x && cell.y < dimensions.y) {
            int num = grid[cell.x+1][cell.y+1];
            if (!(num&WATER || num==BORDER)) shoreLocations |= 0x40;
        }
    }
    if (!(shoreLocations&0x0C)) { // no bottom or left shoreline
        // check the cell to the bottom left
        if (cell.x >= 1 && cell.y < dimensions.y) {
            int num = grid[cell.x-1][cell.y+1];
            if (!(num&WATER || num==BORDER)) shoreLocations |= 0x80;
        }
    }

    // now, based off shore locations, draw the shorelines
    if (shoreLocations & 0xF0) { // there are corner shorelines
        for (int i = 0; i < 4; i++) {
            // if theere is a corner for the specified bit, draw the corresponding corner
            Uint8 bit = shoreLocations & (0x10<<i);
            // draw the image, rotated 90 degrees per corner
            if (bit) tEditor.renderTextureToTexture(BGTexture, shoreline4Tex, &cellRect, 90.0*i);
        }
    }

    // count the number of surrounding shores by counting the number of the first four bits activated
    int numShores = numBits(static_cast<int>(shoreLocations&0x0f));

    switch (numShores)
    {
        case 1: 
            // only one shoreline exists, find it, and draw it
            for (int i = 0; i < 4; i++) {
                Uint8 bit = shoreLocations & (0x01<<i);
                if (bit) tEditor.renderTextureToTexture(BGTexture, shoreline0Tex, &cellRect, 90.0*i);
            }
            break;

        case 2: {
            // if the two shores are opposite each other (top/bottom, or left/right), then shoreline0
            // will be drawn twice. if they are next to each other, shoreline1 will be drawn
            int val = shoreLocations & 0x0F; // first 4 bits

            if ((shoreLocations&0x05)==val || (shoreLocations&0x0A)==val) { // top/bottom OR left/right
                // draw shoreline 0 at any place a bit is active
                for (int i = 0; i < 4; i++) {
                    // if theere is a corner for the specified bit, draw the corresponding corner
                    Uint8 bit = shoreLocations & (0x01<<i);
                    // draw the image, rotated 90 degrees per corner
                    if (bit) tEditor.renderTextureToTexture(BGTexture, shoreline0Tex, &cellRect, 90.0*i);
                }
            } else {
                // draw shoreline1 in the appropriate location
                // check if the top AND left corners have a shore
                if ((shoreLocations&0x09)==val) tEditor.renderTextureToTexture(BGTexture, shoreline1Tex, &cellRect);
                else {
                    // check all other side combinations, in order
                    for (int i = 0; i < 3; i++) {
                        Uint8 ref = 0x03<<i, bits = shoreLocations&ref;
                        if (bits == ref) tEditor.renderTextureToTexture(BGTexture, shoreline1Tex, &cellRect, 90.0*(i+1));
                    }
                }
            }
            break;
        }
        case 3:
            // find the side that DOESN'T have a shore
            for (int i = 0; i < 4; i++) {
                Uint8 bit = shoreLocations & (0x01<<i);
                if (!bit) tEditor.renderTextureToTexture(BGTexture, shoreline2Tex, &cellRect, 90.0*i, SDL_FLIP_VERTICAL);
            }
            break;

        case 4:
            // all four sides occupied, just draw shoreline 3 to the cell
            tEditor.renderTextureToTexture(BGTexture, shoreline3Tex, &cellRect);
            break;
    
        default: break; // no shores, nothing more needs to be done
    }
}



// deals a specified amount of damage to a cell
void Game::damageCell( Vector2Int cell, int damage, Scene *level, bool playSound )
{
    if (level == NULL) level = currLevel;

    if (damage == 0) return;
    int num = level->grid[cell.x][cell.y];

    // nothing in the cell to damage, or it cannot be damaged
    if ((num&255) == 0 || (num&CAN_DIE) == 0) return;

    // find the current amount of health in the cell
    // bit shift it to represent as a regular value
    int health = (num&HEALTH)>>8,
        maxHealth = (num&MAX_HEALTH)>>17;

    // subtract the amount of damage dealth
    int newHealth = clamp(0, maxHealth, health-damage);
    // if the health is now non-positive, destroy the cell
    bool emptyFlag = false;
    if (newHealth <= 0) emptyFlag = true;
    else if (playSound) {
        if (damage > 0) logDestruction->play();
        else if (health < maxHealth) repairSound->play();
    }
    health = newHealth;
    // bit shift health to be in the right position
    health <<= 8;
    // reset the health bits of the cell
    num &= ~HEALTH;
    // set the health bits to the new value
    num |= health;
    level->grid[cell.x][cell.y] = num;

    if (emptyFlag) PlaceObjectInCell(cell, EMPTY, true);
}


void Game::initialise_BGTexture() 
{
    int sideLen = currLevel->cell_sideLen;
    int nx = currLevel->gridDimensions.x, ny = currLevel->gridDimensions.y;
    map = { 0, 0, nx*sideLen, ny*sideLen };

    if (BGTexture != nullptr) BGTexture->free();
    if (overlayTexture != nullptr) overlayTexture->free();
    
    BGTexture = tEditor.createEmptyTexture(map.w, map.h, window);
    overlayTexture = tEditor.createEmptyTexture(map.w, map.h, window);

    for (int y = 0; y < ny; y++) {
        for (int x = 0; x < nx; x++) {
            drawCell(Vector2Int(x, y));
        }
    }
}

void Game::damRiver()
{
    // check the top row of the river to see if there are any barriers
    // river starts at row 12 (index 11)

    int n = currLevel->gridDimensions.x - 1, x;

    for (x = 1; x < n; x++) {
        Vector2Int cell(x, 11);

        // if a valid dam has been found, clear up the river behind it
        if (blocksRiver(cell)) 
        {
            // river spans from y cell 11 to 18 (inclusive)
            for (int y = 11; y < 19; y++) {
                Vector2Int currCell(x, y);
                removeWaterFollowingCell(currCell);
            }
            riverDammed = true;
            break;
        }
    }
    
    bool flag = false;
    for (int y = 11; y < 19; y++) {
        for (int i = 1; i < x; i++) 
        {
            int num = currLevel->grid[i][y];
            if (num&IS_DRIED) {
                Vector2Int curr(i, y);
                currLevel->grid[i][y] &= ~IS_DRIED;
                flag = true;
                PlaceObjectInCell(curr, EMPTY, false);
            }
        }
    }
    if (flag) {
        splashSounds[0]->play();
        riverDammed = false;
    }
}

bool Game::blocksRiver( Vector2Int cell )
{
    // other end of the river reached, return true
    if (!(currLevel->grid[cell.x][cell.y]&WATER)) return true;
    // cell is water and unblocked, return false
    if (!is_occupied(cell)) return false;

    Vector2Int currCell(cell.x, cell.y+1);
    return blocksRiver( currCell );
}


void Game::removeWaterFollowingCell( Vector2Int cell )
{
    int n = currLevel->gridDimensions.x-1;
    for (int x = cell.x+1; x < n; x++) 
    {
        int num = currLevel->grid[x][cell.y];
        Vector2Int currCell(x, cell.y);

        num |= IS_DRIED; // indicate the cell is now dried
        // turn off the barrier bit if the cell is not occupied
        if (!is_occupied(currCell)) num &= ~BARRIER;
        // update the map
        currLevel->grid[x][cell.y] = num;
        // render the new cell
        drawCell(currCell);

        // chance to spawn a stone item in each riverbed cell
        // this may only be done once per day
        if (!riverDammed) {
            float random = (float)rand() / RAND_MAX;
            if (random <= STONE_SPAWN_CHANCE) 
            {
                int sideLen = currLevel->cell_sideLen;
                Vector2 pos(currCell.x*sideLen, currCell.y*sideLen);
                Instantiate(stoneItem, pos, 1);
            }
        }
    }
}