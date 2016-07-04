## EpollServer
基于Epoll模型的简单服务器实现！

### 文件说明
- makefile 生成可执行文件S，SC，MS
- Server.c 服务器实现
- Sgclient.c 单个客户端与服务器的通信
- Mtclient.c 模拟多个客户端的测试脚本
- unity.h    通用函数头文件

### 使用
- makefile
- cd obj
- ./S  （运行服务器端）
- ./SC （运行单个客户端）
- ./MC  (模拟多个客户端，运行压力测试脚本)

### 客户端命令|功能
- USER XXX | 用户名 XXX
- PASS XXX | 密码 XXX
- RETR     | 从服务器上下载文件

### 服务器
- 实现客户端的身份认证
- 当前并发连接数
- 当前服务器的吞吐量
- 命令的解析和输出

### 待完成
- 对服务器的并发测试，需要模拟多个客户机同时发起请求
- user Auth