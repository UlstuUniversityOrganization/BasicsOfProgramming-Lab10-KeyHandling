#include "framework.h"
#include "BasicsOfProgramming-Lab10-KeyHandling.h"
#include <stdio.h>


template<class T>
class myvector
{
public:
    myvector();
    void push_back(T element);
    size_t size();
    T operator[](size_t elementId);
    void clear();


private:
    T* mem;
    size_t countElements = 0;
    size_t countElementsMax = 5;
    size_t startCountElements = 5;
    float factor = 2;
};

template<class T>
myvector<T>::myvector()
{
    mem = (T*)malloc(startCountElements * sizeof(T));
}

template<class T>
void myvector<T>::push_back(T element)
{
    countElements++;
    if (countElements > countElementsMax)
    {
        T* t = mem;
        countElementsMax = countElementsMax * factor;
        mem = (T*)malloc(countElementsMax * sizeof(T));
        memcpy(mem, t, countElements * sizeof(T));
    }
    mem[countElements - 1] = element;
    //memcpy((char*)mem + (countElements - 1) * sizeof(T), element, sizeof(T));
}

template<class T>
size_t myvector<T>::size()
{
    return countElements;
}

template<class T>
T myvector<T>::operator[](size_t elementId)
{
    return mem[elementId];
}

template<class T>
void myvector<T>::clear()
{
    delete[] mem;
    countElements = 0;
}


struct float2
{
    float x;
    float y;
};

struct float3
{
    float x;
    float y;
    float z;
};

class Entity;
class BlockEmpty;
class BlockWall;
class SpikesBlock;
class BlockEmpty;
class BlockFinish;
class Player;
class MapManager;
class Map;
class Block
{
public:
    float2 pos;
    float2 origin;
    float2 size;
    int teamID = 0;
    bool canBeat;
    bool isTransparent = true;
    Block(float3 color, float2 origin, float2 pos, float2 size);
    static Block* GetBlock(int id, float2 origin, float2 pos, float2 size);
    virtual void Update() = 0;
    virtual void OnEntityCollision(Entity* entity);
    virtual void Draw(HDC hdc);
    float3 color;
};



//---------------------BlockEmpty----------------------
class BlockEmpty : public Block
{
public:
    BlockEmpty(float2 origin, float2 pos, float2 size);
    virtual void Update() override;
};

class SpikesBlock : public Block
{
public:
    int timer = 0;
    int spikeOnTimer = 10;
    bool spikesOn;
    int spikesCountX = 3;
    int spikesCountY = 3;
    SpikesBlock(float2 origin, float2 pos, float2 size);
    virtual void Update() override;
    virtual void Draw(HDC hdc) override;
};

class BlockWall : public Block
{
public:
    BlockWall(float2 origin, float2 pos, float2 size);
    virtual void Update() override;
};

class Map
{
public:
    Block*** blocks;
    myvector<Entity*> entities;
    Player* player;
    MapManager* parentMapManager;
    float2 origin;
    float2 blockSize;
    int xCount;
    int yCount;

    Map(MapManager* parentMapManager, float2 origin, float2 blockSize);
    ~Map();
    void Load(const char* filePath);
};

class MapManager
{
public:
    Map map = Map(this, float2{ 0, 0 }, float2{ 50, 50 });
    myvector<const char*> mapPaths;
    int activeMap = 0;
    bool isDrawing = true;
    bool isUpdating = true;
    int gameOverScreenTimer = 0;
    int gameOverScreenTime = 0;
    bool isDrawingGameOverScreen = false;
    MapManager(float2 origin, float2 blockSize);
    ~MapManager();
    void SetGameOverScreen(HDC hdc, HWND hwnd);
};

class BlockFinish : public Block
{
public:
    BlockFinish(float2 origin, float2 pos, float2 size);
    virtual void Update() override;
    virtual void OnEntityCollision(Entity* entity) override;
};

enum MoveDirection
{
    Up = 0,
    Right = 1,
    Down = 2,
    Left = 3
};

class Entity
{
public:
    int teamID = 0;
    float2 pos;
    float2 size;
    float3 color;
    float2 origin;
    MapManager* mapManager;
    Entity(float3 color, MapManager* mapManager, float2 pos, float2 size);
    static Entity* GetEntity(int id, MapManager* mapManager, float2 pos, float2 size);
    virtual bool MoveTo(int dir);
    virtual void Update();
    virtual void Draw(HDC hdc);
};

class Player : public Entity
{
public:
    int health = 3;
    bool immortality = false;
    int immortaltyTimer = 0;
    int immortaltyTime = 3;
    float3 colorAlive;
    float3 colorImmortal;
    int gameOverScreenTimer = 0;
    int gameOverScreenTime = 15;
    Player(MapManager* mapManager, float2 pos, float2 size);
    virtual void Update() override;
};

class EntityCreepDire : public Entity
{
public:
    int dir = 0;
    int rotationTimer = 0;
    int timeToRotate = 5;

    EntityCreepDire(MapManager* mapManager, float2 pos, float2 size);
    virtual void Update();
};

Block::Block(float3 color, float2 origin, float2 pos, float2 size)
{
    this->color = color;
    this->origin = origin;
    this->pos = pos;
    this->size = size;

}
Block* Block::GetBlock(int id, float2 origin, float2 pos, float2 size)
{
    Block* block = nullptr;
    switch (id)
    {
    case 0:
    {
        block = new BlockEmpty(origin, pos, size);
        break;
    }
    case 1:
    {
        block = new BlockWall(origin, pos, size);
        break;
    }
    case 2:
    {
        block = new SpikesBlock(origin, pos, size);
        break;
    }
    case 3:
    {
        block = new BlockFinish(origin, pos, size);
        break;
    }
    }
    return block;
}

void Block::OnEntityCollision(Entity* entity)
{
}

void Block::Draw(HDC hdc)
{
    HBRUSH brush = CreateSolidBrush(RGB(color.x, color.y, color.z));
    SelectObject(hdc, brush);
    Rectangle(hdc, origin.x + pos.x, origin.y + pos.y, origin.x + pos.x + size.x, origin.y + pos.y + size.y);
    DeleteObject(brush);
}



BlockEmpty::BlockEmpty(float2 origin, float2 pos, float2 size) : Block(float3{ 200, 200, 200 }, origin, pos, size)
{
}

void BlockEmpty::Update()
{
}

//---------------------SpikesBlock----------------------

SpikesBlock::SpikesBlock(float2 origin, float2 pos, float2 size) : Block(float3{ 100, 100, 100 }, origin, pos, size)
{
    teamID = 1;
}

void SpikesBlock::Update()
{
    if (timer <= spikeOnTimer)
    {
        timer++;
    }
    else
    {
        timer = 0;
        spikesOn = !spikesOn;
    }
}

void SpikesBlock::Draw(HDC hdc)
{
    Block::Draw(hdc);

    HBRUSH brush = CreateSolidBrush(RGB(240, 240, 240));
    SelectObject(hdc, brush);
    if (spikesOn)
    {
        canBeat = true;
        int spikeSizeX = size.x / spikesCountX;
        int spikeSizeY = size.y / spikesCountY;
        for (int y = 0; y < spikesCountY; y++)
            for (int x = 0; x < spikesCountX; x++)
            {
                Ellipse(hdc, pos.x + x * spikeSizeX, pos.y + y * spikeSizeY, pos.x + spikeSizeX + spikeSizeX * x, pos.y + spikeSizeY + spikeSizeY * y);
            }
    }
    else
    {
        canBeat = false;
    }
    DeleteObject(brush);
}

//---------------------BlockWall----------------------



BlockWall::BlockWall(float2 origin, float2 pos, float2 size) : Block(float3{ 0, 0, 0 }, origin, pos, size)
{
    isTransparent = false;
}

void BlockWall::Update()
{
}
//---------------------Map----------------------


Map::Map(MapManager* parentMapManager, float2 origin, float2 blockSize)
{
    this->parentMapManager = parentMapManager;
    this->origin = origin;
    this->blockSize = blockSize;
    int xCount = 0;
    int yCount = 0;
}

Map::~Map()
{
    for (int x = 0; x < xCount; x++)
        for (int y = 0; y < yCount; y++)
            if (blocks[x][y])
                delete blocks[x][y];

    for (int x = 0; x < xCount; x++)
    {
        if (blocks[x])
            delete[yCount] blocks[x];
    }
    if (blocks)
        delete[xCount] blocks;

    for (int i = 0; i < entities.size(); i++)
        if (entities[i])
            entities[i];
    entities.clear();
}

void Map::Load(const char* filePath)
{
    Map::~Map();
    entities = myvector<Entity*>();
    int** blockID;
    int** entityID;

    FILE* filePointer;
    fopen_s(&filePointer, filePath, "r");

    fscanf_s(filePointer, "%d", &xCount);
    fscanf_s(filePointer, "%d", &yCount);

    blocks = new Block * *[xCount];
    for (int x = 0; x < xCount; x++)
        blocks[x] = new Block * [yCount];


    blockID = new int* [xCount];
    entityID = new int* [xCount];
    for (int i = 0; i < xCount; i++)
    {
        blockID[i] = new int[yCount];
        entityID[i] = new int[yCount];
    }


    for (int i = 0; i < yCount; i++)
        for (int j = 0; j < xCount; j++)
            fscanf_s(filePointer, "%d", &(blockID[j][i]));

    for (int i = 0; i < yCount; i++)
        for (int j = 0; j < xCount; j++)
            fscanf_s(filePointer, "%d", &(entityID[j][i]));

    for (int y = 0; y < yCount; y++)
        for (int x = 0; x < xCount; x++)
        {
            float posX = (float)x * blockSize.x;
            float posY = (float)y * blockSize.y;
            blocks[x][y] = Block::GetBlock(blockID[x][y], origin, float2{ posX, posY }, blockSize);

            Entity* entity = Entity::GetEntity(entityID[x][y], parentMapManager, float2{ posX, posY }, blockSize);
            if (entity)
                entities.push_back(entity);
        }

    for (int x = 0; x < xCount; x++)
    {
        delete[yCount] blockID[x];
        delete[yCount] entityID[x];
    }
    delete[xCount] blockID;
    delete[xCount] entityID;

    for (int i = 0; i < entities.size(); i++)
    {
        player = dynamic_cast<Player*>(entities[i]);
        if (player)
            break;
    }
}

//---------------------MapManager----------------------



MapManager::MapManager(float2 origin, float2 blockSize)
{
    mapPaths.push_back("lvl1.txt");
    mapPaths.push_back("lvl2.txt");
    mapPaths.push_back("lvl3.txt");
    mapPaths.push_back("lvl4.txt");
}

MapManager::~MapManager()
{
}

void MapManager::SetGameOverScreen(HDC hdc, HWND hwnd)
{
    if (isDrawingGameOverScreen == true)
    {
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;

        int fontHight = 100;
        int fontWidth = 0;

        HFONT hFont = CreateFont(fontHight, fontWidth, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Courier New");
        SelectObject(hdc, hFont);
        SetTextColor(hdc, RGB(255, 0, 0));

        TCHAR text[] = _T("Game over");

        TextOut(hdc, width * 0.30f, height * 0.5f - fontHight * 0.5f, (LPCWSTR)text, _tcslen(text));
    }
}


//---------------------BlockFinish----------------------



BlockFinish::BlockFinish(float2 origin, float2 pos, float2 size) : Block(float3{ 255, 255, 255 }, origin, pos, size)
{
}

void BlockFinish::Update()
{
    color = float3{ (float)(rand() % 256), (float)(rand() % 256), (float)(rand() % 256) };
}

void BlockFinish::OnEntityCollision(Entity* entity)
{
    Player* player = dynamic_cast<Player*>(entity);
    if (player)
    {
        MapManager* mapManager = player->mapManager;
        if (mapManager->activeMap + 1 < mapManager->mapPaths.size())
            mapManager->activeMap++;
        else
            mapManager->activeMap = 0;
        mapManager->map.Load(mapManager->mapPaths[mapManager->activeMap]);
    }

}

//---------------------Entity----------------------



bool myClamp(int edge0, int edge1, int value)
{
    if (value >= edge0 && value <= edge1)
        return true;
    return false;
}

Entity::Entity(float3 color, MapManager* mapManager, float2 pos, float2 size)
{
    this->mapManager = mapManager;
    this->pos = pos;
    this->size = size;
    origin = { 0, 0 };
    this->color = color;
}

bool Entity::MoveTo(int dir)
{
    switch (dir)
    {
    case MoveDirection::Up:
    {
        int newPosX = ((int)pos.x - mapManager->map.origin.x) / (int)size.x;
        int newPosY = (((int)pos.y - (int)size.y) - mapManager->map.origin.y) / (int)size.y;

        Block* block = nullptr;
        if (myClamp(0, mapManager->map.xCount - 1, newPosX) && myClamp(0, mapManager->map.yCount - 1, newPosY))
            block = mapManager->map.blocks[newPosX][newPosY];

        if (!block || (block && block->isTransparent == true))
            pos.y -= size.y;

        else
            return false;
        break;
    }
    case MoveDirection::Right:
    {
        int newPosX = (((int)pos.x + size.x) - mapManager->map.origin.x) / (int)size.x;
        int newPosY = ((int)pos.y - mapManager->map.origin.y) / (int)size.y;

        Block* block = nullptr;
        if (myClamp(0, mapManager->map.xCount - 1, newPosX) && myClamp(0, mapManager->map.yCount - 1, newPosY))
            block = mapManager->map.blocks[newPosX][newPosY];

        if (!block || (block && block->isTransparent == true))
            pos.x += size.x;
        else
            return false;

        break;
    }
    case MoveDirection::Down:
    {
        int newPosX = ((int)pos.x - mapManager->map.origin.x) / (int)size.x;
        int newPosY = (((int)pos.y + size.y) - mapManager->map.origin.y) / (int)size.y;

        Block* block = nullptr;
        if (myClamp(0, mapManager->map.xCount - 1, newPosX) && myClamp(0, mapManager->map.yCount - 1, newPosY))
            block = mapManager->map.blocks[newPosX][newPosY];

        if (!block || (block && block->isTransparent == true))
            pos.y += size.y;
        else
            return false;
        break;
    }
    case MoveDirection::Left:
    {
        int newPosX = (((int)pos.x - size.x) - mapManager->map.origin.x) / (int)size.x;
        int newPosY = ((int)pos.y - mapManager->map.origin.y) / (int)size.y;

        Block* block = nullptr;
        if (myClamp(0, mapManager->map.xCount - 1, newPosX) && myClamp(0, mapManager->map.yCount - 1, newPosY))
            block = mapManager->map.blocks[newPosX][newPosY];

        if (!block || (block && block->isTransparent == true))
            pos.x -= size.x;
        else
            return false;
        break;
    }
    }
}

void Entity::Update()
{
    int posX = (pos.x - mapManager->map.origin.x) / size.x;
    int posY = (pos.y - mapManager->map.origin.y) / size.y;

    if (myClamp(0, mapManager->map.xCount - 1, posX) && myClamp(0, mapManager->map.yCount - 1, posY))
        if (mapManager->map.blocks[posX][posY])
            mapManager->map.blocks[posX][posY]->OnEntityCollision(this);
}

void Entity::Draw(HDC hdc)
{
    HBRUSH brush = CreateSolidBrush(RGB((int)color.x, (int)color.y, (int)color.z));
    SelectObject(hdc, brush);
    Rectangle(hdc, origin.x + pos.x, origin.y + pos.y, origin.x + pos.x + size.x, origin.y + pos.y + size.y);
    DeleteObject(brush);
}

Entity* Entity::GetEntity(int id, MapManager* mapManager, float2 pos, float2 size)
{
    Entity* entity = nullptr;
    switch (id)
    {
    case 1:
    {
        entity = new Player(mapManager, pos, size);
        break;
    }
    case 2:
    {
        entity = new EntityCreepDire(mapManager, pos, size);
    }
    }
    return entity;
}

//---------------------Player----------------------


float Interpolate(float a, float b, float t)
{
    return a + (b - a) * t;
}

bool myClamp2(int edge0, int edge1, int value)
{
    if (value >= edge0 && value <= edge1)
        return true;
    return false;
}


Player::Player(MapManager* mapManager, float2 pos, float2 size) : Entity(float3{ 100, 100, 255 }, mapManager, pos, size)
{
    teamID = 0;
    colorAlive = color;
    float t = 0.5f;
    colorImmortal = float3{ Interpolate(colorAlive.x, 255, t), Interpolate(colorAlive.y, 0, t), Interpolate(colorAlive.z, 0, t) };
}

void Player::Update()
{
    Entity::Update();
    int blockPosX = (pos.x - mapManager->map.origin.x) / mapManager->map.blockSize.x;
    int blockPosY = (pos.y - mapManager->map.origin.y) / mapManager->map.blockSize.y;

    if (health <= 0)
    {
        mapManager->isDrawing = false;
        if (gameOverScreenTimer <= gameOverScreenTime)
        {
            mapManager->isDrawingGameOverScreen = true;
            gameOverScreenTimer++;
        }
        else
        {
            gameOverScreenTimer = 0;
            mapManager->isDrawingGameOverScreen = false;
            mapManager->isDrawing = true;
            mapManager->activeMap = 0;
            mapManager->map.Load(mapManager->mapPaths[mapManager->activeMap]);
        }
    }
    if (health > 0)
    {
        if (!immortality)
        {
            color = colorAlive;

            if (myClamp2(0, mapManager->map.xCount - 1, blockPosX) && myClamp2(0, mapManager->map.yCount - 1, blockPosY))
                if (mapManager->map.blocks[blockPosX][blockPosY]->canBeat == true && teamID != mapManager->map.blocks[blockPosX][blockPosY]->teamID)
                {
                    health--;
                    immortality = true;
                }
                else
                {
                    for (int i = 0; i < mapManager->map.entities.size(); i++)
                        if (teamID != mapManager->map.entities[i]->teamID)
                            if (pos.x == mapManager->map.entities[i]->pos.x && pos.y == mapManager->map.entities[i]->pos.y)
                            {
                                health--;
                                immortality = true;
                                break;
                            }
                }
        }
        else
        {
            color = colorImmortal;
            if (immortaltyTimer <= immortaltyTime)
                immortaltyTimer++;
            else
            {
                immortaltyTimer = 0;
                immortality = false;
            }
        }
    }
    else
    {

    }
}

//---------------------EntityCreepDire----------------------


EntityCreepDire::EntityCreepDire(MapManager* mapManager, float2 pos, float2 size) : Entity(float3{ 150, 50, 50 }, mapManager, pos, size)
{
    teamID = 1;
}

void EntityCreepDire::Update()
{
    Entity::Update();
    if (rotationTimer >= timeToRotate)
    {
        rotationTimer++;
    }
    else
    {
        dir = rand() % 10;
        rotationTimer = 0;
    }
    if (dir < 5)
        MoveTo(dir);
}


#define MAX_LOADSTRING 100

HINSTANCE hInst;     
WCHAR szTitle[MAX_LOADSTRING];  
WCHAR szWindowClass[MAX_LOADSTRING];

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BASICSOFPROGRAMMINGLAB10KEYHANDLING, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BASICSOFPROGRAMMINGLAB10KEYHANDLING));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_BASICSOFPROGRAMMINGLAB10KEYHANDLING));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_BASICSOFPROGRAMMINGLAB10KEYHANDLING);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

int** LoadTwoDimensionalArray(const char* filePath, int* columnsCount, int* rowsCount)
{
    int** localTable;
    FILE* filePointer;
    fopen_s(&filePointer, filePath, "r");

    fscanf_s(filePointer, "%d", columnsCount);
    fscanf_s(filePointer, "%d", rowsCount);

    localTable = new int* [(*columnsCount)];
    for (int i = 0; i < (*columnsCount); i++)
        localTable[i] = new int[(*rowsCount)];

    for (int i = 0; i < *rowsCount; i++)
        for (int j = 0; j < (*columnsCount); j++)
            fscanf_s(filePointer, "%d", &(localTable[j][i]));

    return localTable;
}

float2 DrawTwoDimensionalArray(HDC hdc, int** array, int xElementsCount, int yElementsCount, int left, int top, int right, int bottom)
{
    int rectangleWidth = (right - left) / (float)xElementsCount;
    int rectangleHeight = (bottom - top) / (float)yElementsCount;
    for (int y = 0; y < yElementsCount; y++)
    {
        for (int x = 0; x < xElementsCount; x++)
        {
            HBRUSH brush = CreateSolidBrush(RGB(array[x][y] * 255, array[x][y] * 255, array[x][y] * 255));
            SelectObject(hdc, brush);
            Rectangle(hdc, left + rectangleWidth * x, top + rectangleHeight * y, left + rectangleWidth * (1 + x), top + rectangleHeight * (1 + y));
        }
    }
    float2 size = { rectangleWidth, rectangleHeight };
    return size;
}

Block* GetBlock(int id, float2 origin, float2 pos, float2 size)
{
    Block* block = nullptr;
    switch (id)
    {
    case 0:
    {
        block = new BlockEmpty(origin, pos, size);
        break;
    }
    case 1:
    {
        block = new BlockWall(origin, pos, size);
        break;
    }
    case 2:
    {
        block = new SpikesBlock(origin, pos, size);
        break;
    }
    }
    return block;
}

MapManager mapManager(float2{ 0, 0 }, float2{ 50, 50 });

void Update()
{
    for (int x = 0; x < mapManager.map.xCount; x++)
        for (int y = 0; y < mapManager.map.yCount; y++)
            mapManager.map.blocks[x][y]->Update();

    for (int i = 0; i < mapManager.map.entities.size(); i++)
        mapManager.map.entities[i]->Update();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_CREATE:
    {
        myvector<int> test;
        test.push_back(100);
        test.push_back(15);
        test.push_back(26);
        int point1 = test[0];
        int point2 = test[1];
        int point3 = test[2];
        SetTimer(hWnd, 0, 100, (TIMERPROC)NULL);
        mapManager.activeMap = 0;
        mapManager.map.Load(mapManager.mapPaths[mapManager.activeMap]);
    }
    case WM_TIMER:
    {
        switch (wParam)
        {
        case 0:
        {
            Update();
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        }
        }
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_KEYDOWN:
    {

        switch (wParam)
        {
        case VK_UP:
        {
            if (mapManager.map.player)
            {
                mapManager.map.player->MoveTo(MoveDirection::Up);
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
        }
        case VK_RIGHT:
        {
            if (mapManager.map.player)
            {
                mapManager.map.player->MoveTo(MoveDirection::Right);
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
        }
        case VK_DOWN:
        {
            if (mapManager.map.player)
            {
                mapManager.map.player->MoveTo(MoveDirection::Down);
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
        }
        case VK_LEFT:
        {
            if (mapManager.map.player)
            {
                mapManager.map.player->MoveTo(MoveDirection::Left);
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
        }
        }
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        if (mapManager.isDrawing == true)
        {
            for (int x = 0; x < mapManager.map.xCount; x++)
                for (int y = 0; y < mapManager.map.yCount; y++)
                    if (mapManager.map.blocks[x][y])
                    {
                        mapManager.map.blocks[x][y]->origin = mapManager.map.origin;
                        mapManager.map.blocks[x][y]->Draw(hdc);
                    }

            for (int i = 0; i < mapManager.map.entities.size(); i++)
            {
                mapManager.map.entities[i]->size = mapManager.map.blockSize;
                mapManager.map.entities[i]->Draw(hdc);
            }
        }
        mapManager.SetGameOverScreen(hdc, hWnd);

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
