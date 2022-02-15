```bash
  protoc --python_out=. incident.proto
  gcc -I/usr/local/include/hiredis producer4redis.c -lhiredis -o producer4redis
```

```
  https://github.com/redis/hiredis.git 
  https://blog.51cto.com/u_15346415/3675138
```
