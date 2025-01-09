// snake.h
#ifndef SNAKE_H
#define SNAKE_H

#include <stdbool.h>
#include <stdint.h>

#define GAME_WIDTH 40
#define GAME_HEIGHT 20
#define MAX_LENGTH 100
#define INIT_LENGTH 3

typedef struct {
    int x;
    int y;
} Position;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef enum {
    GAME_RUNNING,
    GAME_PAUSED,
    GAME_OVER
} GameState;

// 游戏状态结构体
typedef struct {
    Position snake[MAX_LENGTH];
    int snake_length;
    Direction current_direction;
    Position food;
    int score;
    GameState state;
} Game;

// 初始化函数
Game init_game();                    // 初始化并返回游戏状态
void init_snake(Game* game);         // 初始化蛇的初始位置和长度
void init_score(Game* game);         // 初始化分数

// 显示函数
void flush(const Game* game);        // 刷新整个屏幕显示
void flush_field_border();           // 输出场地边框
void flush_snake_body(const Game* game);  // 输出贪吃蛇身体
void flush_food(const Game* game);   // 输出食物
void flush_score(int score);         // 输出分数
void flush_game_over(int final_score); // 输出游戏结束信息

// 计算函数
void increase_score(Game* game);     // 分数自增
void reset_score(Game* game);        // 分数清零
bool generate_food(Game* game);      // 生成下一个食物
void update_snake_head(Game* game, Direction dir); // 更新蛇头位置
void update_snake_body(Game* game);  // 蛇身跟随蛇头移动
bool check_hit_wall(const Game* game); // 检查是否撞墙
bool check_suicide(const Game* game); // 检查是否吃到自己
bool check_eat_food(const Game* game); // 检查是否吃到食物

// 交互函数
void move_snake(Game* game, Direction dir); // 上下左右移动
void pause_game(Game* game);         // 暂停游戏
void resume_game(Game* game);        // 继续游戏
void exit_game(Game* game);          // 退出游戏

// 工具函数
Direction get_input();  //获取输入

// 游戏主循环
void game_loop(uint8_t game_difficulty);                    // 游戏主循环

#endif
