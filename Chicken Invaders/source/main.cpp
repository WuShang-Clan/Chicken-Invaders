#include <SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cmath>
using namespace sf;
using namespace std;

const int game_width = 800;
const int game_height = 600;

// Player
const float player_speed = 6.0f;

// Chickens
const int chicken_in_row = 8;
const int total_rows = 3;
const float chicken_width = 50.0f;
const float chicken_height = 50.0f;
const float chicken_horizontal_separation = 25.0f;
const float chicken_vertical_separation = 10.0f;
const float chicken_ceiling = 70.0f;

// Eggs
const float egg_width = 20.0f;
const float egg_height = 20.0f;
const int egg_maximum = 25;
int egg_speed = 5.0f;

// Boss related
bool isBossLevel = false;
bool bossActive = false;
float boss_x, boss_y;
int boss_health = 15;
const float boss_width = 150.0f;
const float boss_height = 150.0f;
float boss_angle = 0.0f;
const float boss_radius = 150.0f;
const float boss_speed = 0.02f;
int boss_egg_timer = 0;
const int BOSS_EGG_DELAY = 240;
Sprite boss_sprite;
Texture boss_texture;

// Bullets
const int bullet_maximum = 50;
const float bullet_speed = 10.0f;
int FIRE_RATE = 10;
int EGG_RATE = 60;

// Respawn
bool isRespawning = false;
int respawnTimer = 0;
const int RESPAWN_DELAY = 60;
int blink_counter = 0;

// Game State
int curr_s = 0, high_s = 0, lives = 3;
int game_s = 0;
int fireDelay = 0;
int eggDelay = 0;
int score = 0;
int rewardDelay = 0;
const int REWARD_RATE = 300;

float chicken_speed = 1.5f;
const float chicken_dist_screen = 10.0f;
int movement = 1;

// Data for rewards
const int reward_width = 20.0f;
const int reward_height = 20.0f;
const int reward_maximum = 5;
const int reward_speed = 4.0f;

float reward_xcoordinate[reward_maximum];
float reward_ycoordinate[reward_maximum];
bool reward_active[reward_maximum];
Sprite reward_sprite[reward_maximum];

float player_xcoordinate;
float player_ycoordinate;

Texture reward_texture;
Texture egg_texture, chicken_texture, player_texture, bullet_texture, bg_texture;
Sprite player_sprite, bg_sprite;
Texture logo_texture;
Sprite logo_sprite;
Sprite chicken_sprite[total_rows][chicken_in_row];
bool chicken_active[total_rows][chicken_in_row];
float chicken_xcoordinate[total_rows][chicken_in_row];
float chicken_ycoordinate[total_rows][chicken_in_row];

int difficult = 2;
int current_level = 1;

void difficulty() {
    if (difficult == 1) {
        chicken_speed = 1.5f;
        egg_speed = 5.0f;
        EGG_RATE = 60;
    }
    else if (difficult == 2) {
        chicken_speed = 3.5f;
        egg_speed = 8.0f;
        EGG_RATE = 40;
    }
    else if (difficult == 3) {
        chicken_speed = 5.5f;
        egg_speed = 11.0f;
        EGG_RATE = 20;
    }
}

Sprite egg_sprite[egg_maximum];
bool egg_active[egg_maximum];
float egg_xcoordinate[egg_maximum];
float egg_ycoordinate[egg_maximum];

Sprite bullet_sprite[bullet_maximum];
bool bullet_active[bullet_maximum];

SoundBuffer shootBuffer;
Sound shootSound;
SoundBuffer eggHitBuffer;
Sound eggHitSound;
SoundBuffer chickenHitBuffer;
Sound chickenHitSound;
Music bgMusic;

void centerText(Text& txt, float x, float y) {
    FloatRect bounds = txt.getLocalBounds();
    txt.setOrigin(bounds.width / 2, bounds.height / 2);
    txt.setPosition(x, y);
}

void deactivate_all_chickens() {
    for (int i = 0; i < total_rows; i++) {
        for (int j = 0; j < chicken_in_row; j++) {
            chicken_active[i][j] = false;
        }
    }
}

void save_difficulty() {
    ofstream file("difficulty.txt");
    if (file.is_open()) {
        file << difficult << endl;
        file.close();
    }
}

void load_difficulty() {
    ifstream file("difficulty.txt");
    if (file.is_open()) {
        file >> difficult;
        file.close();
        difficulty();
    }
}

void init_boss() {
    bossActive = true;
    boss_health = 15;
    boss_x = game_width / 2;
    boss_y = 200;
    boss_angle = 0.0f;
    boss_egg_timer = 0;

    boss_sprite.setTexture(boss_texture);
    boss_sprite.setScale(boss_width / boss_texture.getSize().x,
        boss_height / boss_texture.getSize().y);
    boss_sprite.setOrigin(boss_texture.getSize().x / 2, boss_texture.getSize().y / 2);
}

void update_boss() {
    if (!bossActive) return;

    boss_angle += boss_speed;
    float center_x = game_width / 2;
    float center_y = 200;

    boss_x = center_x + cos(boss_angle) * boss_radius;
    boss_y = center_y + sin(boss_angle) * boss_radius;

    boss_sprite.setPosition(boss_x, boss_y);

    boss_egg_timer++;
    if (boss_egg_timer >= BOSS_EGG_DELAY) {
        boss_egg_timer = 0;

        int eggs_to_drop = 10 + (rand() % 3);
        for (int i = 0; i < eggs_to_drop; i++) {
            for (int e = 0; e < egg_maximum; e++) {
                if (!egg_active[e]) {
                    float offset = (i - eggs_to_drop / 2.0f) * 30.0f;
                    egg_xcoordinate[e] = boss_x + offset;
                    egg_ycoordinate[e] = boss_y + boss_height / 2;
                    egg_active[e] = true;
                    egg_sprite[e].setPosition(egg_xcoordinate[e], egg_ycoordinate[e]);
                    break;
                }
            }
        }
    }
}

void boss_bullet_collision() {
    if (!bossActive) return;

    for (int i = 0; i < bullet_maximum; i++) {
        if (!bullet_active[i]) continue;

        float bullet_x = bullet_sprite[i].getPosition().x;
        float bullet_y = bullet_sprite[i].getPosition().y;
        float dx = bullet_x - boss_x;
        float dy = bullet_y - boss_y;
        float dist = sqrt(dx * dx + dy * dy);

        if (dist < 75) {
            bullet_active[i] = false;
            boss_health--;
            chickenHitSound.play();
            curr_s += 50;

            if (boss_health <= 0) {
                bossActive = false;
                curr_s += 500;
                current_level++;
                isBossLevel = false;
            }
        }
    }
}

void draw_boss(RenderWindow& window) {
    if (!bossActive) return;

    window.draw(boss_sprite);

    RectangleShape healthBarBg(Vector2f(200, 20));
    healthBarBg.setPosition(game_width / 2 - 100, 50);
    healthBarBg.setFillColor(Color::Red);

    RectangleShape healthBar(Vector2f((boss_health / 15.0f) * 200, 20));
    healthBar.setPosition(game_width / 2 - 100, 50);
    healthBar.setFillColor(Color::Green);

    window.draw(healthBarBg);
    window.draw(healthBar);
}

void create_chickens() {
    if (current_level >= 4 && current_level % 4 == 0) {
        isBossLevel = true;
        deactivate_all_chickens();
        init_boss();
        return;
    }

    isBossLevel = false;

    int chickens_to_spawn;
    int rows_to_spawn;

    if (current_level == 1) {
        chickens_to_spawn = 6;
        rows_to_spawn = 1;
    }
    else if (current_level == 2) {
        chickens_to_spawn = 7;
        rows_to_spawn = 2;
    }
    else {
        chickens_to_spawn = 8;
        rows_to_spawn = 3;
    }

    float total_width = (chickens_to_spawn * chicken_width) + (chickens_to_spawn - 1) * chicken_horizontal_separation;
    float start_x = ((game_width - total_width) / 2) + (chicken_width / 2);

    deactivate_all_chickens();

    for (int i = 0; i < rows_to_spawn; i++) {
        float y_coordinate = chicken_ceiling + i * (chicken_height + chicken_vertical_separation);
        for (int j = 0; j < chickens_to_spawn; j++) {
            float x_coordinate = start_x + j * (chicken_width + chicken_horizontal_separation);
            chicken_active[i][j] = true;
            chicken_xcoordinate[i][j] = x_coordinate;
            chicken_ycoordinate[i][j] = y_coordinate;

            chicken_sprite[i][j].setTexture(chicken_texture);
            chicken_sprite[i][j].setScale(chicken_width / chicken_texture.getSize().x,
                chicken_height / chicken_texture.getSize().y);
            chicken_sprite[i][j].setOrigin(chicken_texture.getSize().x / 2, chicken_texture.getSize().y / 2);
            chicken_sprite[i][j].setPosition(x_coordinate, y_coordinate);
        }
    }
}

bool are_all_chickens_dead() {
    for (int i = 0; i < total_rows; i++)
        for (int j = 0; j < chicken_in_row; j++)
            if (chicken_active[i][j]) return false;
    return true;
}

void new_wave() {
    if (isBossLevel) {
        if (!bossActive) {
            create_chickens();
        }
    }
    else {
        if (are_all_chickens_dead()) {
            current_level++;
            create_chickens();
        }
    }
}

void create_eggs() {
    for (int i = 0; i < egg_maximum; i++) {
        egg_sprite[i].setTexture(egg_texture);
        egg_sprite[i].setScale(egg_width / egg_texture.getSize().x, egg_height / egg_texture.getSize().y);
        egg_sprite[i].setOrigin(egg_texture.getSize().x / 2, egg_texture.getSize().y / 2);
        egg_active[i] = false;
    }
}

void dropping_eggs() {
    int active_count = 0;
    int active_chickens[total_rows * chicken_in_row][2];

    for (int i = 0; i < total_rows; i++)
        for (int j = 0; j < chicken_in_row; j++)
            if (chicken_active[i][j]) {
                active_chickens[active_count][0] = i;
                active_chickens[active_count][1] = j;
                active_count++;
            }

    if (active_count > 0) {
        int max_eggs = min(active_count, 3);
        int eggs_dropped = 0;

        while (eggs_dropped < max_eggs) {
            int r = rand() % active_count;
            int row = active_chickens[r][0];
            int col = active_chickens[r][1];

            for (int e = 0; e < egg_maximum; e++) {
                if (!egg_active[e]) {
                    egg_xcoordinate[e] = chicken_xcoordinate[row][col];
                    egg_ycoordinate[e] = chicken_ycoordinate[row][col] + chicken_height / 2;
                    egg_active[e] = true;
                    egg_sprite[e].setPosition(egg_xcoordinate[e], egg_ycoordinate[e]);
                    eggs_dropped++;
                    break;
                }
            }
        }
    }
}

void egg_move() {
    for (int i = 0; i < egg_maximum; i++) {
        if (egg_active[i]) {
            egg_ycoordinate[i] += egg_speed;
            if (egg_ycoordinate[i] > game_height) egg_active[i] = false;
            egg_sprite[i].setPosition(egg_xcoordinate[i], egg_ycoordinate[i]);
        }
    }
}

void egg_player_collision() {
    if (isRespawning) return;

    for (int i = 0; i < egg_maximum; i++) {
        if (egg_active[i]) {
            float dx = egg_xcoordinate[i] - player_xcoordinate;
            float dy = egg_ycoordinate[i] - player_ycoordinate;
            float dist = sqrt(dx * dx + dy * dy);
            if (dist < 35) {
                egg_active[i] = false;
                lives--;
                eggHitSound.play();
                isRespawning = true;
                respawnTimer = RESPAWN_DELAY;
                blink_counter = 0;
                if (lives <= 0)
                    game_s = 3;
                return;
            }
        }
    }
}

void init_bullets() {
    for (int i = 0; i < bullet_maximum; i++) {
        bullet_sprite[i].setTexture(bullet_texture);
        bullet_sprite[i].setScale(10.0f / bullet_texture.getSize().x, 35.0f / bullet_texture.getSize().y);
        bullet_sprite[i].setOrigin(bullet_texture.getSize().x / 2, bullet_texture.getSize().y / 2);
        bullet_active[i] = false;
    }
}

void create_bullet() {
    for (int i = 0; i < bullet_maximum; i++) {
        if (!bullet_active[i]) {
            bullet_sprite[i].setPosition(player_sprite.getPosition().x, player_sprite.getPosition().y - 25);
            bullet_active[i] = true;
            fireDelay = FIRE_RATE;
            shootSound.play();
            break;
        }
    }
}

void update_bullet() {
    for (int i = 0; i < bullet_maximum; i++) {
        if (bullet_active[i]) {
            float bullet_x = bullet_sprite[i].getPosition().x;
            float bullet_y = bullet_sprite[i].getPosition().y;
            bullet_y = bullet_y - bullet_speed;
            bullet_sprite[i].setPosition(bullet_x, bullet_y);
            if (bullet_y < 0) bullet_active[i] = false;
        }
    }
}

void bullet_chicken_collision() {
    for (int i = 0; i < bullet_maximum; i++) {
        if (!bullet_active[i]) continue;

        float bullet_x = bullet_sprite[i].getPosition().x;
        float bullet_y = bullet_sprite[i].getPosition().y;
        for (int r = 0; r < total_rows; r++) {
            for (int c = 0; c < chicken_in_row; c++) {
                if (!chicken_active[r][c]) continue;
                float dx = bullet_x - chicken_xcoordinate[r][c];
                float dy = bullet_y - chicken_ycoordinate[r][c];
                float dist = sqrt(dx * dx + dy * dy);
                if (dist < 32) {
                    bullet_active[i] = false;
                    chicken_active[r][c] = false;
                    chickenHitSound.play();
                    curr_s += 10;
                    break;
                }
            }
            if (!bullet_active[i]) break;
        }
    }
}

void moving_chicken_horizontally() {
    for (int i = 0; i < total_rows; i++) {
        for (int j = 0; j < chicken_in_row; j++) {
            if (chicken_active[i][j] == true) {
                chicken_xcoordinate[i][j] = chicken_speed * movement + chicken_xcoordinate[i][j];
                int chicken_distance_right = chicken_xcoordinate[i][j] + chicken_width / 2 + chicken_dist_screen;
                if (chicken_distance_right >= game_width) {
                    movement = movement * -1;
                }
                int chicken_distance_left = chicken_xcoordinate[i][j] - chicken_width / 2 - chicken_dist_screen;
                if (chicken_distance_left <= 0) {
                    movement = movement * -1;
                }
                chicken_sprite[i][j].setPosition(chicken_xcoordinate[i][j], chicken_ycoordinate[i][j]);
            }
        }
    }
}

void draw_bullets(RenderWindow& window) {
    for (int i = 0; i < bullet_maximum; i++)
        if (bullet_active[i])
            window.draw(bullet_sprite[i]);
}

void player_move() {
    if (game_s == 1) {
        if (Keyboard::isKeyPressed(Keyboard::Left)) player_sprite.move(-player_speed, 0);
        if (Keyboard::isKeyPressed(Keyboard::Right)) player_sprite.move(player_speed, 0);
        if (Keyboard::isKeyPressed(Keyboard::Up)) player_sprite.move(0, -player_speed);
        if (Keyboard::isKeyPressed(Keyboard::Down)) player_sprite.move(0, player_speed);

        if (Keyboard::isKeyPressed(Keyboard::A)) player_sprite.move(-player_speed, 0);
        if (Keyboard::isKeyPressed(Keyboard::D)) player_sprite.move(player_speed, 0);
        if (Keyboard::isKeyPressed(Keyboard::W)) player_sprite.move(0, -player_speed);
        if (Keyboard::isKeyPressed(Keyboard::S)) player_sprite.move(0, player_speed);

        if (player_sprite.getPosition().x < 25) player_sprite.setPosition(25, player_sprite.getPosition().y);
        if (player_sprite.getPosition().x > game_width - 25) player_sprite.setPosition(game_width - 25, player_sprite.getPosition().y);
        if (player_sprite.getPosition().y < 25) player_sprite.setPosition(player_sprite.getPosition().x, 25);
        if (player_sprite.getPosition().y > game_height - 25) player_sprite.setPosition(player_sprite.getPosition().x, game_height - 25);
    }
}

void create_reward() {
    for (int r = 0; r < reward_maximum; r++) {
        reward_sprite[r].setTexture(reward_texture);
        reward_sprite[r].setScale(0.2f, 0.2f);
        reward_sprite[r].setOrigin(reward_width / 2, reward_height / 2);
        reward_active[r] = false;
    }
}

void dropping_rewards() {
    const int total_chickens = total_rows * chicken_in_row;
    int active_chickens[total_chickens][2];
    int r = 0;
    for (int i = 0; i < total_rows; i++) {
        for (int j = 0; j < chicken_in_row; j++) {
            if (chicken_active[i][j] == true) {
                active_chickens[r][0] = i;
                active_chickens[r][1] = j;
                r++;
            }
        }
    }

    if (r != 0) {
        int maximum_rewards_dropped;
        int rewards_dropped = 0;

        if (r >= 3) {
            maximum_rewards_dropped = 2;
        }
        else if (r == 2) {
            maximum_rewards_dropped = 1;
        }
        else {
            maximum_rewards_dropped = 1;
        }

        while (maximum_rewards_dropped > rewards_dropped) {
            int random = rand() % r;
            int random_chicken_row = active_chickens[random][0];
            int random_chicken_column = active_chickens[random][1];

            for (int reward = 0; reward < reward_maximum; reward++) {
                if (reward_active[reward] == false) {
                    reward_xcoordinate[reward] = chicken_xcoordinate[random_chicken_row][random_chicken_column];
                    reward_ycoordinate[reward] = (chicken_ycoordinate[random_chicken_row][random_chicken_column]) + chicken_height / 2;
                    reward_active[reward] = true;
                    reward_sprite[reward].setPosition(reward_xcoordinate[reward], reward_ycoordinate[reward]);
                    rewards_dropped++;
                    break;
                }
            }
        }
    }
}

void reward_move() {
    for (int reward = 0; reward < reward_maximum; reward++) {
        if (reward_active[reward] != false) {
            reward_ycoordinate[reward] = reward_speed + reward_ycoordinate[reward];
            if (reward_ycoordinate[reward] > game_height) {
                reward_active[reward] = false;
            }
            reward_sprite[reward].setPosition(reward_xcoordinate[reward], reward_ycoordinate[reward]);
        }
    }
}

void check_rewardCollisions() {
    for (int r = 0; r < reward_maximum; r++) {
        if (reward_active[r] == true) {
            float dx = reward_xcoordinate[r] - player_xcoordinate;
            float dy = reward_ycoordinate[r] - player_ycoordinate;
            float dist = sqrt(dx * dx + dy * dy);
            if (dist < 50) {
                reward_active[r] = false;
                curr_s += 100;
                return;
            }
        }
    }
}

void draw_rewards(RenderWindow& window) {
    for (int r = 0; r < reward_maximum; r++) {
        if (reward_active[r]) {
            window.draw(reward_sprite[r]);
        }
    }
}

void respawn_update() {
    if (!isRespawning) return;

    respawnTimer--;
    blink_counter++;

    if (blink_counter % 10 == 0) {
        Color currentColor = player_sprite.getColor();
        if (currentColor.a == 255) {
            player_sprite.setColor(Color(255, 255, 255, 0));
        }
        else {
            player_sprite.setColor(Color(255, 255, 255, 255));
        }
    }

    if (respawnTimer <= 0) {
        player_sprite.setColor(Color(255, 255, 255, 255));
        isRespawning = false;
        blink_counter = 0;
    }
}

void save_game() {
    ofstream file("save.txt");
    if (file.is_open()) {
        file << curr_s << " " << lives << " " << player_sprite.getPosition().x << " " << player_sprite.getPosition().y << " " << current_level << " " << high_s << " " << difficult << endl;
        for (int i = 0; i < egg_maximum; i++)
            file << egg_xcoordinate[i] << " " << egg_ycoordinate[i] << " " << egg_active[i] << endl;
        for (int r = 0; r < total_rows; r++)
            for (int c = 0; c < chicken_in_row; c++)
                file << chicken_active[r][c] << " " << chicken_xcoordinate[r][c] << " " << chicken_ycoordinate[r][c] << endl;
        file.close();
        cout << "Game Saved!" << endl;
    }
}

void load_game() {
    ifstream file("save.txt");
    if (file.is_open()) {
        file >> curr_s >> lives;
        float px, py;
        file >> px >> py >> current_level >> high_s >> difficult;
        player_sprite.setPosition(px, py);

        for (int i = 0; i < egg_maximum; i++)
            file >> egg_xcoordinate[i] >> egg_ycoordinate[i] >> egg_active[i];
        for (int r = 0; r < total_rows; r++)
            for (int c = 0; c < chicken_in_row; c++)
                file >> chicken_active[r][c] >> chicken_xcoordinate[r][c] >> chicken_ycoordinate[r][c];
        for (int r = 0; r < total_rows; r++)
            for (int c = 0; c < chicken_in_row; c++)
                if (chicken_active[r][c]) {
                    chicken_sprite[r][c].setTexture(chicken_texture);
                    chicken_sprite[r][c].setScale(chicken_width / chicken_texture.getSize().x,
                        chicken_height / chicken_texture.getSize().y);
                    chicken_sprite[r][c].setOrigin(chicken_texture.getSize().x / 2,
                        chicken_texture.getSize().y / 2);
                    chicken_sprite[r][c].setPosition(chicken_xcoordinate[r][c],
                        chicken_ycoordinate[r][c]);
                }
        difficulty();
        file.close();
        cout << "Game Loaded!" << endl;
    }
    else {
        cout << "No save file found" << endl;
    }
}

int main() {
    srand(time(0));
    RenderWindow window(VideoMode(game_width, game_height), "CHICKEN INVADERS!");
    window.setFramerateLimit(60);

    load_difficulty();
    current_level = 1;

    if (!egg_texture.loadFromFile("assets/images/eggs.png") ||
        !chicken_texture.loadFromFile("assets/images/chickens.png") ||
        !player_texture.loadFromFile("assets/images/spaceship.png") ||
        !bullet_texture.loadFromFile("assets/images/bullet.png") ||
        !bg_texture.loadFromFile("assets/images/background.png") ||
        !reward_texture.loadFromFile("assets/images/reward.png") ||
        !logo_texture.loadFromFile("assets/images/logo3.png") ||
        !boss_texture.loadFromFile("assets/images/boss.png")) {
        cout << "Missing textures!" << endl;
        return 0;
    }

    if (!shootBuffer.loadFromFile("assets/sounds/bullet.wav") ||
        !eggHitBuffer.loadFromFile("assets/sounds/egg_hit.mp3") ||
        !chickenHitBuffer.loadFromFile("assets/sounds/chicken_hit.wav") ||
        !bgMusic.openFromFile("assets/sounds/bg_music.mp3")) {
        cout << "Missing audio files!" << endl;
        return 0;
    }

    shootSound.setBuffer(shootBuffer);
    eggHitSound.setBuffer(eggHitBuffer);
    chickenHitSound.setBuffer(chickenHitBuffer);
    bgMusic.setLoop(true);
    bgMusic.play();

    player_sprite.setTexture(player_texture);
    player_sprite.setScale(50.0f / player_texture.getSize().x, 50.0f / player_texture.getSize().y);
    player_sprite.setOrigin(player_texture.getSize().x / 2, player_texture.getSize().y / 2);
    player_sprite.setPosition(game_width / 2, game_height - 50);

    bg_sprite.setTexture(bg_texture);
    bg_sprite.setScale(game_width / float(bg_texture.getSize().x), game_height / float(bg_texture.getSize().y));

    Font font;
    if (!font.loadFromFile("assets/fonts/game_over.ttf")) {
        cout << "Missing font!" << endl;
        return 0;
    }

    create_chickens();
    create_eggs();
    init_bullets();
    create_reward();

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                save_difficulty();
                window.close();
            }

            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::E && (game_s == 0 || game_s == 3)) {
                    save_difficulty();
                    window.close();
                }
                if (game_s == 0) {
                    if (event.key.code == Keyboard::Space) game_s = 1;
                    if (event.key.code == Keyboard::L) { load_game(); game_s = 1; }
                }

                if (event.key.code == Keyboard::P) {
                    if (game_s == 1) game_s = 2;
                    else if (game_s == 2) game_s = 1;
                }

                if (event.key.code == Keyboard::S && game_s == 2)
                    save_game();

                if (event.key.code == Keyboard::R && (game_s == 2 || game_s == 3)) {
                    curr_s = 0; lives = 3; current_level = 1;
                    create_chickens(); create_eggs(); init_bullets();
                    player_sprite.setPosition(game_width / 2, game_height - 50);
                    game_s = 1;
                    fireDelay = 0;
                    eggDelay = 0;
                    rewardDelay = 0;
                    isRespawning = false;
                }

                int return_s;
                if (event.key.code == Keyboard::I && game_s == 0) {
                    game_s = 4;
                    return_s = 0;
                }

                if (event.key.code == Keyboard::I && game_s == 2) {
                    return_s = 2;
                    game_s = 4;
                }

                if (event.key.code == Keyboard::B && game_s == 4) {
                    game_s = return_s;
                }
                if (event.key.code == Keyboard::D && game_s == 0) {
                    return_s = 0;
                    game_s = 5;
                }
                if (event.key.code == Keyboard::D && game_s == 2) {
                    return_s = 2;
                    game_s = 5;
                }
                if (game_s == 5) {
                    if (event.key.code == Keyboard::Num1) {
                        difficult = 1;
                        difficulty();
                    }
                    if (event.key.code == Keyboard::Num2) {
                        difficult = 2;
                        difficulty();
                    }
                    if (event.key.code == Keyboard::Num3) {
                        difficult = 3;
                        difficulty();
                    }
                    if (event.key.code == Keyboard::B) {
                        game_s = return_s;
                    }
                }
                if (event.key.code == Keyboard::M && game_s == 2) {
                    game_s = 0;
                    curr_s = 0; lives = 3; current_level = 1;
                    create_chickens(); create_eggs(); init_bullets();
                    player_sprite.setPosition(game_width / 2, game_height - 50);
                    fireDelay = 0;
                    eggDelay = 0;
                    rewardDelay = 0;
                    isRespawning = false;
                }
                if (event.key.code == Keyboard::M && game_s == 3) {
                    game_s = 0;
                    curr_s = 0; lives = 3; current_level = 1;
                    create_chickens(); create_eggs(); init_bullets();
                    player_sprite.setPosition(game_width / 2, game_height - 50);
                    fireDelay = 0;
                    eggDelay = 0;
                    rewardDelay = 0;
                    isRespawning = false;
                }
            }
        }

        if (game_s == 1) {
            player_move();
            player_xcoordinate = player_sprite.getPosition().x;
            player_ycoordinate = player_sprite.getPosition().y;

            respawn_update();

            if (!isRespawning && Keyboard::isKeyPressed(Keyboard::Space) && fireDelay == 0) create_bullet();
            if (fireDelay > 0) fireDelay--;

            update_bullet();

            if (isBossLevel) {
                update_boss();
                boss_bullet_collision();
            }
            else {
                bullet_chicken_collision();
                moving_chicken_horizontally();
            }

            if (eggDelay == 0) {
                if (!isBossLevel) {
                    dropping_eggs();
                }
                eggDelay = EGG_RATE;
            }
            if (eggDelay > 0) eggDelay--;

            egg_move();
            egg_player_collision();
            new_wave();

            if (rewardDelay == 0) {
                if (!isBossLevel) {
                    dropping_rewards();
                }
                rewardDelay = REWARD_RATE;
            }
            if (rewardDelay > 0)
                rewardDelay--;

            reward_move();
            check_rewardCollisions();

            if (curr_s > high_s)
                high_s = curr_s;
        }

        window.clear();
        window.draw(bg_sprite);

        if (game_s == 0) {
            Text t("CHICKEN INVADERS", font, 150);
            t.setFillColor(Color::Red);
            centerText(t, game_width / 2, game_height / 8);
            if (logo_texture.getSize().x > 0) {
                logo_sprite.setTexture(logo_texture);
                logo_sprite.setPosition(game_width / 5.0f, game_height / 8.0f);
                logo_sprite.setScale(1.25f, 1.5f);
                window.draw(logo_sprite);
            }
            else {
                window.draw(t);
            }

            Text s("Press Space to Start", font, 80);
            s.setFillColor(Color::White);
            centerText(s, game_width / 2, 300);
            Text i("Press I for Instructions", font, 80);
            i.setFillColor(Color::White);
            centerText(i, game_width / 2, 350);
            Text l("Press L to Load", font, 80);
            l.setFillColor(Color::White);
            centerText(l, game_width / 2, 400);
            Text d("Press D for Difficulty", font, 80);
            d.setFillColor(Color::White);
            centerText(d, game_width / 2, 450);
            Text e("Press E to Exit", font, 80);
            e.setFillColor(Color::White);
            centerText(e, game_width / 2, 500);

            window.draw(s);
            window.draw(i);
            window.draw(l);
            window.draw(d);
            window.draw(e);
        }
        else if (game_s == 2) {
            Text paused("GAME PAUSED", font, 150);
            paused.setFillColor(Color::Blue);
            centerText(paused, game_width / 2, game_height / 8);

            Text save("Press S to Save", font, 100);
            save.setFillColor(Color::White);
            centerText(save, game_width / 2, 250);

            Text resume("Press P to Resume", font, 100);
            resume.setFillColor(Color::White);
            centerText(resume, game_width / 2, 300);

            Text instructions("Press I for Instructions", font, 100);
            instructions.setFillColor(Color::White);
            centerText(instructions, game_width / 2, 350);

            Text diff("Press D for Difficulty", font, 100);
            diff.setFillColor(Color::White);
            centerText(diff, game_width / 2, 400);

            Text mainMenu("Press M for Main Menu", font, 100);
            mainMenu.setFillColor(Color::White);
            centerText(mainMenu, game_width / 2, 450);

            window.draw(instructions);
            window.draw(paused);
            window.draw(save);
            window.draw(resume);
            window.draw(diff);
            window.draw(mainMenu);
        }
        else if (game_s == 3) {
            Text go("GAME OVER", font, 150);
            go.setFillColor(Color::Blue);
            centerText(go, game_width / 2, game_height / 8);
            Text score("Score: " + to_string(curr_s), font, 80);
            score.setFillColor(Color::White);
            centerText(score, game_width / 2, 240);
            Text hs("High Score: " + to_string(high_s), font, 80);
            hs.setFillColor(Color::White);
            centerText(hs, game_width / 2, 300);
            Text restart("Press R to Restart", font, 80);
            restart.setFillColor(Color::White);
            centerText(restart, game_width / 2, 360);
            Text mainMenu("Press M for Main Menu", font, 80);
            mainMenu.setFillColor(Color::White);
            centerText(mainMenu, game_width / 2, 420);
            Text exit("Press E to Exit", font, 80);
            exit.setFillColor(Color::White);
            centerText(exit, game_width / 2, 480);
            window.draw(go);
            window.draw(score);
            window.draw(hs);
            window.draw(restart);
            window.draw(exit);
            window.draw(mainMenu);
        }
        else if (game_s == 1) {
            window.draw(player_sprite);

            if (isBossLevel) {
                draw_boss(window);
            }
            else {
                for (int r = 0; r < total_rows; r++)
                    for (int c = 0; c < chicken_in_row; c++)
                        if (chicken_active[r][c]) window.draw(chicken_sprite[r][c]);
            }

            for (int i = 0; i < egg_maximum; i++)
                if (egg_active[i]) window.draw(egg_sprite[i]);
            draw_bullets(window);

            if (!isBossLevel) {
                draw_rewards(window);
            }

            Text score_text("Score: " + to_string(curr_s), font, 60);
            score_text.setFillColor(Color::White);
            score_text.setPosition(10, 0);
            window.draw(score_text);
            Text high_score_text("High Score: " + to_string(high_s), font, 60);
            high_score_text.setFillColor(Color::White);
            high_score_text.setPosition(game_width - 200, 0);
            window.draw(high_score_text);
            Text lives_text("Lives: " + to_string(lives), font, 60);
            lives_text.setFillColor(Color::White);
            lives_text.setPosition(10, 20);
            window.draw(lives_text);
            Text level_text("Level: " + to_string(current_level), font, 60);
            level_text.setFillColor(Color::White);
            level_text.setPosition(game_width / 2 - 100, 0);
            window.draw(level_text);
        }
        else if (game_s == 4) {
            Text menu("INSTRUCTIONS", font, 130);
            menu.setFillColor(Color::Blue);
            centerText(menu, game_width - 400, game_height - 650);

            Text menuText("- Press Space to Start\n"
                "- Press I for Instructions\n"
                "- Press E to Exit\n"
                "- Press L to Load\n"
                "- Press S to Save\n"
                "- Press P to Pause/Resume\n"
                "- Press R to Restart\n\n"
                "Player starts with 3 lives.\n"
                "Control spaceship with arrow keys or WASD.\n"
                "Select difficulty level.\n"
                "Chickens move left/right dropping eggs and gifts.\n"
                "Eggs reduce lives. Gifts add 100 points.\n"
                "Boss appears every 4th level!\n"
                "0 Lives = Game Over.\n", font, 50);
            menuText.setFillColor(Color::White);
            menuText.setPosition(50, 50);

            Text back("Press B to go back", font, 70);
            back.setFillColor(Color::White);
            centerText(back, game_width - 400, game_height - 100);
            window.draw(menu);
            window.draw(menuText);
            window.draw(back);
        }
        else if (game_s == 5) {
            Text title("SELECT DIFFICULTY", font, 120);
            title.setFillColor(Color::Red);
            centerText(title, game_width / 2, 100);
            Text easy("Press 1 for EASY", font, 80);
            easy.setFillColor(difficult == 1 ? Color::Green : Color::White);
            centerText(easy, game_width / 2, 180);
            Text medium("Press 2 for MEDIUM", font, 80);
            medium.setFillColor(difficult == 2 ? Color::Yellow : Color::White);
            centerText(medium, game_width / 2, 260);
            Text hard("Press 3 for HARD", font, 80);
            hard.setFillColor(difficult == 3 ? Color::Red : Color::White);
            centerText(hard, game_width / 2, 340);
            Text current("Current: ", font, 80);
            String diffName = (difficult == 1) ? "EASY" : (difficult == 2) ? "MEDIUM" : "HARD";
            current.setString("Current: " + diffName);
            current.setFillColor(Color::Cyan);
            centerText(current, game_width / 2, 420);
            Text back("Press B to go back", font, 80);
            back.setFillColor(Color::White);
            centerText(back, game_width / 2, 500);

            window.draw(title);
            window.draw(easy);
            window.draw(medium);
            window.draw(hard);
            window.draw(current);
            window.draw(back);
        }

        window.display();
    }

    return 0;
}