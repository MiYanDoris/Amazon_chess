#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <queue>
using namespace std;

#define Black "● "
#define White "○ "
#define Obstacle "× "
#define inf 10000000

int board[8][8] = {};//存储棋盘上的棋子，0为空，1为黑，2为白，-1为障碍
int dx[8] = {0, 0, 1, 1, 1, -1, -1, -1};
int dy[8] = {1, -1, 0, -1, 1, 0, -1, 1};
int step = 0;//当前总共下了几步
int kingmin[3][8][8] = {};
int queenmin[3][8][8] = {};
int power[7] = {1, 2, 4, 8, 16, 32, 64};
int maximum = 3;
int me;//标志机器走的是哪一方
const int timelimit = 0.95 * CLOCKS_PER_SEC;//防止超时
int timeflag = 0;
int start, finish;
queue<int> dis;

void DQ(int player);             //@
void DK(int player);             //@
void draw();//画棋盘以及棋子
void restart();                 //@初始化数组
bool legal(int, int);//判断这个棋是否在棋盘内
bool yh_legal(int , int , int , int , int , int , int);//判断用户这一步下的棋是否合法
void cunpan();//存盘：记录当前棋盘的状态
void dupan();//读盘：恢复上次中止时的棋盘状态
bool judge(int );//判断某方是否输了
int kexing(int , int , int , int );//判断AI这一步下的棋是否合法
void AI(int );//AI决策下哪一步
int evaluate(int );//新的评估方式
double evaluate2(int );//新的评估方式
int Minimax(int , int );//利用alpha-beta剪枝进行AI决策，采用新的评估方式
int Minimax2(int , int );//采用老的评估方式
void AIplay();//AI自己玩
void newgame();//开始新游戏
void mode();//用户可以选择这4种模式:对战AI，看AI自己和自己下棋，继续刚才中止的棋局，结束游戏
void put(int , int , int , int , int , int , int );//模拟AI决策中的落子
void clear(int , int , int , int , int , int , int );//清除AI决策中的模拟落子

void DQ(int player)
{
    for(int i = 0; i <= 7; i++)
        for(int j = 0; j <= 7; j++)
            queenmin[player][i][j] = inf;
    for(int i = 0; i <= 3; i++)
    {
        int tx = queen[player][i][0];
        int ty = queen[player][i][1];
        dis.push(tx * 8 + ty);
        queenmin[player][tx][ty] = 0;
    }
    while(dis.empty() == 0)
    {
        int t = dis.front();//为了让二维数组排成一维的队列，使用加权的办法，行坐标加权8，列坐标加权1
        int x = t / 8;
        int y = t % 8;
        dis.pop();
        for(int i = 0; i <= 7; i++)
        {
            int length = 1;
            while(1)
            {
                int tx = x + dx[i] * length;
                int ty = y + dy[i] * length;
                if(legal(tx, ty))
                {
                    if(board[tx][ty] != 0)
                        break;
                    else if(queenmin[player][tx][ty] == inf)
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

void DK(int player)//player走到当前格的king走法的最少移动次数
{
    for(int i = 0; i <= 7; i++)
        for(int j = 0; j <= 7; j++)
                kingmin[player][i][j] = inf;
    for(int i = 0; i <= 3; i++)
    {
        int tx = queen[player][i][0];
        int ty = queen[player][i][1];
        dis.push(tx * 8 + ty);
        kingmin[player][tx][ty] = 0;
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

void restart()
{
    step = 0;//新游戏从0开始计算
    memset(board, 0, sizeof(board));
    memset(queen, 0, sizeof(queen));
    board[0][2] = 1;
    board[2][0] = 1;
    board[0][5] = 1;
    board[2][7] = 1;
    board[5][0] = 2;
    board[7][2] = 2;
    board[5][7] = 2;
    board[7][5] = 2;
    queen[1][0][1] = 2;
    queen[1][1][0] = 2;
    queen[1][2][1] = 5;
    queen[1][3][0] = 2;
    queen[1][3][1] = 7;
    queen[2][0][0] = 5;
    queen[2][1][0] = 7;
    queen[2][1][1] = 2;
    queen[2][2][0] = 5;
    queen[2][2][1] = 7;
    queen[2][3][0] = 7;
    queen[2][3][1] = 5;
}

void newgame()//开始新游戏
{
    restart();
    char ch;
	system("cls");
    int x0, y0, x1, y1, x2, y2;
	cout << endl;
	cout << "    亲爱的玩家，您想下哪一方呢？" << endl << "    黑方请输入B" << endl << "    白方请输入W" << endl;
    cin >> ch;
    while(ch != 'B' && ch != 'b' && ch != 'W' && ch != 'w')
    {
        cout << "糟糕！蠢萌的bot听不懂您的话，请再输入一次" << endl;
        cout << "    黑方请输入B" << endl << "    白方请输入W" << endl;
        cin >> ch;
    }
    if(ch == 'B' || ch == 'b')
    {
        me = 2;
        system("cls");
		cout << endl;
		draw();
        while(1)
        {
            cout << "轮到您啦！展示您的才华吧(^-^)" << endl;
            cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
            while(!yh_legal(x0, y0, x1, y1, x2, y2, 1))
            {
                cout << "抱歉，这是不合法的下法，请您再试一次" << endl;
                cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
            }
            put(x0, y0, x1, y1, x2, y2, 1);
            system("cls");
			cout << endl;
			cout << "    第" << ++step << "步:" << endl;
			draw();
            if(!judge(2))
            {
                cout << "哇！您成功战胜了蠢萌的小bot，大佬求带飞！！！" << endl;
                break;
            }
            start = clock();
            timeflag = 0;
            Minimax(1, inf);
            if(!judge(1))
            {
                cout << "哦～蠢萌的bot侥幸地战胜了您...继续加油哦～" << endl;
                break;
            }
        }
    }
    else
    {
        me = 1;
        while(1)
        {
            start = clock();
            timeflag = 0;
            Minimax(1, inf);
            if(!judge(2))
            {
                cout << "哦～蠢萌的bot侥幸地战胜了您...继续加油哦" << endl;
                break;
            }
            cout << "轮到你啦！展示您的才华吧(^-^)" << endl;
            cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
            while(!yh_legal(x0, y0, x1, y1, x2, y2, 2))
            {
                cout << "抱歉，这是不合法的下法，请您再试一次" << endl;
                cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
            }
            put(x0, y0, x1, y1, x2, y2, 2);
            system("cls");
			cout << endl;
			cout << "    第" << ++step << "步:" << endl;
			draw();
            if(!judge(1))
            {
                cout << "哇！您成功战胜了蠢萌的小bot，大佬求带飞！！！" << endl;
                break;
            }
        }
    }
    return;
}

void AIplay()//AI自己玩
{
    restart();
    char ch;
    draw();
    cout << "欢迎来到蠢萌bot竞技场(^-^)" << endl;
    cout << "接下来请欣赏两只蠢萌的小bot自己打架，请按任意键以开始" << endl;
    cin.get();
    while(1)
    {
        me = 1;
        start = clock();
        timeflag = 0;
        Minimax(1, inf);
        if(!judge(2))
        {
            cout << "blingbling～～蠢萌1号bot成功打败蠢萌2号bot" << endl << endl;
            break;
        }
        cout << "好奇接下来蠢萌2号会怎么应对嘛？按任意键以继续" << endl;
        me = 2;
        start = clock();
        timeflag = 0;
        Minimax2(1, inf);
        if(!judge(1))
        {
            cout << "blingbling～～蠢萌2号bot成功打败蠢萌1号bot" << endl << endl;
            break;
        }
        cout << "好奇接下来蠢萌1号会怎么应对嘛？按任意键以继续" << endl;
    }
    return ;
}

double TP1(int player)
{
    DQ(1);
    DQ(2);
    double t1 = 0;
    for(int i = 0; i <= 7; i++)
        for(int j = 0; j <= 7; j++)
            if(board[i][j] == 0)
            {
                if(queenmin[player][i][j] < queenmin[3 - player][i][j])
                {
                    t1 += 1;
                }
                else if(queenmin[player][i][j] > queenmin[3 - player][i][j])
                {
                    t1 -= 1;
                }
                else
                {
                    if(queenmin[player][i][j] != inf)
                        t1 += 0.5;
                }
            }
    double p1 = 0;
    for(int i = 0; i <= 7; i++)
        for(int j = 0; j <= 7; j++)
            if(board[i][j] == 0)
            {
                if(queenmin[player][i][j] != inf)
                    p1 += power[7 - queenmin[player][i][j]];
                if(queenmin[3 - player][i][j] != inf)
                    p1 -= power[7 - queenmin[3 - player][i][j]];
            }
    p1 /= 64;
    if(step <= 20)
        return 0.14 * t1 + 0.13 * p1;
    else if(step <= 45)
        return 0.3 * t1 + 0.2 * p1;
    else
        return 0.8 * t1 + 0.05 * p1;
}

double TP2(int player)
{
    DK(1);
    DK(2);
    double t2 = 0;
    for(int i = 0; i <= 7; i++)
        for(int j = 0; j <= 7; j++)
            if(board[i][j] == 0)
            {
                if(kingmin[player][i][j] < kingmin[3 - player][i][j])
                {
                    t2 += 1;
                }
                else if(kingmin[player][i][j] > kingmin[3 - player][i][j])
                {
                    t2 -= 1;
                }
                else
                {
                    if(kingmin[player][i][j] != inf)
                        t2 += 0.5;
                }
            }
    double p2 = 0;
    for(int i = 0; i <= 7; i++)
        for(int j = 0; j <= 7; j++)
            if(board[i][j] == 0)
            {
                if(kingmin[3 - player][i][j] <= kingmin[player][i][j] - 6)
                {
                    p2 += -1;
                }
                else if(kingmin[3 - player][i][j] <= kingmin[player][i][j] + 6)
                {
                    p2 += (kingmin[3 - player][i][j] - kingmin[player][i][j]) / 6;
                }
                else
                {
                    p2 += 1;
                }
            }
    if(step <= 20)
        return 0.37 * t2 + 0.13 * p2;
    else if(step <= 45)
        return 0.25 * t2 + 0.2 * p2;
    else
        return 0.1 * t2 + 0.05 * p2;
}

int evaluate(int player)
{
    DK(1);
    DK(2);
    int t2 = 0;
    for(int i = 0; i <= 7; i++)
        for(int j = 0; j <= 7; j++)
            if(board[i][j] == 0)
            {
                if(kingmin[player][i][j] < kingmin[3 - player][i][j])
                {
                    t2 += 6;
                }
                else if(kingmin[player][i][j] > kingmin[3 - player][i][j])
                {
                    t2 -= 6;
                }
                else
                {
                    if(kingmin[player][i][j] != inf)
                        t2 += 1;
                }
            }
    return t2;
}

double evaluate2(int player)
{
    return TP1(player) + TP2(player);
}

void put(int x0, int y0, int x1, int y1, int x2, int y2, int player)
{
    board[x0][y0] = 0;
    board[x1][y1] = player;
    board[x2][y2] = -1;
    for(int i = 0; i <= 3; i++)
        if(queen[player][i][0] == x0 && queen[player][i][1] == y0)
        {
            queen[player][i][0] = x1;
            queen[player][i][1] = y1;
            break;
        }
}

void clear(int x0, int y0, int x1, int y1, int x2, int y2, int player)
{
    board[x2][y2] = 0;
    board[x1][y1] = 0;
    board[x0][y0] = player;
    for(int i = 0; i <= 3; i++)
        if(queen[player][i][0] == x1 && queen[player][i][1] == y1)
        {
            queen[player][i][0] = x0;
            queen[player][i][1] = y0;
            break;
        }
}

int Minimax(int depth, int previous)
{
    finish = clock();
    if(timeflag == 1 || (finish - start) >= timelimit)
    {
        timeflag = 1;
        return 0;
    }
    if(depth == maximum)
        return evaluate(me);
    int current = (depth + me) % 2 + 1;//确认当前下棋者
    if(judge(current) == 0)
    {
        if(current != me)
            return inf;
        else
            return -inf;
    }
    int ans, tmp;
    int num, x1, y1, x2, y2;//记录最优走法
    if(current == me)
        ans = -inf;
    else
        ans = inf;
    for(int i = 0; i <= 3; i++)//逐一枚举每一个皇后的每一种走法，并评估找到最优解
    {
        int x0 = queen[current][i][0];
        int y0 = queen[current][i][1];
        for(int j = 0; j <= 7; j++)//逐一枚举每一个方向
        {
            int t = 1;//新的皇后的位置与旧的皇后的位置差的king距离
            while(1)
            {
                int tx = x0 + dx[j] * t;
                int ty = y0 + dy[j] * t;
                if(!legal(tx, ty))
                    break;
                if(board[tx][ty] != 0)
                    break;
                board[tx][ty] = current;//移动皇后
                board[x0][y0] = 0;
                queen[current][i][0] = tx;
                queen[current][i][1] = ty;
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
                        if(kexing(x0, y0, xx, yy) == 0)//已经碰到了障碍，不能继续尝试在更远的地方放置障碍了
                            break;
                        else
                            board[xx][yy] = -1;//放障碍
                        if(kexing(x0, y0, xx, yy) == 2)
                            flag = 1;//标记此时碰到了原来皇后在的位置，所以不能继续增加length了
                        tmp = Minimax2(depth + 1, ans);
                        if(current == me)
                        {
                            if(tmp > ans)
                            {
                                ans = tmp;
                                num = i;
                                x1 = tx, y1 = ty;
                                x2 = xx, y2 = yy;
                            }
                        }
                        else
                        {
                            if(tmp < ans)
                            {
                                ans = tmp;
                                num = i;
                                x1 = tx, y1 = ty;
                                x2 = xx, y2 = yy;
                            }
                        }
                        if(current != me && ans <= previous)
                        {
                            clear(x0, y0, tx, ty, xx, yy, current);
                            return ans;
                        }
                        if(current == me && ans >= previous)
                        {
                            if(depth != 1)
                            {
                                clear(x0, y0, tx, ty, xx, yy, current);
                                return ans;
                            }
                            else
                            {
                                put(x0, y0, x1, y1, x2, y2, me);
	                            cout << endl;
	                            cout << "    第" << ++step << "步:" << endl;
	                            draw();
                                return ans;
                            }
                        }
                        board[xx][yy] = 0;//撤回放障碍
                        if(flag == 1)
                            break;//如果当前碰到原来皇后的位置，就结束在这个方向上继续放更远的障碍
                        else
                            length++;
                    }
                }
                board[tx][ty] = 0;//还原皇后
                board[x0][y0] = current;
                queen[current][i][0] = x0;
                queen[current][i][1] = y0;
                t++;
            }
        }
    }
    if(depth == 1)
    {
        int x0 = queen[me][num][0];
        int y0 = queen[me][num][1];
        put(x0, y0, x1, y1, x2, y2, me);
        system("cls");
	    cout << endl;
	    cout << "    第" << ++step << "步:" << endl;
	    draw();
    }
    return ans;
}

int Minimax2(int depth, int previous)
{
    finish = clock();
    if(timeflag == 1 || (finish - start) >= timelimit)
    {
        timeflag = 1;
        return 0;
    }
    if(depth == maximum)
        return evaluate2(me);
    int current = (depth + me) % 2 + 1;//确认当前下棋者
    if(judge(current) == 0)
    {
        if(current != me)
            return inf;
        else
            return -inf;
    }
    int ans, tmp;
    int num, x1, y1, x2, y2;//记录最优走法
    if(current == me)
        ans = -inf;
    else
        ans = inf;
    for(int i = 0; i <= 3; i++)//逐一枚举每一个皇后的每一种走法，并评估找到最优解
    {
        int x0 = queen[current][i][0];
        int y0 = queen[current][i][1];
        for(int j = 0; j <= 7; j++)//逐一枚举每一个方向
        {
            int t = 1;//新的皇后的位置与旧的皇后的位置差的king距离
            while(1)
            {
                int tx = x0 + dx[j] * t;
                int ty = y0 + dy[j] * t;
                if(!legal(tx, ty))
                    break;
                if(board[tx][ty] != 0)
                    break;
                board[tx][ty] = current;//移动皇后
                board[x0][y0] = 0;
                queen[current][i][0] = tx;
                queen[current][i][1] = ty;
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
                        if(kexing(x0, y0, xx, yy) == 0)//已经碰到了障碍，不能继续尝试在更远的地方放置障碍了
                            break;
                        else
                            board[xx][yy] = -1;//放障碍
                        if(kexing(x0, y0, xx, yy) == 2)
                            flag = 1;//标记此时碰到了原来皇后在的位置，所以不能继续增加length了
                        tmp = Minimax(depth + 1, ans);
                        if(current == me)
                        {
                            if(tmp > ans)
                            {
                                ans = tmp;
                                num = i;
                                x1 = tx, y1 = ty;
                                x2 = xx, y2 = yy;
                            }
                        }
                        else
                        {
                            if(tmp < ans)
                            {
                                ans = tmp;
                                num = i;
                                x1 = tx, y1 = ty;
                                x2 = xx, y2 = yy;
                            }
                        }
                        if(current != me && ans <= previous)
                        {
                            clear(x0, y0, tx, ty, xx, yy, current);
                            return ans;
                        }
                        if(current == me && ans >= previous)
                        {
                            if(depth != 1)
                            {
                                clear(x0, y0, tx, ty, xx, yy, current);
                                return ans;
                            }
                            else
                            {
                                put(x0, y0, x1, y1, x2, y2, me);
	                            cout << endl;
	                            cout << "    第" << ++step << "步:" << endl;
	                            draw();
                                return ans;
                            }
                        }
                        board[xx][yy] = 0;//撤回放障碍
                        if(flag == 1)
                            break;//如果当前碰到原来皇后的位置，就结束在这个方向上继续放更远的障碍
                        else
                            length++;
                    }
                }
                board[tx][ty] = 0;//还原皇后
                board[x0][y0] = current;
                queen[current][i][0] = x0;
                queen[current][i][1] = y0;
                t++;
            }
        }
    }
    if(depth == 1)
    {
        int x0 = queen[me][num][0];
        int y0 = queen[me][num][1];
        put(x0, y0, x1, y1, x2, y2, me);
        system("cls");
	    cout << endl;
	    cout << "    第" << ++step << "步:" << endl;
	    draw();
    }
    return ans;
}

void cunpan()//存档：记录当前棋盘的状态
{
    ofstream fout("cun.txt");
    if(!fout)
    {
        cout << "请检查文件是否被其他程序占用";
        return ;
    }
    fout << step << endl;
    for(int i = 0; i <= 7; i++)
        for(int j = 0; j < 7; j++)
            fout << board[i][j] << " ";
    for(int i = 0; i <= 2; i++)
        for(int j = 0; j <= 3; j++)
            for(int k = 0; k <= 1; k++)
                fout << queen[i][j][k] << " ";
    fout.close();
    return ;
}

void dupan()//读盘：恢复上次中止时的棋盘状态
{
    ifstream fin("cun.txt");
    if(!fin)
    {
        cout << "请检查文件是否被其他程序占用";
        return ;
    }
    fin >> step;
    for(int i = 0; i <= 7; i++)
        for(int j = 0; j <= 7; j++)
            fin >> board[i][j];
    for(int i = 0; i <= 2; i++)
        for(int j = 0; j <= 3; j++)
            for(int k = 0; k <= 1; k++)
                fin >> queen[i][j][k];
    fin.close();
    return ;
}

bool legal(int x, int y)//判断这个棋是否在棋盘内
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
    return 1;
}

int kexing(int x0, int y0, int x2, int y2)//判断AI这一步下的棋是否合法
{
    if(board[x2][y2] == 0)//箭是否合法
        return 1;
    else if(x2 == x0 && y2 == y0)//原来皇后的位置虽然不是空的但也可以下棋
        return 2;
    else 
        return 0;//既不是空的也不是原来皇后的位置
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
            switch (board[i][j])
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

bool judge(int player)//判断某方是否输了
{
    for(int i = 0; i <= 3; i++)//这四个皇后中，只要有一个人可以走就没输
        for(int k = 0; k <= 7; k++)
        {
            int tx = queen[player][i][0] + dx[k];
            int ty = queen[player][i][1] + dy[k];
            if(legal(tx, ty))
                if(board[tx][ty] == 0)//只有有一个合法的邻格空着，这个皇后就没死
                    return 1;
        }
    return 0;//如果所有皇后都动不了就输了
}

void mode()
{
    int num;
    cout << "Hello~~欢迎来到亚马逊棋的世界" << endl;
    cout << "在这里您可以有以下几种选择：" << endl;
    cout << "1.和蠢萌的bot下一盘棋" << endl;
    cout << "2.欣赏两只蠢萌的bot自己下一盘棋" << endl;
    cout << "3.继续刚才中止的棋局" << endl;
    cout << "4.结束游戏" << endl;
    cout << "请您输入您想要的模式的编号吧~" << endl;
    while(cin >> num)
    {
        int flag = 0;//标记是否要结束本游戏
        switch (num)
        {
            case 1: newgame();break;
            case 2: AIplay();break;
            case 3: dupan();break;
            case 4:
                {
                    cout << "小bot那么可爱，您居然忍心离开!那好吧，bot自己玩去啦，再见～～" << endl;
                    flag = 1;
                    break;
                }
            default: cout << "对不起，蠢萌的bot听不懂您的话，请再输入一次～" << endl;break;
        }
        if(flag == 1)
            break;
        cout << "您还有什么吩咐嘛～～～：" << endl;
        cout << "1.和蠢萌的bot下一盘棋" << endl;
        cout << "2.欣赏两只蠢萌的bot自己下一盘棋" << endl;
        cout << "3.继续刚才中止的棋局" << endl;
        cout << "4.结束游戏" << endl;
        cout << "请您输入您想要的模式的编号吧~" << endl;
    }
    return;
}

int main()
{
    mode();
    return 0;
}