nasm ipl.nas -o ipl.img 
sudo mount -o loop ipl.img /run/media/ipl/
sudo cp slef.sys /run/media/ipl/
sudo umount /run/media/ipl 
xxd ipl.img > ipl.hex
