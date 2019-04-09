//存放一些比较杂的函数

/********随机打乱一个数组********/
for(i = n - 1; i >= 0; i--)
  index = rand(i + 1) % i + 1//generate random index [0, i]
  if (i != index)
    exchange(i, index)
    

/********vector赋值需要注意的地方********/
int main()
{
    vector<int> vec;
    int *p[10];
    for(int i = 0; i < 10; ++i)
    {
      vec.push_back(i);
      p[i] = &vec[i];//注意这一步
    }
    
    cout << *p[0] << endl;//24
    cout << *p[9] << endl;//9
}
//请注意,在vec.push_back(i)的时候,vec可能会调用resize函数，导致重新分配内存,而使得原先的指针失效
//有时如果直接使用resize之前保存的地址指针,有时会报错,有时会输出不可控制的数据
