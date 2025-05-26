rm -rf sender
rm -rf receiver
rm -rf tracker

g++ sender.cpp -o sender
g++ receiver.cpp -o receiver $(pkg-config --libs --cflags gtk+-3.0)
g++ tracker.cpp -o tracker



