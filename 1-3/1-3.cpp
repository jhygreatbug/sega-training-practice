//todo:
//更多的输入文件容错
//交互优化

#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <regex>
#include <map>
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
    bool isCorrect() const;
private:
    Point player = Point({-1, -1});
    vector<Point> boxes;
    vector<Point> goals;
    char** map;
    int mapWidth;
    int mapHeight;
    int correct;
    bool check() const;
    Point findAWallPoint() const;
};

char** getFormatedMap(char* str, int strLen, int width, int height)
{
    char** map = new char*[height];
    for (int i = 0; i < height; i++) {
        map[i] = new char[width];
        memset(map[i], ' ', sizeof(char) * width);
    }

    int newLineIndex = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (str[newLineIndex] == '\n')
            {
                map[i][j] = ' ';
            }
            else
            {
                map[i][j] = str[newLineIndex];
                newLineIndex++;
            }
        }
        newLineIndex++;
    }
    return map;
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

struct getStageResult { char* mapStr; long len; };
getStageResult getStage(string stage, map<string, string> stages)
{
    ifstream infile;
    infile.open("stages/stage-" + stage + '-' + stages[stage] + ".txt", ifstream::binary);
    infile.seekg(0, ifstream::end);
    long len = infile.tellg();
    infile.seekg(0, ifstream::beg);
    char* mapStr = new char[len];
    infile.read(mapStr, len);
    infile.close();
    return getStageResult({ mapStr, len });
}

void main()
{
    string partten{ "stage-(\\d)-(.*)\\.txt" };
    regex stageFile(partten);
    filesystem::path stagePath("./stages");
    map<string, string> stages;
    for (auto& file : filesystem::directory_iterator(stagePath))
    {
        auto filePath = file.path();
        auto fileName = filePath.filename().string();
        smatch result;
        if (std::regex_match(fileName, result, stageFile)) {
            stages[result[1]] = result[2];
            cout << result[1] << ": " << result[2] << endl;
        }
    }

    cout << "选择一个关卡（输入编号）：" << endl;
    string stage;
    getline(cin, stage);
    auto [mapStr, len] = getStage(stage, stages);
    Status status = Status(mapStr, len);
    if (!status.isCorrect()) {
        cout << "关卡数据错误";
        cin.get();
        return;
    }
    cout << status.isCorrect() << endl;

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

    map = getFormatedMap(mapStr, len, maxWidth, height);
    mapWidth = maxWidth;
    mapHeight = height;
    correct = check();
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
        cout << endl;
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

bool Status::isCorrect() const
{
    return correct;
}

const Point offsets[4] = {
    Point({0, 1}),
    Point({0, -1}),
    Point({1, 0}),
    Point({-1, 0})
};
bool Status::check() const
{
    // 0. 检查是否有player、box、goal
    if (player.x == -1 && player.y == -1) {
        return false;
    }
    if (boxes.size() == 0 || goals.size() == 0) {
        return false;
    }

    // 1. 检查player是否在封闭区域内
    bool* status = new bool[mapWidth * mapHeight];
    memset(status, false, mapWidth * mapHeight);
    status[player.x * mapWidth + player.y] = true;

    vector<Point> queue;
    queue.push_back(player);

    int i = 0;
    while (i < queue.size())
    {
        Point pointer = queue[i];
        for (auto offset : offsets)
        {
            const int x = offset.x + pointer.x;
            const int y = offset.y + pointer.y;
            if (
                x < 0
                || x >= mapHeight
                || y < 0
                || y >= mapWidth
                )
            {
                return false;
            }
            if (map[x][y] == (char)PointType::Space && status[x * mapWidth + y] == false)
            {
                cout << '-' << x << ',' << y << endl;
                Point p = Point({ x, y });
                queue.push_back(p);

                status[x * mapWidth + y] = true;
            }
        }
        i++;
    }

    // 2. 检查各元素是否在player可达范围内
    for (auto box : boxes)
    {
        if (status[box.x * mapWidth + box.y] == false)
        {
            return false;
        }
    }
    for (auto goal : goals)
    {
        if (status[goal.x * mapWidth + goal.y] == false)
        {
            return false;
        }
    }

    // 3. 检查 box 和 goal 数量是否匹配
    if (boxes.size() != goals.size()) {
        return false;
    }

    return true;
}

Point Status::findAWallPoint() const
{
    Point firstPoint;
    for (int i = 0; i < mapHeight; i++)
    {
        for (int j = 0; j < mapWidth; j++)
        {
            if (map[i][j] == (char)PointType::Wall)
            {
                return Point({ i, j });
            }
        }
    }
    return Point({ -1,-1 });
}

