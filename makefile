test:
	g++ testlc.cpp -o testlc -std=c++20 -O3
	@./testlc
	@rm -rf testlc

debug:
	g++ testlc.cpp -o testlc -std=c++20 -g
	@gdb testlc
	@rm -rf testlc

clean:
	find -perm /111 -! -path "*git*" -type f -delete