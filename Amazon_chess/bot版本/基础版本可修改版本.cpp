#include <iostream>
#include <queue>
#include <ctime>
using namespace std;

#define inf 10000000
int board[8][8] = {};//存储棋盘上的棋子，0为空，1为黑，2为白，-1为障碍
int dx[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
int dy[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
int step = 0;//当前总共下了几步
int kingmin[3][8][8] = {};
int maximum ;
int me;//标志机器走的是哪一方
const int timelimit = 0.95 * CLOCKS_PER_SEC;//防止超时
int timeflag = 0;
int start, finish;
queue<int> dis;
struct node
{
    int x0, y0, x1, y1, x2, y2;
};
node Ans;


bool legal(int, int);//判断这个棋是否在棋盘内
void DK(int player);
bool judge(int );//判断某方是否输了
int evaluate(int );//新的评估方式
int Minimax(int , int );//利用alpha-beta剪枝进行AI决策，采用新的评估方式
void put(int , int , int , int , int , int , int );//模拟AI决策中的落子
void clear(int , int , int , int , int , int , int );//清除AI决策中的模拟落子

inline bool legal(int x, int y)//判断这个棋是否在棋盘内
{
    if(x >= 0 && x < 8 && y >= 0 && y < 8)
        return 1;
    else
        return 0;
}

void DK(int player)//player走到当前格的king走法的最少移动次数
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

int evaluate(int current)//评估现在是current下棋的时候，当前局面对我的评估值
{
    int my = judge(me), op = judge(3 - me);
    if(my == 0 && (op != 0 || me == current))
        return -inf;
    if(op == 0 && (my != 0 || me != current))
        return inf;
    DK(1), DK(2);
    int t = 0;
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
                else
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
    return t;
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
            return -inf + 100;
        else
            return inf - 100;
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
            int x0 = i;
            int y0 = l;
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
                                        temp.x0 = x0, temp.y0 = y0, temp.x1 = tx, temp.y1 = ty, temp.x2 = xx, temp.y2 = yy;
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
                                if(depth != 1)
                                {
                                    clear(x0, y0, tx, ty, xx, yy, current);
                                    return ans;
                                }
                                else                            //如果这个时候是我下的第1步，由于previous = inf，
                                {                               //所以说明我下一步之后，对方直接输了，那我就直接下这一步，结束搜索
                                    return ans;
                                }
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
    if(depth == 1 && timeflag == 0)
        Ans = temp;
    return ans;
}

bool judge(int player)//判断某方是否输了
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
    board[0][2] = 1;
    board[2][0] = 1;
    board[5][0] = 1;
    board[7][2] = 1;
    board[0][5] = 2;
    board[2][7] = 2;
    board[5][7] = 2;
    board[7][5] = 2;
    cin >> step;
    int x0, y0, x1, y1, x2, y2;
    cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
    if (x0 == -1)
	{
        me = 1;
        for(int i = 1; i <= step - 1; i++)
        {
            cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
            put(x0, y0, x1, y1, x2, y2, 1);
            cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
            put(x0, y0, x1, y1, x2, y2, 2);
        }
    }
    else
    {
        me = 2;
        put(x0, y0, x1, y1, x2, y2, 1);
        for(int i = 1; i <= step - 1; i++)
        {
            cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
            put(x0, y0, x1, y1, x2, y2, 2);
            cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
            put(x0, y0, x1, y1, x2, y2, 1);
        }
    }
    start = clock();
    timeflag = 0;
    if(step <= 4)
        maximum = 2;
    else
        maximum = 3;
    while(1)
    {
        int tmp = Minimax(1, inf + 233);
        if(tmp == inf || tmp == -inf|| timeflag == 1) //为什么要加  || tmp == -inf ？
        {
            cout << Ans.x0<<' '<<Ans.y0<<' '<<Ans.x1<<' '<<Ans.y1<<' '<<Ans.x2<<' '<<Ans.y2<<endl;
            break;
        }
        if(step >= 15)
            maximum += 2;
        else
            maximum ++;
    }
    return 0;
}