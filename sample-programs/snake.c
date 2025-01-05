// snake.c
#include "snake.h"
#include "../drivers/display.h"
#include "../kernel/util.h"
#include "../cpu/timer.h"
#include "../cpu/random.h"

Game init_game() {
    Game game = {0};  // 初始化所有字段为0
    init_snake(&game);
    init_score(&game);
    game.state = GAME_RUNNING;
    generate_food(&game);
    return game;
}

void init_snake(Game* game) {
    game->snake_length = INIT_LENGTH;
    game->current_direction = RIGHT;
    
    int start_x = GAME_WIDTH / 4;
    int start_y = GAME_HEIGHT / 2;
    
    for (int i = 0; i < game->snake_length; i++) {
        game->snake[i].x = start_x - i;
        game->snake[i].y = start_y;
    }
}

void init_score(Game* game) {
    game->score = 0;
}

void flush(const Game* game) {
    clear_screen();
    flush_field_border();
    flush_snake_body(game);
    flush_food(game);
    flush_score(game->score);
    
    if (game->state == GAME_OVER) {
        flush_game_over(game->score);
    }
}

void flush_field_border() {
    for (int x = 0; x < GAME_WIDTH + 2; x++) {
        set_char_at_video_memory('#', get_offset(x, 0));
        set_char_at_video_memory('#', get_offset(x, GAME_HEIGHT + 1));
    }
    for (int y = 0; y < GAME_HEIGHT + 2; y++) {
        set_char_at_video_memory('#', get_offset(0, y));
        set_char_at_video_memory('#', get_offset(GAME_WIDTH + 1, y));
    }
}

void flush_snake_body(const Game* game) {
    for (int i = 0; i < game->snake_length; i++) {
        char symbol = (i == 0) ? '@' : 'O';
        set_char_at_video_memory(symbol, 
            get_offset(game->snake[i].x + 1, game->snake[i].y + 1));
    }
}

void flush_food(const Game* game) {
    set_char_at_video_memory('*', 
        get_offset(game->food.x + 1, game->food.y + 1));
}

void flush_score(int score) {
    char score_str[32];
    int_to_string(score, score_str);
    int offset = get_offset(GAME_WIDTH + 4, 2);
    set_cursor(offset);
    print_string("Score: ");
    print_string(score_str);
}

void flush_game_over(int final_score) {
    print_string("GAME OVER!");
    int offset = get_offset(GAME_WIDTH/2 - 7, GAME_HEIGHT/2 + 1);
    set_cursor(offset);
    print_string("Final Score: ");
    char score_str[32];
    int_to_string(final_score, score_str);
    print_string(score_str);
}

void increase_score(Game* game) {
    game->score += 10;
}

void reset_score(Game* game) {
    game->score = 0;
}

bool generate_food(Game* game) {
    int attempts = 0;
    bool valid_position;
    
    do {
        valid_position = true;
        game->food.x = get_random(GAME_WIDTH);
        game->food.y = get_random(GAME_HEIGHT);
        
        for (int i = 0; i < game->snake_length; i++) {
            if (game->food.x == game->snake[i].x && 
                game->food.y == game->snake[i].y) {
                valid_position = false;
                break;
            }
        }
        attempts++;
    } while (!valid_position && attempts < 100);
    
    return valid_position;
}

void update_snake_head(Game* game, Direction dir) {
    Position new_head = game->snake[0];
    
    switch (dir) {
        case UP:    new_head.y--; break;
        case DOWN:  new_head.y++; break;
        case LEFT:  new_head.x--; break;
        case RIGHT: new_head.x++; break;
    }
    
    for (int i = game->snake_length - 1; i > 0; i--) {
        game->snake[i] = game->snake[i-1];
    }
    game->snake[0] = new_head;
}

bool check_hit_wall(const Game* game) {
    return (game->snake[0].x < 0 || game->snake[0].x >= GAME_WIDTH ||
            game->snake[0].y < 0 || game->snake[0].y >= GAME_HEIGHT);
}

bool check_suicide(const Game* game) {
    for (int i = 1; i < game->snake_length; i++) {
        if (game->snake[0].x == game->snake[i].x && 
            game->snake[0].y == game->snake[i].y) {
            return true;
        }
    }
    return false;
}

bool check_eat_food(const Game* game) {
    return (game->snake[0].x == game->food.x && 
            game->snake[0].y == game->food.y);
}

void move_snake(Game* game, Direction dir) {
    if (game->state != GAME_RUNNING) return;
    
    update_snake_head(game, dir);
    
    if (check_hit_wall(game) || check_suicide(game)) {
        game->state = GAME_OVER;
        return;
    }
    
    if (check_eat_food(game)) {
        game->snake_length++;
        increase_score(game);
        if (!generate_food(game)) {
            game->state = GAME_OVER;
        }
    }
}

void pause_game(Game* game) {
    if (game->state == GAME_RUNNING)
        game->state = GAME_PAUSED;
}

void resume_game(Game* game) {
    if (game->state == GAME_PAUSED)
        game->state = GAME_RUNNING;
}

void exit_game(Game* game) {
    game->state = GAME_OVER;
}

void game_loop() {
    Game game = init_game();
    
    while (game.state != GAME_OVER) {
        flush(&game);
        
        if (game.state == GAME_RUNNING) {
            Direction input = RIGHT;   //get_input(); //TODO: 获取键盘输入
            move_snake(&game, input);
            delay(200);
        }
    }
    
    flush(&game);
}
