double evaluate(int color)
{
    int mark[8][8] = {};
	int total = 0;//记录总共能行棋的地方的数量
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (board[i][j] != color)
                continue;//确保此处棋子颜色和我想评估的相同
			for (int k = 0; k < 8; ++k)
			{
				for (int delta1 = 1; delta1 < 8; delta1++)
				{
					int xx = i + dx[k] * delta1;//八个方向都走
					int yy = j + dy[k] * delta1;
					if (board[xx][yy] != 0 || !legal(xx, yy))
						break; //一旦遇到障碍就停下来
					if (mark[xx][yy] == 0)
					{
						total++; 
                        mark[xx][yy] = 1;//只有这个格子没有来过，total才会加一。
					}
				}
			}
		}
	}
	return total;
}