#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

enum class Command {
    Up = 'w',
    Right = 'd',
    Down = 's',
    Left = 'a'
};

enum class PointType {
    Player = 'p',
    PlayerOnGoal = 'P',
    Wall = '#',
    Space = ' ',
    Box = 'o',
    BoxOnGoal = 'O',
    Goal = '.'
};

struct Point
{
    bool operator == (const Point b) const {
        return this->x == b.x && this->y == b.y;
    };
    int x;
    int y;
};

class Status
{
public:
    Status(char* mapStr, int len);
    void draw() const;
    void updateStatus(Command command);
    bool move(Point& point, int offsetX, int offsetY);
    bool isWin() const;
private:
    Point player;
    vector<Point> boxes;
    vector<Point> goals;
    vector<string> map;
    int mapWidth;
    int mapHeight;
};

vector<string> split(string str, char spe)
{
    vector<string> strArray;
    while (true)
    {
        const int index = str.find(spe);
        if (index == string::npos)
        {
            break;
        }
        const string s = str.substr(0, index);
        strArray.push_back(s);
        str.erase(0, index + 1);
    }
    if (str.length() > 0)
    {
        strArray.push_back(str);
    }
    return strArray;
}

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

int main()
{

    ifstream infile;
    infile.open("stages/stage-1.txt", ifstream::binary);
    infile.seekg(0, ifstream::end);
    long len = infile.tellg();
    infile.seekg(0, ifstream::beg);
    char* mapStr = new char[len];
    infile.read(mapStr, len);
    infile.close();

    Status status = Status(mapStr, len);

    while (true)
    {
        status.draw();
        Command command = Command(input());
        status.updateStatus(command);
        if (status.isWin())
        {
            cout << "You win!";
            break;
        }
    }
}

Status::Status(char* mapStr, int len)
{
    int maxWidth = 0;
    int width = 0;
    int height = 0;
    int i = 0;
    boxes;
    goals;
    player;
    while (i < len)
    {
        const char currentPoint = mapStr[i];

        if (currentPoint == (char)PointType::Box || currentPoint == (char)PointType::BoxOnGoal)
        {
            boxes.push_back(Point({ height, width }));
        }

        if (currentPoint == (char)PointType::BoxOnGoal || currentPoint == (char)PointType::PlayerOnGoal || currentPoint == (char)PointType::Goal)
        {
            goals.push_back(Point({ height, width }));
        }

        if (currentPoint == (char)PointType::Player || currentPoint == (char)PointType::PlayerOnGoal)
        {
            player = Point({ height, width });
        }

        switch (currentPoint)
        {
        case (char)PointType::Box:
        case (char)PointType::BoxOnGoal:
        case (char)PointType::Player:
        case (char)PointType::PlayerOnGoal:
        case (char)PointType::Goal:
            mapStr[i] = (char)PointType::Space;
        case (char)PointType::Wall:
        case (char)PointType::Space:
            width++;
            break;
            // todo: crlf
        case '\n':
            maxWidth = max(maxWidth, width);
            width = 0;
            height++;
            break;
        default:
            mapStr[i] = (char)PointType::Space;
            width++;
            break;
        }
        i++;
    }
    if (width > 0)
    {
        maxWidth = max(maxWidth, width);
        height++;
    }

    map = split((string)mapStr, '\n');
    mapWidth = maxWidth;
    mapHeight = height;
}


void Status::draw() const
{
    for (int i = 0; i < mapHeight; i++)
    {
        const string line = map[i];
        const int lineLen = line.length();
        for (int j = 0; j < mapWidth; j++)
        {
            const Point point = Point({ i, j });
            const vector<Point>::const_iterator goalsIt = find(goals.begin(), goals.end(), point);
            const bool isOnGoal = goalsIt != goals.end();

            if (point == player) {
                cout << char(
                    isOnGoal
                    ? PointType::PlayerOnGoal
                    : PointType::Player
                    );
                continue;
            }

            const vector<Point>::const_iterator boxesIt = find(boxes.begin(), boxes.end(), point);
            const bool isBox = boxesIt != boxes.end();
            if (isBox)
            {
                cout << char(
                    isOnGoal
                    ? PointType::BoxOnGoal
                    : PointType::Box
                    );
                continue;
            }

            if (isOnGoal)
            {
                cout << char(PointType::Goal);
                continue;
            }

            char out = j + 1 > lineLen ? ' ' : line[j];
            cout << out;
        }
        cout << '\n';
    }
}


void Status::updateStatus(Command command)
{
    switch (command)
    {
    case Command::Up:
        move(player, -1, 0);
        break;
    case Command::Right:
        move(player, 0, 1);
        break;
    case Command::Down:
        move(player, 1, 0);
        break;
    case Command::Left:
        move(player, 0, -1);
        break;
    default:
        break;
    }
}

bool Status::move(Point& point, int offsetX, int offsetY)
{
    Point target = Point({ point.x + offsetX, point.y + offsetY });
    PointType targetType = PointType(map[target.x][target.y]);
    if (targetType == PointType::Wall)
    {
        return false;
    }

    const vector<Point>::iterator boxesIt = find(boxes.begin(), boxes.end(), target);
    if (boxesIt != boxes.end()) {
        bool success = move(*boxesIt, offsetX, offsetY);
        if (!success) {
            return false;
        }
    }
    point.x = target.x;
    point.y = target.y;
    return true;
}

bool Status::isWin() const
{
    for (int i = 0, len = goals.size(); i < len; i++)
    {
        const vector<Point>::const_iterator boxesIt = find(boxes.begin(), boxes.end(), goals[i]);
        if (boxesIt == boxes.end()) {
            return false;
        }
    }
    return true;
}

