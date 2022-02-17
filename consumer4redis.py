# -*- coding: utf-8 -*-
import multiprocessing
import time

import redis
import incident_pb2

# 以下代码是向redis 发命令
QUEUE = 'key_incident_queue'
# redisPool = redis.ConnectionPool(host=config.get_redis_host(), port=6379, db=config.get_redis_db())
redisPool = redis.ConnectionPool(host='localhost', port=6379, db=8)
client = redis.Redis(connection_pool=redisPool)


# 以下代码是向redis 取命令，并且采用多进程来实现计算
def func(a, b, c):
    print(a, b)


def worker(count, pname):
    exit_counter = 0
    while True:
        try:
            if exit_counter >= 6:
                print('Queue is empty, so return.')
                return
            data = client.lpop(QUEUE)
            if not data:
                exit_counter += 1
                time.sleep(5)
                continue

            count += 1
            exit_counter = 0
            key_incident = format_cmd(data)
            print('process name is <{}>, src ip is {}, index is {}'.format(pname, key_incident.sip, count))
        except Exception as e:
            print(e)
            return


def format_cmd(data):
    dst_key_incident = incident_pb2.key_incident()
    dst_key_incident.ParseFromString(data)
    return dst_key_incident
    # print(dst_key_incident.sip)
    # print(dst_key_incident.dip)
    # print(dst_key_incident.sport)
    # print(dst_key_incident.dport)
    # print(dst_key_incident.smac)
    # print(dst_key_incident.dmac)
    # print(dst_key_incident.proto)
    # print(dst_key_incident.app)
    # print(dst_key_incident.flow_time)
    # print(dst_key_incident.pkt_time)


if __name__ == '__main__':
    # 多进程消费
    pro_num = 5
    pool = multiprocessing.Pool(processes=pro_num)
    for pid in range(0, pro_num):
        counter = 0
        pid = 'PROC' + str(pid).zfill(3)
        pool.apply_async(worker, (counter, pid,))
    pool.close()
    pool.join()
