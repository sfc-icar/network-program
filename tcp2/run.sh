gcc -o client client.c
chmod +x client

gcc -o server server.c
chmod +x server

./server &
sleep 1
./client
