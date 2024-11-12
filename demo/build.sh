cp ~/complier/npu_sdk_650/build/libax_skel.so bin/boxdemo/
make p=AX650_emmc  all install
scp -r ../../msp/out/bin/BoxDemo/ root@192.168.0.130:/root
