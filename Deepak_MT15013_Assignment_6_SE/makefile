compile:
	gcc ls.c -o ls
	gcc fget.c -o fget
	gcc fput.c -o fput
	gcc create_dir.c -o create_dir
	gcc chmod.c -o chmod
	gcc getacl.c -o getacl
	gcc setacl.c -o setacl
	g++ -c aclStructure.cpp
	g++ -c functions.cpp
	g++ -c fput_validate.cpp
	g++ -c fget_decrypt.cpp
	g++ -c fput_encrypt.cpp
	g++ -g fput_validate.o functions.o aclStructure.o -o fput_validate -lcrypto
	g++ -g fget_decrypt.o functions.o aclStructure.o -o fget_decrypt -lcrypto
	g++ -g fput_encrypt.o functions.o aclStructure.o -o fput_encrypt -lcrypto
	chmod u+s ls
	chmod u+s fget
	chmod u+s fput
	chmod u+s create_dir
	chmod u+s chmod
	chmod u+s getacl
	chmod u+s setacl
	chmod u+s fput_encrypt
	chmod u+s fget_decrypt
	chmod u+s fput_validate

setup:
	sudo adduser --home /simple_slash/home/fakeroot fakeroot
	sudo adduser --home /simple_slash/home/bill bill
	sudo adduser --home /simple_slash/home/bob bob
	sudo adduser --home /simple_slash/home/harry harry
	sudo addgroup simple_slash
	sudo chown root /simple_slash/home/
	sudo chown root /simple_slash/
	sudo useradd -ou 0 -g 0 fakeroot
	sudo chmod 777 /simple_slash/home/fakeroot
	sudo chmod 777 /simple_slash/home/bill
	sudo chmod 777 /simple_slash/home/bob
	sudo chmod 777 /simple_slash/home/harry
	sudo printf '%s\n' 'bill' 'bob' 'harry' > /simple_slash/home/users.txt
	sudo printf '%s\n' 'fakeroot rwx' > /simple_slash/home/fakeroot/permission.acl
	sudo printf '%s\n' 'bill rwx' > /simple_slash/home/bill/permission.acl
	sudo printf '%s\n' 'bob rwx' > /simple_slash/home/bob/permission.acl
	sudo printf '%s\n' 'harry rwx' > /simple_slash/home/harry/permission.acl

clean:
	rm ls
	rm fget
	rm fput
	rm create_dir
	rm chmod
	rm getacl
	rm setacl
	rm fput_encrypt
	rm fget_decrypt
	rm fput_validate
