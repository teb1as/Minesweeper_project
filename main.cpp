#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

class Tile {
public:
    bool hidden;
    bool flagged;
    sf::Vector2i position;
    unsigned int surrounding_mines;

    Tile(sf::Vector2i pos) : hidden(true), flagged(false), position(pos), surrounding_mines(0) {}

    virtual ~Tile() {}

    bool isHidden() const {
        return hidden;
    }
    void setHidden(bool value) {
        hidden = value;
    }
    bool isFlagged() const {
        return flagged;
    }
    void setFlagged(bool value) {
        flagged = value;
    }
    sf::Vector2i getPosition() const {
        return position;
    }
};

class Mine : public Tile {
public:
    Mine(sf::Vector2i pos) : Tile(pos) {}
};

class Board {
public:
    int rows;
    int cols;
    int mines;
    vector<std::vector<Tile*>> grid;

    // constructor
    Board(int numRows, int numCols, int numMines) : rows(numRows), cols(numCols), mines(numMines) {
        for (int i = 0; i < numRows; ++i) {
            vector<Tile*> row;
            for (int j = 0; j < numCols; ++j) {
                row.push_back(new Tile(sf::Vector2i(j, i)));
            }
            grid.push_back(row);
        }
        addMines(mines);
    }

    ~Board() {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                delete grid[i][j];
            }
        }
    }

    Tile* getTileAt(int x, int y) const {
        return grid[y][x];
    }

    void setTileAt(int x, int y, Tile* tile) {
        // ensure x and y are within bounds
        if (x >= 0 && x < cols && y >= 0 && y < rows) {
            // set the tile at the specified position
            grid[x][y] = tile;
        }
    }

    void revealAllTiles() {
        int rows = this->rows;
        int cols = this->cols;

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                Tile* tile = this->grid[i][j];
                tile->setHidden(false);
            }
        }
    }

    void hideAllTiles() {
        int rows = this->rows;
        int cols = this->cols;

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                Tile* tile = this->grid[i][j];
                if (!tile->flagged) {
                    tile->setHidden(true);
                }
            }
        }
    }

    void addMines(int numMines) {
        int addedMines = 0;
        while (addedMines < numMines) {
            int x = rand() % cols; // random column index
            int y = rand() % rows; // random row index

            Tile* tile = grid[y][x];
            // check if the tile is not already a mine
            if (dynamic_cast<Mine*>(tile) == nullptr) {
                // convert the tile to a Mine and mark it as a mine
                delete tile;
                grid[y][x] = new Mine(sf::Vector2i(x, y));
                addedMines++;
            }
        }
    }

    void assignSurroundingMines(Board &board) {
        int rows = board.rows;
        int cols = board.cols;

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                Tile* currentTile = board.grid[i][j];
                if (dynamic_cast<Mine*>(currentTile) == nullptr) {
                    // iterate through adjacent tiles
                    for (int dx = -1; dx <= 1; ++dx) {
                        for (int dy = -1; dy <= 1; ++dy) {
                            // calculate neighboring tile coordinates
                            int nx = j + dx;
                            int ny = i + dy;
                            // check if neighboring tile is within the board
                            if (nx >= 0 && nx < cols && ny >= 0 && ny < rows) {
                                // get the neighboring tile
                                Tile* neighborTile = board.grid[ny][nx];
                                // if the neighboring tile is a mine, increment the surrounding_mines count
                                if (dynamic_cast<Mine*>(neighborTile) != nullptr) {
                                    currentTile->surrounding_mines++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
};

void drawNumbers(sf::RenderWindow &window, Board &board, sf::Sprite num1, sf::Sprite num2, sf::Sprite num3, sf::Sprite num4, sf::Sprite num5, sf::Sprite num6, sf::Sprite num7, sf::Sprite num8, unsigned int num_cols, unsigned int num_rows) {
    for (int i = 0; i < num_cols; i++) {
        for (int j = 0; j < num_rows; j++) {
            // get the tile at position (i, j) from the board
            Tile* tile = board.getTileAt(i, j);

            // set the position of the sprite based on the tile's position
            sf::Vector2f position(float(32 * i), float(32 * j));
            num1.setPosition(position);
            num2.setPosition(position);
            num3.setPosition(position);
            num4.setPosition(position);
            num5.setPosition(position);
            num6.setPosition(position);
            num7.setPosition(position);
            num8.setPosition(position);

            if (!tile->hidden) {
                switch (tile->surrounding_mines) {
                    case 1:
                        window.draw(num1);
                        break;
                    case 2:
                        window.draw(num2);
                        break;
                    case 3:
                        window.draw(num3);
                        break;
                    case 4:
                        window.draw(num4);
                        break;
                    case 5:
                        window.draw(num5);
                        break;
                    case 6:
                        window.draw(num6);
                        break;
                    case 7:
                        window.draw(num7);
                        break;
                    case 8:
                        window.draw(num8);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

void drawTiles(sf::RenderWindow &window, Board &board, sf::Sprite &hiddenSprite, sf::Sprite &revealedSprite, unsigned int num_cols, unsigned int num_rows) {
    for (int i = 0; i < num_cols; i++) {
        for (int j = 0; j < num_rows; j++) {
            // get the tile at position (i, j) from the board
            Tile* tile = board.getTileAt(i, j);

            // set the position of the sprite based on the tile's position
            sf::Vector2f position(float(32 * i), float(32 * j));
            hiddenSprite.setPosition(position);
            revealedSprite.setPosition(position);

            // draw the hidden sprite if the tile is hidden, otherwise draw the revealed sprite
            if (tile->isHidden()) {
                window.draw(hiddenSprite);
            } else {
                window.draw(revealedSprite);
            }
        }
    }
}

void replaceGrid(Board &board, int colCount, int rowCount) {
    // create a new grid to replace the existing one
    vector<vector<Tile*>> newGrid;

    // populate the new grid with regular tiles
    for (int i = 0; i < rowCount; ++i) {
        vector<Tile*> newRow;
        for (int j = 0; j < colCount; ++j) {
            newRow.push_back(new Tile(sf::Vector2i(j, i))); // creating a regular tile
        }
        newGrid.push_back(newRow);
    }

    // replace the old grid with the new one
    board.grid = newGrid;

    // add mines again
    board.addMines(board.mines);
}

void drawCounter(sf::RenderWindow &window, int number, sf::Sprite digitsSprite, unsigned int num_rows) {
    // calculate the position to start drawing the digits
    int startX = 33;
    int startY = 32 * (num_rows + 0.5) + 16;

    // convert the number to a string
    std::string numberString = std::to_string(number);

    // adjust startX for negative numbers if needed
    if (number < 0) {
        startX -= 21; // adjust for minus sign
        numberString = numberString.substr(1);
    }

    // draw each digit
    for (char digit : numberString) {
        int digitValue = digit - '0'; // convert character to integer value
        digitsSprite.setTextureRect(sf::IntRect(digitValue * 21, 0, 21, 32));
        digitsSprite.setPosition(startX, startY);
        window.draw(digitsSprite);
        startX += 21; // move to the next digit position
    }

    // draw the minus sign if the number is negative
    if (number < 0) {
        digitsSprite.setTextureRect(sf::IntRect(10 * 21, 0, 21, 32));
        digitsSprite.setPosition(startX, startY);
        window.draw(digitsSprite);
    }
}

void drawTimer(sf::RenderWindow& window, int minutes, int seconds, int rowCount, int colCount, sf::Texture& digitsTexture, sf::Sprite& digitsSprite) {
    // minute and second positions
    sf::Vector2f minutesPos((colCount * 32) - 97.f, (rowCount + 0.5f) * 32.f + 16.f);
    sf::Vector2f secondsPos((colCount * 32) - 54.f, (rowCount + 0.5f) * 32.f + 16.f);

    // convert minutes and seconds to strings
    std::string minutesStr = std::to_string(minutes);
    std::string secondsStr = std::to_string(seconds);

    // pad with leading zeros if necessary
    if (minutesStr.length() < 2) minutesStr = "0" + minutesStr;
    if (secondsStr.length() < 2) secondsStr = "0" + secondsStr;

    // set texture for sprite
    digitsSprite.setTexture(digitsTexture);

    // draw minutes
    // first digit
    digitsSprite.setTextureRect(sf::IntRect(21 * (minutesStr[0] - '0'), 0, 21, 32));
    digitsSprite.setPosition(minutesPos);
    window.draw(digitsSprite);
    // second digit
    digitsSprite.setTextureRect(sf::IntRect(21 * (minutesStr[1] - '0'), 0, 21, 32));
    digitsSprite.setPosition((colCount * 32) - 76.f, (rowCount + 0.5f) * 32.f + 16.f);
    window.draw(digitsSprite);

    // draw seconds
    // first digit
    digitsSprite.setTextureRect(sf::IntRect(21 * (secondsStr[0] - '0'), 0, 21, 32));
    digitsSprite.setPosition(secondsPos);
    window.draw(digitsSprite);
    // second digit
    digitsSprite.setTextureRect(sf::IntRect(21 * (secondsStr[1] - '0'), 0, 21, 32));
    digitsSprite.setPosition((colCount * 32) - 32.f, (rowCount + 0.5f) * 32.f + 16.f);
    window.draw(digitsSprite);
}

void drawFlags(sf::RenderWindow &window, Board &board, sf::Sprite &flagSprite, unsigned int num_cols, unsigned int num_rows) {
    for (int i = 0; i < num_cols; i++) {
        for (int j = 0; j < num_rows; j++) {
            Tile* tile = board.getTileAt(i, j);

            flagSprite.setPosition(float(32 * i), float(32 * j));

            if (tile->isFlagged() && tile->isHidden()) {
                window.draw(flagSprite);
            }
        }
    }
}

void drawMines(sf::RenderWindow &window, Board &board, sf::Sprite &mineSprite, unsigned int num_cols, unsigned int num_rows) {
    for (int i = 0; i < num_cols; i++) {
        for (int j = 0; j < num_rows; j++) {
            Tile* tile = board.getTileAt(i, j);

            mineSprite.setPosition(float(32 * i), float(32 * j));

            if (dynamic_cast<Mine*>(tile) != nullptr && !tile->isHidden()) {
                window.draw(mineSprite);
            }
        }
    }
}

bool checkGameWon(Board &board, unsigned int num_cols, unsigned int num_rows)  {
    for (unsigned int y = 0; y < num_rows; ++y) {
        for (unsigned int x = 0; x < num_cols; ++x) {
            Tile* tile = board.getTileAt(x, y);

                if (tile->hidden) {

                    return false;
                }
            }
        }

    return true;
}

void revealTiles(Board &board, int x, int y) {
    int numRows = board.rows;
    int numCols = board.cols;

    if (x >= 0 && x < numCols && y >= 0 && y < numRows) {

        Tile* clickedTile = board.getTileAt(x, y);

        if (clickedTile->isHidden() && !clickedTile->isFlagged()) {
            //reveal clicked tile
            clickedTile->setHidden(false);

            //recursively reveal the tiles neighbors
            if (clickedTile->surrounding_mines == 0) {
                // iterate through the adjacent tiles
                for (int i = -1; i <= 1; ++i) {
                    for (int j = -1; j <= 1; ++j) {
                        // skip the current tile
                        if (i == 0 && j == 0) continue;

                        int newX = x + i;
                        int newY = y + j;

                        revealTiles(board, newX, newY);
                    }
                }
            }
        }
    }
}

void createLeaderboardWindow(int num_cols, int num_rows, const vector<string>& names, const vector<string>& times) {
    sf::RenderWindow leaderboard_window(sf::VideoMode(16 * num_cols, (num_rows * 16) + 50), "Leaderboard Window", sf::Style::Close);

    sf::Font font;
    if (!font.loadFromFile("font.ttf")) {
        cout << "error" << endl;
    }

    // text objs
    sf::Text leaderboard_text("LEADERBOARD", font, 20);
    leaderboard_text.setStyle(sf::Text::Bold | sf::Text::Underlined);
    leaderboard_text.setFillColor(sf::Color::White);
    leaderboard_text.setPosition((16 * num_cols) / 2.0f, 15.0f);
    leaderboard_text.setOrigin(leaderboard_text.getGlobalBounds().width / 2.0f, 0.0f);

    sf::Text score_text("", font, 18);
    score_text.setStyle(sf::Text::Bold);
    score_text.setFillColor(sf::Color::White);
    score_text.setPosition(16.0f, 50.0f);

    // entries vectorpair
    vector<pair<string, string>> leaderboardEntries;

    // fill the vector
    for (size_t i = 0; i < names.size() && i < times.size(); ++i) {
        leaderboardEntries.push_back({times[i], names[i]});
    }

    // sort the entries
    sort(leaderboardEntries.begin(), leaderboardEntries.end());

    // set leaderboard text
    string text;
    for (size_t i = 0; i < 5; ++i) {
        text += to_string(i + 1) + ".\t" + leaderboardEntries[i].first + ", " + leaderboardEntries[i].second + "\n\n";
    }
    score_text.setString(text);

    while (leaderboard_window.isOpen()) {
        sf::Event event;
        while (leaderboard_window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    leaderboard_window.close();
                break;
            }
        }
        leaderboard_window.clear(sf::Color::Blue);
        leaderboard_window.draw(leaderboard_text);
        leaderboard_window.draw(score_text);
        leaderboard_window.display();
    }
}

void revealAllMines(Board &board, int colCount, int rowCount) {
    for (int i = 0; i < colCount; ++i) {
        for (int j = 0; j < rowCount; ++j) {
            Tile* tile = board.getTileAt(i, j);
            if (Mine* mine = dynamic_cast<Mine*>(tile)) {
                mine->setHidden(false);
            }
        }
    }
}

void hideAllMines(Board &board, int colCount, int rowCount) {
    for (int i = 0; i < colCount; ++i) {
        for (int j = 0; j < rowCount; ++j) {
            Tile* tile = board.getTileAt(i, j);
            if (dynamic_cast<Mine*>(tile) != nullptr) {
                tile->setHidden(true);
            }
        }
    }
}

void createGameWindow(int colCount, int rowCount, string name) {
    sf::RenderWindow game_window(sf::VideoMode((colCount * 32), (rowCount * 32 + 100)), "Game Window", sf::Style::Close);

    //read config for mines
    string line;
    ifstream config("config.cfg");
    if (!config.is_open()) {
        cerr << "Error opening file!" << endl;
    }
    getline(config, line);
    getline(config, line);
    getline(config, line);
    int mineCount = stoi(line);

    //leaderboard
    ifstream leaderboard_file("leaderboard.txt");
    if (!leaderboard_file.is_open()) {
        cerr << "Error opening file!" << endl;
    }
    vector<string> times;
    vector<string> names;

    string temp;
    while (getline(leaderboard_file, temp, ',')) {
        times.push_back(temp);
        getline(leaderboard_file, temp);
        names.push_back(temp);
    }

    //game state & board
    Board board(rowCount, colCount, mineCount);
    board.assignSurroundingMines(board);
    bool gameOver = false;
    bool debugMode = false;
    bool isPaused = false;
    bool gameWon = false;
    bool scoreRecorded = false;
    int minesRemaining = board.mines;
    sf::Clock clock;
    sf::Time totalTime;

    int seconds = 0;
    int minutes = 0;

    //sprites and textures
    sf::Texture hidden_tile;
    if(!hidden_tile.loadFromFile("images/tile_hidden.png"))
        cout << "error" << endl;
    sf::Sprite hidden_tile_sprite;
    hidden_tile_sprite.setTexture(hidden_tile);

    sf::Texture revealed_texture;
    if(!revealed_texture.loadFromFile("images/tile_revealed.png"))
        cout << "error" << endl;
    sf::Sprite revealed_tile_sprite;
    revealed_tile_sprite.setTexture(revealed_texture);

    sf::Texture pause_texture;
    if(!pause_texture.loadFromFile("images/pause.png"))
        cout << "error" << endl;
    sf::Sprite pause_sprite;
    pause_sprite.setTexture(pause_texture);
    pause_sprite.setPosition((colCount * 32) - 240, 32 * (rowCount + .5));

    sf::Texture digits_texture;
    if(!digits_texture.loadFromFile("images/digits.png"))
        cout << "error" << endl;
    sf::Sprite digits_sprite;
    digits_sprite.setTexture(digits_texture);

    sf::Texture face_happy_texture;
    if (!face_happy_texture.loadFromFile("images/face_happy.png")) {
        std::cout << "Error" << std::endl;
    }
    sf::Sprite face_happy_sprite;
    face_happy_sprite.setTexture(face_happy_texture);
    face_happy_sprite.setPosition((colCount/2)*32 - 32, 32 * (rowCount + 0.5));

    sf::Texture face_lose_texture;
    if (!face_lose_texture.loadFromFile("images/face_lose.png")) {
        std::cout << "Error" << std::endl;
    }
    sf::Sprite face_lose_sprite;
    face_lose_sprite.setTexture(face_lose_texture);
    face_lose_sprite.setPosition((colCount/2)*32 - 32, 32 * (rowCount + 0.5));

    sf::Texture face_win_texture;
    if (!face_win_texture.loadFromFile("images/face_win.png")) {
        std::cout << "Error" << std::endl;
    }
    sf::Sprite face_win_sprite;
    face_win_sprite.setTexture(face_win_texture);
    face_win_sprite.setPosition((colCount/2)*32 - 32, 32 * (rowCount + 0.5));

    sf::Texture debug_texture;
    if (!debug_texture.loadFromFile("images/debug.png")) {
        std::cout << "Error" << std::endl;
    }
    sf::Texture leaderboard_texture;
    if (!leaderboard_texture.loadFromFile("images/leaderboard.png")) {
        std::cout << "Error" << std::endl;
    }

    sf::Sprite debug_sprite;
    debug_sprite.setTexture(debug_texture);
    debug_sprite.setPosition((colCount * 32) - 304, 32 * (rowCount + 0.5));
    sf::Sprite leaderboard_sprite;
    leaderboard_sprite.setTexture(leaderboard_texture);
    leaderboard_sprite.setPosition((colCount * 32) - 176, 32 * (rowCount + 0.5));

    sf::Texture flag_texture;
    if (!flag_texture.loadFromFile("images/flag.png")) {
        std::cout << "Error" << std::endl;
    }
    sf::Sprite flag_sprite;
    flag_sprite.setTexture(flag_texture);

    sf::Texture number1_texture;
    if (!number1_texture.loadFromFile("images/number_1.png")) {
        std::cout << "Error" << std::endl;
    }
    sf::Sprite number1_sprite;
    number1_sprite.setTexture(number1_texture);

    sf::Texture number2_texture;
    if (!number2_texture.loadFromFile("images/number_2.png")) {
        std::cout << "Error" << std::endl;
    }
    sf::Sprite number2_sprite;
    number2_sprite.setTexture(number2_texture);

    sf::Texture number3_texture;
    if (!number3_texture.loadFromFile("images/number_3.png")) {
        std::cout << "Error" << std::endl;
    }
    sf::Sprite number3_sprite;
    number3_sprite.setTexture(number3_texture);

    sf::Texture number4_texture;
    if (!number4_texture.loadFromFile("images/number_4.png")) {
        std::cout << "Error" << std::endl;
    }
    sf::Sprite number4_sprite;
    number4_sprite.setTexture(number4_texture);

    sf::Texture number5_texture;
    if (!number5_texture.loadFromFile("images/number_5.png")) {
        std::cout << "Error" << std::endl;
    }
    sf::Sprite number5_sprite;
    number5_sprite.setTexture(number5_texture);

    sf::Texture number6_texture;
    if (!number6_texture.loadFromFile("images/number_6.png")) {
        std::cout << "Error" << std::endl;
    }
    sf::Sprite number6_sprite;
    number6_sprite.setTexture(number6_texture);

    sf::Texture number7_texture;
    if (!number7_texture.loadFromFile("images/number_7.png")) {
        std::cout << "Error" << std::endl;
    }
    sf::Sprite number7_sprite;
    number7_sprite.setTexture(number7_texture);

    sf::Texture number8_texture;
    if (!number8_texture.loadFromFile("images/number_8.png")) {
        std::cout << "Error" << std::endl;
    }
    sf::Sprite number8_sprite;
    number8_sprite.setTexture(number8_texture);

    sf::Texture mine_texture;
    if (!mine_texture.loadFromFile("images/mine.png")) {
        std::cout << "Error" << std::endl;
    }
    sf::Sprite mine_sprite;
    mine_sprite.setTexture(mine_texture);

    //game loop
    while (game_window.isOpen()) {
        sf::Event event;
        while (game_window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    game_window.close();
                break;
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        if (!isPaused) {
                            // mouse pos
                            sf::Vector2i mousePos = sf::Mouse::getPosition(game_window);


                            if (mousePos.x >= 0 && mousePos.x < colCount * 32 &&
                                mousePos.y >= 0 && mousePos.y < rowCount * 32) {
                                //grid position from mousepos
                                int gridX = mousePos.x / 32;
                                int gridY = mousePos.y / 32;

                                Tile* tile = board.getTileAt(gridX, gridY);

                                //check if hidden
                                if (tile->isHidden()) {
                                    if (Mine* mine = dynamic_cast<Mine*>(tile)) {
                                        gameOver = true;
                                        revealAllMines(board, colCount, rowCount);
                                    }
                                    revealTiles(board, gridX, gridY);
                                    gameWon = checkGameWon(board, colCount, rowCount);
                                }
                                }
                        }
                    }
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(game_window);

                    // check mousepos
                    sf::FloatRect debugBounds = debug_sprite.getGlobalBounds();
                    if (debugBounds.contains(sf::Vector2f(mousePos))) {
                        debugMode = !debugMode;
                        if(debugMode) {
                            revealAllMines(board, colCount, rowCount);
                        }
                        else {
                            hideAllMines(board, colCount, rowCount);
                        }

                    }
                    sf::FloatRect faceBounds = face_happy_sprite.getGlobalBounds();
                    if (faceBounds.contains(sf::Vector2f(mousePos))) {

                        replaceGrid(board, colCount, rowCount);
                        gameOver = false;
                        gameWon = false;
                        board.addMines(board.mines);
                        board.assignSurroundingMines(board);
                        minesRemaining = board.mines;
                        clock.restart();
                        isPaused = false;
                        totalTime = sf::Time::Zero;
                    }
                    sf::FloatRect pauseBounds = pause_sprite.getGlobalBounds();
                    if (pauseBounds.contains(sf::Vector2f(mousePos))) {
                        isPaused = !isPaused;
                        clock.restart();
                    }
                    sf::FloatRect leaderboardBounds = leaderboard_sprite.getGlobalBounds();
                    if (leaderboardBounds.contains(sf::Vector2f(mousePos))) {
                        createLeaderboardWindow(colCount, rowCount, names, times);
                    }
                }

                if (event.mouseButton.button == sf::Mouse::Right) {

                    sf::Vector2i mousePos = sf::Mouse::getPosition(game_window);

                    if (mousePos.x >= 0 && mousePos.x < colCount * 32 && mousePos.y >= 0 && mousePos.y < rowCount * 32) {

                        int gridX = mousePos.x / 32;
                        int gridY = mousePos.y / 32;

                        Tile* tile = board.getTileAt(gridX, gridY);

                        if (tile->isHidden()) {
                            // remove flag
                            if (tile->isFlagged()) {
                                tile->setFlagged(false);
                                minesRemaining++;
                            }
                            else {
                                // set flag
                                tile->setFlagged(true);
                                minesRemaining--;
                            }
                        }
                        }
                }
            }
            break;
        }

        // calculate time
        sf::Time elapsedTime = clock.getElapsedTime();
        if (!isPaused)
            totalTime += elapsedTime;

        clock.restart();

        int totalSeconds = totalTime.asSeconds();
        minutes = totalSeconds / 60;
        seconds = totalSeconds % 60;

        game_window.clear(sf::Color::White);
        drawTiles(game_window, board, hidden_tile_sprite, revealed_tile_sprite, colCount, rowCount);
        drawMines(game_window, board, mine_sprite, colCount, rowCount);
        if(!isPaused) {
            drawNumbers(game_window, board, number1_sprite, number2_sprite, number3_sprite, number4_sprite, number5_sprite, number6_sprite, number7_sprite, number8_sprite, colCount, rowCount);
            drawFlags(game_window, board, flag_sprite, colCount, rowCount);
        }


        drawCounter(game_window, minesRemaining, digits_sprite, rowCount);
        drawTimer(game_window, minutes, seconds, rowCount, colCount, digits_texture, digits_sprite);
        game_window.draw(pause_sprite);
        if(gameOver) {
            board.revealAllTiles();
            game_window.draw(face_lose_sprite);
            isPaused = true;
        }
        else if (gameWon){
            board.revealAllTiles();
            game_window.draw(face_win_sprite);
            isPaused = true;

        }
        else {
            game_window.draw(face_happy_sprite);
        }

        game_window.draw(debug_sprite);
        game_window.draw(leaderboard_sprite);
        game_window.display();

        if (gameWon && !scoreRecorded) {
            //write to file
            ofstream leaderboardFile("leaderboard.txt", ios::app); // Open the file in append mode
            if (leaderboardFile.is_open()) {
                if (minutes == 0) {
                    leaderboardFile << "*00:" << (seconds < 10 ? "0" : "") << seconds << ", " << name << endl;
                } else {
                    leaderboardFile << "*" << minutes << ":" << (seconds < 10 ? "0" : "") << seconds << ", " << name << endl;
                }
                leaderboardFile.close();
                scoreRecorded = true;
            }
            else {
                cerr << "error" << endl;
            }
        }
    }
}

void createWelcomeWindow(int colCount, int rowCount){
    sf::RenderWindow welcome_window(sf::VideoMode((colCount * 32), (rowCount * 32 + 100)), "Welcome Window", sf::Style::Close);

    sf::Font font;
    if (!font.loadFromFile("font.TTF")) {
        cout << "error" << endl;

    }

    // text objects
    sf::Text welcome_text;
    welcome_text.setFont(font);
    welcome_text.setString("Welcome to Minesweeper");
    welcome_text.setCharacterSize(24);
    welcome_text.setFillColor(sf::Color::White);
    welcome_text.setPosition((colCount * 32 - welcome_text.getLocalBounds().width) / 2.f, 50.f);

    sf::Text instruction_text;
    instruction_text.setFont(font);
    instruction_text.setString("Enter your name:");
    instruction_text.setCharacterSize(20);
    instruction_text.setFillColor(sf::Color::White);
    instruction_text.setPosition((colCount * 32 - instruction_text.getLocalBounds().width) / 2.f, 150.f);

    sf::Text name_text;
    sf::String name_string;
    name_text.setFont(font);
    name_text.setString("|");
    name_text.setCharacterSize(24);
    name_text.setFillColor(sf::Color::White);
    name_text.setPosition((colCount * 32 - name_text.getLocalBounds().width) / 2.f, 200.f);

    while (welcome_window.isOpen()) {
        sf::Event event;
        while (welcome_window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    welcome_window.close();
                break;
                case sf::Event::TextEntered:
                    if (event.text.unicode < 128 && event.text.unicode != 8 && name_string.getSize() < 10) {
                        if (name_string.isEmpty() || name_string.getSize() == 0 || name_string[name_string.getSize() - 1] == ' ') {
                            name_string += static_cast<char>(toupper(static_cast<unsigned char>(event.text.unicode)));
                        } else {
                            name_string += static_cast<char>(tolower(static_cast<unsigned char>(event.text.unicode)));
                        }
                    }
                    else if (event.text.unicode == 8 && !name_string.isEmpty()) { // backspace key
                        // remove the last character from the string
                        name_string.erase(name_string.getSize() - 1);
                    }
                // update the text object with the modified string
                name_text.setString(name_string + "|");

                name_text.setPosition((colCount * 32 - name_text.getLocalBounds().width) / 2.f, 200.f);
                break;
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Enter && name_string != "") {
                        welcome_window.close();
                        string name = name_string;
                        createGameWindow(colCount, rowCount, name);
                    }
                break;
            }
        }

        welcome_window.clear(sf::Color::Blue);
        welcome_window.draw(welcome_text);
        welcome_window.draw(instruction_text);
        welcome_window.draw(name_text);
        welcome_window.display();
    }
}

int main() {
    // Read from the config for rows, cols, and mines #
    string line;
    ifstream config("config.cfg");

    if (!config.is_open()) {
        cerr << "config error" << endl;

    }

    getline(config, line);
    int colCount = stoi(line);
    getline(config, line);
    int rowCount = stoi(line);

    createWelcomeWindow(colCount, rowCount);

    return 0;
}
