sudo docker run -v  $PWD:/app  -e CBURL="ws://<IPADDRESS:PORT>/ws" -e CBREALM="realm1"  --rm -it crossbario/autobahn-python:cpy3 python  client_component_subscribe.py
