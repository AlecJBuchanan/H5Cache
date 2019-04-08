cacheMain: ./src/main/h5main.c ./src/main/h5cache.c ./src/main/h5chunks.c ./src/main/shmMain.c
	#gcc `pkg-config --cflags glib-2.0` -lpthread -lrt ./src/main/h5main.c ./src/main/h5cache.c ./src/main/h5chunks.c ./src/main/shmMain.c -o ./bin/main/main `pkg-config --libs glib-2.0` -I/mnt/common/abuchan1/code/h5cache/includes/ -L/mnt/common/abuchan1/code/h5cache/lib -lhdf5 -lrt -lm -lshmht
	gcc -I/mnt/common/abuchan1/code/h5cache/includes/ -L/mnt/common/abuchan1/code/h5cache/lib -lhdf5 -lrt -lm -lshmht_0.1 -lpthread -lrt ./src/main/h5main.c ./src/main/h5cache.c ./src/main/h5chunks.c ./src/main/shmMain.c -o ./bin/main/main
cacheLib: ./src/main/h5cache.c
	gcc -shared -fPIC ./src/main/h5cache.c -o ./bin/main/libH5Cache.so -I/mnt/common/abuchan1/programs/hdf5-1.10.4/src/
	#export LD_PRELOAD=$PWD/libH5Cache.so
preloadMain: ./src/tests/preLoadTest/preLoadMain.c
	gcc ./src/tests/preLoadTest/preLoadMain.c -o ./bin/preLoadTest/main -I/mnt/common/abuchan1/code/h5cache/includes/ -L/mnt/common/abuchan1/code/h5cache/lib -lhdf5
	#h5cc ./src/tests/preLoadTest/preLoadMain.c -o ./bin/preLoadTest/main -I/mnt/common/abuchan1/programs/hdf5-1.10.4/src/
	#rm ./*.o
preloadLib: ./src/tests/preLoadTest/preLoadLib.c
	gcc -shared -fPIC ./src/tests/preLoadTest/preLoadLib.c -o ./bin/preLoadTest/libPreLoad.so -I/mnt/common/abuchan1/code/h5cache/includes/ -L/mnt/common/abuchan1/code/h5cache/lib -lhdf5
	#h5cc -shared -fPIC ./src/tests/preLoadTest/preLoadLib.c -o ./bin/preLoadTest/libPreLoad.so -I/mnt/common/abuchan1/programs/hdf5-1.10.4/src/
	#rm ./*.o
shm: ./src/tests/shmTester/shmMain.c
	gcc ./src/tests/shmTester/*.c -o ./bin/shmTester/output -lrt -lpthread

	#gcc `pkg-config --cflags glib-2.0` -lpthread -lrt ./src/main/h5main.c ./src/main/h5cache.c ./src/main/h5chunks.c ./src/main/shmMain.c -o ./bin/main/main `pkg-config --libs glib-2.0` -I/mnt/common/abuchan1/code/h5cache/includes/:/mnt/common/abuchan1/programs/libshmht_dist_0.1_x86/include -L/mnt/common/abuchan1/code/h5cache/lib:/mnt/common/abuchan1/programs/libshmht_dist_0.1_x86/lib -lshmht_0.1 -lhdf5 -lrt -lm
