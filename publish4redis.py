# -*- coding: utf-8 -*-

import redis

number_list = ['300033', '300032', '300031', '300030']
signal = ['1', '-1', '1', '-1']

rc = redis.StrictRedis()
for i in range(len(number_list)):
    value_new = str(number_list[i]) + ' ' + str(signal[i])
    rc.publish('fuck', value_new)
    print('end')
