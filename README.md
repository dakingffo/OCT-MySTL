# OCT-MySTL
OCT是一套STL容器库，旨在锻炼我的C++编程能力，这套库的核心目的是符合C++17标准规范。
include文件夹中包含所有的头文件，可以从test文件夹中下载功能测试样例或压力测试样例。
## 容器特色
### 序列式容器: array
+ 对array<Ty, 0>进行偏特化
+ 支持结构化绑定
+ 提供模版辅助推导
### 序列式容器: vector
+ 在模版参数中提供是否自动收缩选项
+ 对于trivial类型提供特化allocator
+ 使用compress_pair进行空基类优化
### 序列式容器: deque
+ 对于Mapptr调用memmove
+ 使用compress_pair进行空基类优化
+ (后续)能在模版参数中支持自定义缓存空间长度
### 序列式容器: forward_list
+ 使用compress_pair进行空基类优化
### 序列式容器: list
+ 使用compress_pair进行空基类优化
### 关联式容器: set
+ 在模版参数中提供底层平衡树选项(当前仅支持red_black红黑树)
+ 使用compress_pair进行空基类优化
+ (后续)提供更多底层平衡树实现(AVL树、slpay伸展树、treap树堆)
### 关联式容器: map
+ 支持结构化绑定
+ 在模版参数中提供底层平衡树选项(当前仅支持red_black红黑树)
+ 使用compress_pair进行空基类优化
+ (后续)提供更多底层平衡树实现(AVL树、slpay伸展树、treap树堆)
### 容器适配器: queue
+ 默认使用oct::deque作为底层容器
+ 提供<<的运算符重载
+ 使用compress_pair进行空基类优化
### 容器适配器: priority_queue
+ 默认使用oct::vector作为底层容器
+ 提供<<的运算符重载
+ 使用compress_pair进行空基类优化
### 容器适配器: stack
+ 默认使用oct::vector作为底层容器
+ 提供<<与>>的运算符重载
### (施工中)字符串流: string
+ 提供char_traits
+ 提供SSO短字符串优化
+ 使用compress_pair进行空基类优化
### (施工中)关联式容器: unoredered-
施工中
### 元编程设施: type_traits
+ 提供模版参数重绑定工具
+ 提供对allocator的萃取工具
+ 提供对iterator的萃取工具
## 未来计划
+ 使allocator赋值标准规范
+ 完善deque::insert/erase
+ 完善关联式容器unordered-/multi
+ 完善关联式容器句柄结构
+ 强化元编程设施
+ 提供部分algorithm
+ 增强线程安全/异常安全
+ 提供CMake文件
