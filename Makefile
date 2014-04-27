
regfs: main.o Interface.o IHiveFile.o IHiveMmap.o KeyEntry.o MemManager.o
	g++ -o regfs main.o Interface.o IHiveFile.o IHiveMmap.o KeyEntry.o MemManager.o -lfuse -pthread -L/usr/local/lib

main.o: main.cpp
	g++ -c -o main.o main.cpp `pkg-config fuse --cflags` -DHAVE_SETXATTR=1

Interface.o: Interface.cpp
	g++ -c -o Interface.o Interface.cpp `pkg-config fuse --cflags` -DHAVE_SETXATTR=1

IHiveFile.o: IHiveFile.cpp
	g++ -c -o IHiveFile.o IHiveFile.cpp

IHiveMmap.o: IHiveMmap.cpp
	g++ -c -o IHiveMmap.o IHiveMmap.cpp

KeyEntry.o: KeyEntry.cpp
	g++ -c -o KeyEntry.o KeyEntry.cpp

MemManager.o: MemManager.cpp
	g++ -c -o MemManager.o MemManager.cpp -std=c++0x

clean:
	rm main.o
	rm Interface.o
	rm IHiveFile.o
	rm IHiveMmap.o
	rm KeyEntry.o
	rm MemManager.o
	rm regfs
