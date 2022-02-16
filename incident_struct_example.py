# -*- coding: utf-8 -*-
import time
import random
import unittest

import incident_pb2

"""
Two execution methods:
1. python -m unittest incident_struct_example
2. python incident_struct_example.py
"""


class RandomData(object):
    @staticmethod
    def random_mac():
        return '{0[0]}{0[1]}:{0[2]}{0[3]}:{0[4]}{0[5]}:{0[6]}{0[7]}:{0[8]}{0[9]}:{0[10]}{0[11]}'.format(
            random.sample('0123456789abcdef' * 12, 12))

    @staticmethod
    def random_address():
        return '.'.join(map(str, random.sample(range(1, 256) * 4, 4)))

    @staticmethod
    def random_port():
        return random.randint(0, 65536)

    @staticmethod
    def random_protocol():
        return random.choice(['tcp', 'udp', 'snmp', 'icmp'])

    @staticmethod
    def random_app():
        return random.choice(['mysql', 'oracle', 'http', 'pop3', 'modbus', 'dnp3'])

    @staticmethod
    def random_datetime():
        start_dt = (2000, 1, 1, 0, 0, 0, 0, 0, 0)  # 设置开始日期时间元组（1976-01-01 00：00：00）
        end_dt = (2022, 2, 14, 23, 59, 59, 0, 0, 0)  # 设置结束日期时间元组（1990-12-31 23：59：59）
        random_dt = time.localtime(random.randint(int(time.mktime(start_dt)), int(time.mktime(end_dt))))
        return time.strftime('%Y-%m-%d %H:%M:%S', random_dt)


src_key_incident = incident_pb2.key_incident()
dst_key_incident = incident_pb2.key_incident()

src_key_incident.sip = RandomData.random_address()
src_key_incident.dip = RandomData.random_address()
src_key_incident.sport = RandomData.random_port()
src_key_incident.dport = RandomData.random_port()
src_key_incident.smac = RandomData.random_mac()
src_key_incident.dmac = RandomData.random_mac()
src_key_incident.proto = RandomData.random_protocol()
src_key_incident.app = RandomData.random_app()
src_key_incident.flow_time = RandomData.random_datetime()
src_key_incident.pkt_time = RandomData.random_datetime()

print('=====================原始数据=======================')
# print(src_key_incident)

print('====================序列化数据=======================')
dump_data = src_key_incident.SerializeToString()
# print(dump_data)

print('====================反序列化数据======================')
load_data = dst_key_incident.ParseFromString(dump_data)
# print(dst_key_incident)
assert src_key_incident == dst_key_incident


class TestProtobuf(unittest.TestCase):
    def setUp(self):
        self.src_key_incident = incident_pb2.key_incident()
        self.dst_key_incident = incident_pb2.key_incident()

        self.src_key_incident.sip = RandomData.random_address()
        self.src_key_incident.dip = RandomData.random_address()
        self.src_key_incident.sport = RandomData.random_port()
        self.src_key_incident.dport = RandomData.random_port()
        self.src_key_incident.smac = RandomData.random_mac()
        self.src_key_incident.dmac = RandomData.random_mac()
        self.src_key_incident.proto = RandomData.random_protocol()
        self.src_key_incident.app = RandomData.random_app()
        self.src_key_incident.flow_time = RandomData.random_datetime()
        self.src_key_incident.pkt_time = RandomData.random_datetime()

    def test_serialization(self):
        self.assertTrue(self.src_key_incident.SerializeToString())

    def test_deserialization(self):
        result = self.src_key_incident.SerializeToString()
        self.dst_key_incident.ParseFromString(result)
        self.assertEqual(self.dst_key_incident, self.src_key_incident)


if __name__ == '__main__':
    unittest.main()
