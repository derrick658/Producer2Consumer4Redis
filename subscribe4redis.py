# -*- coding: utf-8 -*-

import redis

rc = redis.StrictRedis()
ps = rc.pubsub()
ps.subscribe('fuck')  # 订阅消息
a = 0
for item in ps.listen():  # 监听状态：有消息发布了就拿过来
    print(type(item))
    print(item)
    a += 1
    if a % 1000 == 0:
        print(a)
