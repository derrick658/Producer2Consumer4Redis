## 编译

```bash
protoc --python_out=. incident.proto
protoc --c_out=. incident.proto
gcc -I/usr/local/include/hiredis producer4redis.c -lhiredis -o producer4redis
gcc producer4redis.c  incident.pb-c.c -o producer4redis -lprotobuf-c -lhiredis -I/usr/local/include/hiredis
```



## 参考 ##

> - [1] [GitHub hiredis](https://github.com/redis/hiredis.git)
> - [2] [Hiredis 基本使用](https://www.cnblogs.com/jabnih/p/4814212.html)
> - [3] [protobuf-c wiki](https://github.com/protobuf-c/protobuf-c/wiki)
> - [4] [Redis 的 C 语言客户端](https://is-cloud.blog.csdn.net/article/details/106362722)
> - [5] [C 语言操作 Redis](https://blog.51cto.com/u_15346415/3675138)
> - [6] [protobuf-c的学习总结](https://www.cnblogs.com/anker/p/3416541.html)
> - [7] [范桂飓 CSDN 博客](https://is-cloud.blog.csdn.net/?type=blog)

