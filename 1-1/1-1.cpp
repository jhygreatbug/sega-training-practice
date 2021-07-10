// 1-1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
using namespace std;

enum class Command {
    Up = 'w',
    Right = 'd',
    Down = 's',
    Left = 'a'
};

enum class PointType {
    Player = 'P',
    Wall = '#',
    Space = ' ',
    Box = 'O',
    BoxPlace = '.'
};

struct Point
{
    bool operator == (const Point b) const {
        return this->x == b.x && this->y == b.y;
    };
    int x;
    int y;
};

struct Status
{
    Point player;
    Point boxes[5];
};

char input()
{
    while (true)
    {
        char input_char;
        cin >> input_char;
        switch (Command(input_char))
        {
            case Command::Up:
            case Command::Right:
            case Command::Down:
            case Command::Left:
                return input_char;
            default:
                break;
        }
    }
}

bool move(char map[5][9], Status &status, Point &point, int offsetX, int offsetY) {
    Point target = Point({ point.x + offsetX, point.y + offsetY });
    PointType targetType = PointType(map[target.x][target.y]);
    if (targetType == PointType::Wall)
    {
        return false;
    }
    if (target == status.boxes[0] || target == status.boxes[1]) {
        Point &box = target == status.boxes[0] ? status.boxes[0] : status.boxes[1];
        bool success = move(map, status, box, offsetX, offsetY);
        if (!success) {
            return false;
        }
    }
    point.x = target.x;
    point.y = target.y;
    return true;
}

void updateStatus(char map[5][9], Status &status, Command command)
{
    Point &player = status.player;
    switch (command)
    {
    case Command::Up:
        move(map, status, player, -1, 0);
        break;
    case Command::Right:
        move(map, status, player, 0, 1);
        break;
    case Command::Down:
        move(map, status, player, 1, 0);
        break;
    case Command::Left:
        move(map, status, player, 0, -1);
        break;
    default:
        break;
    }
}

void draw(char map[5][9], Status &status)
{
    for (int i = 0; i < 5; i ++)
    {
        for (int j = 0; j < 9; j++)
        {
            const Point p = Point({ i, j });
            char out;
            if (p == status.player) {
                out = char(PointType::Player);
            }
            else if (p == status.boxes[0] || p == status.boxes[1])
            {
                out = char(PointType::Box);
            }
            else
            {
                out = map[i][j];
            }
            cout << out;
        }
        cout << '\n';
    }
}

int main()
{
    char map[5][9] = {
        "########",
        "# ..   #",
        "#      #",
        "#      #",
        "########"
    };
    Status status = {
        Point({ 1, 5 }),
        Point({ 2, 2 }),
        Point({ 2, 3 })
    };
    Point flags[] = { Point({ 1, 2 }), Point({1, 3}) };

    while (true)
    {
        draw(map, status);
        Command command = Command(input());
        updateStatus(map, status, command);
        if (
            (flags[0] == status.boxes[0] && flags[1] == status.boxes[1])
            || (flags[0] == status.boxes[1] && flags[1] == status.boxes[0])
            )
        {
            cout << "You win!";
            break;
        }
    }
}
