gcc -o client client.c
chmod +x client

gcc -o server server.c
chmod +x server

./server &

echo "--- 1 ---"
sleep 1
./client

echo "\n--- 2 ----"
sleep 1
./client
