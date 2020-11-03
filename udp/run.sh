gcc -o send send.c
chmod +x send

gcc -o receive receive.c
chmod +x receive

./receive &
sleep 1
./send

