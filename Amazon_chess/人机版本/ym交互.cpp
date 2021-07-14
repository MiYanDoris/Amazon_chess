
//姓名：严汨

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <queue>
#include <ctime>
using namespace std;

#define Black "● "
#define White "○ "
#define Obstacle "× "
#define inf 10000000


queue<int> dis;                                 //用来bfs的队列
int timelimit = 0.95 * CLOCKS_PER_SEC;          //防止超时
int board[8][8] = {};                           //存储棋盘上的棋子，0为空，1为黑，2为白，-1为障碍
int dx[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
int dy[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
int step = 0;                                   //当前总共下了几步
int kingmin[3][8][8] = {};                      //kingmin[1][x][y]存储棋盘上黑手用king走法走到（x， y）这个空格需要的最短距离
int queenmin[3][8][8] = {};                     //queen走法走到这个格的最短距离
int maximum ;                                   //搜索的最深层数
int me;                                         //标志机器走的是哪一方
int timeflag = 0;                               //标记是否超时
int start, finish;                              //记录开始和结束时间
int power[12] = {0, 64, 32, 16, 8, 4, 2, 1, 0, 0, 0};//存储2的幂次，避免反复调用pow函数
int degree;
int level[4] = {};
struct node{
    int x0, y0, x1, y1, x2, y2;
}Ans;                                           //记录最终答案

void xuanze();                                       //选择关卡
void restart();                                      //初始化数组
void draw();                                         //画棋盘以及棋子
int Minimax(int , int );                             //利用alpha-beta剪枝进行AI决策
double evaluate(int );                               //评估函数
void clear(int , int , int , int , int , int , int );//清除AI决策中的模拟落子
bool judge(int );                                    //判断某方是否输了
bool legal(int, int);                                //判断这个棋是否在棋盘内
void put(int , int , int , int , int , int , int );  //模拟AI决策中的落子
void DK(int player);                                 //计算king距离的函数
void DQ(int player);                                 //计算queen距离的函数
double P1(int current);                              //评估局面的第一种方式：sigma（2 ^（-我方距离） - 2 ^（-敌方距离））
double P2(int current);                              //评估局面的第二种方式：
bool yh_legal(int, int, int, int,int ,int , int);    //判断用户这一步下的棋是否合法
void cunpan();                                       //存盘：记录当前棋盘的状态
void dupan();                                        //读盘：恢复上次中止时的棋盘状态
int kexing(int , int , int , int );                  //判断AI这一步下的棋是否合法
void AIplay();                                       //AI自己玩
void newgame();                                      //开始新游戏
void mode();                                         //用户可以选择这4种模式:对战AI，看AI自己和自己下棋，继续刚才中止的棋局，结束游戏
void AI( );                                          //Ai下棋
void human( );                                        //人类下棋 
void play(int );                                      //博弈的部分
void jiaohu();

void AI()
{
    start = clock();
    Ans.x0 = 0, Ans.y0 = 0, Ans.x1 = 0, Ans.y1 = 0, Ans.x2 = 0, Ans.y2 = 0;
    timeflag = 0;
    if(step <= 8)//开局可能性太多，不能搜太多层
        maximum = 2;
    else
        maximum = 3;
    while(1)
    {
        int tmp = Minimax(1, inf + 100);//由于剪枝的时候，不能把depth = 1时的剪掉，所以需要保证此时ans不可能大于等于previous,因此是inf + 100
        if(tmp == inf || timeflag == 1) //如果已经找到一种必胜策略，或者是已经超时了，就赶紧输出
        {
            put(Ans.x0, Ans.y0, Ans.x1, Ans.y1, Ans.x2, Ans.y2, me);
            cout << endl;
			cout << "    第" << ++step << "步:" << endl;
            draw();
            return;
        }
        if(step >= 24)//如果搜完了一层，继续增加层数
            maximum += 2;
        else
            maximum ++;
    }
}

void xuanze()
{
    char ch[100];
    cout << "关卡1:新手入门版本：千里之行，始于足下！只有反复锤炼，才能赢得“至尊勋章”！" << endl;
    cout << "关卡2:棋术进阶版本：没有最好，只有更好！向着至尊勋章的万丈光芒前进吧！" << endl;
    cout << "关卡3:登峰造极版本：加油！至尊勋章就在你的眼前！" << endl;
    cout << "请输入对应的编号" << endl;
    cin.getline(ch, 100);
    while(ch[0] != '1' && ch[0] != '2' && ch[0] != '3')
    {
        cout << "糟糕！您一定是在说小黄鸭国语言吧，亚马逊国的bot听不懂！" << endl;
        cout << "请再输入一次" << endl;
        cin.getline(ch, 100);
    }
    switch (ch[0])
    {
        case '1' : timelimit = 0.3 * CLOCKS_PER_SEC;degree = 1;break;
        case '2' : timelimit = 0.5 * CLOCKS_PER_SEC;degree = 2;break;
        case '3' : timelimit = 0.95 * CLOCKS_PER_SEC;degree = 3;break;
        default: break;
    }
    return;
}

void newgame()
{
    restart();
    char ch[100];
    int x0, y0, x1, y1, x2, y2;
	cout << endl;
    cout << "请选择您想挑战的关卡：" << endl;
    xuanze();
	cout << "    亲爱的玩家，您想下哪一方呢？" << endl << "    黑方请输入B" << endl << "    白方请输入W" << endl;
    cin.getline(ch, 100);
    while(ch[0] != 'B' && ch[0] != 'b' && ch[0] != 'W' && ch[0] != 'w')
    {
        cout << "糟糕！您一定是在说小黄鸭国语言吧，亚马逊国的bot听不懂！" << endl;
        cout << "请再输入一次" << endl;
        cout << "    黑方请输入B" << endl << "    白方请输入W" << endl;
        cin.getline(ch, 100);
    }
    if(ch[0] == 'B' || ch[0] == 'b')
    {
        me = 2;
        cout << endl;
		draw();
        play(1);
    }
    else
    {
        me = 1;
		play(2);
    }
    return;
}

void human()
{
    char ch[100];
    int x0, y0, x1, y1, x2, y2;
    cout << "轮到您啦！展示您的才华吧(^-^)" << endl;
    cout << "如果您想休息，请输入q，否则请输入您想下的棋步" << endl;
    cout << "请按照先列后行的顺序依次输入原来皇后、现在皇后的位置，以及障碍的位置" << endl;
    cout << "请确保输入的每两个数字之间恰有一个空格哦(￣∇￣)" << endl;
    cin.getline(ch, 100);
    x0 = ch[0] - '0', y0 = ch[2] - '0', x1 = ch[4] - '0', y1 = ch[6] - '0', x2 = ch[8] - '0', y2 = ch[10] - '0';
    while(!yh_legal(x0, y0, x1, y1, x2, y2, 3 - me) && x0 != int('q' - '0') && x0 != int('Q' - '0'))
    {
        cout << "糟糕！您输入有误，愚蠢的小bot听不懂！" << endl;
        cout << "请再输入一次" << endl;
        cin.getline(ch, 100);
        x0 = ch[0] - '0', y0 = ch[2] - '0', x1 = ch[4] - '0', y1 = ch[6] - '0', x2 = ch[8] - '0', y2 = ch[10] - '0';
    }
    if(x0 == int('q' - '0') || x0 == int('Q' - '0'))
    {
        cunpan();
        return;
    }
    else
        put(x0, y0, x1, y1, x2, y2, 3 - me);
	cout << endl;
	cout << "    第" << ++step << "步:" << endl;
	draw();
    return;
}

void play(int mode)
{
    if(mode == 1)//人是先手
    {
        while(1)
        {
            human();
            if(!judge(me))
            {
                cout << "哇！恭喜您！" << endl;
                cout << "您成功完成了第" << degree << "关。" << endl;
                level[degree] = 1;
                if(level[1] == 1 && level[2] == 1 && level[3] == 1)
                    cout << "勇士，您已摘得亚马逊王国“至尊勋章”Y(^_^)Y" << endl;
                break;
            }
            AI();
            if(!judge(3 - me))
            {
                cout << "哦～蠢萌的bot侥幸地战胜了您...继续加油哦～" << endl;
                break;
            }
        }
    }
    else
    {
        while(1)
        {
            AI();
            if(!judge(3 - me))
            {
                cout << "哦～蠢萌的bot侥幸地战胜了您...继续加油哦～" << endl;
                break;
            }
            human();
            if(!judge(me))
            {
                cout << "哇！恭喜您！" << endl;
                cout << "您成功完成了第" << degree << "关。" << endl;
                level[degree] = 1;
                if(level[1] == 1 && level[2] == 1 && level[3] == 1)
                    cout << "勇士，您已摘得亚马逊王国“至尊勋章”Y(^_^)Y" << endl;
                break;
            }
        }
    }
    return;
}

void mode()
{
    char ch[100];
    cout << endl << endl << endl << endl << endl << endl;
    cout << "☆*:.｡. o(≧▽ ≦)o .｡.:*☆" << endl;
    cout << "勇士！欢迎来到亚马逊棋王国 O(∩_∩)O~ " << endl;
    cout << "我是您的向导严小汨，接下来的冒险将由我带领您完成～～(按任意键以继续)" << endl;
    cin.getline(ch, 100);
    cout << "在这片智慧之光笼罩的国土之上，只要修炼出精湛的棋术" << endl;
    cout << "就能赢得国王授予的“至尊勋章” ★彡…… ，受到全国人民的景仰！(按任意键以继续)" << endl;
    cin.getline(ch, 100);
    cout << "现在，请开始您的冒险吧！" << endl;
    cout << "在这里您可以有以下几种选择：(按任意键以继续)" << endl;
    cin.getline(ch, 100);
    cout << endl;
    cout << "1.现在就开始我的挑战！" << endl;
    cout << "2.欣赏两只本国的bot自己下一盘棋" << endl;
    cout << "3.继续刚才中止的棋局" << endl;
    cout << "4.结束游戏" << endl;
    cout << endl;
    cout << "请您输入您想要的模式的编号吧~" << endl;
    while(cin.getline(ch, 100))
    {
        int flag = 0;//标记是否要结束本游戏
        int num = ch[0] - '0';
        switch (num)
        {
            case 1: newgame();break;
            case 2: AIplay();break;
            case 3: dupan();break;
            case 4:
                {
                    cout << "小bot那么可爱，您居然忍心离开!那好吧，小bot自己玩去啦，再见～～" << endl;
                    cout << endl << endl;
                    flag = 1;
                    break;
                }
            default: cout << "对不起，蠢萌的bot听不懂您的话，请再输入一次～(按任意键以继续)" << endl;cin.getline(ch, 100);break;
        }
        if(flag == 1)
            break;
        cout << "您还有什么吩咐嘛～～～：" << endl;
        cout << endl;
        cout << "1.继续我的挑战！" << endl;
        cout << "2.欣赏两只本国的bot自己下一盘棋" << endl;
        cout << "3.继续刚才中止的棋局" << endl;
        cout << "4.结束游戏" << endl;
        cout << endl;
        cout << "请您输入您想要的模式的编号吧~" << endl;
    }
    return;
}

void jiaohu()
{
    char ch[100];
    cout << "您还有什么吩咐嘛～～～：" << endl;
    cout << endl;
    cout << "1.继续我的挑战！" << endl;
    cout << "2.欣赏两只本国的bot自己下一盘棋" << endl;
    cout << "3.继续刚才中止的棋局" << endl;
    cout << "4.结束游戏" << endl;
    cout << endl;
    cout << "请您输入您想要的模式的编号吧~" << endl;
    while(cin.getline(ch, 100))
    {
        int flag = 0;//标记是否要结束本游戏
        int num = ch[0] - '0';
        switch (num)
        {
            case 1: newgame();break;
            case 2: AIplay();break;
            case 3: dupan();break;
            case 4:
                {
                    cout << "小bot那么可爱，您居然忍心离开!那好吧，小bot自己玩去啦，再见～～" << endl;
                    cout << endl << endl;
                    flag = 1;
                    exit(0);
                }
            default: cout << "对不起，蠢萌的bot听不懂您的话，请再输入一次～(按任意键以继续)" << endl;cin.getline(ch, 100);break;
        }
        if(flag == 1)
            break;
        cout << "您还有什么吩咐嘛～～～：" << endl;
        cout << endl;
        cout << "1.继续我的挑战！" << endl;
        cout << "2.欣赏两只本国的bot自己下一盘棋" << endl;
        cout << "3.继续刚才中止的棋局" << endl;
        cout << "4.结束游戏" << endl;
        cout << endl;
        cout << "请您输入您想要的模式的编号吧~" << endl;
    }
    return;
}

void AIplay()
{
    restart();
    char ch[100];
    draw();
    cout << "欢迎来到蠢萌bot竞技场(^-^)" << endl;
    cout << "接下来请欣赏两只蠢萌的小bot自己打架，请按任意键以开始" << endl;
    cin.getline(ch, 100);
    while(1)
    {
        me = 1;
        AI();
        if(!judge(2))
        {
            cout << "blingbling～～蠢萌1号bot成功打败蠢萌2号bot" << endl << endl;
            break;
        }
        cout << "好奇接下来蠢萌2号会怎么应对嘛？按任意键以继续" << endl;
        cout << "如果您想休息，请输入q" << endl;
        cin.getline(ch, 100);
        if(ch[0] == 'q' || ch[0] == 'Q')
        {
            cout << "您还想继续尝试别的功能嘛？想继续尝试的话，请输入1，想直接退出请输入0" << endl;
            cin.getline(ch, 100);
            while(ch[0] != '0' && ch[0] != '1')
            {
                cout << "糟糕！您一定是在说小黄鸭国语言吧，亚马逊国的bot听不懂！" << endl;
                cout << "请再输入一次" << endl;
                cin.getline(ch, 100);
            }
            if(ch[0] == '1')
            {
                jiaohu();
                exit(0);
            }
             exit(0);
        }
        me = 2;
        AI();
        if(!judge(1))
        {
            cout << "blingbling～～蠢萌2号bot成功打败蠢萌1号bot" << endl << endl;
            break;
        }
        cout << "好奇接下来蠢萌1号会怎么应对嘛？按任意键以继续" << endl;
        cout << "如果您想休息，请输入q" << endl;
        cin.getline(ch, 100);
        if(ch[0] == 'q' || ch[0] == 'Q')
        {
            cout << "您还想继续尝试别的功能嘛？想继续尝试的话，请输入1，想直接退出请输入0" << endl;
            cin.getline(ch, 100);
            while(ch[0] != '0' && ch[0] != '1')
            {
                cout << "糟糕！您一定是在说小黄鸭国语言吧，亚马逊国的bot听不懂！" << endl;
                cout << "请再输入一次" << endl;
                cin.getline(ch, 100);
            }
            if(ch[0] == '1')
            {
                jiaohu();
                exit(0);
            }
             exit(0);
        }
    }
    return ;
}

void restart()
{
    step = 0;               //新游戏从0开始计算
    memset(board, 0, sizeof(board));
    board[0][2] = 1;
    board[2][0] = 1;
    board[5][0] = 1;
    board[7][2] = 1;
    board[0][5] = 2;
    board[2][7] = 2;
    board[5][7] = 2;
    board[7][5] = 2;
    memset(level, 0, sizeof(level));
}

void draw()//画棋盘以及棋子
{
    cout << endl;
    cout << "    0   1   2   3   4   5   6   7" << endl;
    cout << "  ┌───┬───┬───┬───┬───┬───┬───┬───┐" << endl;
    for(int i = 0; i < 8; i++)
    {
        cout << " " << i;
        for(int j = 0; j < 8; j++)
        {
            switch (board[j][i])
            {
            case 1 :
                cout << "│ " << setw(2) << Black;
                break;
            case 2 :
                cout << "│ " << setw(2) << White;
                break;
            case -1 :
                cout << "│ " << setw(2) << Obstacle;
                break;
            default:
                cout << "│ " << setw(2) << ' ';
                break;
            }
        }
        cout << "│ " << i << endl;
		if(i < 7)
            cout << "  ├───┼───┼───┼───┼───┼───┼───┼───┤" << endl;
    }
    cout << "  └───┴───┴───┴───┴───┴───┴───┴───┘" << endl;
	cout << "    0   1   2   3   4   5   6   7" << endl;
	cout << endl;
}

void cunpan()//存档：记录当前棋盘的状态
{
    char ch[100];
    cout << "您还想继续尝试别的功能嘛？想继续尝试的话，请输入1，想直接退出请输入0" << endl;
    cin.getline(ch, 100);
    while(ch[0] != '0' && ch[0] != '1')
    {
        cout << "糟糕！您一定是在说小黄鸭国语言吧，亚马逊国的bot听不懂！" << endl;
        cout << "请再输入一次" << endl;
        cin.getline(ch, 100);
    }
    if(ch[0] == '1')
    {
        jiaohu();
        exit(0);
    }
    else
    {
        cout << "如果在离开我们美丽富饶的亚马逊国前，想要保存本次尚未下完的棋局，请输入1，否则请输入0" << endl;
    cin.getline(ch, 100);
    while(ch[0] != '0' && ch[0] != '1')
    {
        cout << "糟糕！您一定是在说小黄鸭国语言吧，亚马逊国的bot听不懂！" << endl;
        cout << "请再输入一次" << endl;
        cin.getline(ch, 100);
    }
    if(ch[0] == '0')
        exit(0);
    ofstream fout("cun.txt");
    if(!fout)
    {
        cout << "请检查文件是否被其他程序占用";
        return ;
    }
    fout << me << endl;
    fout << step << endl;
    fout << degree << endl;
    fout << level[1] << " " << level[2] << " " << level[3] << endl;
    for(int i = 0; i <= 7; i++)
        for(int j = 0; j <= 7; j++)
            fout << board[i][j] << " ";
    fout.close();
    exit(0);
    }
}

void dupan()//读盘：恢复上次中止时的棋盘状态
{
    ifstream fin("cun.txt");
    if(!fin)
    {
        cout << "请检查文件是否被其他程序占用";
        return ;
    }
    fin >> me;
    fin >> step;
    fin >> degree;
    fin >> level[1] >> level[2] >> level[3];
    for(int i = 0; i <= 7; i++)
        for(int j = 0; j <= 7; j++)
            fin >> board[i][j];
    fin.close();
    cout << "    刚才由bot下了第" << step << "步:" << endl;
    draw();
    play(3 - me);
    return ;
}

inline bool legal(int x, int y)//判断这个棋是否在棋盘内
{
    if(x >= 0 && x < 8 && y >= 0 && y < 8)
        return 1;
    else
        return 0;
}


bool yh_legal(int x0, int y0, int x1, int y1, int x2, int y2, int player)//判断用户这一步下的棋是否合法
{
    if(!legal(x0, y0) || !legal(x1, y1) || !legal(x2, y2))
        return 0;
    if(board[x0][y0] != player || board[x1][y1] != 0 )//新下的皇后是否合法
        return 0;
    if(board[x2][y2] != 0 && !(x2 == x0 && y2 == y0))//箭是否合法
        return 0;
    int flag = 1;
    return 1;
}

void DQ(int player)//计算queen距离的函数
{
    for(int i = 0; i <= 7; i++)
        for(int j = 0; j <= 7; j++)
            queenmin[player][i][j] = inf;
    for(int i = 0; i <= 7; i++)
        for(int j = 0; j <= 7; j++)
        if(board[i][j] == player)
        {
            dis.push(i * 8 + j);//为了让二维数组排成一维的队列，使用加权的办法，行坐标加权8，列坐标加权1
            queenmin[player][i][j] = 0;//这些皇后所在的格子距离为0
        }
    while(dis.empty() == 0)
    {
        int t = dis.front();//寻找从这个队列中第一个格出发可以走到的地方
        int x = t / 8;//读出它的横纵坐标
        int y = t % 8;
        dis.pop();
        for(int i = 0; i <= 7; i++)//分别向八个方向走
        {
            int length = 1;
            while(1)
            {
                int tx = x + dx[i] * length;
                int ty = y + dy[i] * length;
                if(legal(tx, ty))
                {
                    if(board[tx][ty] != 0)//如果遇到障碍了就结束这个方向
                        break;
                    else if(queenmin[player][tx][ty] == inf)//还未入过队则入队
                    {
                        queenmin[player][tx][ty] = queenmin[player][x][y] + 1;
                        dis.push(tx * 8 + ty);
                    }
                }
                else
                    break;
                length++;
            }
        }
    }
    return;
}

void DK(int player)//player走到当前格的king走法的最少移动次数，与DQ函数大同小异
{
    for(int i = 0; i <= 7; i++)
        for(int j = 0; j <= 7; j++)
                kingmin[player][i][j] = inf;
    for(int i = 0; i <= 7; i++)
        for(int j = 0; j <= 7; j++)
        if(board[i][j] == player)
        {
            dis.push(i * 8 + j);
            kingmin[player][i][j] = 0;
        }
    while(dis.empty() == 0)
    {
        int t = dis.front();//为了让二维数组排成一维的队列，使用加权的办法，行坐标加权8，列坐标加权1
        int x = t / 8;
        int y = t % 8;
        dis.pop();
        for(int i = 0; i <= 7; i++)
        {
            int tx = x + dx[i];
            int ty = y + dy[i];
            if(legal(tx, ty))
                if(board[tx][ty] == 0 && kingmin[player][tx][ty] == inf)
                {
                    kingmin[player][tx][ty] = kingmin[player][x][y] + 1;
                    dis.push(tx * 8 + ty);
                }
        }
    }
    return;
}

double P1(int current)//评估局面的第一种方式
{
    DQ(1);
    DQ(2);
    double p1 = 0;
    for(int i = 0; i <= 7; i++)
        for(int j = 0; j <= 7; j++)
            if(board[i][j] == 0)
            {
                if(queenmin[me][i][j] != inf)//如果这个空格我方可以走到
                    p1 += power[queenmin[me][i][j]];
                if(queenmin[3 - me][i][j] != inf)
                    p1 -= power[queenmin[3 - me][i][j]];
            }
    p1 /= 64;
    return p1;
}

double P2(int current)//评估局面的第二种方式
{
    DK(1);
    DK(2);
    double t = 0;
    for(int i = 0; i <= 7; i++)
        for(int j = 0; j <= 7; j++)
            if(board[i][j] == 0)
            {
                if(kingmin[me][i][j] < kingmin[3 - me][i][j])
                {
                    t += 6;
                }
                else if(kingmin[me][i][j] > kingmin[3 - me][i][j])
                {
                    t -= 6;
                }
                else//如果双方到达的最短距离一样，则先手方有优势
                {
                    if(kingmin[me][i][j] != inf)
                    {
                        if(me == current)
                            t += 1;
                        else
                            t -= 1;
                    }
                }
            }
    return t / 6;
}

double evaluate(int current)//评估现在是current下棋的时候，当前局面对我的评估值
{
    int my = judge(me), op = judge(3 - me);
    if(my == 0 && (op != 0 || me == current))//如果我无路可走了，这是最坏的局面
        return -inf;
    if(op == 0 && (my != 0 || me != current))//如果对手无路可走了，这是最好的局面
        return inf;
    if(step >= 15)//为了防止在评估函数上消耗大量时间，选择分段处理，开局时少用一种函数
        return P2(current);
    else
        return P1(current) + P2(current);
}

void put(int x0, int y0, int x1, int y1, int x2, int y2, int player)
{
    board[x0][y0] = 0;
    board[x1][y1] = player;
    board[x2][y2] = -1;
}

void clear(int x0, int y0, int x1, int y1, int x2, int y2, int player)
{
    board[x2][y2] = 0;
    board[x1][y1] = 0;
    board[x0][y0] = player;
}

int Minimax(int depth, int previous)
{
    finish = clock();
    int current = (depth + me) % 2 + 1;//确认当前下棋者
    if(timeflag == 1 || (finish - start) >= timelimit)//如果已经超时了，就结束
    {
        timeflag = 1;
        if(current == me)
            return inf;
        else
            return -inf;
    }
    if(depth == maximum)//达到预定深度，评估当前局面
        return evaluate(current);
    int ans, tmp;
    if(current == me)//如果现在是我下，那么我希望达到最大值，所以初始化ans为负无穷
        ans = -inf;
    else
        ans = inf;
    int xiaguo = 0;//标记是否下过
    node temp;
    for(int i = 0; i <= 7; i++)//逐一枚举每一个皇后的每一种走法，并评估找到最优解
    {
        for(int l = 0; l <= 7; l++)
        {
            if(board[i][l] != current)
                continue;
            int x0 = i;//原来皇后位置
            int y0 = l;
            for(int j = 0; j <= 7; j++)//逐一枚举每一个方向
            {
                int t = 1;//新的皇后的位置与旧的皇后的位置差的king距离
                while(1)
                {
                    int tx = x0 + dx[j] * t;//新的皇后位置
                    int ty = y0 + dy[j] * t;
                    if(!legal(tx, ty))
                        break;
                    if(board[tx][ty] != 0)
                        break;
                    board[tx][ty] = current;//移动皇后
                    board[x0][y0] = 0;
                    for(int j = 0; j <= 7; j++)//开始放箭，逐一枚举每一个方向
                    {
                        int flag = 0;
                        int length = 1;
                        while(1)
                        {
                            int xx = tx + dx[j] * length;
                            int yy = ty + dy[j] * length;
                            if(!legal(xx, yy))//超出棋盘了，不能继续尝试在更远的地方放置障碍了
                                break;
                            if(board[xx][yy] != 0 && !(xx == x0 && yy == y0))//如果既不是空的，也不是之前皇后的位置，那么不能放箭
                                break;
                            board[xx][yy] = -1;//放障碍
                            tmp = Minimax(depth + 1, ans);//我方下完后，由对方进行下一步使得对我最不利
                            if(current == me)//如果现在是我下
                            {
                                if(xiaguo == 0 || tmp > ans)//如果对方对我最不利的策略比之前得到的答案好，就更新答案
                                {
                                    ans = tmp;
                                    xiaguo = 1;
                                    if(depth == 1)
                                    {
                                        temp.x0 = x0, temp.y0 = y0, temp.x1 = tx, temp.y1 = ty, temp.x2 = xx, temp.y2 = yy;
                                    }
                                }
                            }
                            else                //如果现在是对方下
                            {
                                if(xiaguo == 0 || tmp < ans)//如果可以对我更加不利，就更新策略
                                {
                                    ans = tmp;
                                    xiaguo = 1;
                                }
                            }
                            if(current != me && ans <= previous)//如果第k步是对方下棋，并且在我进行这样的第k - 1步下，
                            {                                   //他可以找到一种操作使得：比我先前找到的一种第k - 1步的策略更糟糕，那么我一定不会选择这样的第k-1步，直接剪枝
                                clear(x0, y0, tx, ty, xx, yy, current);
                                return ans;
                            }
                            if(current == me && ans >= previous)//如果现在是我下第k步，并且对手在这样的第k - 1步下，
                            {                                   //我可以找到一种操作使得：比他先前找到的一种第k - 1步的策略对他更不利，那么他一定不会选择这样的第k-1步，直接剪枝
                                clear(x0, y0, tx, ty, xx, yy, current);
                                return ans;
                            }
                            board[xx][yy] = 0;//撤回放障碍
                            length++;
                        }
                    }
                    board[tx][ty] = 0;//还原皇后
                    board[x0][y0] = current;
                    t++;
                }
            }
        }
    }
    if(depth == 1)
    {
        if(Ans.x0 == 0 && Ans.y0 == 0 && Ans.x1 == 0 && Ans.y1 == 0)//如果还没有答案，不管是否超时，先找一个答案出来再说
            Ans = temp;
        else if(timeflag == 0)//如果已经有答案并且也没有超时（表示完整地搜完了一层）
            Ans = temp;
    }
    return ans;
}

bool judge(int player)//判断某方是否输了，输了返回0
{
    for(int i = 0; i <= 7; i++)//这四个皇后中，只要有一个人可以走就没输
        for(int k = 0; k <= 7; k++)
        {
            if(board[i][k] != player)
                continue;
            for(int j = 0; j <= 7; j++)
            {
                int tx = i + dx[j];
                int ty = k + dy[j];
                if(legal(tx, ty))
                    if(board[tx][ty] == 0)//只有有一个合法的邻格空着，这个皇后就没死
                        return 1;
            }
        }
    return 0;//如果所有皇后都动不了就输了
}

int main()
{
    mode();
    return 0;
}