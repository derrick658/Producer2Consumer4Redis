# -*- coding: utf-8 -*-
import json
import redis

# 以下代码是向redis 发命令
QUEUE = 'code'  # 队列名称key

redisPool = redis.ConnectionPool(host='localhost', port=6379, db=8)
client = redis.Redis(connection_pool=redisPool)


def send_cmd(seaweed):
    json_cmd = json.dumps(seaweed, ensure_ascii=False)
    client.rpush(QUEUE, json_cmd)


if __name__ == '__main__':
    for k in range(100):
        send_cmd({'label': k, 'timd': 20160503, 'timm': 20170430})
