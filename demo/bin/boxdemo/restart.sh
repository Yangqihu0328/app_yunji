#!/bin/sh

/usr/bin/systemctl stop yj-aibox
/usr/bin/systemctl stop yj-mediaserver

/usr/bin/systemctl start yj-mediaserver
/usr/bin/systemctl start yj-aibox