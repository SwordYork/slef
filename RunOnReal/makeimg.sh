sudo mkdir -p /run/media/boot/
sudo mount -o loop boot.img /run/media/boot/
sudo cp kernel.sys /run/media/boot/
sudo umount /run/media/boot 
xxd boot.img > boot.hex
