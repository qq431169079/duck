#!/usr/bin/python
import httplib
import urllib
conn = httplib.HTTPConnection("www.baidu.com", 80)
conn.request("GET", "/")
r = conn.getresponse()

print(r.status)
print(r.reason)
print(r.version)
print(r.msg)
