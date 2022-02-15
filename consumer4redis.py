# -*- coding: utf-8 -*-
import json
import multiprocessing
import time

import redis

# 以下代码是向redis 发命令
QUEUE = 'code'
# redisPool = redis.ConnectionPool(host=config.get_redis_host(), port=6379, db=config.get_redis_db())
redisPool = redis.ConnectionPool(host='localhost', port=6379, db=8)
client = redis.Redis(connection_pool=redisPool)


# 以下代码是向redis 取命令，并且采用多进程来实现计算
def func(a, b, c):
    print(a, b)


def worker(pname):
    exit_counter = 0
    while True:
        try:
            if exit_counter >= 30:
                print('Queue is empty, so return.')
                return
            cmd = client.lpop(QUEUE)
            if not cmd:
                exit_counter += 1
                time.sleep(5)
                continue
            exit_counter = 0
            print('process name is <{}>, msg is {}'.format(pname, cmd))
        except Exception as e:
            print(e)
            return


def format_cmd(cmd):
    return json.loads(cmd)


if __name__ == '__main__':
    # 多进程消费
    pro_num = 5
    pool = multiprocessing.Pool(processes=pro_num)
    for pid in range(1, pro_num):
        pid = 'PROC' + str(pid).zfill(3)
        pool.apply_async(worker, (pid,))
    pool.close()
    pool.join()
